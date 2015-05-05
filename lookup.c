/*
 * lookup.c
 * ascd, the ASC daemon
 *
 * This file contains functions that ascd uses to process a request
 * from a local client for learning data about a program. See lookup()
 * in this file.
 */

#include <string.h>

#include "ascd.h"


struct learning_data *lookup_any(char *prog_name)
{
    return db_learning_data_by_name(prog_name);
}

struct learning_data *lookup_regime(struct program *prog)
{
    // TODO implement this
    return NULL;
}

struct learning_data *lookup(struct program *prog)
{
    if (prog->any_regime)
        return lookup_any(prog->name);
    else
        return lookup_regime(prog);
}
