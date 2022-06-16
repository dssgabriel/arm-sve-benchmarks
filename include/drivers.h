#pragma once

#include "config.h"

int driver_init(config_t *config);
int driver_copy(config_t *config);
int driver_reduc(config_t *config);
int driver_dotprod(config_t *config);
int driver_gaxpy(config_t *config);
int driver_vec_sum(config_t *config);
int driver_vec_scale(config_t *config);
