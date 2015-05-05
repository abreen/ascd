/*
 * handlers.c
 * ascd, the ASC daemon
 */

#include <signal.h>

#include "ascd.h"


void handle_sigint(int signum)
{
    // shutdown control socket
    close(control_sock);
    unlink(CONTROL_SOCKET_PATH);

    // run database cleanup
    db_end();

    signal(signum, SIG_DFL);
    raise(signum);
}

void handle_sigterm(int signum)
{
    // shutdown control socket
    close(control_sock);
    unlink(CONTROL_SOCKET_PATH);

    // run database cleanup
    db_end();

    signal(signum, SIG_DFL);
    raise(signum);
}

void handle_sigquit(int signum)
{
    close(control_sock);

    db_end();

    signal(signum, SIG_DFL);
    raise(signum);
}
