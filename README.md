***Authors - Lucas Traynor & Zakaria Elmi***

INET 4021 Lab 1 README

The purpose of LAB 1 is to build an HTTP web server based on RCF/1945 guidelines and HTTP 1.0 protocols.

***Website:***
There is a simple but functional website located in the /WebTemplate/ directory. This website template has been taken from Overview 4 with an approval from Mark to showcase a working server

***CGI:***
Server can identify a CGI request and grab the query_string, but at this point neither CGI Get or Post are functioning.

***Compilation:***
Makefile is included in the root directory for compilation. After compilation run "./server" to start the server. 

***Client Requests and HTTP Responses:***
This server can accept and handle the requests of clients concurrently. It does this by generating a new pthread when a client connects, and handles their request in the ______ function. The HTTP request is then broken into two structures--one for request_data and the other for config_data--with both structures being defined within server.h. The construction and writing of responses to the socket is completed within httpresponse.c 


***Log Files:***
There are 2 log files: access.log and error.log. 

***Current Issues***
Though the server is fully working, there has been some issues with displaying certain content from the Webpage.
***Run Instructions:***
Edit ./conf/httpd.conf to contain correct configuration data 
Run "make"  
Run ./server 
Navigate to the URI localhost:[port] from a web browser






