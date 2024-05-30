#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 12345
#define SERVER_IP "127.0.0.1"
#define MAX_BUFFER_SIZE 1024

int main() {
    int client_socket;
    struct sockaddr_in server_address;
    char buffer[MAX_BUFFER_SIZE];

    // Create a TCP socket
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = inet_addr(SERVER_IP);

    // Connect to the server
    if (connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1) {
        perror("connect failed");
        exit(EXIT_FAILURE);
    }

    printf("Connected to server at %s:%d\n", SERVER_IP, PORT);

    while (1) {
        int number;
        printf("Enter a number ('exit' to quit): ");
        if (fgets(buffer, MAX_BUFFER_SIZE, stdin) == NULL) {
            perror("fgets failed");
            close(client_socket);
            exit(EXIT_FAILURE);
        }

        buffer[strcspn(buffer, "\n")] = '\0';

        if (send(client_socket, buffer, strlen(buffer), 0) == -1) {
            perror("send failed");
            close(client_socket);
            exit(EXIT_FAILURE);
        }

        if (strcmp(buffer, "exit") == 0) {
            printf("Exiting...\n");
            break;
        }

        ssize_t bytes_received = recv(client_socket, buffer, MAX_BUFFER_SIZE, 0);
        if (bytes_received <= 0) {
            if (bytes_received == 0) {
                printf("Server disconnected. Exiting...\n");
            } else {
                perror("recv failed");
            }
            close(client_socket);
            exit(EXIT_FAILURE);
        }

        buffer[bytes_received] = '\0';

        printf("Result from server: %s", buffer);
    }

    close(client_socket);

    return 0;
}
