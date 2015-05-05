/*
 * help.c
 * ascd, the ASC daemon
 */

#include "ascd.h"


static char format[] = "`ascd', the daemon for `asc' programs";

void usage()
{
    printf("%s\n", format);
}
