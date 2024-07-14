#include "logger.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <pthread.h>


// #define SOCKET_PATH "/tmp/parasite.sock"
#define MAX_EVENTS 8
#define BUFFER_SIZE 1024

enum Command {
    ADD_STRING = 0x0000,
    REMOVE_STRING = 0x0001,
    GET_STRINGS = 0x0002
};

static void handle_client(int client_fd);
static void handle_command(int client_fd, unsigned short command, const char *data);

static void *server_thread(void *) {
    int server_fd, client_fd, epoll_fd;
    struct sockaddr_un addr;
    struct epoll_event ev, events[MAX_EVENTS];
    int nfds, i;

    server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    unlink(SOCKET_PATH);
    if (bind(server_fd, (struct sockaddr *)&addr, sizeof(struct sockaddr_un)) == -1) {
        perror("bind");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 5) == -1) {
        perror("listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        perror("epoll_create1");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    ev.events = EPOLLIN;
    ev.data.fd = server_fd;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &ev) == -1) {
        perror("epoll_ctl: server_fd");
        close(server_fd);
        close(epoll_fd);
        exit(EXIT_FAILURE);
    }

    while (1) {
        nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        if (nfds == -1) {
            perror("epoll_wait");
            close(server_fd);
            close(epoll_fd);
            exit(EXIT_FAILURE);
        }

        for (i = 0; i < nfds; i++) {
            if (events[i].data.fd == server_fd) {
                client_fd = accept(server_fd, NULL, NULL);
                if (client_fd == -1) {
                    perror("accept");
                } else {
                    ev.events = EPOLLIN;
                    ev.data.fd = client_fd;
                    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &ev) == -1) {
                        perror("epoll_ctl: client_fd");
                        close(client_fd);
                    }
                }
            } else {
                handle_client(events[i].data.fd);
            }
        }
    }

    close(server_fd);
    close(epoll_fd);
    unlink(SOCKET_PATH);
    return NULL;
}

static void handle_client(int client_fd) {
    unsigned short command;
    char buffer[BUFFER_SIZE];
    ssize_t num_bytes;

    num_bytes = read(client_fd, &command, sizeof(command));
    if (num_bytes <= 0) {
        if (num_bytes == -1) perror("read");
        close(client_fd);
        return;
    }

    command = ntohs(command);

    num_bytes = read(client_fd, buffer, sizeof(buffer) - 1);
    if (num_bytes <= 0) {
        if (num_bytes == -1) perror("read");
        close(client_fd);
        return;
    }

    buffer[num_bytes] = '\0';
    handle_command(client_fd, command, buffer);
}

static void handle_command(int client_fd, unsigned short command, const char *data) {
    switch (command) {
        case ADD_STRING:
            add_string(data);
            break;
        case REMOVE_STRING:
            remove_string(data);
            break;
        case GET_STRINGS: {
            char *strings = get_strings();
            write(client_fd, strings, strlen(strings));
            free(strings);
            break;
        }
        default:
            OSSLTRACE_LOG("parasite: unknown command: %hu\n", command);
    }
}

void run_parasite_server() {

}
