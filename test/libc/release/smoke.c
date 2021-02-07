int main(int argc, char *argv[]) {
  int rc;
  char *s;
  FILE *f;
  s = strdup(argv[0]);
  f = fopen("/dev/null", "w");
  fprintf(f, "hello world %d %s\n", argc, s);
  fclose(f);
  rc = system("exit 42");
  CHECK_NE(-1, rc);
  CHECK_EQ(42, WEXITSTATUS(rc));
  free(s);
  return 0;
}
