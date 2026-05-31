/* PSA Firmware Framework service API */

/*
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
 */

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <time.h>
#include <assert.h>

#include "service.h"
#include "init.h"
#include "error_ext.h"
#include "common.h"
#include "util.h"

#define MAX_CLIENTS 128
#define MAX_MESSAGES 32

struct connection {
    uint32_t client;
    void *rhandle;
    int client_to_server_q;
};

/* Note that this implementation is functional and not secure. */
int __psa_ff_client_security_state = NON_SECURE;

static psa_msg_t messages[MAX_MESSAGES]; /* Message slots */
static uint8_t pending_message[MAX_MESSAGES] = { 0 }; /* Booleans indicating active message slots */
static uint32_t message_client[MAX_MESSAGES] = { 0 }; /* Each client's response queue  */
static int nsacl[32];
static int strict_policy[32] = { 0 };
static uint32_t rot_svc_versions[32];
static int rot_svc_incoming_queue[32] = { -1 };
static struct connection connections[MAX_CLIENTS] = { { 0 } };

static uint32_t exposed_signals = 0;

void print_vectors(vector_sizes_t *sizes)
{
    INFO("Printing iovec sizes");
    for (int j = 0; j < PSA_MAX_IOVEC; j++) {
        INFO("Invec  %d: %lu", j, sizes->invec_sizes[j]);
    }

    for (int j = 0; j < PSA_MAX_IOVEC; j++) {
        INFO("Outvec %d: %lu", j, sizes->outvec_sizes[j]);
    }
}

int find_connection(uint32_t client)
{
    for (int i = 1; i < MAX_CLIENTS; i++) {
        if (client == connections[i].client) {
            return i;
        }
    }
    return -1;
}

void destroy_connection(uint32_t client)
{
    int idx = find_connection(client);
    if (idx >= 0) {
        connections[idx].client = 0;
        connections[idx].rhandle = 0;
        INFO("Destroying connection");
    } else {
        ERROR("Couldn't destroy connection for %u", client);
    }
}

int find_free_connection()
{
    INFO("Allocating connection");
    return find_connection(0);
}

static void reply(psa_handle_t msg_handle, psa_status_t status)
{
    pending_message[msg_handle] = 1;
    psa_reply(msg_handle, status);
    pending_message[msg_handle] = 0;
}

psa_signal_t psa_wait(psa_signal_t signal_mask, uint32_t timeout)
{
    psa_signal_t mask;
    struct message msg;
    vector_sizes_t sizes;
    struct msqid_ds qinfo;
    uint32_t requested_version;
    ssize_t len;
    int idx;

    if (timeout == PSA_POLL) {
        INFO("psa_wait: Called in polling mode");
    }

    do {
        mask = signal_mask;

        /* Check the status of each queue */
        for (int i = 0; i < 32; i++) {
            if (mask & 0x1) {
                if (i < 3) {
                    // do nothing (reserved)
                } else if (i == 3) {
                    // this must be psa doorbell
                } else {
                    /* Check if this signal corresponds to a queue */
                    if (rot_svc_incoming_queue[i] >= 0 && (pending_message[i] == 0)) {

                        /* AFAIK there is no "peek" method in SysV, so try to get a message */
                        len = msgrcv(rot_svc_incoming_queue[i],
                                     &msg,
                                     sizeof(struct message_text),
                                     0,
                                     IPC_NOWAIT);
                        if (len > 0) {

                            INFO("Storing that QID in message_client[%d]", i);
                            INFO("The message handle will be %d", i);

                            msgctl(rot_svc_incoming_queue[i], IPC_STAT, &qinfo);
                            messages[i].client_id = qinfo.msg_lspid; /* PID of last msgsnd(2) call */
                            message_client[i] = msg.message_text.qid;
                            idx = find_connection(msg.message_text.qid);

                            if (msg.message_type & NON_SECURE) {
                                /* This is a non-secure message */

                                /* Check if NS client is allowed for this RoT service */
                                if (nsacl[i] <= 0) {
#if 0
                                    INFO(
                                        "Rejecting non-secure client due to manifest security policy");
                                    reply(i, PSA_ERROR_CONNECTION_REFUSED);
                                    continue; /* Skip to next signal */
#endif
                                }

                                msg.message_type &= ~(NON_SECURE); /* clear */
                                messages[i].client_id = messages[i].client_id * -1;
                            }

                            INFO("Got a message from client ID %d", messages[i].client_id);
                            INFO("Message type is %lu", msg.message_type);
                            INFO("PSA message type is %d", msg.message_text.psa_type);

                            messages[i].handle = i;

                            switch (msg.message_text.psa_type) {
                                case PSA_IPC_CONNECT:

                                    if (len >= 16) {
                                        memcpy(&requested_version, msg.message_text.buf,
                                               sizeof(requested_version));
                                        INFO("Requesting version %u", requested_version);
                                        INFO("Implemented version %u", rot_svc_versions[i]);
                                        /* TODO: need to check whether the policy is strict,
                                         * and if so, then reject the client if the number doesn't match */

                                        if (requested_version > rot_svc_versions[i]) {
                                            INFO(
                                                "Rejecting client because requested version that was too high");
                                            reply(i, PSA_ERROR_CONNECTION_REFUSED);
                                            continue; /* Skip to next signal */
                                        }

                                        if (strict_policy[i] == 1 &&
                                            (requested_version != rot_svc_versions[i])) {
                                            INFO(
                                                "Rejecting client because enforcing a STRICT version policy");
                                            reply(i, PSA_ERROR_CONNECTION_REFUSED);
                                            continue; /* Skip to next signal */
                                        } else {
                                            INFO("Not rejecting client");
                                        }
                                    }

                                    messages[i].type = PSA_IPC_CONNECT;

                                    if (idx < 0) {
                                        idx = find_free_connection();
                                    }

                                    if (idx >= 0) {
                                        connections[idx].client = msg.message_text.qid;
                                    } else {
                                        /* We've run out of system wide connections */
                                        reply(i, PSA_ERROR_CONNECTION_BUSY);
                                        ERROR("Ran out of free connections");
                                        continue;
                                    }

                                    break;
                                case PSA_IPC_DISCONNECT:
                                    messages[i].type = PSA_IPC_DISCONNECT;
                                    break;
                                case VERSION_REQUEST:
                                    INFO("Got a version request");
                                    reply(i, rot_svc_versions[i]);
                                    continue; /* Skip to next signal */
                                    break;

                                default:

                                    /* PSA CALL */
                                    if (msg.message_text.psa_type >= 0) {
                                        messages[i].type = msg.message_text.psa_type;
                                        memcpy(&sizes, msg.message_text.buf, sizeof(sizes));
                                        print_vectors(&sizes);
                                        memcpy(&messages[i].in_size, &sizes.invec_sizes,
                                               (sizeof(size_t) * PSA_MAX_IOVEC));
                                        memcpy(&messages[i].out_size, &sizes.outvec_sizes,
                                               (sizeof(size_t) * PSA_MAX_IOVEC));
                                    } else {
                                        FATAL("UNKNOWN MESSAGE TYPE RECEIVED %li",
                                              msg.message_type);
                                    }
                                    break;
                            }
                            messages[i].handle = i;

                            /* Check if the client has a connection */
                            if (idx >= 0) {
                                messages[i].rhandle = connections[idx].rhandle;
                            } else {
                                /* Client is begging for a programmer error */
                                reply(i, PSA_ERROR_PROGRAMMER_ERROR);
                                continue;
                            }

                            /* House keeping */
                            pending_message[i] = 1; /* set message as pending */
                            exposed_signals |= (0x1 << i); /* assert the signal */
                        }
                    }
                }
                mask = mask >> 1;
            }
        }

        if ((timeout == PSA_BLOCK) && (exposed_signals > 0)) {
            break;
        } else {
            /* There is no 'select' function in SysV to block on multiple queues, so busy-wait :( */
        }
    } while (timeout == PSA_BLOCK);

    /* Assert signals */
    return signal_mask & exposed_signals;
}

static int signal_to_index(psa_signal_t signal)
{
    int i;
    int count = 0;
    int ret = -1;

    for (i = 0; i < 32; i++) {
        if (signal & 0x1) {
            ret = i;
            count++;
        }
        signal = signal >> 1;
    }

    if (count > 1) {
        ERROR("ERROR: Too many signals");
        return -1; /* Too many signals */
    }
    return ret;
}

static void clear_signal(psa_signal_t signal)
{
    exposed_signals = exposed_signals & ~signal;
}

void raise_signal(psa_signal_t signal)
{
    exposed_signals |= signal;
}

psa_status_t psa_get(psa_signal_t signal, psa_msg_t *msg)
{
    int index = signal_to_index(signal);
    if (index < 0) {
        ERROR("Bad signal");
    }

    clear_signal(signal);

    assert(messages[index].handle != 0);

    if (pending_message[index] == 1) {
        INFO("There is a pending message!");
        memcpy(msg, &messages[index], sizeof(struct psa_msg_t));
        assert(msg->handle != 0);
        return PSA_SUCCESS;
    } else {
        INFO("no pending message");
    }

    return PSA_ERROR_DOES_NOT_EXIST;
}

static inline int is_valid_msg_handle(psa_handle_t h)
{
    if (h > 0 && h < MAX_MESSAGES) {
        return 1;
    }
    ERROR("Not a valid message handle");
    return 0;
}

static inline int is_call_msg(psa_handle_t h)
{
    assert(messages[h].type >= PSA_IPC_CALL);
    return 1;
}

void psa_set_rhandle(psa_handle_t msg_handle, void *rhandle)
{
    is_valid_msg_handle(msg_handle);
    int idx = find_connection(message_client[msg_handle]);
    INFO("Setting rhandle to %p", rhandle);
    assert(idx >= 0);
    connections[idx].rhandle = rhandle;
}

/* Sends a message from the server to the client. Does not wait for a response */
static void send_msg(psa_handle_t msg_handle,
                     int ctrl_msg,
                     psa_status_t status,
                     size_t amount,
                     const void *data,
                     size_t data_amount)
{
    struct message response;
    int flags = 0;

    assert(ctrl_msg > 0); /* According to System V, it must be greater than 0 */

    response.message_type = ctrl_msg;
    if (ctrl_msg == PSA_REPLY) {
        memcpy(response.message_text.buf, &status, sizeof(psa_status_t));
    } else if (ctrl_msg == READ_REQUEST || ctrl_msg == WRITE_REQUEST || ctrl_msg == SKIP_REQUEST) {
        memcpy(response.message_text.buf, &status, sizeof(psa_status_t));
        memcpy(response.message_text.buf+sizeof(size_t), &amount, sizeof(size_t));
        if (ctrl_msg == WRITE_REQUEST) {
            /* TODO: Check if too big */
            memcpy(response.message_text.buf + (sizeof(size_t) * 2), data, data_amount);
        }
    }

    /* TODO: sizeof doesn't need to be so big here for small responses */
    if (msgsnd(message_client[msg_handle], &response, sizeof(response.message_text), flags) == -1) {
        ERROR("Failed to reply");
    }
}

static size_t skip(psa_handle_t msg_handle, uint32_t invec_idx, size_t num_bytes)
{
    if (num_bytes < (messages[msg_handle].in_size[invec_idx] - num_bytes)) {
        messages[msg_handle].in_size[invec_idx] = messages[msg_handle].in_size[invec_idx] -
                                                  num_bytes;
        return num_bytes;
    } else {
        if (num_bytes >= messages[msg_handle].in_size[invec_idx]) {
            size_t ret = messages[msg_handle].in_size[invec_idx];
            messages[msg_handle].in_size[invec_idx] = 0;
            return ret;
        } else {
            return num_bytes;
        }
    }
}

size_t psa_read(psa_handle_t msg_handle, uint32_t invec_idx,
                void *buffer, size_t num_bytes)
{
    size_t sofar = 0;
    struct message msg = { 0 };
    int idx;
    ssize_t len;

    is_valid_msg_handle(msg_handle);
    is_call_msg(msg_handle);

    if (invec_idx >= PSA_MAX_IOVEC) {
        ERROR("Invalid iovec number");
    }

    /* If user wants more data than what's available, truncate their request */
    if (num_bytes > messages[msg_handle].in_size[invec_idx]) {
        num_bytes = messages[msg_handle].in_size[invec_idx];
    }

    while (sofar < num_bytes) {
        INFO("Server: requesting %lu bytes from client", (num_bytes - sofar));
        send_msg(msg_handle, READ_REQUEST, invec_idx, (num_bytes - sofar), NULL, 0);

        idx = find_connection(message_client[msg_handle]);
        assert(idx >= 0);

        len = msgrcv(connections[idx].client_to_server_q, &msg, sizeof(struct message_text), 0, 0);
        len = (len - offsetof(struct message_text, buf));

        if (len < 0) {
            FATAL("Internal error: failed to dispatch read request to the client");
        }

        if (len > (num_bytes - sofar)) {
            if ((num_bytes - sofar) > 0) {
                memcpy(buffer+sofar, msg.message_text.buf, (num_bytes - sofar));
            }
        } else {
            memcpy(buffer + sofar, msg.message_text.buf, len);
        }

        INFO("Printing what i got so far: %s", msg.message_text.buf);

        sofar = sofar + len;
    }

    /* Update the seek count */
    skip(msg_handle, invec_idx, num_bytes);
    INFO("Finished psa_read");
    return sofar;
}

void psa_write(psa_handle_t msg_handle, uint32_t outvec_idx,
               const void *buffer, size_t num_bytes)
{
    size_t sofar = 0;
    struct message msg = { 0 };
    int idx;
    ssize_t len;

    is_valid_msg_handle(msg_handle);
    is_call_msg(msg_handle);

    if (outvec_idx >= PSA_MAX_IOVEC) {
        ERROR("Invalid iovec number");
    }

    if (num_bytes > messages[msg_handle].out_size[outvec_idx]) {
        ERROR("Program tried to write too much data %lu/%lu", num_bytes,
              messages[msg_handle].out_size[outvec_idx]);
    }

    while (sofar < num_bytes) {
        size_t sending = (num_bytes - sofar);
        if (sending > (MAX_FRAGMENT_SIZE - (sizeof(size_t) * 2))) {
            sending = MAX_FRAGMENT_SIZE - (sizeof(size_t) * 2);
        }

        INFO("Server: sending %lu bytes to client, sofar = %lu", sending, (long) sofar);

        send_msg(msg_handle, WRITE_REQUEST, outvec_idx, sending, buffer + sofar, sending);

        idx = find_connection(message_client[msg_handle]);
        assert(idx >= 0);

        len = msgrcv(connections[idx].client_to_server_q, &msg, sizeof(struct message_text), 0, 0);
        if (len < 1) {
            FATAL("Client didn't give me a full response");
        }
        sofar = sofar + sending;
    }

    /* Update the seek count */
    messages[msg_handle].out_size[outvec_idx] -= num_bytes;
}

size_t psa_skip(psa_handle_t msg_handle, uint32_t invec_idx, size_t num_bytes)
{
    is_valid_msg_handle(msg_handle);
    is_call_msg(msg_handle);

    size_t ret = skip(msg_handle, invec_idx, num_bytes);

    /* notify client to skip */
    send_msg(msg_handle, SKIP_REQUEST, invec_idx, num_bytes, NULL, 0);
    return ret;
}

static void destroy_temporary_queue(int myqid)
{
    if (msgctl(myqid, IPC_RMID, NULL) != 0) {
        INFO("ERROR: Failed to delete msg queue %d", myqid);
    }
}

static int make_temporary_queue()
{
    int myqid;
    if ((myqid = msgget(IPC_PRIVATE, 0660)) == -1) {
        INFO("msgget: myqid");
        return -1;
    }
    return myqid;
}

/**
 * Assumes msg_handle is the index into the message array
 */
void psa_reply(psa_handle_t msg_handle, psa_status_t status)
{
    int idx, q;
    is_valid_msg_handle(msg_handle);

    if (pending_message[msg_handle] != 1) {
        ERROR("Not a valid message handle");
    }

    if (messages[msg_handle].type == PSA_IPC_CONNECT) {
        switch (status) {
            case PSA_SUCCESS:
                idx = find_connection(message_client[msg_handle]);
                q = make_temporary_queue();
                if (q > 0 && idx >= 0) {
                    connections[idx].client_to_server_q = q;
                    status = q;
                } else {
                    FATAL("What happened?");
                }
                break;
            case PSA_ERROR_CONNECTION_REFUSED:
                destroy_connection(message_client[msg_handle]);
                break;
            case PSA_ERROR_CONNECTION_BUSY:
                destroy_connection(message_client[msg_handle]);
                break;
            case PSA_ERROR_PROGRAMMER_ERROR:
                destroy_connection(message_client[msg_handle]);
                break;
            default:
                ERROR("Not a valid reply %d", status);
        }
    } else if (messages[msg_handle].type == PSA_IPC_DISCONNECT) {
        idx = find_connection(message_client[msg_handle]);
        if (idx >= 0) {
            destroy_temporary_queue(connections[idx].client_to_server_q);
        }
        destroy_connection(message_client[msg_handle]);
    }

    send_msg(msg_handle, PSA_REPLY, status, 0, NULL, 0);

    pending_message[msg_handle] = 0;
    message_client[msg_handle] = 0;
}

/* TODO: make sure you only clear interrupt signals, and not others */
void psa_eoi(psa_signal_t signal)
{
    int index = signal_to_index(signal);
    if (index >= 0 && (rot_svc_incoming_queue[index] >= 0)) {
        clear_signal(signal);
    } else {
        ERROR("Tried to EOI a signal that isn't an interrupt");
    }
}

void psa_notify(int32_t partition_id)
{
    char pathname[PATHNAMESIZE] = { 0 };

    if (partition_id < 0) {
        ERROR("Not a valid secure partition");
    }

    snprintf(pathname, PATHNAMESIZE, "/tmp/psa_notify_%u", partition_id);
    INFO("psa_notify: notifying partition %u using %s",
         partition_id, pathname);
    INFO("psa_notify is unimplemented");
}

void psa_clear(void)
{
    clear_signal(PSA_DOORBELL);
}

void __init_psasim(const char **array,
                   int size,
                   const int allow_ns_clients_array[32],
                   const uint32_t versions[32],
                   const int strict_policy_array[32])
{
    static uint8_t library_initialised = 0;
    key_t key;
    int qid;
    FILE *fp;
    char doorbell_file[PATHNAMESIZE] = { 0 };
    char queue_path[PATHNAMESIZE];
    snprintf(doorbell_file, PATHNAMESIZE, "psa_notify_%u", getpid());

    if (library_initialised > 0) {
        return;
    } else {
        library_initialised = 1;
    }

    if (size != 32) {
        FATAL("Unsupported value. Aborting.");
    }

    array[3] = doorbell_file;

    for (int i = 0; i < 32; i++) {
        if (strncmp(array[i], "", 1) != 0) {
            INFO("Setting up %s", array[i]);
            memset(queue_path, 0, sizeof(queue_path));
            snprintf(queue_path, sizeof(queue_path), "%s%s", TMP_FILE_BASE_PATH, array[i]);

            /* Create file if doesn't exist */
            fp = fopen(queue_path, "ab+");
            if (fp) {
                fclose(fp);
            }

            if ((key = ftok(queue_path, PROJECT_ID)) == -1) {
                FATAL("Error finding message queue during initialisation");
            }

            /* TODO: Investigate. Permissions are likely to be too relaxed */
            if ((qid = msgget(key, IPC_CREAT | 0660)) == -1) {
                FATAL("Error opening message queue during initialisation");
            } else {
                rot_svc_incoming_queue[i] = qid;
            }
        }
    }

    memcpy(nsacl, allow_ns_clients_array, sizeof(int) * 32);
    memcpy(strict_policy, strict_policy_array, sizeof(int) * 32);
    memcpy(rot_svc_versions, versions, sizeof(uint32_t) * 32);
    memset(&connections, 0, sizeof(struct connection) * MAX_CLIENTS);

    __psa_ff_client_security_state = 0; /* Set the client status to SECURE */
}
