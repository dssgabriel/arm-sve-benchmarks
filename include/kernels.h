#pragma once

#include <stddef.h>

/**
 * Compiler-generated kernels.
 **/
void compiler_init(const double k, double *restrict x, const size_t len);

void compiler_copy(double *restrict x, const double *restrict y,
                   const size_t len);

void compiler_reduc(const double *restrict x, double *r, const size_t len);

void compiler_dotprod(const double *restrict x, const double *restrict y,
                      double *d, const size_t len);

void compiler_gaxpy(const double a, const double *restrict x,
                    double *restrict y, const size_t len);

void compiler_vec_sum(double *restrict x, const double *restrict y,
                      const size_t len);

void compiler_vec_scale(const double k, double *restrict x, const size_t len);

/**
 * Hand-written assembly kernels.
 **/
void assembly_init(const double k, double *restrict x, const size_t len);

void assembly_copy(double *restrict x, const double *restrict y,
                   const size_t len);

void assembly_reduc(const double *restrict x, double *r, const size_t len);

void assembly_dotprod(const double *restrict x, const double *restrict y,
                      double *d, const size_t len);

void assembly_gaxpy(const double a, const double *restrict x,
                    double *restrict y, const size_t len);

void assembly_vec_sum(double *restrict x, const double *restrict y,
                      const size_t len);

void assembly_vec_scale(const double k, double *restrict x, const size_t len);
