/* ============================================================================


usage:
	webserver [options]
options:
	-p port (default 8888)
	-t number of worker threads (default 1, range 1-1000)
	-f path to static files (default .)
	-h show help message
	
 
============================================================================ */
#include <arpa/inet.h>
#include <ctype.h>
#include <netdb.h>
#include <netinet/in.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

/* CONSTANTS =============================================================== */
#define BUFFER_SIZE 1024

void capitalizeBuffer(char *buffer) {
  do {
      *buffer = toupper((unsigned char) *buffer);
  } while (*buffer++);
}

int main(int argc, char **argv) {

  int socket_fd = 0;
  int client_socket_fd = 0;
  int args = 0;//number of command line arguments expected
  int server_port = 8888;
  int worker_threads = 1;
  int total_bytes = 0;
  int len = 0;
  int num_bytes = 0;
  int pool_count = 0;
  
  char buffer[BUFFER_SIZE];
  char static_path[100] = ".";
  char revbuf[BUFFER_SIZE]; // Receiver buffer
  
 
  
  int set_reuse_addr = 1; // ON == 1  
  int max_pending_connections = 1;

  struct sockaddr_in server;
  struct sockaddr_in client;
  struct hostent *client_host_info;
  char *client_host_ip;
  socklen_t client_addr_len; 
  
 

  for (args=1; args < argc; args = args + 2){
	 // printf ("checking %d \n", args);
	  
	  
	  if (strcmp("-p", argv[args]) == 0){		  
		  //printf ("%d %s %s \n",args, argv[args ], argv[ args + 1]);
		  server_port =  atoi (argv[ args + 1]);
		  printf("Setting server port to %d \n", server_port);
	  }
		  
	  if (strcmp("-t", argv[args]) == 0){
	  		//printf ("%d %s %s \n",args, argv[args ], argv[ args + 1]);
	  		worker_threads = atoi (argv[ args + 1]);
	  		printf("Setting worker threads to %d \n", worker_threads);
	  }
	  
	  if (strcmp("-f", argv[args]) == 0){
	  		  //printf ("%d %s %s \n",args, argv[args ], argv[ args + 1]);
	  		  strcpy (static_path, argv[args + 1]);
	  		  printf ("Setting static file path to %s \n",static_path );
	 }
	  
	  if (strcmp("-h", argv[args]) == 0){
	  	  		   
	  	  		  printf ("\n\nusage: \n");
	  	  		  printf("\t\twebserver [options]\n");
	  	  		  printf("options:\n");
	  	  		  printf("\t\t-p port (default 8888)\n");
	  	  		  printf("\t\t-t number of worker threads (default 1, range 1-1000)\n");
	  	  		  printf("\t\t-f pather to static files (default .)\n");
	  	  		  printf("\t\t-h show help message\n\n\n");
	  	  		  return 0;
	  	 }
	  
  }// end args for
  
  
  //create worker threads
  fprintf(stdout, "create a pool of worker threads %d \n", worker_threads );
  for (pool_count = 0; pool_count < worker_threads; pool_count++){
	  
	  fprintf(stdout, "creating thread %d \n", pool_count );
  }//for pool
  
  // Create socket 
  if (0 > (socket_fd = socket(AF_INET, SOCK_STREAM, 0))) {
    fprintf(stderr, "server failed to create the listening socket\n");
    exit(1);
  }//if create socket

  // Set socket 
  if (0 != setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &set_reuse_addr, sizeof(set_reuse_addr))) {
    fprintf(stderr, "server failed to set SO_REUSEADDR socket option (not fatal)\n");
  }//if set socket

  // Configure server socket address 
  bzero(&server, sizeof(server));
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = htonl(INADDR_ANY);
  server.sin_port = htons(server_port);

  // Bind the socket
  if (0 > bind(socket_fd, (struct sockaddr *)&server, sizeof(server))) {
    fprintf(stdout, "server failed to bind\n");
    exit(1);
  }//if bind

  // Listen on the socket for up to some maximum pending connections
  if (0 > listen(socket_fd, max_pending_connections)) {
    fprintf(stderr, "server failed to listen\n");
    exit(1);
  } else {
    fprintf(stdout, "server listening for a connection on port %d\n", server_port);
  }//if listen
  
  int success = 0;
      while(success == 0)
      {

         client_addr_len = sizeof(client);

 
          if (0 > (client_socket_fd = accept(socket_fd, (struct sockaddr *)&client, &client_addr_len))) 
          {
              fprintf(stdout, "ERROR: Obtaining new Socket Despcritor.\n");
              exit(1);
          }//if accept
          
          
          
          else 
              printf("[Server] Server connected from %s.\n", inet_ntoa(client.sin_addr));

          /*Receive File from Client */
          char* fr_name = "/dvlp/server_data/request.txt";
          FILE *fr = fopen(fr_name, "ab+");
          if(fr == NULL)
              printf("File %s Cannot be opened file on server.\n", fr_name);
          else
          {
              bzero(revbuf, BUFFER_SIZE); 
              int fr_block_sz = 0;
            
              while((fr_block_sz = recv(client_socket_fd, revbuf, BUFFER_SIZE, 0)) > 0) 
              {
                  int write_sz = fwrite(revbuf, sizeof(char), fr_block_sz, fr);
                  if(write_sz < fr_block_sz)
                  {
                      error("File write failed on server.\n");
                  }//
                
                  if (fr_block_sz == 0 || fr_block_sz != 1024) 
                  {
                      break;
                  }
              }
 
              printf("Ok received from client request for %s? \n", revbuf);
             fclose(fr); 
             printf("Ok received from client request for %s? \n", revbuf);
          
          }
    
          printf("[Server] Sending to the Client...");
              char* fs_name = strcat(static_path ,revbuf);
              char sdbuf[BUFFER_SIZE]; // Send buffer
              printf("[Server] Sending %s to the Client...", fs_name);
              FILE *fs = fopen(fs_name, "r");
              if(fs == NULL)
              {
                  fprintf(stdout, "ERROR: File not found on server.\n");
                  exit(1);
              }

              bzero(sdbuf, BUFFER_SIZE); 
              int fs_block_sz; 
              while((fs_block_sz = fread(sdbuf, sizeof(char), BUFFER_SIZE, fs))>0)
              {
            	  
                  if(send(client_socket_fd, sdbuf, fs_block_sz, 0) < 0)
                  {
                      fprintf(stdout, "ERROR: Failed to send file.\n");
                      exit(1);
                  }
                  bzero(sdbuf, BUFFER_SIZE);
              }
              printf("Ok sent to client!\n");
              success = 1;
              
              close(client_socket_fd);
              printf("[Server] Connection with Client closed. Server will wait now...\n");
              while(waitpid(-1, NULL, WNOHANG) > 0);
        
      }
      printf("hear I am..");
  
}

