/**
 * Author: Alberto Ielpo
 * Bserver client: communication is text plain
 *
 * Client logic flow
 * socket()
 * connect()
 * write()
 * close()
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>
#include <errno.h>
#include <limits.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_DEFAULT_PORT 1234
#define BUFFER_SIZE 1024

// ------ global ------
int client_fd = -1;
// --------------------

void c_socket()
{
    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) // AF_INET (Ipv4), SOCK_STREAM (byte stream), 0 (AUTO)
    {
        printf("Cannot open socket\n");
        exit(1);
    }

    printf("Socket is open with fd %d\n", client_fd);
}

void c_close()
{
    printf("Socker client closing...\n");
    if (client_fd != -1)
    {
        close(client_fd);
    }
    printf("Socket client is closed\n");
};

void c_connect(char *server_host, uint16_t server_port)
{
    // pton
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);

    if (inet_pton(AF_INET, server_host, &server_addr.sin_addr) <= 0)
    {

        printf("Cannot pton host %s port %d\n", server_host, server_port);
        c_close();
        exit(1);
    }

    // Connect
    if (connect(client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        printf("Connection failed\n");
        c_close();
        exit(1);
    }

    printf("Connected to host %s port %d\n", server_host, server_port);
    printf("Quit sequence is ':q!'\n");
}
void c_write()
{
    char buffer[BUFFER_SIZE];

    while (fgets(buffer, BUFFER_SIZE - 1, stdin))
    {
        // quit sequence is ":q!" or 58 113 33
        if (buffer[0] == 58 && buffer[1] == 113 && buffer[2] == 33)
        {
            printf("Client sends quit sequence\n");
            break;
        }

        // Send to server
        if (write(client_fd, buffer, BUFFER_SIZE - 1) < 0)
        {
            printf("Buffer not written to server");
            break;
        }

        // wait here until the server sends a message
        int n = read(client_fd, buffer, BUFFER_SIZE - 1);
        if (n <= 0)
        {
            printf("Server closed connection.\n");
            break;
        }
        buffer[n] = 0;
        printf("> %s\n", buffer);

        // reset the buffer
        memset(buffer, 0, BUFFER_SIZE);
    }
}

/**
 * server handle sigint signal (ctrl+c)
 */
void c_handle_sigint(int sig)
{
    printf("\nCaught signal %d\n", sig);
    c_close();
    exit(0);
}

int main(int argc, char const *argv[])
{
    uint8_t IP_SIZE = 16;
    char host[] = {"127.127.127.127"};
    uint16_t port = SERVER_DEFAULT_PORT;
    if (argc > 1)
    {
        memcpy(host, argv[1], IP_SIZE);
        for (int ii = 0; ii < IP_SIZE; ii++)
        {
            if (host[ii] == 0)
                break;
        }
        printf("host: %s\n", host);
        if (argc > 2)
        {
            char *endptr;
            errno = 0;
            uint64_t argv_port = strtoul(argv[2], &endptr, 10);
            if (errno != 0 || *endptr != '\0' || argv_port > USHRT_MAX)
            {
                printf("Conversion error or out of range\n");
                exit(1);
            }

            port = (uint16_t)argv_port;
        }
    }
    c_socket();
    c_connect(host, port);
    signal(SIGINT, c_handle_sigint);
    c_write();
    c_close();
    return 0;
}
