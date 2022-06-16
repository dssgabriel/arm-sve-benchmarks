#pragma once

#include <stddef.h>

/**
 * Compiler-generated kernels.
 **/
void compiler_init(double *restrict x, const double k, const size_t len);

void compiler_copy(double *restrict x, const double *restrict y,
                   const size_t len);

void compiler_reduc(const double *restrict x, double *r, const size_t len);

void compiler_dotprod(const double *restrict x, const double *restrict y,
                      double *d, const size_t len);

void compiler_gaxpy(const double *restrict x, const double *restrict y,
                    double *restrict z, const double a, const size_t len);

void compiler_vec_sum(const double *restrict x, const double *restrict y,
                      double *restrict z, const size_t len);

void compiler_vec_scale(const double k, const double *restrict x,
                        double *restrict y, const size_t len);

/**
 * Hand-written assembly kernels.
 **/
void assembly_init(double *restrict x, const double k, const size_t len);

void assembly_copy(double *restrict x, const double *restrict y,
                   const size_t len);

void assembly_reduc(const double *restrict x, double *r, const size_t len);

void assembly_dotprod(const double *restrict x, const double *restrict y,
                      double *d, const size_t len);

void assembly_gaxpy(const double *restrict x, const double *restrict y, 
                    double *restrict z, const double a, const size_t len);

void assembly_vec_sum(const double *restrict x, const double *restrict y,
                      double *restrict z, const size_t len);

void assembly_vec_scale(const double k, const double *restrict x,
                        double *restrict y, const size_t len);
