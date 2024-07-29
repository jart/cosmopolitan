#include <cerrno>
#include <system_error>

bool test_errc_mapping(int posix_error, std::errc expected_errc) {
  std::error_code ec(posix_error, std::generic_category());
  return ec.value() == posix_error &&  //
         std::error_condition(expected_errc) == ec;
}

int main() {
  if (!test_errc_mapping(EACCES, std::errc::permission_denied))
    return 1;
  if (!test_errc_mapping(ENOENT, std::errc::no_such_file_or_directory))
    return 2;
  if (!test_errc_mapping(EEXIST, std::errc::file_exists))
    return 3;
  if (!test_errc_mapping(EINVAL, std::errc::invalid_argument))
    return 4;
  if (!test_errc_mapping(ENOSPC, std::errc::no_space_on_device))
    return 5;
  return 0;
}
