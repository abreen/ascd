/*
 * main.c
 * ascd, the ASC daemon
 */

#include <dirent.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <sqlite3.h>

#include "ascd.h"


int handle_messages()
{
    char cmd_buf[CONTROL_BUFFER_SIZE];
    char *cmdp;
    struct sockaddr_storage sender;
    socklen_t size;
    char *resp;
    struct program *prog;
    struct learning_data *data;

    size = sizeof(sender);
    memset(&sender, 0, sizeof(sender));

    while (1) {
        if (recvfrom(control_sock, &cmd_buf, sizeof(cmd_buf), 0,
            (struct sockaddr *)&sender, &size) < 0)
        {
            return -1;
        }

        cmdp = cmd_buf;

        if (strstr(cmd_buf, ASCD_MSG_QUIT) == cmd_buf) {
            info("got quit message from client\n");
            goto finish;

        } else if (strstr(cmd_buf, ASCD_MSG_LOOKUP) == cmd_buf) {
            info("got lookup message from client\n");

            /*
             * Need to skip past the first line, which in this case
             * contains "LOOKUP".
             */
            cmdp = eol(cmdp) + 1;

            prog = deserialize_program(cmdp);

            if (prog == NULL) {
                info("got malformed program data from client\n");
                continue;
            } else {
                info("got lookup for program '%s'\n", prog->name);
            }

            data = lookup(prog);

            if (data == NULL) {
                info("no data for program '%s'\n", prog->name);
                // TODO move this to message interface
                sendto(control_sock, "NONE", 5, 0,
                       (struct sockaddr *)&sender, size);
            } else {
                info("sent data for program '%s'\n", prog->name);

                resp = serialize_learning_data(data);

                if (sendto(control_sock, resp, strlen(resp), 0,
                           (struct sockaddr *)&sender, size) < 0)
                {
                    info("error sending response\n");
                    perror("error");
                }

                free(resp);
                free(data);
            }

            free(prog);

        } else if (strstr(cmd_buf, ASCD_MSG_STORE) == cmd_buf) {
            // TODO implement store
            continue;

        } else {
            info("got malformed message from a client\n");
        }
    }

  finish:
    return 0;
}

int main(int argc, char *argv[])
{
    struct sockaddr_un addr;

    signal(SIGINT, handle_sigint);
    signal(SIGTERM, handle_sigterm);
    signal(SIGQUIT, handle_sigquit);

    if (options(argc, argv) < 0) {
        usage();
        goto failure;
    }

    if (flags & HELP) {
        usage();
        goto success;
    }

    if (flags & DAEMON)
        if (daemon(1, 1) < 0)
            goto failure;

    if (db_start() < 0)
        goto failure;

    if ((control_sock = socket(AF_UNIX, SOCK_DGRAM, 0)) < 0)
        goto failure;

    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, CONTROL_SOCKET_PATH, UNIX_PATH_MAX);

    if (bind(control_sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
        goto failure;

    if (handle_messages() < 0)
        goto failure;

    close(control_sock);
    unlink(CONTROL_SOCKET_PATH);

    if (db_end() < 0)
        goto failure;

    return 0;

  failure:
    if (control_sock != -1) {
        close(control_sock);
        unlink(CONTROL_SOCKET_PATH);
    }

    return 1;

  success:
    return 0;
}
