#
# build webserver and webclient 
# 

CC=gcc

webserver: webserver.c
	  gcc webserver.c -o webserver

webclient: webclient.c
	  gcc webclient.c -o webclient

all: 
	  gcc  webserver.c -o webserver
	  gcc webclient.c -o webclient
clean:
	rm webserver webclient 
