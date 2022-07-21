int main(int argc, char *argv[]) {
  int rc;
  char *s;
  FILE *f;
  ShowCrashReports();
  s = strdup(argv[0]);
  s[0] = 'Z';
  f = fopen("/dev/null", "w");
  fputs(_gc(xiso8601ts(NULL)), f);
  fputs(_gc(xasprintf("hello world %d %s\n", argc, s)), f);
  fclose(f);
  free(s);
  return 0;
}
