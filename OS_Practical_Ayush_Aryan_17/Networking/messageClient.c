#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "unistd.h"
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 12345
#define SERVER_IP "127.0.0.1"
#define MAX_BUFFER_SIZE 1024

int main() {
    int client_socket;
    struct sockaddr_in server_address;
    char buffer[MAX_BUFFER_SIZE];

    // Create a UDP socket
    if ((client_socket = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Initialize server address
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = inet_addr(SERVER_IP);

    while (1) {
        // Prompt the user to enter a message
        printf("Enter a message ('exit' to quit): ");
        fgets(buffer, MAX_BUFFER_SIZE, stdin);

        // Remove trailing newline character
        buffer[strcspn(buffer, "\n")] = 0;

        // Send message to server
        sendto(client_socket, buffer, strlen(buffer), 0, (const struct sockaddr *)&server_address, sizeof(server_address));

        if (strcmp(buffer, "exit") == 0) {
            printf("Exiting...\n");
            break;
        }
    }

    // Close the socket
    close(client_socket);

    return 0;
}
