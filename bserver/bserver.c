/**
 * Author: Alberto Ielpo
 * Bserver, Basic server using socket from scratch
 *
 * Acceptance flow:
 * 1. create socket
 * 2. bind
 * 3. listen
 * 4. accept
 * 5. read
 * 6. write
 * 7. close
 *
 */

#include <arpa/inet.h>
#include <errno.h>
#include <limits.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_SIZE 1024
#define CONN_QUEUE_SIZE 128
#define SERVER_DEFAULT_PORT 1234

// ------ global ------
int socket_fd = -1;
// --------------------

/**
 * client close function
 */
void c_close(int client_fd) {
    if (client_fd != -1) {
        close(client_fd);
    }
    printf("Client connection closed for client_fd %d\n", client_fd);
}

/**
 * server socket init
 */
void s_socket() {
    // AF_INET (Ipv4), SOCK_STREAM (byte stream), 0 (AUTO)
    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Cannot open socket\n");
        exit(1);
    }

    printf("Socket is open with fd %d\n", socket_fd);
}

/**
 * server close socket
 */
void s_close() {
    // close the socket server (fd)
    printf("Socker server closing...\n");
    if (socket_fd != -1) {
        close(socket_fd);
    }
    printf("Socket server is closed\n");
}

/**
 * server bind socket to port
 */
void s_bind(uint16_t port) {
    // server address
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY; // listen on all interfaces
    server_addr.sin_port = htons(port);
    // bind socket on port
    if (bind(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        printf("Cannot bind socket on port %d\n", port);
        s_close();
        exit(1);
    }
    printf("Socket is bind on port %d\n", port);
}

/**
 * server listen to socket fd
 */
void s_listen(int conn_queue_size) {
    if (listen(socket_fd, conn_queue_size) < 0) {
        printf("Socket is not listening\n");
        s_close();
        exit(1);
    }
    printf("Socket is listening\n");
}

/**
 * server accept on socket_fd
 */
int s_accept() {
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    int client_fd;
    if ((client_fd = accept(socket_fd, (struct sockaddr *)&client_addr, &client_len)) < 0) {
        printf("Cannot accept client\n");
        s_close();
        exit(1);
    }
    printf("Client is connected with port %d and client_fd %d\n", client_addr.sin_port, client_fd);
    return client_fd;
}

/**
 * server write data to the client
 */
void s_write_client(int client_fd, char *msg, size_t msg_length) {
    if (write(client_fd, msg, msg_length) < 0) {
        printf("Buffer not written to server");
    }
};

/**
 * server read data from client
 */
void s_read_client(int client_fd) {
    char buffer[BUFFER_SIZE];

    // until I have data...
    while (1) {
        memset(buffer, 0, BUFFER_SIZE); // reset the buffer
        int bytes_read = read(client_fd, buffer, BUFFER_SIZE - 1);
        if (bytes_read <= 0) {
            // no bytes read
            printf("Client disconnected\n");
            break;
        }

        printf("%s", buffer);
        s_write_client(client_fd, "OK", 3);
    }
};

/**
 * server handle sigint signal (ctrl+c or kill -2)
 */
void c_handle_sigint(int sig) {
    printf("\nReceived sigint %d\n", sig);
    s_close();
    exit(0);
}

/**
 * server start
 */
int main(int argc, char const *argv[]) {
    uint16_t port = SERVER_DEFAULT_PORT;
    if (argc > 1) {
        char *endptr;
        errno = 0;
        uint64_t argv_port = strtoul(argv[1], &endptr, 10);
        if (errno != 0 || *endptr != '\0' || argv_port > USHRT_MAX) {
            printf("Conversion error or out of range\n");
            exit(1);
        }

        port = (uint16_t)argv_port;
    }

    pid_t pid = getpid();
    printf("Starting bserver with pid %d\n", pid);

    // create a server socket
    s_socket();

    // bind the server socket on port
    s_bind(port);

    // server socket in listen mode
    s_listen(CONN_QUEUE_SIZE);

    signal(SIGINT, c_handle_sigint);

    // server is running until a stop signal is received
    while (1) {
        // accept incoming request from client
        int client_fd = s_accept();

        // read data from client
        s_read_client(client_fd);

        // close the client socket (fd)
        c_close(client_fd);
        printf("Socket client is closed\n");
    }

    // close the socket server (fd)
    s_close();

    return 0;
}
