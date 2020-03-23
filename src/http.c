/***********************************************************
 * Filename: httpResponses.c
 * Description: Handles the creation and sending of HTTP 1.0
 *      responses for server.c
 * Author: Will Firth (firth008)
 * Course: INET 4021
 * Professor: Mark Langanki
 * Project: HTTP 1.0 server with CGI and Concurrency
 * Due Date: April 1st, 2019
 *
 * Note: Descriptions of functions are given as comments at
 *      their declaration in the header file (server.h), and
 *      all HTTP Responses are based on RFC/1945
 ***********************************************************/

/***************************************************************************************
 * HEADERS
 **************************************************************************************/

#include "server.h"

/***************************************************************************************
 * HELPER FUNCTIONS
 **************************************************************************************/

/* Note: Code for this function is based on that found at the following link -
https://stackoverflow.com/questions/7548759/generate-a-date-string-in-http-response-date-format-in-c */
int get_current_date (char *current) {
    char buf[MAX_STRING_LENGTH];
    time_t now = time(0);
    struct tm tm = *gmtime(&now);
    strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S %Z", &tm);
    sprintf(current, "Date: %s\r\n", buf);
    return(0);
}

int get_last_modified_date (char *path, char *last) {
    struct stat data;
    stat(path, &data);
    sprintf(last, "Last-Modified: %s\r\n", ctime(&data.st_mtime));
    return(0);
}

int send_general_headers (int client) {
    /* Generate string headers */
    General_Header actual_gen_headers;
    General_Header *gen_headers = &actual_gen_headers;
    get_current_date(gen_headers->date);
    sprintf(gen_headers->server, "Server: %s\r\n", SERVER);
    sprintf(gen_headers->connection, "Connection: close\r\n");
    /* Send headers */;
    write(client, gen_headers->date, strlen(gen_headers->date));
    write(client, gen_headers->server, strlen(gen_headers->server));
    write(client, gen_headers->connection, strlen(gen_headers->connection));
    return(0);
}

int get_content_type (int file_type, char *content_type) {
    char type[16];
    /* Set the data type of the entity-body */
    switch (file_type) {
        case 0:
            strcpy(type, HTML);  // "text/html"
            break;
        case 1:
            strcpy(type, GRAPHIC_IMAGE);  // "image/jpeg"
            break;
        case 2:
            strcpy(type, GRAPHIC_GIF);  // "image/gif"
            break;
        default:
            strcpy(type, CSS);     // "text/css"
            break;
    }
    sprintf(content_type, "Content-Type: %s\r\n", type);
    return(0);
}

int send_text_file (int client, Entity_Header *ent_headers, request_data *request, int simple) {
    int fd = open(request->abs_path, O_RDONLY);
    if (fd == -1) {
        fprintf(stderr, "Unable to open file: %s\n", request->abs_path);
        if (simple) simple_internal_server_error(client);
        else full_internal_server_error(client, request);
        return(-1);
    }
    char buffer[TEXT_ENTITY_BODY_SIZE];
    int size = read(fd, buffer, TEXT_ENTITY_BODY_SIZE); // File copied into entity body
    if (size == -1) {
        fprintf(stderr, "Unable to read file: %s\n", request->abs_path);
        if (simple) simple_internal_server_error(client);
        else full_internal_server_error(client, request);
        return(-1);
    }
    /* Set size of Entity-Body in bytes */
    if (!simple) {
        sprintf(ent_headers->content_length, "Content-Length: %i\r\n\r\n", size);
        write(client, ent_headers->content_type, strlen(ent_headers->content_type));
        write(client, ent_headers->content_length, strlen(ent_headers->content_length));  // Send content-length header
    }
    write(client, buffer, strlen(buffer));  // Send Entity-Body
    return(0);
}


int send_graphics_file (int client, Entity_Header *ent_headers, request_data *request, int simple) {
    FILE *file = NULL;
    unsigned char buf[MAX_ENTITY_BODY_SIZE];
    size_t bytesRead = 0;
    long size = 0;

    file = fopen(request->abs_path, "rb");

    if (file == NULL) {     // Make sure file was opened successfully
        fprintf(stderr, "Unable to open file: %s\n", request->abs_path);
        if (simple) simple_internal_server_error(client);
        else full_internal_server_error(client, request);
        return(0);
    }
    /* Determine size of the file */
    fseek(file, 0, SEEK_END);   // Go to end of file to determine its length
    size = ftell(file);
    rewind(file);   // Point back to beginning of file

    if (size <= 0) {    // Make sure file was traversed successfully
        fprintf(stderr, "Unable to read file: %s\n", request->abs_path);
        if (simple) simple_internal_server_error(client);
        else full_internal_server_error(client, request);
        return(-1);
    }
    /* If Full-Response, send content-length and content-type to client */
    if (!simple) {
        sprintf(ent_headers->content_length, "Content-Length: %li\r\n\r\n", size);   // Send content-length header
        write(client, ent_headers->content_type, strlen(ent_headers->content_type));
        write(client, ent_headers->content_length, strlen(ent_headers->content_length));
    }
    /* Read file and send - if file is too large for the buffer then it will
        be sent in chunks until it is all sent */
    while ((bytesRead = fread(buf, 1, sizeof(buf), file)) > 0) {
        write(client, buf, bytesRead);      // Send Entity-Body
    }
    return(0);
}

int send_file_as_entity (int client, request_data *request, int simple) {
    Entity_Header actual_ent_headers;
    Entity_Header *ent_headers = &actual_ent_headers;
    get_content_type(request->file_type, ent_headers->content_type);
    /* Send either a text or binary body depending on the requested file */
    if (request->file_type <= 0) send_text_file(client, ent_headers, request, simple);
    else send_graphics_file(client, ent_headers, request, simple);
    return(0);
}

int send_string_as_entity (int client, request_data *request, char *body) {
    /* Generate string headers */
    Entity_Header actual_ent_headers;
    Entity_Header *ent_headers = &actual_ent_headers;
    sprintf(ent_headers->content_type, "Content-Type: %s\r\n", HTML);
    sprintf(ent_headers->content_length, "Content-Length: %lu\r\n\r\n", strlen(body));
    /* Send entity */
    write(client, ent_headers->content_type, strlen(ent_headers->content_type));
    write(client, ent_headers->content_length, strlen(ent_headers->content_length));
    write(client, body, strlen(body));
    return(0);
}


/***************************************************************************************
 * SIMPLE RESPONSES
 **************************************************************************************/

//** 2XX SUCCESS RESPONSES **//

void simple_ok (int client, request_data *request) {
    /* Send HTTP Simple-Response to socket */
    send_file_as_entity(client, request, 1);
}

//* 3XX REDIRECTION RESPONSES *//


//** 4XX CLIENT ERROR RESPONSES **//

void simple_bad_request (int client) { // 400
    char buf[BUFFER_SIZE];
    sprintf(buf, "<html><h1>Client Error 400</h1><p>Server could not understand the request due to malformed syntax.</p></html>\r\n");
    write(client, buf, strlen(buf));
}

void simple_unauthorized (int client) { // 401
    char buf[BUFFER_SIZE];
    sprintf(buf, "<html><h1>Client Error 401</h1><p>The request requires user authentication.</p></html>\r\n");
    write(client, buf, strlen(buf));
}

void simple_forbidden (int client) { // 403
    char buf[BUFFER_SIZE];
    sprintf(buf, "<html><h1>Client Error 403</h1><p>The server refuses to fulfill this request.</p></html>\r\n");
    write(client, buf, strlen(buf));
}

void simple_not_found (int client) { // 404
    char buf[BUFFER_SIZE];
    sprintf(buf, "<html><h1>Client Error 404</h1><p>The server has not found anything matching the Request-URI.</p></html>\r\n");
    write(client, buf, strlen(buf));
}

//** 5XX SERVER ERROR RESPONSES **//

void simple_internal_server_error (int client) { // 500
    char buf[BUFFER_SIZE];
    sprintf(buf, "<html><h1>Server Error 500</h1><p>Server encountered an unexpected condition which prevented it from fulfilling the request.</p></html>\r\n");
    write(client, buf, strlen(buf));
}

void simple_not_implemented (int client) { // 501
    char buf[BUFFER_SIZE];
    sprintf(buf, "<html><h1>Server Error 501</h1><p>Server does not support the functionality required to fulfill the request.</p></html>\r\n");
    write(client, buf, strlen(buf));
}

void simple_bad_gateway (int client) { // 502
    char buf[BUFFER_SIZE];
    sprintf(buf, "<html><h1>Server Error 502</h1><p>Server received an invalid response from an upstream server.</p></html>\r\n");
    write(client, buf, strlen(buf));
}

void simple_service_unavailable (int client) { // 503
    char buf[BUFFER_SIZE];
    sprintf(buf, "<html><h1>Server Error 503</h1><p>Server is temporarily unable to handle the request.</p></html>\r\n");
    write(client, buf, strlen(buf));
}

/***************************************************************************************
 * FULL RESPONSES
 **************************************************************************************/

//** 2XX SUCCESS RESPONSES **//
void full_ok (int client, request_data *request) {
    /* Generate status line */
    char status_line[MAX_RESPONSE_LINE_SIZE];
    sprintf(status_line, "%s 200 OK\r\n", request->protocol);
    /* Send HTTP Full-Response to socket */
    write(client, status_line, strlen(status_line));
    send_general_headers(client);
    send_file_as_entity(client, request, 0);
}

//* 3XX REDIRECTION RESPONSES *//


//** 4XX CLIENT ERROR RESPONSES **//

void full_bad_request (int client, request_data *request) { // 400
    /* Generate status line */
    char status_line[MAX_RESPONSE_LINE_SIZE];
    sprintf(status_line, "%s 400 Bad Request\r\n", request->protocol);
    /* Define Entity-Body */
    char body[MAX_STRING_LENGTH] =
        "<html><h1>Client Error 400</h1><p>Server could not understand the request due to malformed syntax.</p></html>";
    /* Send HTTP Full-Response to socket */
    write(client, status_line, strlen(status_line));
    send_general_headers(client);
    send_string_as_entity(client, request, body);
}

void full_unauthorized (int client, request_data *request) { // 401
    /* Generate status line */
    char status_line[MAX_RESPONSE_LINE_SIZE];
    sprintf(status_line, "%s 401 Unauthorized\r\n", request->protocol);
    /* Define Entity-Body */
    char body[MAX_STRING_LENGTH] =
        "<html><h1>Client Error 401</h1><p>The request requires user authentication.</p></html>";
    /* Send HTTP Full-Response to socket */
    write(client, status_line, strlen(status_line));
    send_general_headers(client);
    send_string_as_entity(client, request, body);
}

void full_forbidden (int client, request_data *request) { // 403
    /* Generate status line */
    char status_line[MAX_RESPONSE_LINE_SIZE];
    sprintf(status_line, "%s 403 Forbidden\r\n", request->protocol);
    /* Define Entity-Body */
    char body[MAX_STRING_LENGTH] =
        "<html><h1>Client Error 403</h1><p>The server refuses to fulfill this request.</p></html>";
    /* Send HTTP Full-Response to socket */
    write(client, status_line, strlen(status_line));
    send_general_headers(client);
    send_string_as_entity(client, request, body);
}

void full_not_found (int client, request_data *request) { // 404
    /* Generate status line */
    char status_line[MAX_RESPONSE_LINE_SIZE];
    sprintf(status_line, "%s 404 Not Found\r\n", request->protocol);

    char body[MAX_STRING_LENGTH] =
        "<html><h1>Client Error 404</h1><p>The server has not found anything matching the Request-URI.</p></html>";
    /* Send HTTP Full-Response to socket */
    write(client, status_line, strlen(status_line));
    send_general_headers(client);
    send_string_as_entity(client, request, body);
}

//** 5XX SERVER ERROR RESPONSES **//

void full_internal_server_error (int client, request_data *request) { // 500
    /* Generate status line */
    char status_line[MAX_RESPONSE_LINE_SIZE];
    sprintf(status_line, "%s 500 Internal Server Error\r\n", request->protocol);
    /* Define Entity-Body */
    char body[MAX_STRING_LENGTH] =
        "<html><h1>Server Error 500</h1><p>Server encountered an unexpected condition which prevented it from fulfilling the request.</p></html>";
    /* Send HTTP Full-Response to socket */
    write(client, status_line, strlen(status_line));
    send_general_headers(client);
    send_string_as_entity(client, request, body);
}

void full_not_implemented (int client, request_data *request) { // 501
    /* Generate status line */
    char status_line[MAX_RESPONSE_LINE_SIZE];
    sprintf(status_line, "%s 501 Method Not Implemented\r\n", request->protocol);
    /* Define Entity-Body */
    char body[MAX_STRING_LENGTH] =
        "<html><h1>Server Error 501</h1><p>Server does not support the functionality required to fulfill the request.</p></html>";
    /* Send HTTP Full-Response to socket */
    write(client, status_line, strlen(status_line));
    send_general_headers(client);
    send_string_as_entity(client, request, body);
}

void full_bad_gateway (int client, request_data *request) { // 502
    /* Generate status line */
    char status_line[MAX_RESPONSE_LINE_SIZE];
    sprintf(status_line, "%s 502 Bad Gateway\r\n", request->protocol);
    /* Define Entity-Body */
    char body[MAX_STRING_LENGTH] =
        "<html><h1>Server Error 502</h1><p>Server received an invalid response from an upstream server.</p></html>";
    /* Send HTTP Full-Response to socket */
    write(client, status_line, strlen(status_line));
    send_general_headers(client);
    send_string_as_entity(client, request, body);
}

void full_service_unavailable (int client, request_data *request) { // 503
    /* Generate status line */
    char status_line[MAX_RESPONSE_LINE_SIZE];
    sprintf(status_line, "%s 503 Service Unavailable\r\n", request->protocol);
    /* Define Entity-Body */
    char body[MAX_STRING_LENGTH] =
        "<html><h1>Server Error 503</h1><p>Server is temporarily unable to handle the request.</p></html>";
    /* Send HTTP Full-Response to socket */
    write(client, status_line, strlen(status_line));
    send_general_headers(client);
    send_string_as_entity(client, request, body);
}