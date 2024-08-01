#include <__system_error/errc.h>

_LIBCPP_BEGIN_NAMESPACE_STD

static std::errc __err_to_errc_impl(int err) noexcept {
  if (err == EAFNOSUPPORT) return errc::address_family_not_supported;
  if (err == EADDRINUSE) return errc::address_in_use;
  if (err == EADDRNOTAVAIL) return errc::address_not_available;
  if (err == EISCONN) return errc::already_connected;
  if (err == E2BIG) return errc::argument_list_too_long;
  if (err == EDOM) return errc::argument_out_of_domain;
  if (err == EFAULT) return errc::bad_address;
  if (err == EBADF) return errc::bad_file_descriptor;
  if (err == EBADMSG) return errc::bad_message;
  if (err == EPIPE) return errc::broken_pipe;
  if (err == ECONNABORTED) return errc::connection_aborted;
  if (err == EALREADY) return errc::connection_already_in_progress;
  if (err == ECONNREFUSED) return errc::connection_refused;
  if (err == ECONNRESET) return errc::connection_reset;
  if (err == EXDEV) return errc::cross_device_link;
  if (err == EDESTADDRREQ) return errc::destination_address_required;
  if (err == EBUSY) return errc::device_or_resource_busy;
  if (err == ENOTEMPTY) return errc::directory_not_empty;
  if (err == ENOEXEC) return errc::executable_format_error;
  if (err == EEXIST) return errc::file_exists;
  if (err == EFBIG) return errc::file_too_large;
  if (err == ENAMETOOLONG) return errc::filename_too_long;
  if (err == ENOSYS) return errc::function_not_supported;
  if (err == EHOSTUNREACH) return errc::host_unreachable;
  if (err == EIDRM) return errc::identifier_removed;
  if (err == EILSEQ) return errc::illegal_byte_sequence;
  if (err == ENOTTY) return errc::inappropriate_io_control_operation;
  if (err == EINTR) return errc::interrupted;
  if (err == EINVAL) return errc::invalid_argument;
  if (err == ESPIPE) return errc::invalid_seek;
  if (err == EIO) return errc::io_error;
  if (err == EISDIR) return errc::is_a_directory;
  if (err == EMSGSIZE) return errc::message_size;
  if (err == ENETDOWN) return errc::network_down;
  if (err == ENETRESET) return errc::network_reset;
  if (err == ENETUNREACH) return errc::network_unreachable;
  if (err == ENOBUFS) return errc::no_buffer_space;
  if (err == ECHILD) return errc::no_child_process;
  if (err == ENOLINK) return errc::no_link;
  if (err == ENOLCK) return errc::no_lock_available;
  if (err == ENOMSG) return errc::no_message;
  if (err == (ENODATA ? ENODATA : ENOMSG)) return errc::no_message_available;
  if (err == ENOPROTOOPT) return errc::no_protocol_option;
  if (err == ENOSPC) return errc::no_space_on_device;
  if (err == ENOMEM) return errc::not_enough_memory;
  if (err == (ENOSR ? ENOSR : ENOMEM)) return errc::no_stream_resources;
  if (err == ENXIO) return errc::no_such_device_or_address;
  if (err == ENODEV) return errc::no_such_device;
  if (err == ENOENT) return errc::no_such_file_or_directory;
  if (err == ESRCH) return errc::no_such_process;
  if (err == ENOTDIR) return errc::not_a_directory;
  if (err == ENOTSOCK) return errc::not_a_socket;
  if (err == (ENOSTR ? ENOSTR : EINVAL)) return errc::not_a_stream;
  if (err == ENOTCONN) return errc::not_connected;
  if (err == ENOTSUP) return errc::not_supported;
  if (err == ECANCELED) return errc::operation_canceled;
  if (err == EINPROGRESS) return errc::operation_in_progress;
  if (err == EPERM) return errc::operation_not_permitted;
  if (err == EOPNOTSUPP) return errc::operation_not_supported;
  if (err == EWOULDBLOCK) return errc::operation_would_block;
  if (err == EOWNERDEAD) return errc::owner_dead;
  if (err == EACCES) return errc::permission_denied;
  if (err == EPROTO) return errc::protocol_error;
  if (err == EPROTONOSUPPORT) return errc::protocol_not_supported;
  if (err == EROFS) return errc::read_only_file_system;
  if (err == EDEADLK) return errc::resource_deadlock_would_occur;
  if (err == EAGAIN) return errc::resource_unavailable_try_again;
  if (err == ERANGE) return errc::result_out_of_range;
  if (err == ENOTRECOVERABLE) return errc::state_not_recoverable;
  if (err == ETIME) return errc::stream_timeout;
  if (err == ETXTBSY) return errc::text_file_busy;
  if (err == ETIMEDOUT) return errc::timed_out;
  if (err == ENFILE) return errc::too_many_files_open_in_system;
  if (err == EMFILE) return errc::too_many_files_open;
  if (err == EMLINK) return errc::too_many_links;
  if (err == ELOOP) return errc::too_many_symbolic_link_levels;
  if (err == EOVERFLOW) return errc::value_too_large;
  if (err == EPROTOTYPE) return errc::wrong_protocol_type;
  return errc::not_supported;
}

static int __errc_to_err_impl(std::errc err) noexcept {
  if (err == errc::address_family_not_supported) return EAFNOSUPPORT;
  if (err == errc::address_in_use) return EADDRINUSE;
  if (err == errc::address_not_available) return EADDRNOTAVAIL;
  if (err == errc::already_connected) return EISCONN;
  if (err == errc::argument_list_too_long) return E2BIG;
  if (err == errc::argument_out_of_domain) return EDOM;
  if (err == errc::bad_address) return EFAULT;
  if (err == errc::bad_file_descriptor) return EBADF;
  if (err == errc::bad_message) return EBADMSG;
  if (err == errc::broken_pipe) return EPIPE;
  if (err == errc::connection_aborted) return ECONNABORTED;
  if (err == errc::connection_already_in_progress) return EALREADY;
  if (err == errc::connection_refused) return ECONNREFUSED;
  if (err == errc::connection_reset) return ECONNRESET;
  if (err == errc::cross_device_link) return EXDEV;
  if (err == errc::destination_address_required) return EDESTADDRREQ;
  if (err == errc::device_or_resource_busy) return EBUSY;
  if (err == errc::directory_not_empty) return ENOTEMPTY;
  if (err == errc::executable_format_error) return ENOEXEC;
  if (err == errc::file_exists) return EEXIST;
  if (err == errc::file_too_large) return EFBIG;
  if (err == errc::filename_too_long) return ENAMETOOLONG;
  if (err == errc::function_not_supported) return ENOSYS;
  if (err == errc::host_unreachable) return EHOSTUNREACH;
  if (err == errc::identifier_removed) return EIDRM;
  if (err == errc::illegal_byte_sequence) return EILSEQ;
  if (err == errc::inappropriate_io_control_operation) return ENOTTY;
  if (err == errc::interrupted) return EINTR;
  if (err == errc::invalid_argument) return EINVAL;
  if (err == errc::invalid_seek) return ESPIPE;
  if (err == errc::io_error) return EIO;
  if (err == errc::is_a_directory) return EISDIR;
  if (err == errc::message_size) return EMSGSIZE;
  if (err == errc::network_down) return ENETDOWN;
  if (err == errc::network_reset) return ENETRESET;
  if (err == errc::network_unreachable) return ENETUNREACH;
  if (err == errc::no_buffer_space) return ENOBUFS;
  if (err == errc::no_child_process) return ECHILD;
  if (err == errc::no_link) return ENOLINK;
  if (err == errc::no_lock_available) return ENOLCK;
  if (err == errc::no_message) return ENOMSG;
  if (err == errc::no_message_available) return (ENODATA ? ENODATA : ENOMSG);
  if (err == errc::no_protocol_option) return ENOPROTOOPT;
  if (err == errc::no_space_on_device) return ENOSPC;
  if (err == errc::not_enough_memory) return ENOMEM;
  if (err == errc::no_stream_resources) return (ENOSR ? ENOSR : ENOMEM);
  if (err == errc::no_such_device_or_address) return ENXIO;
  if (err == errc::no_such_device) return ENODEV;
  if (err == errc::no_such_file_or_directory) return ENOENT;
  if (err == errc::no_such_process) return ESRCH;
  if (err == errc::not_a_directory) return ENOTDIR;
  if (err == errc::not_a_socket) return ENOTSOCK;
  if (err == errc::not_a_stream) return (ENOSTR ? ENOSTR : EINVAL);
  if (err == errc::not_connected) return ENOTCONN;
  if (err == errc::not_supported) return ENOTSUP;
  if (err == errc::operation_canceled) return ECANCELED;
  if (err == errc::operation_in_progress) return EINPROGRESS;
  if (err == errc::operation_not_permitted) return EPERM;
  if (err == errc::operation_not_supported) return EOPNOTSUPP;
  if (err == errc::operation_would_block) return EWOULDBLOCK;
  if (err == errc::owner_dead) return EOWNERDEAD;
  if (err == errc::permission_denied) return EACCES;
  if (err == errc::protocol_error) return EPROTO;
  if (err == errc::protocol_not_supported) return EPROTONOSUPPORT;
  if (err == errc::read_only_file_system) return EROFS;
  if (err == errc::resource_deadlock_would_occur) return EDEADLK;
  if (err == errc::resource_unavailable_try_again) return EAGAIN;
  if (err == errc::result_out_of_range) return ERANGE;
  if (err == errc::state_not_recoverable) return ENOTRECOVERABLE;
  if (err == errc::stream_timeout) return ETIME;
  if (err == errc::text_file_busy) return ETXTBSY;
  if (err == errc::timed_out) return ETIMEDOUT;
  if (err == errc::too_many_files_open_in_system) return ENFILE;
  if (err == errc::too_many_files_open) return EMFILE;
  if (err == errc::too_many_links) return EMLINK;
  if (err == errc::too_many_symbolic_link_levels) return ELOOP;
  if (err == errc::value_too_large) return EOVERFLOW;
  if (err == errc::wrong_protocol_type) return EPROTOTYPE;
  return ENOTSUP;
}

std::errc __err_to_errc(int err) noexcept {
  if (!err)
    return (std::errc)0;
  if (err >= 65536)
    return (std::errc)err;
  return __err_to_errc_impl(err);
}

int __errc_to_err(std::errc err) noexcept {
  if (!(int)err)
    return 0;
  if ((int)err < 65536)
    return (int)err;
  return __errc_to_err_impl(err);
}

_LIBCPP_END_NAMESPACE_STD
