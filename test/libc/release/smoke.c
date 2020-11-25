int main() {
  FILE *f;
  f = fopen("/dev/null", "w");
  fprintf(f, "hello world\n");
  fclose(f);
  return 0;
}
