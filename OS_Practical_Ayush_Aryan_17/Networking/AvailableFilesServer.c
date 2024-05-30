#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <dirent.h>

#define PORT 12345
#define MAX_BUFFER_SIZE 1024
#define FILES_DIRECTORY "./files"

// Function to get the list of available files in the directory
void getFilesList(char *filesList) {
    DIR *dir;
    struct dirent *entry;
    if ((dir = opendir(FILES_DIRECTORY)) != NULL) {
        while ((entry = readdir(dir)) != NULL) {
            if (entry->d_type == DT_REG) { // If it's a regular file
                strcat(filesList, entry->d_name);
                strcat(filesList, "\n");
            }
        }
        closedir(dir);
    }
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

    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT);

    if (bind(server_socket, (const struct sockaddr *)&server_address, sizeof(server_address)) == -1) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

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

        char filesList[MAX_BUFFER_SIZE] = "";
        getFilesList(filesList);

        if (send(client_socket, filesList, strlen(filesList), 0) == -1) {
            perror("send failed");
            close(client_socket);
            continue;
        }

        ssize_t bytes_received = recv(client_socket, buffer, MAX_BUFFER_SIZE, 0);
        if (bytes_received == -1) {
            perror("recv failed");
            close(client_socket);
            continue;
        }

        buffer[bytes_received] = '\0';

        char filePath[MAX_BUFFER_SIZE];
        snprintf(filePath, MAX_BUFFER_SIZE, "%s/%s", FILES_DIRECTORY, buffer);

        FILE *file = fopen(filePath, "r");
        if (file == NULL) {
            perror("fopen failed");
            close(client_socket);
            continue;
        }

        while (1) {
            ssize_t bytes_read = fread(buffer, 1, MAX_BUFFER_SIZE, file);
            if (bytes_read == 0) {
                if (feof(file)) {
                    printf("End of file reached. File sent.\n");
                } else {
                    perror("fread failed");
                }
                break;
            }

            ssize_t bytes_sent = send(client_socket, buffer, bytes_read, 0);
            if (bytes_sent == -1) {
                perror("send failed");
                fclose(file);
                close(client_socket);
                exit(EXIT_FAILURE);
            }
        }

        fclose(file);

        close(client_socket);
    }
    close(server_socket);

    return 0;
}
