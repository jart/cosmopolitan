#include <chrono>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <random>
#include <string>
#include <thread>

#define ASSERT(condition)                                           \
  if (!(condition)) {                                               \
    fprintf(stderr, "%s:%d: test failed: %s\n", __FILE__, __LINE__, \
            #condition);                                            \
    return 1;                                                       \
  }

namespace fs = std::filesystem;

fs::path g_temp_path;
fs::path g_orig_path;
std::string g_tmpdir;

void setup() {
  g_orig_path = fs::current_path();
  fs::path temp_path = fs::temp_directory_path();
  auto now = std::chrono::system_clock::now();
  auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
  auto value = now_ms.time_since_epoch();
  long duration = value.count();
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(0, 999999);
  int random_number = dis(gen);
  std::string dir_name =
      "temp_" + std::to_string(duration) + "_" + std::to_string(random_number);
  g_temp_path = temp_path / dir_name;
  fs::create_directory(g_temp_path);
  fs::current_path(g_temp_path);
  fs::create_directory("tmp");
  g_tmpdir = fs::absolute("tmp");
  setenv("TMPDIR", g_tmpdir.c_str(), true);
}

void teardown() {
  fs::current_path(g_orig_path);
  fs::remove_all(g_temp_path);
}

int test_create_directory() {
  fs::path dir = "test_dir";
  ASSERT(fs::create_directory(dir));
  ASSERT(fs::is_directory(dir));
  ASSERT(!fs::create_directory(dir));
  fs::remove(dir);
  return 0;
}

int test_create_directories() {
  fs::path dirs = "test_dir/nested/deep";
  ASSERT(fs::create_directories(dirs));
  ASSERT(fs::is_directory(dirs));
  ASSERT(!fs::create_directories(dirs));
  fs::remove_all("test_dir");
  return 0;
}

int test_remove() {
  fs::path file = "test_file.txt";
  std::ofstream(file).put('a');
  ASSERT(fs::remove(file));
  ASSERT(!fs::remove(file));
  return 0;
}

int test_remove_all() {
  fs::path dir = "test_dir/nested/deep";
  fs::create_directories(dir);
  ASSERT(fs::remove_all("test_dir") > 0);
  ASSERT(fs::remove_all("test_dir") == 0);
  return 0;
}

int test_rename() {
  fs::path old_name = "old.txt";
  fs::path new_name = "new.txt";
  std::ofstream(old_name).put('a');
  fs::rename(old_name, new_name);
  ASSERT(!fs::exists(old_name));
  ASSERT(fs::exists(new_name));
  fs::remove(new_name);
  return 0;
}

int test_copy() {
  fs::path src = "src.txt";
  fs::path dst = "dst.txt";
  std::ofstream(src) << "test";
  fs::copy(src, dst);
  ASSERT(fs::exists(dst));
  ASSERT(fs::file_size(src) == fs::file_size(dst));
  fs::remove(src);
  fs::remove(dst);
  return 0;
}

int test_copy_file() {
  fs::path src = "src.txt";
  fs::path dst = "dst.txt";
  std::ofstream(src) << "test";
  ASSERT(fs::copy_file(src, dst));
  ASSERT(!fs::copy_file(src, dst, fs::copy_options::skip_existing));
  fs::remove(src);
  fs::remove(dst);
  return 0;
}

int test_exists() {
  fs::path file = "test.txt";
  ASSERT(!fs::exists(file));
  std::ofstream(file).put('a');
  ASSERT(fs::exists(file));
  fs::remove(file);
  return 0;
}

int test_is_regular_file() {
  fs::path file = "test.txt";
  fs::path dir = "test_dir";
  std::ofstream(file).put('a');
  fs::create_directory(dir);
  ASSERT(fs::is_regular_file(file));
  ASSERT(!fs::is_regular_file(dir));
  fs::remove(file);
  fs::remove(dir);
  return 0;
}

int test_is_directory() {
  fs::path file = "test.txt";
  fs::path dir = "test_dir";
  std::ofstream(file).put('a');
  fs::create_directory(dir);
  ASSERT(!fs::is_directory(file));
  ASSERT(fs::is_directory(dir));
  fs::remove(file);
  fs::remove(dir);
  return 0;
}

int test_is_symlink() {
  fs::path file = "test.txt";
  fs::path link = "test_link";
  std::ofstream(file).put('a');
  fs::create_symlink(file, link);
  ASSERT(!fs::is_symlink(file));
  ASSERT(fs::is_symlink(link));
  fs::remove(file);
  fs::remove(link);
  return 0;
}

int test_file_size() {
  fs::path file = "test.txt";
  std::ofstream(file) << "test";
  ASSERT(fs::file_size(file) == 4);
  fs::remove(file);
  return 0;
}

int test_last_write_time() {
  fs::path file = "test.txt";
  auto now = fs::file_time_type::clock::now();
  std::ofstream(file).put('a');
  fs::last_write_time(file, now);
  ASSERT(fs::last_write_time(file) == now);
  fs::remove(file);
  return 0;
}

int test_permissions() {
  fs::path file = "test.txt";
  std::ofstream(file).put('a');
  fs::permissions(file, fs::perms::owner_read | fs::perms::owner_write);
  auto perms = fs::status(file).permissions();
  ASSERT((perms & fs::perms::owner_read) != fs::perms::none);
  ASSERT((perms & fs::perms::owner_write) != fs::perms::none);
  ASSERT((perms & fs::perms::owner_exec) == fs::perms::none);
  fs::remove(file);
  return 0;
}

int test_current_path() {
  auto original_path = fs::current_path();
  fs::path new_path = fs::temp_directory_path();
  fs::current_path(new_path);
  ASSERT(fs::current_path() == new_path);
  fs::current_path(original_path);
  return 0;
}

int test_absolute() {
  fs::path relative = "test.txt";
  auto abs_path = fs::absolute(relative);
  ASSERT(abs_path.is_absolute());
  return 0;
}

int test_canonical() {
  fs::path dir = "test_dir";
  fs::path file = dir / "test.txt";
  fs::create_directories(dir);
  std::ofstream(file).put('a');
  auto can_path = fs::canonical(file);
  ASSERT(can_path.is_absolute());
  ASSERT(!can_path.lexically_normal().string().empty());
  fs::remove_all(dir);
  return 0;
}

int test_read_symlink() {
  fs::path file = "test.txt";
  fs::path link = "test_link";
  std::ofstream(file).put('a');
  fs::create_symlink(file, link);
  ASSERT(fs::read_symlink(link) == file);
  fs::remove(file);
  fs::remove(link);
  return 0;
}

int test_create_symlink_and_hard_link() {
  fs::path file = "test.txt";
  fs::path symlink = "test_symlink";
  fs::path hardlink = "test_hardlink";
  std::ofstream(file).put('a');
  fs::create_symlink(file, symlink);
  fs::create_hard_link(file, hardlink);
  ASSERT(fs::exists(symlink));
  ASSERT(fs::exists(hardlink));
  ASSERT(fs::is_symlink(symlink));
  ASSERT(!fs::is_symlink(hardlink));
  fs::remove(file);
  fs::remove(symlink);
  fs::remove(hardlink);
  return 0;
}

int test_space() {
  auto space_info = fs::space(".");
  ASSERT(space_info.capacity > 0);
  ASSERT(space_info.free > 0);
  ASSERT(space_info.available > 0);
  return 0;
}

int test_equivalent() {
  fs::path file1 = "test1.txt";
  fs::path file2 = "test2.txt";
  std::ofstream(file1).put('a');
  fs::create_hard_link(file1, file2);
  ASSERT(fs::equivalent(file1, file2));
  fs::remove(file1);
  fs::remove(file2);
  return 0;
}

int test_resize_file() {
  fs::path file = "test.txt";
  std::ofstream(file) << "test";
  fs::resize_file(file, 10);
  ASSERT(fs::file_size(file) == 10);
  fs::remove(file);
  return 0;
}

int test_status() {
  fs::path file = "test.txt";
  std::ofstream(file).put('a');
  auto status = fs::status(file);
  ASSERT(status.type() == fs::file_type::regular);
  fs::remove(file);
  return 0;
}

int test_copy_enoent() {
  fs::path src = "non_existent_file.txt";
  fs::path dst = "destination.txt";
  try {
    fs::copy(src, dst);
    ASSERT(false);
  } catch (const fs::filesystem_error& e) {
    if (e.code() == std::errc::no_such_file_or_directory) {
      return 0;
    } else {
      ASSERT(false);
    }
  } catch (const std::exception& e) {
    ASSERT(false);
  }
}

#define RUN(func)  \
  result = func(); \
  if (result)      \
  return result

int test() {
  int result = 0;
  RUN(test_copy_enoent);
  RUN(test_create_directory);
  RUN(test_create_directories);
  RUN(test_remove);
  RUN(test_remove_all);
  RUN(test_rename);
  RUN(test_copy);
  RUN(test_copy_file);
  RUN(test_exists);
  RUN(test_is_regular_file);
  RUN(test_is_directory);
  RUN(test_is_symlink);
  RUN(test_file_size);
  RUN(test_last_write_time);
  RUN(test_permissions);
  RUN(test_current_path);
  RUN(test_absolute);
  RUN(test_canonical);
  RUN(test_read_symlink);
  RUN(test_create_symlink_and_hard_link);
  RUN(test_space);
  RUN(test_equivalent);
  RUN(test_resize_file);
  RUN(test_status);
  return 0;
}

int main() {
  int rc;
  setup();
  rc = test();
  teardown();
  return rc;
}
