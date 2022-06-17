#include "kernels.h"

void compiler_init(const double k, double *restrict x, const size_t len)
{
   for (size_t i = 0; i < len; ++i) {
      x[i] = k;
   }
}

void compiler_copy(double *restrict x, const double *restrict y,
                   const size_t len)
{
   for (size_t i = 0; i < len; ++i) {
      x[i] = y[i];
   }
}

void compiler_reduc(const double *restrict x, double *r, const size_t len)
{
   double acc = 0.0;
   for (size_t i = 0; i < len; ++i) {
      acc += x[i];
   }
   *r = acc;
}

void compiler_dotprod(const double *restrict x, const double *restrict y,
                      double *d, const size_t len)
{
   double acc = 0.0;
   for (size_t i = 0; i < len; ++i) {
      acc += (x[i] * y[i]);
   }
   *d = acc;
}

void compiler_gaxpy(const double a, const double *restrict x,
                    double *restrict y, const size_t len)
{
   for (size_t i = 0; i < len; ++i) {
      y[i] += a * x[i];
   }
}

void compiler_vec_sum(double *restrict x, const double *restrict y,
                      const size_t len)
{
   for (size_t i = 0; i < len; ++i) {
      x[i] += y[i];
   }
}

void compiler_vec_scale(const double k, double *restrict x, const size_t len)
{
   for (size_t i = 0; i < len; ++i) {
      x[i] *= k;
   }
}
