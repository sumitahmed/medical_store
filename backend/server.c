#include <microhttpd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define PORT 8080

// Medicine structure
typedef struct {
    int id;
    char name[50];
    float price;
} Medicine;

Medicine medicines[100];
int medicine_count = 0;

// Helper: Parse POST data
void parse_post_data(const char *data, int *id, char *name, float *price) {
    sscanf(data, "id=%d&name=%49[^&]&price=%f", id, name, price);
}

// Handler for HTTP requests
int handle_request(void *cls, struct MHD_Connection *connection,
                   const char *url, const char *method, 
                   const char *version, const char *upload_data, 
                   size_t *upload_data_size, void **con_cls) {
    static int post_recieved = 0;
    char response_buffer[2048] = "";
    const char *response_text;
    struct MHD_Response *response;
    int ret;

    // Handle POST data
    if (strcmp(method, "POST") == 0) {
        if (*upload_data_size > 0) {
            int id;
            char name[50];
            float price;

            parse_post_data(upload_data, &id, name, &price);
            if (strcmp(url, "/insert") == 0) {
                medicines[medicine_count].id = id;
                strcpy(medicines[medicine_count].name, name);
                medicines[medicine_count].price = price;
                medicine_count++;
                sprintf(response_buffer, "Medicine inserted: ID=%d, Name=%s, Price=%.2f", id, name, price);
            } else if (strcmp(url, "/update") == 0) {
                int found = 0;
                for (int i = 0; i < medicine_count; i++) {
                    if (medicines[i].id == id) {
                        strcpy(medicines[i].name, name);
                        medicines[i].price = price;
                        found = 1;
                        sprintf(response_buffer, "Medicine updated: ID=%d, Name=%s, Price=%.2f", id, name, price);
                        break;
                    }
                }
                if (!found) {
                    strcpy(response_buffer, "Medicine ID not found for update.");
                }
            }
            *upload_data_size = 0;
            post_recieved = 1;
            return MHD_YES;
        }
    }

    // Handle GET requests
    if (strcmp(method, "GET") == 0) {
        if (strcmp(url, "/list") == 0) {
            if (medicine_count == 0) {
                strcpy(response_buffer, "No medicines available.");
            } else {
                for (int i = 0; i < medicine_count; i++) {
                    char temp[256];
                    sprintf(temp, "ID: %d, Name: %s, Price: %.2f\n", medicines[i].id, medicines[i].name, medicines[i].price);
                    strcat(response_buffer, temp);
                }
            }
        } else if (strncmp(url, "/delete?", 8) == 0) {
            int id_to_delete;
            sscanf(url, "/delete?id=%d", &id_to_delete);
            int found = 0;
            for (int i = 0; i < medicine_count; i++) {
                if (medicines[i].id == id_to_delete) {
                    for (int j = i; j < medicine_count - 1; j++) {
                        medicines[j] = medicines[j + 1];
                    }
                    medicine_count--;
                    found = 1;
                    break;
                }
            }
            if (found) {
                sprintf(response_buffer, "Medicine with ID=%d deleted.", id_to_delete);
            } else {
                sprintf(response_buffer, "Medicine with ID=%d not found.", id_to_delete);
            }
        } else {
            strcpy(response_buffer, "Invalid endpoint. Use /list, /insert, /delete, or /update.");
        }
    }

    response_text = response_buffer;
    response = MHD_create_response_from_buffer(strlen(response_text), (void*)response_text, MHD_RESPMEM_PERSISTENT);
    ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
    MHD_destroy_response(response);
    return ret;
}

int main() {
    struct MHD_Daemon *server;

    // Start the HTTP server
    server = MHD_start_daemon(MHD_USE_SELECT_INTERNALLY, PORT, NULL, NULL,
                              &handle_request, NULL, MHD_OPTION_END);
    if (!server) {
        printf("Failed to start server\n");
        return 1;
    }

    printf("Server running on http://localhost:%d\n", PORT);
    getchar();  // Keep the server running
    MHD_stop_daemon(server);
    return 0;
}
