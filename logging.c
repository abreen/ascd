/*
 * logging.c
 * ascd, the ASC daemon
 */

#include <stdio.h>
#include <stdarg.h>

#include "ascd.h"


void info(const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);

    vprintf(fmt, ap);

    va_end(ap);
}

void error(const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);

    vfprintf(stderr, fmt, ap);

    va_end(ap);
}
