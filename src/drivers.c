#include "config.h"
#include "consts.h"
#include "drivers.h"
#include "kernels.h"
#include "logs.h"

#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

typedef struct vectors_s {
   double *ref_vec;
   double *asm_vec;
   size_t nb_elem;
} vectors_t;

vectors_t init_vectors(size_t size)
{
   if (!(size % sizeof(double) == 0)) {
      log_warn(
         "size is not a multiple of %d, using the nearest multiple instead.");
      size -= (size % sizeof(double));
   }

   vectors_t vecs = {
      .ref_vec = aligned_alloc(ALIGNMENT, size),
      .asm_vec = aligned_alloc(ALIGNMENT, size),
      .nb_elem = size / sizeof(double),
   };
   if (!vecs.ref_vec || !vecs.asm_vec) {
      log_error("failed to allocate arrays.\n");
      exit(EXIT_FAILURE);
   }

   for (size_t i = 0; i < vecs.nb_elem; ++i) {
      const double rand_val =
         ((double)(rand() % vecs.nb_elem) / 100.0) - 10000.0;
      vecs.ref_vec[i] = rand_val;
      vecs.asm_vec[i] = rand_val;
   }

   return vecs;
}

// int driver_generic(config_t* config, size_t nb_arrays, void (*ref_fn));

int driver_init(config_t *config)
{
   srand(0);
   vectors_t a = init_vectors(config->nb_bytes);
   const double value = (double)(rand() % a.nb_elem) / 100.0 - 10000.0;
   struct timespec start, end;

   // Run reference benchmark
   clock_gettime(CLOCK_MONOTONIC_RAW, &start);
   for (size_t i = 0; i < config->nb_iterations; ++i) {
      ref_init(a.ref_vec, value, a.nb_elem);
   }
   clock_gettime(CLOCK_MONOTONIC_RAW, &end);
   config->ref_latency =
      (end.tv_sec - start.tv_sec + (end.tv_nsec - start.tv_nsec) / 1e9) /
      config->nb_iterations;

   // Run assembly benchmark
   clock_gettime(CLOCK_MONOTONIC_RAW, &start);
   for (size_t i = 0; i < config->nb_iterations; ++i) {
      asm_init(a.asm_vec, value, a.nb_elem);
   }
   clock_gettime(CLOCK_MONOTONIC_RAW, &end);
   config->asm_latency =
      (end.tv_sec - start.tv_sec + (end.tv_nsec - start.tv_nsec) / 1e9) /
      config->nb_iterations;

   config->speedup = config->ref_latency / config->asm_latency;

   // Compute error
   for (size_t i = 0; i < config->nb_iterations; ++i) {
      config->computed_error +=
         fabs((a.ref_vec[i] - a.asm_vec[i]) / a.ref_vec[i]);
   }
   config->computed_error /= config->nb_iterations;
   if (config->computed_error > config->error_tolerance)
      config->passed = false;
   else
      config->passed = true;

   free(a.ref_vec);
   free(a.asm_vec);
   return 0;
}

int driver_copy(config_t *config)
{
   srand(0);
   vectors_t a = init_vectors(config->nb_bytes);
   vectors_t b = init_vectors(config->nb_bytes);
   struct timespec start, end;

   // Run reference benchmark
   clock_gettime(CLOCK_MONOTONIC_RAW, &start);
   for (size_t i = 0; i < config->nb_iterations; ++i) {
      ref_copy(a.ref_vec, b.ref_vec, a.nb_elem);
   }
   clock_gettime(CLOCK_MONOTONIC_RAW, &end);
   config->ref_latency =
      (end.tv_sec - start.tv_sec + (end.tv_nsec - start.tv_nsec) / 1e9) /
      config->nb_iterations;

   // Run assembly benchmark
   clock_gettime(CLOCK_MONOTONIC_RAW, &start);
   for (size_t i = 0; i < config->nb_iterations; ++i) {
      asm_copy(a.asm_vec, b.asm_vec, a.nb_elem);
   }
   clock_gettime(CLOCK_MONOTONIC_RAW, &end);
   config->asm_latency =
      (end.tv_sec - start.tv_sec + (end.tv_nsec - start.tv_nsec) / 1e9) /
      config->nb_iterations;

   config->speedup = config->ref_latency / config->asm_latency;

   // Compute error
   for (size_t i = 0; i < config->nb_iterations; ++i) {
      config->computed_error +=
         fabs((a.ref_vec[i] - a.asm_vec[i]) / a.ref_vec[i]);
   }
   config->computed_error /= config->nb_iterations;
   if (config->computed_error > config->error_tolerance)
      config->passed = false;
   else
      config->passed = true;

   free(a.ref_vec);
   free(a.asm_vec);
   free(b.ref_vec);
   free(b.asm_vec);
   return 0;
}

int driver_reduc(config_t *config)
{
   srand(0);
   vectors_t a = init_vectors(config->nb_bytes);
   double value;
   struct timespec start, end;

   // Run reference benchmark
   clock_gettime(CLOCK_MONOTONIC_RAW, &start);
   for (size_t i = 0; i < config->nb_iterations; ++i) {
      ref_reduc(a.ref_vec, &value, a.nb_elem);
   }
   clock_gettime(CLOCK_MONOTONIC_RAW, &end);
   config->ref_latency =
      (end.tv_sec - start.tv_sec + (end.tv_nsec - start.tv_nsec) / 1e9) /
      config->nb_iterations;

   // Run assembly benchmark
   clock_gettime(CLOCK_MONOTONIC_RAW, &start);
   for (size_t i = 0; i < config->nb_iterations; ++i) {
      asm_reduc(a.asm_vec, &value, a.nb_elem);
   }
   clock_gettime(CLOCK_MONOTONIC_RAW, &end);
   config->asm_latency =
      (end.tv_sec - start.tv_sec + (end.tv_nsec - start.tv_nsec) / 1e9) /
      config->nb_iterations;

   config->speedup = config->ref_latency / config->asm_latency;

   // Compute error
   for (size_t i = 0; i < config->nb_iterations; ++i) {
      config->computed_error +=
         fabs((a.ref_vec[i] - a.asm_vec[i]) / a.ref_vec[i]);
   }
   config->computed_error /= config->nb_iterations;
   if (config->computed_error > config->error_tolerance)
      config->passed = false;
   else
      config->passed = true;

   free(a.ref_vec);
   free(a.asm_vec);
   return 0;
}

int driver_dotprod(config_t *config)
{
   srand(0);
   vectors_t a = init_vectors(config->nb_bytes);
   vectors_t b = init_vectors(config->nb_bytes);
   double value;
   struct timespec start, end;

   // Run reference benchmark
   clock_gettime(CLOCK_MONOTONIC_RAW, &start);
   for (size_t i = 0; i < config->nb_iterations; ++i) {
      ref_dotprod(a.ref_vec, b.ref_vec, &value, a.nb_elem);
   }
   clock_gettime(CLOCK_MONOTONIC_RAW, &end);
   config->ref_latency =
      (end.tv_sec - start.tv_sec + (end.tv_nsec - start.tv_nsec) / 1e9) /
      config->nb_iterations;

   // Run assembly benchmark
   clock_gettime(CLOCK_MONOTONIC_RAW, &start);
   for (size_t i = 0; i < config->nb_iterations; ++i) {
      asm_dotprod(a.asm_vec, b.asm_vec, &value, a.nb_elem);
   }
   clock_gettime(CLOCK_MONOTONIC_RAW, &end);
   config->asm_latency =
      (end.tv_sec - start.tv_sec + (end.tv_nsec - start.tv_nsec) / 1e9) /
      config->nb_iterations;

   config->speedup = config->ref_latency / config->asm_latency;

   // Compute error
   for (size_t i = 0; i < config->nb_iterations; ++i) {
      config->computed_error +=
         fabs((a.ref_vec[i] - a.asm_vec[i]) / a.ref_vec[i]);
   }
   config->computed_error /= config->nb_iterations;
   if (config->computed_error > config->error_tolerance)
      config->passed = false;
   else
      config->passed = true;

   free(a.ref_vec);
   free(a.asm_vec);
   free(b.ref_vec);
   free(b.asm_vec);
   return 0;
}

int driver_daxpy(config_t *config)
{
   srand(0);
   vectors_t a = init_vectors(config->nb_bytes);
   vectors_t b = init_vectors(config->nb_bytes);
   vectors_t c = init_vectors(config->nb_bytes);
   struct timespec start, end;

   // Run reference benchmark
   clock_gettime(CLOCK_MONOTONIC_RAW, &start);
   for (size_t i = 0; i < config->nb_iterations; ++i) {
      ref_daxpy(a.ref_vec, b.ref_vec, c.ref_vec, a.nb_elem);
   }
   clock_gettime(CLOCK_MONOTONIC_RAW, &end);
   config->ref_latency =
      (end.tv_sec - start.tv_sec + (end.tv_nsec - start.tv_nsec) / 1e9) /
      config->nb_iterations;

   // Run assembly benchmark
   clock_gettime(CLOCK_MONOTONIC_RAW, &start);
   for (size_t i = 0; i < config->nb_iterations; ++i) {
      asm_daxpy(a.asm_vec, b.asm_vec, c.asm_vec, a.nb_elem);
   }
   clock_gettime(CLOCK_MONOTONIC_RAW, &end);
   config->asm_latency =
      (end.tv_sec - start.tv_sec + (end.tv_nsec - start.tv_nsec) / 1e9) /
      config->nb_iterations;

   config->speedup = config->ref_latency / config->asm_latency;

   // Compute error
   for (size_t i = 0; i < config->nb_iterations; ++i) {
      config->computed_error +=
         fabs((a.ref_vec[i] - a.asm_vec[i]) / a.ref_vec[i]);
   }
   config->computed_error /= config->nb_iterations;
   if (config->computed_error > config->error_tolerance)
      config->passed = false;
   else
      config->passed = true;

   free(a.ref_vec);
   free(a.asm_vec);
   free(b.ref_vec);
   free(b.asm_vec);
   free(c.ref_vec);
   free(c.asm_vec);
   return 0;
}

int driver_vec_sum(config_t *config)
{
   srand(0);
   vectors_t a = init_vectors(config->nb_bytes);
   vectors_t b = init_vectors(config->nb_bytes);
   vectors_t c = init_vectors(config->nb_bytes);
   struct timespec start, end;

   // Run reference benchmark
   clock_gettime(CLOCK_MONOTONIC_RAW, &start);
   for (size_t i = 0; i < config->nb_iterations; ++i) {
      ref_vec_sum(a.ref_vec, b.ref_vec, c.ref_vec, a.nb_elem);
   }
   clock_gettime(CLOCK_MONOTONIC_RAW, &end);
   config->ref_latency =
      (end.tv_sec - start.tv_sec + (end.tv_nsec - start.tv_nsec) / 1e9) /
      config->nb_iterations;

   // Run assembly benchmark
   clock_gettime(CLOCK_MONOTONIC_RAW, &start);
   for (size_t i = 0; i < config->nb_iterations; ++i) {
      asm_vec_sum(a.asm_vec, b.asm_vec, c.asm_vec, a.nb_elem);
   }
   clock_gettime(CLOCK_MONOTONIC_RAW, &end);
   config->asm_latency =
      (end.tv_sec - start.tv_sec + (end.tv_nsec - start.tv_nsec) / 1e9) /
      config->nb_iterations;

   config->speedup = config->ref_latency / config->asm_latency;

   // Compute error
   for (size_t i = 0; i < config->nb_iterations; ++i) {
      config->computed_error +=
         fabs((a.ref_vec[i] - a.asm_vec[i]) / a.ref_vec[i]);
   }
   config->computed_error /= config->nb_iterations;
   if (config->computed_error > config->error_tolerance)
      config->passed = false;
   else
      config->passed = true;

   free(a.ref_vec);
   free(a.asm_vec);
   free(b.ref_vec);
   free(b.asm_vec);
   free(c.ref_vec);
   free(c.asm_vec);
   return 0;
}

int driver_vec_scale(config_t *config)
{
   srand(0);
   vectors_t a = init_vectors(config->nb_bytes);
   vectors_t b = init_vectors(config->nb_bytes);
   vectors_t c = init_vectors(config->nb_bytes);
   struct timespec start, end;

   // Run reference benchmark
   clock_gettime(CLOCK_MONOTONIC_RAW, &start);
   for (size_t i = 0; i < config->nb_iterations; ++i) {
      ref_vec_scale(a.ref_vec, b.ref_vec, c.ref_vec, a.nb_elem);
   }
   clock_gettime(CLOCK_MONOTONIC_RAW, &end);
   config->ref_latency =
      (end.tv_sec - start.tv_sec + (end.tv_nsec - start.tv_nsec) / 1e9) /
      config->nb_iterations;

   // Run assembly benchmark
   clock_gettime(CLOCK_MONOTONIC_RAW, &start);
   for (size_t i = 0; i < config->nb_iterations; ++i) {
      asm_vec_scale(a.asm_vec, b.asm_vec, c.asm_vec, a.nb_elem);
   }
   clock_gettime(CLOCK_MONOTONIC_RAW, &end);
   config->asm_latency =
      (end.tv_sec - start.tv_sec + (end.tv_nsec - start.tv_nsec) / 1e9) /
      config->nb_iterations;

   config->speedup = config->ref_latency / config->asm_latency;

   // Compute error
   for (size_t i = 0; i < config->nb_iterations; ++i) {
      config->computed_error +=
         fabs((a.ref_vec[i] - a.asm_vec[i]) / a.ref_vec[i]);
   }
   config->computed_error /= config->nb_iterations;
   if (config->computed_error > config->error_tolerance)
      config->passed = false;
   else
      config->passed = true;

   free(a.ref_vec);
   free(a.asm_vec);
   free(b.ref_vec);
   free(b.asm_vec);
   free(c.ref_vec);
   free(c.asm_vec);
   return 0;
}
