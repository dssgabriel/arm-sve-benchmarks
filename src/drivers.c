#include "drivers.h"

#include "config.h"
#include "consts.h"
#include "kernels.h"
#include "logs.h"

#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

typedef struct vectors_s {
   double *compiler_vec;
   double *assembly_vec;
   size_t len;
} vectors_t;

vectors_t init_vectors(const size_t size)
{
   vectors_t vecs = {
      .compiler_vec = aligned_alloc(ALIGNMENT, size),
      .assembly_vec = aligned_alloc(ALIGNMENT, size),
      .len = size / sizeof(double),
   };
   if (!vecs.compiler_vec || !vecs.assembly_vec) {
      log_error("failed to allocate vectors.\n");
      exit(EXIT_FAILURE);
   }

   for (size_t i = 0; i < vecs.len; ++i) {
      const double rand_val = ((double)(rand() % RAND_MAX));
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

double compute_avg_latency(const struct timespec start,
                           const struct timespec end,
                           const size_t nb_repetitions)
{
   return ((end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec)) /
          nb_repetitions;
}

int driver_init(config_t *config)
{
   srand(0);
   vectors_t x = init_vectors(config->nb_bytes);
   const double value = (double)(rand() % RAND_MAX);
   struct timespec start, end;

   // Run compiler benchmark
   clock_gettime(CLOCK_MONOTONIC_RAW, &start);
   for (size_t i = 0; i < config->nb_repetitions; ++i) {
      compiler_init(x.compiler_vec, value, x.len);
   }
   clock_gettime(CLOCK_MONOTONIC_RAW, &end);
   config->compiler_latency =
      compute_avg_latency(start, end, config->nb_repetitions);

   // Run assembly benchmark
   clock_gettime(CLOCK_MONOTONIC_RAW, &start);
   for (size_t i = 0; i < config->nb_repetitions; ++i) {
      assembly_init(x.assembly_vec, value, x.len);
   }
   clock_gettime(CLOCK_MONOTONIC_RAW, &end);
   config->assembly_latency =
      compute_avg_latency(start, end, config->nb_repetitions);

   // Compute speedup
   config->speedup = config->compiler_latency / config->assembly_latency;

   // Compute error
   for (size_t i = 0; i < config->nb_repetitions; ++i) {
      config->computed_error +=
         fabs((x.compiler_vec[i] - x.assembly_vec[i]) / x.compiler_vec[i]);
   }
   config->computed_error /= config->nb_repetitions;
   if (config->computed_error > config->error_tolerance) {
      config->passed = false;
   }
   else {
      config->passed = true;
   }

   destroy_vectors(&x);
   return 0;
}

int driver_copy(config_t *config)
{
   srand(0);
   vectors_t x = init_vectors(config->nb_bytes);
   vectors_t y = init_vectors(config->nb_bytes);
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
      assembly_copy(x.assembly_vec, y.assembly_vec, y.len);
   }
   clock_gettime(CLOCK_MONOTONIC_RAW, &end);
   config->assembly_latency =
      compute_avg_latency(start, end, config->nb_repetitions);

   // Compute speedup
   config->speedup = config->compiler_latency / config->assembly_latency;

   // Compute error
   for (size_t i = 0; i < config->nb_repetitions; ++i) {
      config->computed_error +=
         fabs((x.compiler_vec[i] - x.assembly_vec[i]) / x.compiler_vec[i]);
   }
   config->computed_error /= config->nb_repetitions;
   if (config->computed_error > config->error_tolerance) {
      config->passed = false;
   }
   else {
      config->passed = true;
   }

   destroy_vectors(&x);
   destroy_vectors(&y);
   return 0;
}

int driver_reduc(config_t *config)
{
   srand(0);
   vectors_t x = init_vectors(config->nb_bytes);
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
   for (size_t i = 0; i < config->nb_repetitions; ++i) {
      config->computed_error += fabs(
         (compiler_results[i] - assembly_results[i]) / compiler_results[i]);
   }
   config->computed_error /= config->nb_repetitions;
   if (config->computed_error > config->error_tolerance) {
      config->passed = false;
   }
   else {
      config->passed = true;
   }

   destroy_vectors(&x);
   return 0;
}

int driver_dotprod(config_t *config)
{
   srand(0);
   vectors_t x = init_vectors(config->nb_bytes);
   vectors_t y = init_vectors(config->nb_bytes);
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
   for (size_t i = 0; i < config->nb_repetitions; ++i) {
      config->computed_error += fabs(
         (compiler_results[i] - assembly_results[i]) / compiler_results[i]);
   }
   config->computed_error /= config->nb_repetitions;
   if (config->computed_error > config->error_tolerance) {
      config->passed = false;
   }
   else {
      config->passed = true;
   }

   destroy_vectors(&x);
   destroy_vectors(&y);
   return 0;
}

int driver_gaxpy(config_t *config)
{
   srand(0);
   const double a = (double)(rand() % RAND_MAX);
   vectors_t x = init_vectors(config->nb_bytes);
   vectors_t y = init_vectors(config->nb_bytes);
   struct timespec start, end;

   // Run compiler benchmark
   clock_gettime(CLOCK_MONOTONIC_RAW, &start);
   for (size_t i = 0; i < config->nb_repetitions; ++i) {
      compiler_gaxpy(a, x.compiler_vec, y.compiler_vec, x.len);
   }
   clock_gettime(CLOCK_MONOTONIC_RAW, &end);
   config->compiler_latency =
      compute_avg_latency(start, end, config->nb_repetitions);

   // Run assembly benchmark
   clock_gettime(CLOCK_MONOTONIC_RAW, &start);
   for (size_t i = 0; i < config->compiler_latency; ++i) {
      assembly_gaxpy(a, x.assembly_vec, y.assembly_vec, x.len);
   }
   clock_gettime(CLOCK_MONOTONIC_RAW, &end);
   config->assembly_latency =
      compute_avg_latency(start, end, config->nb_repetitions);

   // Compute speedup
   config->speedup = config->compiler_latency / config->assembly_latency;

   // Compute error
   for (size_t i = 0; i < config->nb_repetitions; ++i) {
      config->computed_error +=
         fabs((x.compiler_vec[i] - x.assembly_vec[i]) / x.compiler_vec[i]);
   }
   config->computed_error /= config->nb_repetitions;
   if (config->computed_error > config->error_tolerance) {
      config->passed = false;
   }
   else {
      config->passed = true;
   }

   destroy_vectors(&x);
   destroy_vectors(&y);
   destroy_vectors(&z);
   return 0;
}

int driver_vec_sum(config_t *config)
{
   srand(0);
   vectors_t x = init_vectors(config->nb_bytes);
   vectors_t y = init_vectors(config->nb_bytes);
   vectors_t z = init_vectors(config->nb_bytes);
   struct timespec start, end;

   // Run compiler benchmark
   clock_gettime(CLOCK_MONOTONIC_RAW, &start);
   for (size_t i = 0; i < config->nb_repetitions; ++i) {
      compiler_vec_sum(x.compiler_vec, y.compiler_vec, z.compiler_vec, x.len);
   }
   clock_gettime(CLOCK_MONOTONIC_RAW, &end);
   config->compiler_latency =
      compute_avg_latency(start, end, config->nb_repetitions);

   // Run assembly benchmark
   clock_gettime(CLOCK_MONOTONIC_RAW, &start);
   for (size_t i = 0; i < config->compiler_latency; ++i) {
      assembly_vec_sum(x.assembly_vec, y.assembly_vec, z.assembly_vec, x.len);
   }
   clock_gettime(CLOCK_MONOTONIC_RAW, &end);
   config->assembly_latency =
      compute_avg_latency(start, end, config->nb_repetitions);

   // Compute speedup
   config->speedup = config->compiler_latency / config->assembly_latency;

   // Compute error
   for (size_t i = 0; i < config->nb_repetitions; ++i) {
      config->computed_error +=
         fabs((z.compiler_vec[i] - z.assembly_vec[i]) / z.compiler_vec[i]);
   }
   config->computed_error /= config->nb_repetitions;
   if (config->computed_error > config->error_tolerance) {
      config->passed = false;
   }
   else {
      config->passed = true;
   }

   destroy_vectors(&x);
   destroy_vectors(&y);
   destroy_vectors(&z);
   return 0;
}

int driver_vec_scale(config_t *config)
{
   srand(0);
   const double k = (double)(rand() % RAND_MAX);
   vectors_t x = init_vectors(config->nb_bytes);
   vectors_t y = init_vectors(config->nb_bytes);
   struct timespec start, end;

   // Run compiler benchmark
   clock_gettime(CLOCK_MONOTONIC_RAW, &start);
   for (size_t i = 0; i < config->nb_repetitions; ++i) {
      compiler_vec_scale(k, x.compiler_vec, y.compiler_vec, x.len);
   }
   clock_gettime(CLOCK_MONOTONIC_RAW, &end);
   config->compiler_latency =
      compute_avg_latency(start, end, config->nb_repetitions);

   // Run assembly benchmark
   clock_gettime(CLOCK_MONOTONIC_RAW, &start);
   for (size_t i = 0; i < config->compiler_latency; ++i) {
      assembly_vec_scale(k, x.assembly_vec, y.assembly_vec, x.len);
   }
   clock_gettime(CLOCK_MONOTONIC_RAW, &end);
   config->assembly_latency =
      compute_avg_latency(start, end, config->nb_repetitions);

   // Compute speedup
   config->speedup = config->compiler_latency / config->assembly_latency;

   // Compute error
   for (size_t i = 0; i < config->nb_repetitions; ++i) {
      config->computed_error +=
         fabs((x.compiler_vec[i] - x.assembly_vec[i]) / x.compiler_vec[i]);
   }
   config->computed_error /= config->nb_repetitions;
   if (config->computed_error > config->error_tolerance) {
      config->passed = false;
   }
   else {
      config->passed = true;
   }

   destroy_vectors(&x);
   destroy_vectors(&y);
   return 0;
}
