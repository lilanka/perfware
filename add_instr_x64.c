__declspec(noinline) int add(int A, int B) {
  return A + B;
}

#pragma optimize("", off)
int main(int argc, char **args) {
  return add(1234, 5678);
}
