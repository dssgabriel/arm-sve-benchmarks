#pragma once

#include <stddef.h>
#include <time.h>

double rand_double(const double min, const double max);

double compute_avg_latency(const struct timespec start,
                           const struct timespec end,
                           const size_t nb_repetitions);

double compute_error(const double *compiler, const double *assembly,
                     const size_t len);
