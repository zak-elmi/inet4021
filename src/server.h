#ifndef MYFILE_H_  
#define MYFILE_H_
void acceptConnections();
void error_logging(int i, char *msg);
void reaper(int sig);
void callCGI(int socket, char *args[]);
void getServerConfig();
void sendError(int socket, char *errMsg);
#define BUFSIZE 2*1024

#endif