#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#define PORT 8080
#define SERVER_ADDR "127.0.0.1"
#define BUFFER_SIZE 1024

void insertMedicine(SOCKET socket);
void deleteMedicine(SOCKET socket);
void updateMedicine(SOCKET socket);
void listMedicines(SOCKET socket);
void sendRequest(SOCKET socket, const char* request);
void receiveResponse(SOCKET socket);

int main() {
    WSADATA wsaData;
    SOCKET client_socket;
    struct sockaddr_in server_addr;

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed\n");
        return 1;
    }

    // Create socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == INVALID_SOCKET) {
        printf("Socket creation failed\n");
        WSACleanup();
        return 1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_ADDR);
    server_addr.sin_port = htons(PORT);

    // Connect to the server
    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("Connection failed\n");
        closesocket(client_socket);
        WSACleanup();
        return 1;
    }

    printf("Connected to server at %s:%d\n", SERVER_ADDR, PORT);

    // Menu for client operations
    int choice;
    while (1) {
        printf("\n1. Insert Medicine\n2. Delete Medicine\n3. Update Medicine\n4. List Medicines\n5. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        getchar();  // to consume the newline character

        switch (choice) {
            case 1:
                insertMedicine(client_socket);
                break;
            case 2:
                deleteMedicine(client_socket);
                break;
            case 3:
                updateMedicine(client_socket);
                break;
            case 4:
                listMedicines(client_socket);
                break;
            case 5:
                closesocket(client_socket);
                WSACleanup();
                return 0;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    }

    // Close the socket and cleanup
    closesocket(client_socket);
    WSACleanup();
    return 0;
}

void insertMedicine(SOCKET socket) {
    char name[50], manufacturer[50];
    float price;
    int quantity;
    char request[BUFFER_SIZE];

    printf("Enter medicine name: ");
    fgets(name, sizeof(name), stdin);
    name[strcspn(name, "\n")] = 0;  // Remove trailing newline

    printf("Enter manufacturer: ");
    fgets(manufacturer, sizeof(manufacturer), stdin);
    manufacturer[strcspn(manufacturer, "\n")] = 0;  // Remove trailing newline

    printf("Enter price: ");
    scanf("%f", &price);

    printf("Enter quantity: ");
    scanf("%d", &quantity);
    getchar();  // consume the newline character

    // Prepare the insert request
    snprintf(request, sizeof(request), "POST /insert HTTP/1.1\r\n"
                                       "Content-Type: application/x-www-form-urlencoded\r\n"
                                       "Content-Length: %lu\r\n\r\n"
                                       "name=%s&manufacturer=%s&price=%.2f&quantity=%d",
             strlen(name) + strlen(manufacturer) + sizeof(price) + sizeof(quantity) + 50, name, manufacturer, price, quantity);

    sendRequest(socket, request);
    receiveResponse(socket);
}

void deleteMedicine(SOCKET socket) {
    char name[50];
    char request[BUFFER_SIZE];

    printf("Enter medicine name to delete: ");
    fgets(name, sizeof(name), stdin);
    name[strcspn(name, "\n")] = 0;  // Remove trailing newline

    // Prepare the delete request
    snprintf(request, sizeof(request), "POST /delete HTTP/1.1\r\n"
                                       "Content-Type: application/x-www-form-urlencoded\r\n"
                                       "Content-Length: %lu\r\n\r\n"
                                       "name=%s", strlen(name) + 5, name);

    sendRequest(socket, request);
    receiveResponse(socket);
}

void updateMedicine(SOCKET socket) {
    char name[50], manufacturer[50];
    float price;
    int quantity;
    char request[BUFFER_SIZE];

    printf("Enter medicine name to update: ");
    fgets(name, sizeof(name), stdin);
    name[strcspn(name, "\n")] = 0;  // Remove trailing newline

    printf("Enter new manufacturer: ");
    fgets(manufacturer, sizeof(manufacturer), stdin);
    manufacturer[strcspn(manufacturer, "\n")] = 0;  // Remove trailing newline

    printf("Enter new price: ");
    scanf("%f", &price);

    printf("Enter new quantity: ");
    scanf("%d", &quantity);
    getchar();  // consume the newline character

    // Prepare the update request
    snprintf(request, sizeof(request), "POST /update HTTP/1.1\r\n"
                                       "Content-Type: application/x-www-form-urlencoded\r\n"
                                       "Content-Length: %lu\r\n\r\n"
                                       "name=%s&manufacturer=%s&price=%.2f&quantity=%d",
             strlen(name) + strlen(manufacturer) + sizeof(price) + sizeof(quantity) + 50, name, manufacturer, price, quantity);

    sendRequest(socket, request);
    receiveResponse(socket);
}

void listMedicines(SOCKET socket) {
    char request[BUFFER_SIZE];

    // Prepare the list request
    snprintf(request, sizeof(request), "GET /list HTTP/1.1\r\n"
                                       "Content-Type: text/html\r\n"
                                       "Content-Length: 0\r\n\r\n");

    sendRequest(socket, request);
    receiveResponse(socket);
}

void sendRequest(SOCKET socket, const char* request) {
    send(socket, request, strlen(request), 0);
}

void receiveResponse(SOCKET socket) {
    char buffer[BUFFER_SIZE];
    int recv_size = recv(socket, buffer, BUFFER_SIZE, 0);

    if (recv_size == SOCKET_ERROR) {
        printf("Error receiving response\n");
        return;
    }

    buffer[recv_size] = '\0';  // Null-terminate the response
    printf("Server response: \n%s\n", buffer);
}
