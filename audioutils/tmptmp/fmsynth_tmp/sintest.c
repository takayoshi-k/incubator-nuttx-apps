#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>

#include "pseudo_sin256.h"

int cnt = 0;

#define SINTABLE_DELTA ((M_PI / 2.) / 256.)

#define FS  (48000)
#define TCNT  (FS * 3)

// #define HZ  (25)
#define HZ  (4186)

void actual_test(void)
{
  int t;
  float delta;
  float deltaact;
  int deltai;
  float max_diff = 0.f;
  float ref_sin;
  int sin_val;
  float norm_sin;

  delta  = M_PI * 2. * (float)HZ / (float)FS;
  deltaact = (float)PSEUDOSIN_PI * 2. * (float)HZ / (float)FS;
  // delta = FM_PI * 2 * HZ;

  printf("delta=%f, deltai=%d\n", delta, deltai);
  for (t = 0; t < TCNT; t++)
    {
      sin_val = pseudo_sin256((int)(deltaact * t));
      ref_sin = sinf(delta * t);

      norm_sin = (float)sin_val / (float)SHRT_MAX;
      printf("t=%d, pseudo_sin256(%d)=%d, norm_sin=%f, sinf(%f)=%f ", t, (int)(deltaact * t), sin_val, norm_sin, delta * t, ref_sin);
      if (fabsf(norm_sin - ref_sin) >= 0.005)
        {
          float diff = fabsf(norm_sin - ref_sin);
          printf("  ERROR : %f\n", diff);
          max_diff = max_diff < diff ? diff :  max_diff;
        }
      else
        {
          printf("\n");
        }
    }

  printf("\n\nMAX Diff = %f\n", max_diff);
}

int main(void)
{
  float theta;
  int ret;
  float result;
  float sin_ref;
  float max_diff = 0.f;

  actual_test();

  return 0;
}
