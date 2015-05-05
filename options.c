/*
 * options.c
 * ascd, the ASC daemon
 */

#include <getopt.h>
#include <limits.h>
#include <math.h>
#include <unistd.h>

#include "ascd.h"


static struct option switches[] = {
    {"help", no_argument, 0, 'h'},
    {"daemon", no_argument, 0, 'd'},
    {0, 0, 0, 0}
};

static char const *letters = "hd";

int options(int argc, char *argv[])
{
    int c, i;

    while ((c = getopt_long(argc, argv, letters, switches, &i)) != -1) {
        switch (c) {
        case 'h':
            flags |= HELP;
            break;

        case 'd':
            flags |= DAEMON;
            break;

        default:
            goto failure;
        }
    }

  success:
    return 0;

  failure:
    return -1;
}
