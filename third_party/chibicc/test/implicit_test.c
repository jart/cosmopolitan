add(x, y) {
  return x + y;
}

implicit_functions_are_long() {
  if (add(0xffffffff, 0xffffffff) != 0x0001fffffffe) {
    __builtin_trap();
  }
}

external_functions_are_long() {
  if (_bsrl(0x0001000000000000) != 48) {
    __builtin_trap();
  }
}

auto_variables_are_long() {
  auto x, y, z;
  x = 0x0101fffffffe;
  y = 0x0201fffffffe;
  z = x + y;
  if (z != 0x0303fffffffc) {
    __builtin_trap();
  }
}

static_variables_are_long() {
  static x, y, z;
  x = 0x0101fffffffe;
  y = 0x0201fffffffe;
  z = x + y;
  if (z != 0x0303fffffffc) {
    __builtin_trap();
  }
}

x_ = 0x0101fffffffe;
y_ = 0x0201fffffffe;
z_;

globo_variables_are_long() {
  z_ = x_ + y_;
  if (z_ != 0x0303fffffffc) {
    __builtin_trap();
  }
}

implicit_function_string() {
  auto lol = strdup("hello");
  if (strlen(lol) != 5) {
    __builtin_trap();
  }
  free(lol);
}

main() {
  ShowCrashReports();
  implicit_functions_are_long();
  external_functions_are_long();
  auto_variables_are_long();
  static_variables_are_long();
  globo_variables_are_long();
  implicit_function_string();
}
