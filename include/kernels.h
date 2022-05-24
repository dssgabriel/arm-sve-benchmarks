#pragma once

#include <stddef.h>

void ref_init(double *restrict a, const double b, size_t n);
void asm_init(double *restrict a, const double b, size_t n);

void ref_copy(double *restrict a, double *restrict b, size_t n);
void asm_copy(double *restrict a, double *restrict b, size_t n);

void ref_reduc(double *restrict a, double *b, size_t n);
void asm_reduc(double *restrict a, double *b, size_t n);

void ref_dotprod(double *restrict a, double *restrict b, double *c, size_t n);
void asm_dotprod(double *restrict a, double *restrict b, double *c, size_t n);

void ref_daxpy(double *restrict a, double *restrict b, double *restrict c,
               size_t n);
void asm_daxpy(double *restrict a, double *restrict b, double *restrict c,
               size_t n);

void ref_vec_sum(double *restrict a, double *restrict b, double *restrict c,
                 size_t n);
void asm_vec_sum(double *restrict a, double *restrict b, double *restrict c,
                 size_t n);

void ref_vec_scale(double *restrict a, double *restrict b, double *restrict c,
                   size_t n);
void asm_vec_scale(double *restrict a, double *restrict b, double *restrict c,
                   size_t n);
