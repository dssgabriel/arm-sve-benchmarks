#include "config.h"
#include "consts.h"
#include "drivers.h"
#include "logs.h"

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[argc + 1])
{
   config_t config = {
      .benchmark_kind = BENCH_KIND__MAX,
      .nb_bytes = DEFAULT_SIZE,
      .nb_iterations = DEFAULT_ITER,
      .error_tolerance = DEFAULT_ERROR,
      .computed_error = 0.0,
      .ref_latency = 0.0,
      .asm_latency = 0.0,
      .speedup = 0.0,
      .passed = false,
   };

   config_init(&config, argc, argv);
   config_print(&config);

   switch (config.benchmark_kind) {
      case BENCH_KIND_INIT:
         driver_init(&config);
         break;
      case BENCH_KIND_COPY:
         driver_copy(&config);
         break;
      case BENCH_KIND_REDUC:
         driver_reduc(&config);
         break;
      case BENCH_KIND_DOTPROD:
         driver_dotprod(&config);
         break;
      case BENCH_KIND_DAXPY:
         driver_daxpy(&config);
         break;
      case BENCH_KIND_SUM:
         driver_vec_sum(&config);
         break;
      case BENCH_KIND_SCALE:
         driver_vec_scale(&config);
         break;
      default:
         log_error("unreachable");
         exit(EXIT_FAILURE);
   }

   config_result(&config);
   return 0;
}
