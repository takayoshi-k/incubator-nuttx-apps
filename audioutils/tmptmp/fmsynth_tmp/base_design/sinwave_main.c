#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>

// #define USE_FLOAT

#if defined(USE_FLOAT)
void print_sintable(int samples)
{
  float delta = M_PI / 2. / (samples);

  printf("static const float f_sintbl[] = {\n");
  printf("  %1.20e,\n", sin(-delta));

  for (int i=0; i < samples; i += 2)
    {
      printf("  %1.20e, %1.20e,\n",
            sin(delta * i), sin(delta * (i + 1)));
    }

  printf("  %1.20e,\n", 1.);
  printf("};\n");
}
#else
void print_sintable(int samples)
{
  float delta = M_PI / 2. / (samples);

  printf("static const short s_sintbl[] = {\n");
  printf("  0x%04hx,\n", (short)(SHRT_MAX * sin(-delta)));

  for (int i=0; i < samples; i += 4)
    {
      printf("  0x%04hx, 0x%04hx, 0x%04hx, 0x%04hx,\n",
            (short)(SHRT_MAX * sin(delta * (i + 0))), (short)(SHRT_MAX * sin(delta * (i + 1))),
            (short)(SHRT_MAX * sin(delta * (i + 2))), (short)(SHRT_MAX * sin(delta * (i + 3))));
    }

  printf("  0x%04hx,\n", SHRT_MAX);
  printf("};\n");
}
#endif

int main(int argc, char **argv)
{
  double delta = 1.2e-1;
  int samples = 256;

  if (argc == 2)
    {
      samples = atoi(argv[1]);
    }

  if (samples <= 0)
    {
      fprintf(stderr, "$ %s ([sample num])\n", argv[0]);
      return -1;
    }

  print_sintable(samples);

  return 0;
}
