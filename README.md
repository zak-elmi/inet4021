This is the README for Lab1 INET4021 Spring 2016.

***************************************
Authors:
***************************************
	Mohamed Yunis: noorx004@umn.edu
	Mohamoud Egal: egalx011@umn.edu

****************************************
University Machine tested on: 
****************************************
KH4250-41

****************************************
Files/Directories:
****************************************
src/ : Where the source codes for the webserver and cgi are
   webServer.c: File that contains source code for the webserver implementation
   cgi.cpp    : File that contains source code for the cgi written in C++ language. 
   helper_funcs: File containing functions implemetned in the webserver
   webServer.h: Header file 

conf/: Where configurations files used in this project reside. 
    http.conf: where the port, number of connections, and root directory for looking files are. 
    mime.type: where the types of extensions accepted by the server reside. This is pulled from the apache http server project. 

cgi-bin/: Where the executable of the webserver and the cgi are found. 

log/    : Where the log files are found. If this directory doesn't originally exist, then the directory will be created during execution. 
   access.log: keeps logging of server accesses
   error.log : keeps error loggings to the server. 

web/    : Contains sample html/htm file for testing. Some of these files (Bill of Right and Amendements) are from are the university
	 site are used for testing like I had done in a previous class. 

	images/ : subfolder : Contain images that can be used for testing. Similar folder can be found under the web folder. 


***********************************************
Syntax and Usage:
***********************************************
From the Webserver folder, navigate to the src directory.
>cd src
From this directory you can run the make file. 
>make
This will compile the webserver and cgi and move the executables to the cgi-bin and also remove older executables
Then navigate to the cgi-bin directory
> cd ../cgi-bin
Then you can run the webserver executable. 
>./webServer

This will pull the port, number of maximnum allowable process and other essential information for running the webserver.

Then open up a browser of your choice and request one of the test htm/html or images provided by doing something like:

http://localhost:9001/cgi-bin/index.html or
http://localhost:9001/cgi-bin/Bill.htm
http://localhost:9001/cgi-bin/fancy.gif
http://localhost:9001/cgi-bin/images/fancy.gif

If you are remotely running the program from another machine, say like using ssh, then you can substitute the localhost part with ip address of the machine the program is running on. This had been tested and works! For example:

http://128.101.37.41:9001/cgi-bin/images/fancy.gif
http://128.101.37.41:9001/cgi-bin/fancy.gif

NOTE:
This webserver had been designed to be interactive (by displaying information on the terminal). and this can be seen on the terminal when you are running the program. 
In the case of essential errors, like file not opening up, or not being created, it has been designed to output the exact format of the error on the termninal. This was done to help us during the program development and to make grading easier. 
The process handling a certain request can be known by the number of "Server accepted a connection.." messages displayed on the terminal. 

***********************************************
Issues:
***********************************************
One of the issue found during the testing of the webserver project is ":Address already in use error message". Since the port number is not 
specified by user but gets pulled from the conf folder, the operating system may assign that same port for something else. This is not for long and the port can be used in a very short while. 

## HOW TO RUN
1. Edit ./conf/httpd.conf to contain correct configuration data
2. Run "make" (Makefile may need to be edited based on user OS)
3. Run ./server
4. Navigate to the URI localhost:[port] from a web browser
