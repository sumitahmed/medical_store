#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#define PORT 8080
#define MAX_CLIENTS 5

// Medicine structure
typedef struct {
    int id;
    char name[50];
    float price;
} Medicine;

Medicine medicines[100];  // Array to hold medicines
int medicine_count = 0;   // Count of medicines

// Function to handle client requests
void handleClient(SOCKET client_socket) {
    char buffer[1024];
    int bytesReceived;

    // Read the client's request
    bytesReceived = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
    if (bytesReceived <= 0) {
        return;
    }
    buffer[bytesReceived] = '\0';
    
    printf("Received request: %s\n", buffer);

    // Handle different client commands
    if (strcmp(buffer, "insert") == 0) {
        // Insert Medicine
        send(client_socket, "Enter Medicine ID: ", 18, 0);
        recv(client_socket, buffer, sizeof(buffer), 0);
        int id = atoi(buffer);
        
        send(client_socket, "Enter Medicine Name: ", 21, 0);
        recv(client_socket, buffer, sizeof(buffer), 0);
        char name[50];
        strcpy(name, buffer);

        send(client_socket, "Enter Medicine Price: ", 22, 0);
        recv(client_socket, buffer, sizeof(buffer), 0);
        float price = atof(buffer);

        medicines[medicine_count].id = id;
        strcpy(medicines[medicine_count].name, name);
        medicines[medicine_count].price = price;
        medicine_count++;

        send(client_socket, "Medicine inserted successfully!", 30, 0);
    } else if (strcmp(buffer, "delete") == 0) {
        // Delete Medicine
        send(client_socket, "Enter Medicine ID to delete: ", 27, 0);
        recv(client_socket, buffer, sizeof(buffer), 0);
        int id = atoi(buffer);

        int found = 0;
        for (int i = 0; i < medicine_count; i++) {
            if (medicines[i].id == id) {
                for (int j = i; j < medicine_count - 1; j++) {
                    medicines[j] = medicines[j + 1];
                }
                medicine_count--;
                found = 1;
                break;
            }
        }

        if (found) {
            send(client_socket, "Medicine deleted successfully!", 30, 0);
        } else {
            send(client_socket, "Medicine not found!", 21, 0);
        }
    } else if (strcmp(buffer, "update") == 0) {
        // Update Medicine
        send(client_socket, "Enter Medicine ID to update: ", 27, 0);
        recv(client_socket, buffer, sizeof(buffer), 0);
        int id = atoi(buffer);

        int found = 0;
        for (int i = 0; i < medicine_count; i++) {
            if (medicines[i].id == id) {
                send(client_socket, "Enter new Medicine Name: ", 24, 0);
                recv(client_socket, buffer, sizeof(buffer), 0);
                strcpy(medicines[i].name, buffer);

                send(client_socket, "Enter new Medicine Price: ", 25, 0);
                recv(client_socket, buffer, sizeof(buffer), 0);
                medicines[i].price = atof(buffer);

                found = 1;
                break;
            }
        }

        if (found) {
            send(client_socket, "Medicine updated successfully!", 30, 0);
        } else {
            send(client_socket, "Medicine not found!", 21, 0);
        }
    } else if (strcmp(buffer, "list") == 0) {
        // List all Medicines
        if (medicine_count == 0) {
            send(client_socket, "No medicines available.", 23, 0);
        } else {
            char medicine_list[1024] = "";
            for (int i = 0; i < medicine_count; i++) {
                char temp[100];
                sprintf(temp, "ID: %d, Name: %s, Price: %.2f\n", medicines[i].id, medicines[i].name, medicines[i].price);
                strcat(medicine_list, temp);
            }
            send(client_socket, medicine_list, strlen(medicine_list), 0);
        }
    }

    // Close the connection after handling the request
    closesocket(client_socket);
}

int main() {
    WSADATA wsaData;
    SOCKET server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    int client_len = sizeof(client_addr);

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed\n");
        return 1;
    }

    // Create a socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == INVALID_SOCKET) {
        printf("Socket creation failed\n");
        WSACleanup();
        return 1;
    }

    // Setup the server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind the socket
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("Bind failed\n");
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    // Listen for incoming connections
    if (listen(server_socket, MAX_CLIENTS) == SOCKET_ERROR) {
        printf("Listen failed\n");
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    printf("Server listening on port %d...\n", PORT);

    // Accept connections
    while (1) {
        client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_len);
        if (client_socket == INVALID_SOCKET) {
            printf("Accept failed\n");
            closesocket(server_socket);
            WSACleanup();
            return 1;
        }

        // Handle the client request
        handleClient(client_socket);
    }

    closesocket(server_socket);
    WSACleanup();
    return 0;
}
