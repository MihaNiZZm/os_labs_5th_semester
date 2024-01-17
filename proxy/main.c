#define _GNU_SOURCE

#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include <arpa/inet.h>
#include <netdb.h>

#define HOST_LENGTH 1024
#define BUFFER_SIZE 4096

typedef struct {
    int client_socket;
    char *request;
} context;

int create_server_socket(int port) {
    struct sockaddr_in server_addr;
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Error while creating server socket");
        return -1;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error while binding server socket");
        close(server_socket);
        return -1;
    }

    if (listen(server_socket, SOMAXCONN) == -1) {
        perror("Error while listening on server socket");
        close(server_socket);
        return -1;
    }

    return server_socket;
}

int read_request(int client_socket, char *request) {
    ssize_t bytes_read = recv(client_socket, request, BUFFER_SIZE, 0);
    if (bytes_read == -1) {
        perror("Error while reading request from client");
        return -1;
    }
    else if (bytes_read == 0) {
        fprintf(stderr, "Client disconnected\n");
        return -1;
    }

    request[bytes_read] = '\0';
    printf("TID: %d | Received a message from a client.\n", gettid());
    return 0;
}

int connect_to_remote_host(char *host) {
    struct addrinfo hints, *resolved;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    int status = getaddrinfo(host, "80", &hints, &resolved);
    if (status != 0) {
        fprintf(stderr, "Error getting address info: %s\n", gai_strerror(status));
        return -1;
    }

    int destination_socket = socket(resolved->ai_family, resolved->ai_socktype, resolved->ai_protocol);
    if (destination_socket == -1) {
        perror("Error while creating remote server socket");
        freeaddrinfo(resolved);
        return -1;
    }

    if (connect(destination_socket, resolved->ai_addr, resolved->ai_addrlen) == -1) {
        perror("Error while connecting to remote server");
        close(destination_socket);
        freeaddrinfo(resolved);
        return -1;
    }

    freeaddrinfo(resolved);
    return destination_socket;
}

int parse_host_via_url(char* request, unsigned char* resolved_host) {
    char *host_start = strstr(request, "http://");
    if (host_start == NULL) {
        return -1;
    }
    host_start += strlen("http://");

    const char *host_end = strchr(host_start, '/');
    if (host_end == NULL) {
        return -1;
    }

    size_t host_length = host_end - host_start;
    if (host_length <= 0) {
        return -1;
    }
    strncpy(resolved_host, host_start, host_length);
    resolved_host[host_length] = '\0';

    return 0;
}

int parse_host_from_request_body(char* request, unsigned char* resolved_host) {
    char *host_start = strstr(request, "Host:");
    if (host_start == NULL) {
        return -1;
    }
    host_start += strlen("Host:");

    const char *host_end = strpbrk(host_start, " \r\n:");
    if (host_end == NULL) {
        return -1;
    }

    size_t host_length = host_end - host_start;
    if (host_length <= 0) {
        return -1;
    }
    strncpy(resolved_host, host_start, host_length);
    resolved_host[host_length] = '\0';

    return 0;
}

void *handle_connection(void *arg) {
    context *connection_context = (context *)arg;
    int client_socket = connection_context->client_socket;
    char *request0 = connection_context->request;
    char request[BUFFER_SIZE];
    strcpy(request, request0);

    unsigned char host[HOST_LENGTH];
    if (parse_host_from_request_body(request, host) != 0) {
        if (parse_host_via_url(request, host) != 0) {
            printf("TID: %d | Error parsing host from request.\n", gettid());
            close(client_socket);
            free(request0);
            return NULL;
        }
    }

    int destination_socket = connect_to_remote_host((char *)host);
    if (destination_socket == -1) {
        close(client_socket);
        return NULL;
    }

    ssize_t bytes_sent = write(destination_socket, request, strlen(request));
    if (bytes_sent == -1) {
        close(client_socket);
        close(destination_socket);
        return NULL;
    }

    char *buffer = calloc(BUFFER_SIZE, sizeof(char));
    ssize_t bytes_read;
    while ((bytes_read = read(destination_socket, buffer, BUFFER_SIZE)) > 0) {
        printf("TID: %d | Received a response from remote server.\n", gettid());
        bytes_sent = write(client_socket, buffer, bytes_read);
        if (bytes_sent == -1) {
            close(client_socket);
            close(destination_socket);
            free(buffer);
            return NULL;
        }
        printf("TID: %d | Sent a response to a client.\n", gettid());
    }

    close(client_socket);
    close(destination_socket);
    free(buffer);
    free(request0);

    return NULL;
}

int main() {
    int port;
    printf("Enter a port: ");
    scanf("%d", &port);

    printf("HTTP-proxy started on port %d\n", port);
    int server_socket = create_server_socket(port);
    if (server_socket == -1) {
        perror("Error creating server socket");
        exit(EXIT_FAILURE);
    }

    while (1) {
        int client_socket;
        struct sockaddr_in client_addr;
        socklen_t client_addr_size = sizeof(client_addr);
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_size);
        if (client_socket == -1) {
            perror("Error accepting client connection");
            continue;
        }

        char *request = calloc(BUFFER_SIZE, sizeof(char));
        int err = read_request(client_socket, request);
        if (err == -1) {
            perror("Error reading client request");
            free(request);
            close(client_socket);
            continue;
        }

        context connection_context = {client_socket, request};
        pthread_t connection_handler;
        err = pthread_create(&connection_handler, NULL, &handle_connection, &connection_context);
        if (err != 0) {
            perror("Error creating handler thread");
            free(request);
            close(client_socket);
            continue;
        }

        pthread_detach(connection_handler);
    }

    close(server_socket);
    return 0;
}