/* PSA firmware framework client API */

/*
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
 */

#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <inttypes.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include "client.h"
#include "common.h"
#include "error_ext.h"
#include "util.h"

typedef struct internal_handle {
    int server_qid;
    int client_qid;
    int internal_server_qid;
    int valid;
} internal_handle_t;

typedef struct vectors {
    const psa_invec *in_vec;
    size_t in_len;
    psa_outvec *out_vec;
    size_t out_len;
} vectors_t;

/* Note that this implementation is functional and not secure */
int __psa_ff_client_security_state = NON_SECURE;

/* Access to this global is not thread safe */
#define MAX_HANDLES 32
static internal_handle_t handles[MAX_HANDLES] = { { 0 } };

static int get_next_free_handle()
{
    /* Never return handle 0 as it's a special null handle */
    for (int i = 1; i < MAX_HANDLES; i++) {
        if (handles[i].valid == 0) {
            return i;
        }
    }
    return -1;
}

static int handle_is_valid(psa_handle_t handle)
{
    if (handle > 0 && handle < MAX_HANDLES) {
        if (handles[handle].valid == 1) {
            return 1;
        }
    }
    ERROR("ERROR: Invalid handle");
    return 0;
}

static int get_queue_info(char *path, int *cqid, int *sqid)
{
    key_t server_queue_key;
    int rx_qid, server_qid;

    INFO("Attempting to contact a RoT service queue");

    if ((rx_qid = msgget(IPC_PRIVATE, 0660)) == -1) {
        ERROR("msgget: rx_qid");
        return -1;
    }

    if ((server_queue_key = ftok(path, PROJECT_ID)) == -1) {
        ERROR("ftok");
        return -2;
    }

    if ((server_qid = msgget(server_queue_key, 0)) == -1) {
        ERROR("msgget: server_qid");
        return -3;
    }

    *cqid = rx_qid;
    *sqid = server_qid;

    return 0;
}

static psa_status_t process_response(int rx_qid, vectors_t *vecs, int type,
                                     int *internal_server_qid)
{
    struct message response, request;
    psa_status_t ret = PSA_ERROR_CONNECTION_REFUSED;
    size_t invec_seek[4] = { 0 };
    size_t data_size;
    psa_status_t invec, outvec; /* TODO: Should these be size_t ? */

    assert(internal_server_qid > 0);

    while (1) {
        data_size = 0;
        invec = 0;
        outvec = 0;

        /* read response from server */
        if (msgrcv(rx_qid, &response, sizeof(struct message_text), 0, 0) == -1) {
            ERROR("   msgrcv failed");
            return ret;
        }

        /* process return message from server */
        switch (response.message_type) {
            case PSA_REPLY:
                memcpy(&ret, response.message_text.buf, sizeof(psa_status_t));
                INFO("   Message received from server: %d", ret);
                if (type == PSA_IPC_CONNECT && ret > 0) {
                    *internal_server_qid = ret;
                    INFO("   ASSSIGNED q ID %d", *internal_server_qid);
                    ret = PSA_SUCCESS;
                }
                return ret;
                break;
            case READ_REQUEST:
                /* read data request */
                request.message_type = READ_RESPONSE;

                assert(vecs != 0);

                memcpy(&invec, response.message_text.buf, sizeof(psa_status_t));
                memcpy(&data_size, response.message_text.buf+sizeof(size_t), sizeof(size_t));
                INFO("   Partition asked for %lu bytes from invec %d", data_size, invec);

                /* need to add more checks here */
                assert(invec >= 0 && invec < PSA_MAX_IOVEC);

                if (data_size > MAX_FRAGMENT_SIZE) {
                    data_size = MAX_FRAGMENT_SIZE;
                }

                /* send response */
                INFO("   invec_seek[invec] is %lu", invec_seek[invec]);
                INFO("   Reading from offset %p", vecs->in_vec[invec].base + invec_seek[invec]);
                memcpy(request.message_text.buf,
                       (vecs->in_vec[invec].base + invec_seek[invec]),
                       data_size);

                /* update invec base TODO: check me */
                invec_seek[invec] = invec_seek[invec] + data_size;

                INFO("   Sending message of type %li", request.message_type);
                INFO("       with content %s", request.message_text.buf);

                if (msgsnd(*internal_server_qid, &request,
                           sizeof(int) + sizeof(uint32_t) + data_size, 0) == -1) {
                    ERROR("Internal error: failed to respond to read request");
                }
                break;
            case WRITE_REQUEST:
                assert(vecs != 0);

                request.message_type = WRITE_RESPONSE;

                memcpy(&outvec, response.message_text.buf, sizeof(psa_status_t));
                memcpy(&data_size, response.message_text.buf + sizeof(size_t), sizeof(size_t));
                INFO("   Partition wants to write %lu bytes to outvec %d", data_size, outvec);

                assert(outvec >= 0 && outvec < PSA_MAX_IOVEC);

                /* copy memory into message and send back amount written */
                size_t sofar = vecs->out_vec[outvec].len;
                memcpy(vecs->out_vec[outvec].base + sofar,
                       response.message_text.buf+(sizeof(size_t)*2), data_size);
                INFO("   Data size is %lu", data_size);
                vecs->out_vec[outvec].len += data_size;

                INFO("   Sending message of type %li", request.message_type);

                /* send response */
                if (msgsnd(*internal_server_qid, &request, sizeof(int) + data_size, 0) == -1) {
                    ERROR("Internal error: failed to respond to write request");
                }
                break;
            case SKIP_REQUEST:
                memcpy(&invec, response.message_text.buf, sizeof(psa_status_t));
                memcpy(&data_size, response.message_text.buf+sizeof(size_t), sizeof(size_t));
                INFO("   Partition asked to skip %lu bytes in invec %d", data_size, invec);
                assert(invec >= 0 && invec < PSA_MAX_IOVEC);
                /* update invec base TODO: check me */
                invec_seek[invec] = invec_seek[invec] + data_size;
                break;

            default:
                FATAL("   ERROR: unknown internal message type: %ld",
                      response.message_type);
        }
    }
}

static psa_status_t send(int rx_qid, int server_qid, int *internal_server_qid,
                         int32_t type, uint32_t minor_version, vectors_t *vecs)
{
    psa_status_t ret = PSA_ERROR_CONNECTION_REFUSED;
    size_t request_msg_size = (sizeof(int) + sizeof(long)); /* msg type plus queue id */
    struct message request;
    request.message_type = 1; /* TODO: change this */
    request.message_text.psa_type = type;
    vector_sizes_t vec_sizes;

    /* If the client is non-secure then set the NS bit */
    if (__psa_ff_client_security_state != 0) {
        request.message_type |= NON_SECURE;
    }

    assert(request.message_type >= 0);

    INFO("SEND: Sending message of type %ld with psa_type %d", request.message_type, type);
    INFO("     internal_server_qid = %i", *internal_server_qid);

    request.message_text.qid = rx_qid;

    if (type == PSA_IPC_CONNECT) {
        memcpy(request.message_text.buf, &minor_version, sizeof(minor_version));
        request_msg_size = request_msg_size + sizeof(minor_version);
        INFO("   Request msg size is %lu", request_msg_size);
    } else {
        assert(internal_server_qid > 0);
    }

    if (vecs != NULL && type >= PSA_IPC_CALL) {

        memset(&vec_sizes, 0, sizeof(vec_sizes));

        /* Copy invec sizes */
        for (size_t i = 0; i < (vecs->in_len); i++) {
            vec_sizes.invec_sizes[i] = vecs->in_vec[i].len;
            INFO("   Client sending vector %lu: %lu", i, vec_sizes.invec_sizes[i]);
        }

        /* Copy outvec sizes */
        for (size_t i = 0; i < (vecs->out_len); i++) {
            vec_sizes.outvec_sizes[i] = vecs->out_vec[i].len;

            /* Reset to 0 since we need to eventually fill in with bytes written */
            vecs->out_vec[i].len = 0;
        }

        memcpy(request.message_text.buf, &vec_sizes, sizeof(vec_sizes));
        request_msg_size = request_msg_size + sizeof(vec_sizes);
    }

    INFO("   Sending and then waiting");

    /* send message to server */
    if (msgsnd(server_qid, &request, request_msg_size, 0) == -1) {
        ERROR("   msgsnd failed");
        return ret;
    }

    return process_response(rx_qid, vecs, type, internal_server_qid);
}


uint32_t psa_framework_version(void)
{
    return PSA_FRAMEWORK_VERSION;
}

psa_handle_t psa_connect(uint32_t sid, uint32_t minor_version)
{
    int idx;
    psa_status_t ret;
    char pathname[PATHNAMESIZE] = { 0 };

    idx = get_next_free_handle();

    /* if there's a free handle available */
    if (idx >= 0) {
        snprintf(pathname, PATHNAMESIZE - 1, TMP_FILE_BASE_PATH "psa_service_%u", sid);
        INFO("Attempting to contact RoT service at %s", pathname);

        /* if communication is possible */
        if (get_queue_info(pathname, &handles[idx].client_qid, &handles[idx].server_qid) >= 0) {

            ret = send(handles[idx].client_qid,
                       handles[idx].server_qid,
                       &handles[idx].internal_server_qid,
                       PSA_IPC_CONNECT,
                       minor_version,
                       NULL);

            /* if connection accepted by RoT service */
            if (ret >= 0) {
                handles[idx].valid = 1;
                return idx;
            } else {
                ERROR("Server didn't like you");
            }
        } else {
            ERROR("Couldn't contact RoT service. Does it exist?");

            if (__psa_ff_client_security_state == 0) {
                ERROR("Invalid SID");
            }
        }
    }

    INFO("Couldn't obtain a free handle");
    return PSA_ERROR_CONNECTION_REFUSED;
}

uint32_t psa_version(uint32_t sid)
{
    int idx;
    psa_status_t ret;
    char pathname[PATHNAMESIZE] = { 0 };

    idx = get_next_free_handle();

    if (idx >= 0) {
        snprintf(pathname, PATHNAMESIZE, TMP_FILE_BASE_PATH "psa_service_%u", sid);
        if (get_queue_info(pathname, &handles[idx].client_qid, &handles[idx].server_qid) >= 0) {
            ret = send(handles[idx].client_qid,
                       handles[idx].server_qid,
                       &handles[idx].internal_server_qid,
                       VERSION_REQUEST,
                       0,
                       NULL);
            INFO("psa_version: Recieved from server %d", ret);
            if (ret > 0) {
                return ret;
            }
        }
    }
    ERROR("psa_version failed: does the service exist?");
    return PSA_VERSION_NONE;
}

psa_status_t psa_call(psa_handle_t handle,
                      int32_t type,
                      const psa_invec *in_vec,
                      size_t in_len,
                      psa_outvec *out_vec,
                      size_t out_len)
{
    handle_is_valid(handle);

    if ((in_len + out_len) > PSA_MAX_IOVEC) {
        ERROR("Too many iovecs: %lu + %lu", in_len, out_len);
    }

    vectors_t vecs = { 0 };
    vecs.in_vec = in_vec;
    vecs.in_len = in_len;
    vecs.out_vec = out_vec;
    vecs.out_len = out_len;

    return send(handles[handle].client_qid,
                handles[handle].server_qid,
                &handles[handle].internal_server_qid,
                type,
                0,
                &vecs);
}

void psa_close(psa_handle_t handle)
{
    handle_is_valid(handle);
    if (send(handles[handle].client_qid, handles[handle].server_qid,
             &handles[handle].internal_server_qid, PSA_IPC_DISCONNECT, 0, NULL)) {
        ERROR("ERROR: Couldn't send disconnect msg");
    } else {
        if (msgctl(handles[handle].client_qid, IPC_RMID, NULL) != 0) {
            ERROR("ERROR: Failed to delete msg queue");
        }
    }
    INFO("Closing handle %u", handle);
    handles[handle].valid = 0;
}
