#include "drivers.h"

#include "config.h"
#include "consts.h"
#include "kernels.h"
#include "logs.h"
#include "utils.h"

#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

typedef struct vectors_s {
   double *compiler_vec;
   double *assembly_vec;
   size_t len;
} vectors_t;

vectors_t init_vectors(const size_t size, const bool mode)
{
   srand(0);
   vectors_t vecs = {
      .compiler_vec = aligned_alloc(ALIGNMENT, size),
      .assembly_vec = aligned_alloc(ALIGNMENT, size),
      .len = size / sizeof(double),
   };
   if (!vecs.compiler_vec || !vecs.assembly_vec) {
      log_error("failed to allocate vectors.\n");
      exit(EXIT_FAILURE);
   }

   double rand_val = 0.0;
   for (size_t i = 0; i < vecs.len; ++i) {
      if (mode) {
         rand_val = rand_double(-1.0, 1.0);
      }
      vecs.compiler_vec[i] = rand_val;
      vecs.assembly_vec[i] = rand_val;
   }

   return vecs;
}

void destroy_vectors(vectors_t *vecs)
{
   if (!vecs) {
      return;
   }
   free(vecs->compiler_vec);
   free(vecs->assembly_vec);
}

int driver_init(config_t *config)
{
   vectors_t x = init_vectors(config->nb_bytes, false);
   const double k = rand_double(-1.0, 1.0);
   struct timespec start, end;

   // Run compiler benchmark
   clock_gettime(CLOCK_MONOTONIC_RAW, &start);
   for (size_t i = 0; i < config->nb_repetitions; ++i) {
      compiler_init(k, x.compiler_vec, x.len);
   }
   clock_gettime(CLOCK_MONOTONIC_RAW, &end);
   config->compiler_latency =
      compute_avg_latency(start, end, config->nb_repetitions);

   // Run assembly benchmark
   clock_gettime(CLOCK_MONOTONIC_RAW, &start);
   for (size_t i = 0; i < config->nb_repetitions; ++i) {
      assembly_init(k, x.assembly_vec, x.len);
   }
   clock_gettime(CLOCK_MONOTONIC_RAW, &end);
   config->assembly_latency =
      compute_avg_latency(start, end, config->nb_repetitions);

   // Compute speedup
   config->speedup = config->compiler_latency / config->assembly_latency;

   // Compute error
   config->computed_error =
      compute_error(x.compiler_vec, x.assembly_vec, x.len);
   if (config->computed_error <= config->error_tolerance) {
      config->passed = true;
   }

   destroy_vectors(&x);
   return 0;
}

int driver_copy(config_t *config)
{
   vectors_t x = init_vectors(config->nb_bytes, false);
   vectors_t y = init_vectors(config->nb_bytes, true);
   struct timespec start, end;

   // Run compiler benchmark
   clock_gettime(CLOCK_MONOTONIC_RAW, &start);
   for (size_t i = 0; i < config->nb_repetitions; ++i) {
      compiler_copy(x.compiler_vec, y.compiler_vec, x.len);
   }
   clock_gettime(CLOCK_MONOTONIC_RAW, &end);
   config->compiler_latency =
      compute_avg_latency(start, end, config->nb_repetitions);

   // Run assembly benchmark
   clock_gettime(CLOCK_MONOTONIC_RAW, &start);
   for (size_t i = 0; i < config->nb_repetitions; ++i) {
      assembly_copy(x.assembly_vec, y.assembly_vec, x.len);
   }
   clock_gettime(CLOCK_MONOTONIC_RAW, &end);
   config->assembly_latency =
      compute_avg_latency(start, end, config->nb_repetitions);

   // Compute speedup
   config->speedup = config->compiler_latency / config->assembly_latency;

   // Compute error
   config->computed_error =
      compute_error(x.compiler_vec, x.assembly_vec, x.len);
   if (config->computed_error <= config->error_tolerance) {
      config->passed = true;
   }

   destroy_vectors(&x);
   destroy_vectors(&y);
   return 0;
}

int driver_reduc(config_t *config)
{
   vectors_t x = init_vectors(config->nb_bytes, true);
   double compiler_results[config->nb_repetitions];
   double assembly_results[config->nb_repetitions];
   struct timespec start, end;

   // Run compiler benchmark
   clock_gettime(CLOCK_MONOTONIC_RAW, &start);
   for (size_t i = 0; i < config->nb_repetitions; ++i) {
      compiler_reduc(x.compiler_vec, compiler_results + i, x.len);
   }
   clock_gettime(CLOCK_MONOTONIC_RAW, &end);
   config->compiler_latency =
      compute_avg_latency(start, end, config->nb_repetitions);

   // Run assembly benchmark
   clock_gettime(CLOCK_MONOTONIC_RAW, &start);
   for (size_t i = 0; i < config->nb_repetitions; ++i) {
      assembly_reduc(x.assembly_vec, assembly_results + i, x.len);
   }
   clock_gettime(CLOCK_MONOTONIC_RAW, &end);
   config->assembly_latency =
      compute_avg_latency(start, end, config->nb_repetitions);

   // Compute speedup
   config->speedup = config->compiler_latency / config->assembly_latency;

   // Compute error
   config->computed_error =
      compute_error(compiler_results, assembly_results, config->nb_repetitions);
   if (config->computed_error <= config->error_tolerance) {
      config->passed = true;
   }

   destroy_vectors(&x);
   return 0;
}

int driver_dotprod(config_t *config)
{
   vectors_t x = init_vectors(config->nb_bytes, true);
   vectors_t y = init_vectors(config->nb_bytes, true);
   double compiler_results[config->nb_repetitions];
   double assembly_results[config->nb_repetitions];
   struct timespec start, end;

   // Run compiler benchmark
   clock_gettime(CLOCK_MONOTONIC_RAW, &start);
   for (size_t i = 0; i < config->nb_repetitions; ++i) {
      compiler_dotprod(x.compiler_vec, y.compiler_vec, compiler_results + i,
                       x.len);
   }
   clock_gettime(CLOCK_MONOTONIC_RAW, &end);
   config->compiler_latency =
      compute_avg_latency(start, end, config->nb_repetitions);

   // Run assembly benchmark
   clock_gettime(CLOCK_MONOTONIC_RAW, &start);
   for (size_t i = 0; i < config->nb_repetitions; ++i) {
      assembly_dotprod(x.assembly_vec, y.assembly_vec, assembly_results + i,
                       y.len);
   }
   clock_gettime(CLOCK_MONOTONIC_RAW, &end);
   config->assembly_latency =
      compute_avg_latency(start, end, config->nb_repetitions);

   // Compute speedup
   config->speedup = config->compiler_latency / config->assembly_latency;

   // Compute error
   config->computed_error =
      compute_error(compiler_results, assembly_results, config->nb_repetitions);
   if (config->computed_error <= config->error_tolerance) {
      config->passed = true;
   }

   destroy_vectors(&x);
   destroy_vectors(&y);
   return 0;
}

int driver_gaxpy(config_t *config)
{
   vectors_t x = init_vectors(config->nb_bytes, true);
   vectors_t y = init_vectors(config->nb_bytes, true);
   const double a = rand_double(-1.0, 1.0);
   struct timespec start, end;

   // Run compiler benchmark
   clock_gettime(CLOCK_MONOTONIC_RAW, &start);
   for (size_t i = 0; i < config->nb_repetitions; ++i) {
      compiler_gaxpy(a, x.compiler_vec, y.compiler_vec, y.len);
   }
   clock_gettime(CLOCK_MONOTONIC_RAW, &end);
   config->compiler_latency =
      compute_avg_latency(start, end, config->nb_repetitions);

   // Run assembly benchmark
   clock_gettime(CLOCK_MONOTONIC_RAW, &start);
   for (size_t i = 0; i < config->nb_repetitions; ++i) {
      assembly_gaxpy(a, x.assembly_vec, y.assembly_vec, y.len);
   }
   clock_gettime(CLOCK_MONOTONIC_RAW, &end);
   config->assembly_latency =
      compute_avg_latency(start, end, config->nb_repetitions);

   // Compute speedup
   config->speedup = config->compiler_latency / config->assembly_latency;

   // Compute error
   config->computed_error =
      compute_error(y.compiler_vec, y.assembly_vec, y.len);
   if (config->computed_error <= config->error_tolerance) {
      config->passed = true;
   }

   destroy_vectors(&x);
   destroy_vectors(&y);
   return 0;
}

int driver_vec_sum(config_t *config)
{
   vectors_t x = init_vectors(config->nb_bytes, true);
   vectors_t y = init_vectors(config->nb_bytes, true);
   struct timespec start, end;

   // Run compiler benchmark
   clock_gettime(CLOCK_MONOTONIC_RAW, &start);
   for (size_t i = 0; i < config->nb_repetitions; ++i) {
      compiler_vec_sum(x.compiler_vec, y.compiler_vec, x.len);
   }
   clock_gettime(CLOCK_MONOTONIC_RAW, &end);
   config->compiler_latency =
      compute_avg_latency(start, end, config->nb_repetitions);

   // Run assembly benchmark
   clock_gettime(CLOCK_MONOTONIC_RAW, &start);
   for (size_t i = 0; i < config->nb_repetitions; ++i) {
      assembly_vec_sum(x.assembly_vec, y.assembly_vec, x.len);
   }
   clock_gettime(CLOCK_MONOTONIC_RAW, &end);
   config->assembly_latency =
      compute_avg_latency(start, end, config->nb_repetitions);

   // Compute speedup
   config->speedup = config->compiler_latency / config->assembly_latency;

   // Compute error
   config->computed_error =
      compute_error(x.compiler_vec, x.assembly_vec, x.len);
   if (config->computed_error <= config->error_tolerance) {
      config->passed = true;
   }

   destroy_vectors(&x);
   destroy_vectors(&y);
   return 0;
}

int driver_vec_scale(config_t *config)
{
   vectors_t x = init_vectors(config->nb_bytes, true);
   const double k = rand_double(-1.0, 1.0);
   struct timespec start, end;

   // Run compiler benchmark
   clock_gettime(CLOCK_MONOTONIC_RAW, &start);
   for (size_t i = 0; i < config->nb_repetitions; ++i) {
      compiler_vec_scale(k, x.compiler_vec, x.len);
   }
   clock_gettime(CLOCK_MONOTONIC_RAW, &end);
   config->compiler_latency =
      compute_avg_latency(start, end, config->nb_repetitions);

   // Run assembly benchmark
   clock_gettime(CLOCK_MONOTONIC_RAW, &start);
   for (size_t i = 0; i < config->nb_repetitions; ++i) {
      assembly_vec_scale(k, x.assembly_vec, x.len);
   }
   clock_gettime(CLOCK_MONOTONIC_RAW, &end);
   config->assembly_latency =
      compute_avg_latency(start, end, config->nb_repetitions);

   // Compute speedup
   config->speedup = config->compiler_latency / config->assembly_latency;

   // Compute error
   config->computed_error =
      compute_error(x.compiler_vec, x.assembly_vec, x.len);
   if (config->computed_error <= config->error_tolerance) {
      config->passed = true;
   }

   destroy_vectors(&x);
   return 0;
}
