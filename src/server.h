/***********************************************************
 * Filename: server.h
 * Description: Header file for all C files in src
 * Author: Will Firth (firth008)
 * Course: INET 4021
 * Professor: Mark Langanki
 * Project: HTTP 1.0 server with CGI and Concurrency
 * Due Date: April 1st, 2019
 ***********************************************************/

/***************************************************************************************
 * HEADERS
 **************************************************************************************/

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <pthread.h>
#include <ctype.h>
#include <time.h>
#include <stdio.h>
#include <fcntl.h>

/***************************************************************************************
 * MACRO DEFINITIONS
 **************************************************************************************/

/* General definitions */
#define ISspace(x) isspace((int)(x))        // function that identifies spaces in a string
#define MAX_PATH 255                        // maximum number of characters allowed in root directory path
#define MAX_IDX 128                         // maximum number of characters allowed in index file name
#define BUFFER_SIZE 1024                    // standard size of buffer
#define MAX_STRING_LENGTH 255               // maximum number of characters allowed in single string
#define MAX_RESPONSE_LINE_SIZE 512          // maximum size of a single line of an HTTP Response
#define MAX_WORD_COUNT 255                  // maximum number of words in configuration file
#define MAX_ENTITY_BODY_SIZE 32768          // maximum size of entity-body when responding with graphics file
#define TEXT_ENTITY_BODY_SIZE 8096          // maximum size of entity-body when responding with text file
#define CONFIG_FILE "conf/httpd.conf"       // path to config file from root directory
#define SERVER "ServiceAce"                 // server name

/* Keywords for identifying relevant values in httpd.conf via string matching */
#define MAX_CONNECTIONS_KWD "MAX_CONNECTIONS"
#define ROOT_KWD "ROOT"
#define INDEX_KWD "INDEX"
#define PORT_KWD "PORT"

/* MIME types */
#define HTML "text/html"
#define CSS "text/css"
#define GRAPHIC_IMAGE "image/jpeg"
#define GRAPHIC_GIF "image/gif"

/* Allowed graphics file types */
#define JPG "jpg"
#define GIF "gif"

#define HTTP09 "HTTP/0.9"

/***************************************************************************************
 * STRUCTURE DEFINITIONS
 **************************************************************************************/

// config_data: holds configuration data that will be read in from httpd.conf
typedef struct {
    int max_connections;        // maximum number of connections allowed on the server
    char root_path[MAX_PATH];   // path to root directory
    char index[MAX_IDX];        // index file name
    int port;                   // port number
} config_data;

// request_data: holds data from the request received from the client
typedef struct {
    char type[MAX_STRING_LENGTH];       // Type of request (GET or POST)
    char rel_path[MAX_STRING_LENGTH];   // relative path (given in request)
    char abs_path[MAX_STRING_LENGTH];   // absolute path (root_path + "/" + rel_path)
    char protocol[MAX_STRING_LENGTH];
    int file_type;                // -1 = css, 0 = html, 1 = .jpg, 2 = .gif
} request_data;

typedef struct {
    char date[MAX_RESPONSE_LINE_SIZE];
    char server[MAX_RESPONSE_LINE_SIZE];
    char connection[MAX_RESPONSE_LINE_SIZE];
} General_Header;

// Entity_Header: store header fields for an Entity
typedef struct {
    char content_type[MAX_RESPONSE_LINE_SIZE];
    char content_length[MAX_RESPONSE_LINE_SIZE];
    char last_modified[MAX_RESPONSE_LINE_SIZE];
} Entity_Header;

// entity_body: stores an entity_body for an HTTP Response
typedef struct {
    Entity_Header header_fields;
    char body[MAX_ENTITY_BODY_SIZE];    // enity-body data as a string
} Entity;

/* Full_Response: stores all data needed to complete an HTTP Full-Response */
typedef struct {
    char status_line[MAX_RESPONSE_LINE_SIZE];
    General_Header gen_headers;
    Entity entity;
} Full_Response;

/***************************************************************************************
 * FUNCTION DECLARATIONS
 **************************************************************************************/

/* server.c functions */
int populate_config ();                                      // populate the given config_data object
int populate_request (char *buf, request_data *request);     // populate the given request_data object
int send_get_response (int client, request_data *request);          // identify and send the corresponding response to a GET request
void *handle_request (int client);                                    // handle a request from a specific client
int valid_path (char *abs_path);                             // determine whether a Request-URI is valid
int file_type (char *path);                                      // determine what type of graphic file is being requested
int get_query_string (char *query_string, char *uri);           // for a cgi request, get the query string
int handle_cgi (int client, request_data *request);             // handle a cgi request from a client

/* httpResponses.c functions */
int get_current_date (char *current);    // Get the current date in rfc1123 format
int get_last_modified_date (char *path, char *last);    // Get the last date the given file was modified in rfc1123 format
int send_file_as_entity (int client, request_data *request, int simple);    // Send an entity-body along with its content-length header
int send_text_file (int client, Entity_Header *ent_headers, request_data *request, int simple);
int send_graphics_file (int client, Entity_Header *ent_headers, request_data *request, int simple);
int get_content_type (int file_type, char *content_type);
int send_general_headers (int client);
int send_string_as_entity (int client, request_data *request, char *body);

void simple_ok (int client, request_data *request);    // Simple 200

void simple_bad_request (int client);                  // Simple 400
void simple_unauthorized (int client);                 // Simple 401
void simple_forbidden (int client);                    // Simple 403
void simple_not_found (int client);                    // Simple 404

void simple_internal_server_error (int client);        // Simple 500
void simple_not_implemented (int client);              // Simple 501
void simple_bad_gateway (int client);                  // Simple 502
void simple_service_unavailable (int client);          // Simple 503

void full_ok (int client, request_data *request);                             // Full 200

void full_bad_request (int client, request_data *request);                    // Full 400
void full_unauthorized (int client, request_data *request);                   // Full 401
void full_forbidden (int client, request_data *request);                      // Full 403
void full_not_found (int client, request_data *request);                      // Full 404

void full_internal_server_error (int client, request_data *request);          // Full 500
void full_not_implemented (int client, request_data *request);                // Full 501
void full_bad_gateway (int client, request_data *request);                    // Full 502
void full_service_unavailable (int client, request_data *request);            // Full 503