int main(int argc, char *argv[]) {
  int rc;
  FILE *f;
  f = fopen("/dev/null", "w");
  fprintf(f, "hello world %d\n", argc);
  fclose(f);
  rc = system("exit 42");
  CHECK_NE(-1, rc);
  CHECK_EQ(42, WEXITSTATUS(rc));
  return 0;
}
