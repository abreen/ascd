#define _GNU_SOURCE

#include <errno.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/uio.h>
#include <sqlite3.h>

#include "libascd.h"

/* Runtime configuration flags.  */
enum flags {
    HELP   = 1 << 0,
    DAEMON = 1 << 1
};

/* Generic error message macro.  */
#define diagnostic(...)                                                      \
do {                                                                         \
    error_at_line(0, errno, __FILE__, __LINE__, __VA_ARGS__);                \
} while (0)

/* Generic trace macro.  */
#define tr()                                                                 \
do {                                                                         \
    fprintf(stderr, "%s:%d\n", __FUNCTION__, __LINE__);                      \
} while (0)

/* Constants for the control socket.  */
#define UNIX_PATH_MAX 108
#define CONTROL_SOCKET_PATH "ascd.sock"
#define CONTROL_BUFFER_SIZE 1 << 6
#define BACKLOG 6

/* Globals. */
enum flags flags;
int control_sock;
sqlite3 *db;

/* Logging functions.  */
void info(const char *fmt, ...);
void error(const char *fmt, ...);

/* Signal handlers.  */
void handle_sigint(int signum);
void handle_sigterm(int signum);
void handle_sigquit(int signum);

/* Function signatures.  */
void usage(void);
int options(int argc, char *argv[]);

int db_start(void);
int db_end(void);
struct learning_data *db_learning_data_by_name(char *prog_name);

struct learning_data *lookup(struct program *prog);
