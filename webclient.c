/* ============================================================================

usage:
	webclient [options]
options:
	-s sever address (Default: 0.0.0.0)
	-p server port (default 8888)
	-t number of worker threads (Default 1, range 1-1000)
	-w path to workload file (Default: workload.txt)
	-d path to download file directory (Default: null)
	-r number of total requests (Default:10, Range: 1,1000)
	-m path to metrics file (Default: metrics.txt)
	-h show help message

============================================================================ */
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

/* CONSTANTS =============================================================== */

#define BUFFER_SIZE 1024

pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;          /* mutex lock for buffer */
pthread_cond_t c_cons = PTHREAD_COND_INITIALIZER; /* consumer waits on this cond var */
pthread_cond_t c_prod = PTHREAD_COND_INITIALIZER; /* producer waits on this cond var */


int main(int argc, char **argv) {

  int args = 0;//number of command line arguments expected
  int socket_fd = 0;
  int server_port = 8888;
  int worker_threads = 1;
  int request_count = 10;
  int total_bytes = 0;
  int len = 0;
  int th =0;
  struct sockaddr_in server_socket_addr;
  char buffer[BUFFER_SIZE];
  char server_ip [25] = "0.0.0.0";
  char metrics_file[50] = "metrics.txt";
  char workload_file[50] = "workload.txt";
  char download_dir[50] = "";
  char file_task[201];
  char revbuf[BUFFER_SIZE]; 

  // Converts localhost into 0.0.0.0
  struct hostent *he = gethostbyname(server_ip);
  unsigned long server_addr_nbo = *(unsigned long *)(he->h_addr_list[0]);
  
  for (args = 1; args < argc; args = args + 2){
  	 
	  //set what server ip
	  if (strcmp("-s", argv[args]) == 0){
		  //printf ("%d %s %s \n",args, argv[args ], argv[ args + 1]);
	  	  strcpy (server_ip, argv[ args + 1]);
	  	  printf("Setting server ip to %s \n", server_ip);
	      }
	  
  	  //set what port to use
  	  if (strcmp("-p", argv[args]) == 0){
  		  //printf ("%d %s %s \n",args, argv[args ], argv[ args + 1]);
  		  server_port =  atoi (argv[ args + 1]);
  		  printf("Setting port to %d \n", server_port);
  	      }
  	 
  	  //check # of threads 
  	  if (strcmp("-t", argv[args]) == 0){
   		  //printf ("%d %s %s \n",args, argv[args ], argv[ args + 1]);
   		  worker_threads = atoi (argv[ args + 1]);
   		  printf("Setting thread count to %d \n", worker_threads);
  	      }
  	  
  	  //set path to workload
  	  if (strcmp("-w", argv[args]) == 0){
  		  //printf ("%d %s %s \n",args, argv[args ], argv[ args + 1]);
  		  strcpy (workload_file, argv[args + 1]);
  		  printf("Setting work file path to %s \n", workload_file);
  	      }
  	  
  	  //set path to download file directory
  	  if (strcmp("-d", argv[args]) == 0){
  	  	  //printf ("%d %s %s \n",args, argv[args ], argv[ args + 1]);
  	  	  strcpy (download_dir, argv[args + 1]);
  	  	  printf("Setting download directory to %s \n",download_dir);
  	  	  }
  	  
  	  //determine number of total requests
	  if (strcmp("-r", argv[args]) == 0){
		  //printf ("%d %s %s \n",args, argv[args ], argv[ args + 1]);
		  request_count = atoi (argv[ args + 1]);
		  printf("Setting request count to %d \n", request_count);
	     }
	  
  	  //set path to metrics file
  	  if (strcmp("-m", argv[args]) == 0){
  	  	  //printf ("%d %s %s \n",args, argv[args ], argv[ args + 1]);
  	  	  strcpy (metrics_file, argv[args + 1]);
  	  	  printf ("Setting metrics file path to %s \n",metrics_file);
  	  	  
  	  	  }
  	  
  	  if (strcmp("-h", argv[args]) == 0){
  	  	  		   
  		  printf ("\n\nusage: \n");
  		  printf("\t webclient [options]\n");
  		  printf("options:\n");
  		  printf("\t\t-s sever address (Default: 0.0.0.0)\n");
  		  printf("\t\t-p port (default 8888)\n");
  		  printf("\t\t-t number of worker threads (default 1, range 1-1000)\n");
  		  printf("\t\t-w path to workload file (Default: workload.txt)\n");
  		  printf("\t\t-d path to download file directory (Default: null)\n");
  		  printf("\t\t-r number of total requests (Default:10, Range: 1,1000)\n");
  		  printf("\t\t-m path to metrics file (Default: metrics.txt)\n");
  		  printf("\t\t-h show help message\n\n\n");

  		  return 0;
  		  
  	  	 }
  	  
    }// end args for

  fprintf(stdout,"create workers \n");
  for (th = 0 ; th < worker_threads; th++){
	  
	  fprintf(stdout,"tid%d \n", th);
	   
	// char buffer23[20];
	 //itoa(th,buffer23,10) ;
	   
  }
  
  // Create socket (IPv4, stream-based, protocol likely set to TCP)
  if (0 > (socket_fd = socket(AF_INET, SOCK_STREAM, 0))) {
    fprintf(stderr, "client failed to create socket\n");
    exit(1);
  }

  // Configure server socket address structure (init to zero, IPv4,
  // network byte order for port and address) 
  bzero(&server_socket_addr, sizeof(server_socket_addr));
  server_socket_addr.sin_family = AF_INET;
  server_socket_addr.sin_port = htons(server_port);
  server_socket_addr.sin_addr.s_addr = server_addr_nbo;

  // Connect socket to server
  if (0 > connect(socket_fd, (struct sockaddr *)&server_socket_addr, sizeof(server_socket_addr))) {
    fprintf(stderr, "client failed to connect to %s:%d!\n", server_ip, server_port);
    close(socket_fd);
    exit(1);
  } else {
    fprintf(stdout, "client connected to to %s:%d!\n", server_ip, server_port);
  }
         
          char sdbuf[BUFFER_SIZE]; 
          
          //read the workload file
          FILE *fs = fopen(workload_file, "r");
          char line[201];
         // fgets(line,sizeof line,fs);
          
          if (fgets (line,60, fs)!=NULL){
          	
          	size_t ln = strlen(line) -1;

          	if (line[ln] == '\n')
          		line[ln] = '\0';

                  printf("line =  **%s** \n",line);
          	}
          
          fprintf(stdout,"call to GETFILE for: ***%s***\n",line); //print the file contents on stdout.
                   

          if(fs == NULL)
          {
              printf("ERROR: File %s not found.\n", workload_file);
              exit(1);
          }

          bzero(sdbuf, BUFFER_SIZE); 
          int fs_block_sz; 
          printf("[Client] Sending %s to the Server... \n", line);
         /* while((fs_block_sz = fread(sdbuf, sizeof(char), BUFFER_SIZE, fs)) > 0)
          {
              if(send(socket_fd, sdbuf, fs_block_sz, 0) < 0)
            	   
              {
                  fprintf(stdout, "ERROR: Failed to send file\n");
                  break;
              }
              bzero(sdbuf, BUFFER_SIZE);
          }*/
          
          if(send(socket_fd, line, strlen(line), 0) < 0)
                      	   
                        {
                            fprintf(stdout, "ERROR: Failed to send file\n");
                            
                        }
           
          printf("Request for File %s was Sent!\n", line);
      //}

      /* Receive File from Server */
      printf("[Client] Receiveing file from Server and saving it as %s...\n", line );
      //char* fr_name = "/dvlp/client_data/client_receipt.txt";
      char* fr_name = strcat(download_dir ,line);

      FILE *fr = fopen(fr_name, "ab+");
      if(fr == NULL)
          printf("File %s Cannot be opened.\n", line);
      else
      {
          bzero(revbuf, BUFFER_SIZE); 
          int fr_block_sz = 0;
          while((fr_block_sz = recv(socket_fd, revbuf, BUFFER_SIZE, 0)) > 0)
          {
              int write_sz = fwrite(revbuf, sizeof(char), fr_block_sz, fr);
              if(write_sz < fr_block_sz)
              {
                  printf("File write failed.\n");
              }
              bzero(revbuf, BUFFER_SIZE);
              if (fr_block_sz == 0 || fr_block_sz != 512) 
              {
                  break;
              }
          }
 
          printf("Ok received from server!\n");
          fclose(fr);
      }
      close (socket_fd);
      printf("[Client] Connection closed? or lost.\n");
  
  

  return 0; 
}

