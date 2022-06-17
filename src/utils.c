#include "utils.h"

#include <math.h>
#include <stdlib.h>

inline double rand_double(const double min, const double max)
{
   return min + ((double)(rand()) / (double)(RAND_MAX) / (max - min));
}

inline double compute_avg_latency(const struct timespec start,
                                  const struct timespec end,
                                  const size_t nb_repetitions)
{
   return (double)((double)((end.tv_sec - start.tv_sec) * 1e9) +
                   (end.tv_nsec - start.tv_nsec)) /
          (double)(nb_repetitions);
}

double compute_error(const double *compiler, const double *assembly,
                     const size_t len)
{
   double err = 0.0;
   for (size_t i = 0; i < len; ++i) {
      err += fabs((compiler[i] - assembly[i]) / compiler[i]);
   }
   return err / (double)(len);
}

