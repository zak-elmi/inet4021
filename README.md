### HANDLING CLIENT REQUESTS AND SENDING HTTP RESPONSES
This server is able to successfully accept and handle the requests of multiple clients
simultaneously. It is able to do this by creating a new pthread each time a client
connects, and handling their request in the function 'handle_request()'. From here, the
HTTP request is parsed into a 'request_data' structure by the function 'populate_request()',
and the config file is parsed into a 'config_data' structure by the function 'populate_config()'.
Both of these structures are defined in the header file server.h. The actual construction
and writing of responses to the socket is done in the file httpResponses.c. Here, there are
functions for sending a number of different response types to the client, and for each there
is a Simple-Response and a Full-Response version depending on the request protocol. There are
individual helper functions for sending text files (such as html & css), graphics files, and
hard-coded responses. There are Response functions for all 4xx and 5xx status codes (although
some of these are not actually used by the server), in addition to a 200 OK response. These
are all functional, and the 200 OK response works for all text/html, text/css, image/gif, and
image/jpeg file types.

### WEBSITE
There is a simple but functional website located in the /html/ directory, in addition to the
index.html file located in root /Lab1/ directory. The website has four pages:
1. index.html - A basic "homepage" containing no graphics
2. gifsTable.html - Contains a table, where a unique gif is displayed when the user hovers their mouse over an individual cell of the table. The browser sends an HTTP Request each time a different gif is displayed.
3. imagesTable.html - The same premise as gifsTable.html, but with jpg images instead.
4. cgi.html - Allows the user to input two integer values and add them together.

### COMPILATION
A makefile is included in the root directory for easy compilation. After compilation run 
./server to start the server. Modifications may need to be made to Makefile depending on the OS.

### LOG FILES
There are two log files: access.log and error.log. All stderr output is redirected to
error.log, and all stdout output is redirected to access.log

### CGI
The server is able to identify a CGI request and grab the query_string, but at this point
neither CGI Get or Post are functioning. A 501 Not Implemented response if given instead.

#Note: All development and testing was done on macOS, and WSL: Ubuntu

## HOW TO RUN
1. Edit ./conf/httpd.conf to contain correct configuration data
2. Run "make" (Makefile may need to be edited based on user OS)
3. Run ./server
4. Navigate to the URI localhost:[port] from a web browser