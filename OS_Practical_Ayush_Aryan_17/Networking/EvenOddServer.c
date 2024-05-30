#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 12345
#define MAX_BUFFER_SIZE 1024

int isEven(int num) {
    return num % 2 == 0 ? 1 : 0;
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_address, client_address;
    char buffer[MAX_BUFFER_SIZE];

    // Create a TCP socket
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Initialize server address
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT);

    // Bind the socket to the server address
    if (bind(server_socket, (const struct sockaddr *)&server_address, sizeof(server_address)) == -1) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_socket, 5) == -1) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    printf("TCP server is listening on port %d\n", PORT);

    while (1) {
        socklen_t client_address_len = sizeof(client_address);
        if ((client_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_address_len)) == -1) {
            perror("accept failed");
            continue;
        }

        printf("Client connected: %s:%d\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));

        while (1) {
            // Receive number from client
            ssize_t bytes_received = recv(client_socket, buffer, MAX_BUFFER_SIZE, 0);
            if (bytes_received <= 0) {
                if (bytes_received == 0) {
                    printf("Client disconnected. Closing client socket.\n");
                } else {
                    perror("recv failed");
                }
                close(client_socket);
                break;
            }
            buffer[bytes_received] = '\0';

            int number = atoi(buffer);

            if (isEven(number)) {
                snprintf(buffer, MAX_BUFFER_SIZE, "%d is even\n", number);
            } else {
                snprintf(buffer, MAX_BUFFER_SIZE, "%d is odd\n", number);
            }

            // Send the result back to the client
            if (send(client_socket, buffer, strlen(buffer), 0) == -1) {
                perror("send failed");
                close(client_socket);
                break;
            }

            printf("Result sent to client: %s", buffer);
        }
    }

    close(server_socket);

    return 0;
}
