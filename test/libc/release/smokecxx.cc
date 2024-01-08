int main() {
  int *x = new int[32];
  x[0] = 2;
  x[1] = 2;
  x[2] = x[0] + x[1];
  return x[2] - 4;
}
