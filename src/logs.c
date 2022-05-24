#include "logs.h"

#include <stdarg.h>
#include <stdio.h>

void log_error(const char *fmt, ...)
{
   printf("\033[1;31m[ERROR]:   \033[0m");

   va_list ap;
   va_start(ap, fmt);
   vprintf(fmt, ap);
   va_end(ap);
   printf("\n");
}

void log_warn(const char *fmt, ...)
{
   printf("\033[1;33m[WARNING]: \033[0m");

   va_list ap;
   va_start(ap, fmt);
   vprintf(fmt, ap);
   va_end(ap);
   printf("\n");
}

void log_info(const char *fmt, ...)
{
   printf("\033[1;36m[INFO]:    \033[0m");

   va_list ap;
   va_start(ap, fmt);
   vprintf(fmt, ap);
   va_end(ap);
   printf("\n");
}
