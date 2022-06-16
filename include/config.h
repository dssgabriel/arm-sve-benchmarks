#pragma once

#include <stdbool.h>
#include <stddef.h>

typedef enum bench_kind_e {
    BENCH_KIND_INIT,
    BENCH_KIND_COPY,
    BENCH_KIND_REDUC,
    BENCH_KIND_DOTPROD,
    BENCH_KIND_GAXPY,
    BENCH_KIND_SUM,
    BENCH_KIND_SCALE,
    BENCH_KIND__MAX,
} bench_kind_t;

typedef struct config_s {
    bench_kind_t benchmark_kind;
    size_t nb_bytes;
    size_t nb_repetitions; 
    double error_tolerance;
    double computed_error;
    double compiler_latency;
    double assembly_latency;
    double speedup;
    bool passed;
} config_t;

int config_init(config_t *config, int argc, char *argv[argc + 1]);
int config_print(const config_t *config);
int config_result(const config_t *config);
