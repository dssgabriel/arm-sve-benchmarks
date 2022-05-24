#include "kernels.h"

void ref_init(double *restrict a, double b, size_t n)
{
   for (size_t i = 0; i < n; ++i) {
      a[i] = b;
   }
}

void ref_copy(double *restrict a, double *restrict b, size_t n)
{
   for (size_t i = 0; i < n; ++i) {
      a[i] = b[i];
   }
}

void ref_reduc(double *restrict a, double *b, size_t n)
{
   double acc = 0.0;
   for (size_t i = 0; i < n; ++i) {
      acc += a[i];
   }
   *b = acc;
}

void ref_dotprod(double *restrict a, double *restrict b, double *c, size_t n)
{
   double acc = 0.0;
   for (size_t i = 0; i < n; ++i) {
      acc += (a[i] * b[i]);
   }
   *c = acc;
}

void ref_daxpy(double *restrict a, double *restrict b, double *restrict c,
               size_t n)
{
   for (size_t i = 0; i < n; ++i) {
      c[i] += (a[i] * b[i]);
   }
}

void ref_vec_sum(double *restrict a, double *restrict b, double *restrict c,
                 size_t n)
{
   for (size_t i = 0; i < n; ++i) {
      c[i] = (a[i] + b[i]);
   }
}

void ref_vec_scale(double *restrict a, double *restrict b, double *restrict c,
                   size_t n)
{
   for (size_t i = 0; i < n; ++i) {
      c[i] = (a[i] * b[i]);
   }
}
