/***********************************************************
 * Filename: server.c
 * Description: Basic HTTP 1.0 server based on RFC/1945
 * Author: Will Firth (firth008)
 * Course: INET 4021
 * Professor: Mark Langanki
 * Project: HTTP 1.0 server with CGI and Concurrency
 * Due Date: April 1st, 2019
 *
 * Note: Descriptions of functions are given as comments at
 *      their declaration in the header file (server.h)
 ***********************************************************/

/***************************************************************************************
 * HEADERS & GLOBAL VARIABLES
 **************************************************************************************/

#include "server.h"

// Global config_data object for storing configuration data
config_data actual_config;
config_data *config = &actual_config;
// Used for keeping track of the number of connections to the server
int num_connections = 0;

/***************************************************************************************
 * HELPER FUNCTIONS
 **************************************************************************************/

int populate_config () {
    FILE *cf;   // configuration file
    char mode = 'r';    // read-only mode
    char *filename = CONFIG_FILE;

    cf = fopen(filename, &mode);    // open configuration file
    if (cf == NULL) {
        //  printf("Oh dear, something went wrong with read()! %s\n", strerror(errno));
        fprintf(stderr, "Unable to open config file: %s\n", filename);
    }

    char strings[MAX_WORD_COUNT][MAX_STRING_LENGTH];    // array of strings to hold config data
    char data[MAX_STRING_LENGTH];    // used to temporarily contain individual strings
    /* copy configuration data to 'strings' array */
    int i = 0;
    while (fscanf(cf, " %1023s", data) == 1) {
        strcpy(strings[i], data);
        i++;
    }

    /* identify relevent configuration data & populate config_data object */
    int j = 0;
    while (j < i) {
        if (!strcmp(strings[j], MAX_CONNECTIONS_KWD)) {     // identify allowable number of simultaneous connections
            config->max_connections = atoi(strings[j+1]);
        } else if (!strcmp(strings[j], ROOT_KWD)) {         // identify ROOT directory
            strcpy(config->root_path, strings[j+1]);
        } else if (!strcmp(strings[j], INDEX_KWD)) {        // identify INDEX filename
            strcpy(config->index, strings[j+1]);
        } else if (!strcmp(strings[j], PORT_KWD)) {         // identify PORT to run server on
            config->port = atoi(strings[j+1]);
        } j++;
    }
    fclose(cf); // close config file
    return(0);
}

int populate_request (char *buf, request_data *request) {
    int i=0, j=0;
    /* Copy request type to request_data object */
    while (!ISspace(buf[i]) && (j < MAX_STRING_LENGTH - 1)) {
        request->type[j] = buf[i];
        i++;
        j++;
    } i++;

    j = 0;
    /* Copy request path to request_data object */
    while (!ISspace(buf[i]) && (j < MAX_STRING_LENGTH - 1)) {
        request->rel_path[j] = buf[i];
        i++;
        j++;
    } i++;

    j = 0;
    /* Copy request protocol to request_data object */
    while (!ISspace(buf[i]) && (j < MAX_STRING_LENGTH - 1)) {
        request->protocol[j] = buf[i];
        i++;
        j++;
    }
    /* Generate the absolute path and set request->abs_path to it */
    strcpy(request->abs_path, config->root_path);   // ROOT path
    if (!strcmp(request->rel_path, "/")) strcat(request->rel_path, config->index);  // Account for defaut '/' input
    strcat(request->abs_path, request->rel_path);   // Append relative path

    request->file_type = file_type(request->rel_path); // Determine if request is for graphic or tex
    return(0);
}

int file_type (char *path) {
    char file_type[4];
    int i = 0, j = 0, copy = 0;
    /* Identify file type */
    while (i < strlen(path)) {
        if (copy) {
            if (j >= 3) break;
            file_type[j] = path[i];
            j++;
        }
        if (path[i] == '.') {
            copy = 1;
        } i++;
    }
    if (!strcmp(file_type, JPG)) return(1);          // request is for a .jpg
    else if (!strcmp(file_type, GIF)) return(2);     // request is for a .gif
    else if (!strcmp(file_type, "css")) return(-1);  // request is for a .css file
    else if (!strcmp(file_type, "cgi")) return(-2);  // request is for a .cgi script
    return(0);                                       // request is for none of the above
}
int valid_path (char *abs_path) {
    return(!access(abs_path, F_OK));   // Check if file exists, return truth value
}
int send_get_response (int client, request_data *request) {
    if (valid_path(request->abs_path)) {
        // Send corresponding 200 OK response
        if (!strcasecmp(request->protocol, HTTP09)) simple_ok(client, request);
        else full_ok(client, request);
    } else {
        // Send 404 Not Found response
        fprintf(stderr, "Invalid URI: %s\n", request->abs_path);
        if (!strcasecmp(request->protocol, HTTP09)) simple_not_found(client);
        else full_not_found(client, request);
    }
    return(0);
}

int get_query_string (char *query_string, char *uri) {
    int i = 0, j = 0, copy = 0;
    /* Identify file type */
    while (i < strlen(uri)) {
        if (copy) {
            query_string[j] = uri[i];
            j++;
        }
        if (uri[i] == '?') {
            copy = 1;
        } i++;
    }
    if (copy) return(1);
    return(0);
}

/***************************************************************************************
 * CGI REQUEST HANDLING
 **************************************************************************************/

int handle_cgi (int client, request_data *request) {

    fprintf(stderr, "Requested method is not imlemented: %s\n", request->type);
    if (!strcasecmp(request->protocol, HTTP09)) simple_not_implemented(client);
    else full_not_implemented(client, request);

    return(0);
}

/***************************************************************************************
 * CLIENT REQUEST HANDLING
 **************************************************************************************/

void *handle_request (int client) {
    ssize_t ssize;
    request_data request;
    char buffer[BUFFER_SIZE];

    /* receive message from client */
    ssize = recv(client, &buffer, sizeof(buffer), 0);
    if (ssize < 0)
            perror("Did not receive anything because ");
    else {
        fprintf(stdout,"Got \"%s\" from socket %i\n", buffer, client);
        fprintf(stderr,"String length = %lu \n", strlen(buffer));
    }

    populate_request(buffer, &request); // parse relevant data from client request

    if (request.file_type == -2) { // cgi request
        handle_cgi(client, &request);
    } else {
        if (!strcasecmp(request.type, "GET")) { // GET request
            send_get_response(client, &request);
        } else {    // Invalid request
            fprintf(stderr, "Requested method is not imlemented: %s\n", request.type);
            if (!strcasecmp(request.protocol, HTTP09)) simple_not_implemented(client);
            else full_not_implemented(client, &request);
        }
    }

    close(client);
    return NULL;
}

/***************************************************************************************
 * MAIN FUNCTION
 **************************************************************************************/

int main () {
    /* Redirect output to log files */
    int access = open("logs/access.log", O_RDWR|O_CREAT|O_APPEND, 0600);
    int error = open("logs/error.log", O_RDWR|O_CREAT|O_APPEND, 0600);
    if (access == -1) {
        perror("Unable to open logs/access.log because ");
        return(0);
    } else if (error == -1) {
        perror("Unable to open logs/error.log because ");
        return(0);
    }

    int stdout_dup = dup(fileno(stdout));
    int stderr_dup = dup(fileno(stderr));

    if (dup2(access, fileno(stdout)) == -1) {
        perror("Unable to redirect stdout because ");
        return(0);
    }
    if (dup2(error, fileno(stderr)) == -1) {
        perror("Unable to redirect stderr because ");
        return(0);
    }

    /* Run the server */
    populate_config(); // Populate global object 'config' with relevant data from httpd.conf

    int s, b;

	struct sockaddr_in sin;     // Socket address
    unsigned int addrlen;
	int new_socket;
	struct sockaddr_in address;
    pthread_t new_thread;

	bzero((char *) &sin, sizeof(sin));

    sin.sin_addr.s_addr = INADDR_ANY;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(config->port);

	s = socket(PF_INET, SOCK_STREAM, 0);
	if (s < 0) {
		perror("Could not create socket because ");
	}

	b = bind(s, (struct sockaddr *)&sin, sizeof(sin));
	if (b < 0) {
		perror("Could not bind to socket because ");
	}

	listen(s, 12);

    while (num_connections <= config->max_connections) {
        addrlen = sizeof(struct sockaddr_in);
        new_socket = accept(s, (struct sockaddr *)&address, &addrlen);
        if (new_socket<0) {
            perror("Accept connection");
        } else {
            fprintf(stderr, "got a connection\n");
        }
        num_connections++;
        if (pthread_create(&new_thread , NULL, handle_request, new_socket) != 0) perror("Unable to create pthread because ");
    }
    /* Redirect output back to console */
    fflush(stdout);
    close(access);
    fflush(stderr);
    close(error);

    dup2(stdout_dup, fileno(stdout));
    dup2(stderr_dup, fileno(stderr));

    close(stdout_dup);
    close(stderr_dup);

    close(s); // Close socket
    return 1;
}