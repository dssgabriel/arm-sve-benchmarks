#include "config.h"
#include "consts.h"
#include "logs.h"

#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *bench_kind_to_string(bench_kind_t kind)
{
   switch (kind) {
      case BENCH_KIND_INIT:
         return "INIT";
      case BENCH_KIND_COPY:
         return "COPY";
      case BENCH_KIND_REDUC:
         return "REDUC";
      case BENCH_KIND_DOTPROD:
         return "DOTPROD";
      case BENCH_KIND_DAXPY:
         return "DAXPY";
      case BENCH_KIND_SUM:
         return "VECTOR SUM";
      case BENCH_KIND_SCALE:
         return "VECTOR SCALE";
      default:
         return "???";
   }
}

int config_init(config_t *config, int argc, char *argv[argc + 1])
{
   bool is_kind_set = false;

   int opt;
   while ((opt = getopt(argc, argv, "e:i:k:s:vh")) != -1) {
      switch (opt) {
         case 'k': {
            if (!strcmp(optarg, "init")) {
               config->benchmark_kind = BENCH_KIND_INIT;
            }
            else if (!strcmp(optarg, "copy")) {
               config->benchmark_kind = BENCH_KIND_COPY;
            }
            else if (!strcmp(optarg, "reduc")) {
               config->benchmark_kind = BENCH_KIND_REDUC;
            }
            else if (!strcmp(optarg, "dotprod")) {
               config->benchmark_kind = BENCH_KIND_DOTPROD;
            }
            else if (!strcmp(optarg, "daxpy")) {
               config->benchmark_kind = BENCH_KIND_DAXPY;
            }
            else if (!strcmp(optarg, "vec_sum")) {
               config->benchmark_kind = BENCH_KIND_SUM;
            }
            else if (!strcmp(optarg, "vec_scale")) {
               config->benchmark_kind = BENCH_KIND_SCALE;
            }
            else {
               log_error("unkown benchmark kind %s. See help for available "
                         "benchmarks.",
                         optarg);
               exit(EXIT_FAILURE);
            }
            is_kind_set = true;
            break;
         }
         case 's': {
            char *endptr;
            size_t size = strtoul(optarg, &endptr, INTEGER_BASE);
            if (size) {
               config->nb_bytes = size;
            }
            else {
               config->nb_bytes = DEFAULT_SIZE;
               log_warn("unable to parse %s, using default vector size (%zu).",
                        optarg, DEFAULT_SIZE);
            }
            break;
         }
         case 'i': {
            char *endptr;
            size_t iter = strtoul(optarg, &endptr, INTEGER_BASE);
            if (iter) {
               config->nb_iterations = iter;
            }
            else {
               config->nb_iterations = DEFAULT_ITER;
               log_warn("unable to parse %s, using default number of "
                        "iterations (%zu).",
                        optarg, DEFAULT_ITER);
            }
            break;
         }
         case 'e': {
            char *endptr;
            double error = strtod(optarg, &endptr);
            if (error) {
               config->error_tolerance = error;
            }
            else {
               config->error_tolerance = DEFAULT_ERROR;
               log_warn(
                  "unable to parse %s, using default error tolerance (%e).",
                  optarg, DEFAULT_ERROR);
            }
            break;
         }
         case 'h': {
            printf(
               "\033[1mUsage: %s <ARGS> [OPTIONS]\033[0m\n"
               "\n\033[1mArguments:\033[0m\n"
               "\t-k <BENCH_KIND>       Runs one of the following benchmarks:\n"
               "\t                        INIT, COPY, REDUC, DOTPROD, DAXPY, "
               "VECTOR SUM, VECTOR SCALE.\n"
               "\n\033[1mOptions:\033[0m\n"
               "\t-s [SIZE]             Size of the vectors in bytes (8MiB by "
               "default).\n"
               "\t-i [NB_ITER]          Number of iterations to perform (10 by "
               "default).\n"
               "\t-e [ERROR_TOLERANCE]  Maximum error tolerance (1e-15 by "
               "default).\n"
               "\t-v                    Prints the version number.\n"
               "\t-h                    Prints this help.\n\n",
               argv[0]);
            exit(EXIT_SUCCESS);
         }
         case 'v': {
            printf("\033[1mMini ARM SVE benchmarks - v0.1\n");
            exit(EXIT_SUCCESS);
         }
         default: {
            log_error("unknown option %s. See help for the available options.");
            exit(EXIT_FAILURE);
         }
      }
   }

   if (!is_kind_set) {
      log_error(
         "benchmark kind needs to be set. See help for available benchmarks.");
      exit(EXIT_FAILURE);
   }
   return 0;
}

int config_print(config_t *config)
{
   char *bench_kind = bench_kind_to_string(config->benchmark_kind);
   float readable_size = config->nb_bytes;
   char *readable_unit = "B";
   if (config->nb_bytes > 1e9) {
      readable_size /= ONE_GIB;
      readable_unit = "GiB";
   }
   else if (config->nb_bytes > 1e6) {
      readable_size /= ONE_MIB;
      readable_unit = "MiB";
   }
   else if (config->nb_bytes > 1e3) {
      readable_size /= ONE_KIB;
      readable_unit = "KiB";
   }

   log_info("running `%s` benchmark with vectors of size %.2lf %s and %zu "
            "iterations.",
            bench_kind, readable_size, readable_unit, config->nb_iterations);
   return 0;
}

int config_result(config_t *config)
{
   if (config->passed) {
      printf("\033[1;32m`%s` benchmark passed!\033[0m\n"
             "  Reference latency: %.9lfs\n"
             "   Assembly latency: %.9lfs\n"
             "Hand-written SVE kernel speedup: %.3lfx\n",
             bench_kind_to_string(config->benchmark_kind), config->ref_latency,
             config->asm_latency, config->speedup);
   }
   else {
      printf("\033[1;31m`%s` benchmark failed:\033[0m\n"
             "  Error tolerance: %e\n"
             "   Error computed: %e\n",
             bench_kind_to_string(config->benchmark_kind),
             config->error_tolerance, config->computed_error);
   }
   return 0;
}
