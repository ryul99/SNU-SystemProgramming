long int fibonacci(long int n)
{
  long int f0 = 0;
  long int f1 = 1;

  if (n <= 0) return f0;
  if (n == 1) return f1;

  long int fn = f1 + f0;
  n = n-2;

  while (n > 0) {
    f0 = f1; f1 = fn;
    fn = f1 + f0;
    n--;
  }

  return fn;
}

