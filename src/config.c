#include "config.h"

#include "consts.h"
#include "logs.h"

#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_help(const char *bin)
{
   printf("\033[1mUsage: %s <ARGS> [OPTIONS]\033[0m\n"
          "\n\033[1mArguments:\033[0m\n"
          "\t-k <BENCH_KIND>       Runs a benchmark where <BENCH_KIND> is "
          "one of the following:\n"
          "\t                       - init;\n"
          "\t                       - copy;\n"
          "\t                       - reduc;\n"
          "\t                       - dotprod;\n"
          "\t                       - GAXPY;\n"
          "\t                       - vec_sum;\n"
          "\t                       - vec_scale.\n"
          "\n\033[1mOptions:\033[0m\n"
          "\t-s [SIZE]             Vector size in bytes (default: %dB).\n"
          "\t-r [NB_REP]           Number of repetitions (default: %d).\n"
          "\t-e [ERROR_TOLERANCE]  Error tolerance (default: %e).\n"
          "\t-v                    Prints the version number.\n"
          "\t-h                    Prints this help.\n\n",
          bin, DEFAULT_SIZE, DEFAULT_REP, DEFAULT_ERROR);
}

char *bench_kind_to_string(const bench_kind_t kind)
{
   switch (kind) {
      case BENCH_KIND_INIT:
         return "init";
      case BENCH_KIND_COPY:
         return "copy";
      case BENCH_KIND_REDUC:
         return "reduc";
      case BENCH_KIND_DOTPROD:
         return "dotprod";
      case BENCH_KIND_GAXPY:
         return "gaxpy";
      case BENCH_KIND_SUM:
         return "vec_sum";
      case BENCH_KIND_SCALE:
         return "vec_scale";
      default:
         return "???";
   }
}

int config_init(config_t *config, int argc, char *argv[argc + 1])
{
   bool is_kind_set = false;

   int opt;
   while ((opt = getopt(argc, argv, "e:r:k:s:vh")) != -1) {
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
            else if (!strcmp(optarg, "gaxpy")) {
               config->benchmark_kind = BENCH_KIND_GAXPY;
            }
            else if (!strcmp(optarg, "vec_sum")) {
               config->benchmark_kind = BENCH_KIND_SUM;
            }
            else if (!strcmp(optarg, "vec_scale")) {
               config->benchmark_kind = BENCH_KIND_SCALE;
            }
            else {
               log_error("unkown benchmark kind `%s`. "
                         "See help for available benchmarks.",
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
               log_warn("unable to parse `%s`, "
                        "using default vector size (%zu).",
                        optarg, DEFAULT_SIZE);
            }
            break;
         }
         case 'r': {
            char *endptr;
            size_t rep = strtoul(optarg, &endptr, INTEGER_BASE);
            if (rep) {
               config->nb_repetitions = rep;
            }
            else {
               config->nb_repetitions = DEFAULT_REP;
               log_warn("unable to parse `%s`, "
                        "using default number of repetitions (%zu).",
                        optarg, DEFAULT_REP);
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
               log_warn("unable to parse `%s`, "
                        "using default error tolerance (%e).",
                        optarg, DEFAULT_ERROR);
            }
            break;
         }
         case 'h': {
            print_help(argv[0]);
            exit(EXIT_SUCCESS);
         }
         case 'v': {
            printf("\033[1mMini Arm SVE benchmarks - v0.2\n");
            exit(EXIT_SUCCESS);
         }
         default: {
            log_error("unknown option `%s`. "
                      "See help for the available options.");
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

int config_print(const config_t *config)
{
   float readable_size = config->nb_bytes;
   char *readable_unit = "B";
   if (config->nb_bytes > ONE_GIB) {
      readable_size /= ONE_GIB;
      readable_unit = "GiB";
   }
   else if (config->nb_bytes > ONE_MIB) {
      readable_size /= ONE_MIB;
      readable_unit = "MiB";
   }
   else if (config->nb_bytes > ONE_KIB) {
      readable_size /= ONE_KIB;
      readable_unit = "KiB";
   }

   char *bench_kind = bench_kind_to_string(config->benchmark_kind);
   log_info("running `%s` benchmark with vectors of size %.2lf %s, "
            "%zu repetitions and error tolerance of %.0e.",
            bench_kind, readable_size, readable_unit, config->nb_repetitions,
            config->error_tolerance);
   return 0;
}

int config_result(const config_t *config)
{
   if (config->passed) {
      printf("\033[1;32m`%s` benchmark passed!\033[0m\n"
             "  Compiler latency: %.3lfµs\n"
             "  Assembly latency: %.3lfµs\n"
             "Hand-written assembly speedup: %.3lfx\n",
             bench_kind_to_string(config->benchmark_kind),
             config->compiler_latency, config->assembly_latency,
             config->speedup);
   }
   else {
      printf("\033[1;31m`%s` benchmark failed.\033[0m\n"
             "  Error tolerance: %.0e\n"
             "  Error computed:  %.0e\n",
             bench_kind_to_string(config->benchmark_kind),
             config->error_tolerance, config->computed_error);
   }
   return 0;
}
