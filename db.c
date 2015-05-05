/*
 * db.c
 * ascd, the ASC daemon
 */

#include <stdio.h>
#include <string.h>
#include <sqlite3.h>

#include "ascd.h"
#include "statements.h"

#define DB_PATH "ascd.db"
#define MAX_STATEMENT_LENGTH -1


int db_batch_statements(sqlite3 *db, const char *stmts)
{
    sqlite3_stmt *stmt;
    const char *tail;
    int len;

    tail = stmts;
    len = strlen(stmts);

    while (tail < (stmts + len)) {
        if (sqlite3_prepare_v2(db, tail, MAX_STATEMENT_LENGTH,
                               &stmt, &tail) != SQLITE_OK)
            goto failure;

        if (sqlite3_step(stmt) == SQLITE_ERROR)
            goto failure;
    }

  success:
    return 1;

  failure:
    return -1;
}

int db_start()
{
    sqlite3_stmt *start;

    if (sqlite3_open(DB_PATH, &db) != SQLITE_OK) {
        error("failed to open database: %s\n", sqlite3_errmsg(db));
        goto failure;
    }

    if (db_batch_statements(db, stmt_start) < 0) {
        error("failed to set up database: %s\n", sqlite3_errmsg(db));
        goto failure;
    }

  success:
    return 1;

  failure:
    return -1;
}

int db_end()
{
    sqlite3_stmt *end;

    if (db_batch_statements(db, stmt_end) < 0) {
        error("failed to tear down database: %s\n", sqlite3_errmsg(db));
        goto failure;
    }

    if (sqlite3_close(db) != SQLITE_OK)
        goto failure;

  success:
    return 1;

  failure:
    return -1;
}


/*
 * Functions for querying database for ASC data
 */

struct learning_data *db_learning_data_by_name(char *prog_name)
{
    sqlite3_stmt *stmt;
    int rv;
    struct learning_data *data;
    mpz_t ex, *exp;

    data = NULL;

    if (sqlite3_prepare_v2(db, stmt_any_by_name, MAX_STATEMENT_LENGTH,
                           &stmt, NULL) != SQLITE_OK)
    {
        goto failure;
    }

    if (sqlite3_bind_text(stmt, 1, prog_name, -1,
                          SQLITE_STATIC) != SQLITE_OK)
    {
        goto failure;
    }

    rv = sqlite3_step(stmt);

    if (rv == SQLITE_ERROR)
        goto failure;

    if (rv != SQLITE_ROW)
        goto success;

    data = malloc(sizeof(data));

    data->regime.mode = sqlite3_column_int(stmt, 0);
    data->regime.address = sqlite3_column_int64(stmt, 1);
    data->regime.period = sqlite3_column_int64(stmt, 2);
    data->regime.instructions = sqlite3_column_int64(stmt, 3);
    data->regime.dim = sqlite3_column_int64(stmt, 4);

    mpz_init(ex);
    mpz_set_str(ex, sqlite3_column_text(stmt, 5), 16);

    exp = malloc(sizeof(mpz_t));
    memcpy(exp, &ex, sizeof(ex));

    data->excitations = exp;
    data->model = string_to_fann((char *)sqlite3_column_text(stmt, 6));
    data->tdata =
        string_to_fann_train_data((char *)sqlite3_column_text(stmt, 7));

  success:
    return data;

  failure:
    return NULL;
}
