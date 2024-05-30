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

    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = inet_addr(SERVER_IP);

    if (connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1) {
        perror("connect failed");
        exit(EXIT_FAILURE);
    }

    printf("Connected to server at %s:%d\n", SERVER_IP, PORT);

    ssize_t bytes_received = recv(client_socket, buffer, MAX_BUFFER_SIZE, 0);
    if (bytes_received == -1) {
        perror("recv failed");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    buffer[bytes_received] = '\0';

    printf("Available files:\n%s\n", buffer);

    printf("Enter the file name: ");
    fgets(buffer, MAX_BUFFER_SIZE, stdin);

    buffer[strcspn(buffer, "\n")] = '\0';

    // Send the file name to the server
    if (send(client_socket, buffer, strlen(buffer), 0) == -1) {
        perror("send failed");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    printf("File contents:\n");
    while (1) {
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

        fwrite(buffer, 1, bytes_received, stdout);

        if (bytes_received < MAX_BUFFER_SIZE) {
            break;
        }
    }

    close(client_socket);

    return 0;
}
