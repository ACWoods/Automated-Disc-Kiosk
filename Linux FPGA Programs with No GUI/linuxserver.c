/*****************************************************************
/*Server Program for Lab 3
/*Author:  Allen Woods
/*(Some code was borrowed (mainly comments) from a program called
/* usm.cpp, created by the author, written for CMSC 355, Fall 2013)
/*****************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>


#define MAXCHARSIZE	255
#define PIPESIZE 2
#define TITLESIZE 100
#define MAXBACKLOG 5
#define PORTNUMBER 2200		/* Arbitrary port assignment*/


void *server(void *ptr)
{
	int stage = 0;	/*In stage 0 the server sends the client the list of titles.  In stage 1 the server burns a specific title*/
	int i = 0, title_ID_number = 0, err_check = 0, burn_commands_pipe_file_desc[PIPESIZE];
	int socket_file_desc_1 = 0, acceptsocket_file_desc_1 = 0;	/*socket_file_desc_1 will be a listening socket; acceptsocket_file_desc_1 will be a new file descriptor for the first pending connection on the listening socket*/
	char read_buffer[MAXCHARSIZE], infilename[MAXCHARSIZE], line[MAXCHARSIZE], title_name[TITLESIZE], *token;
	char burn_DVD_pipe_string[MAXCHARSIZE], blank_DVD_pipe_string[MAXCHARSIZE], CL_output[MAXCHARSIZE], status_message_to_client[MAXCHARSIZE];
	
	socklen_t client_address_length = 0;
	struct sockaddr_in server_address, client_address;		/*sockaddr_in is a structure that contains Internet address information*/
	
	FILE *in_file = NULL;	/*Stream for input file*/
	FILE *child_process = NULL;	/*Stream for pipe to CL burning functions*/
	
	do
	{
		strcpy(infilename, "title_database.txt");
		in_file = fopen(infilename, "r");
		if(in_file == NULL)
		{
			printf("ERROR: Input file could not be opened.\n");
			perror(NULL);
		}	/*end if*/
	}	while(in_file == NULL);
	
	
	socket_file_desc_1 = socket(AF_INET, SOCK_STREAM, 0);		/*Creating the socket: AF_INET for Internet address domain, SOCK_STREAM for continuous stream socket type, and 0 to choose TCP as protocol for stream*/
	if(socket_file_desc_1 < 0)
		printf("ERROR: Socket could not be opened.\n");
		
	bzero(&server_address, sizeof(server_address));	/*Initializing the server address buffer to 0*/
	
	/***Setting the fields in server_address***/
	/*The sockaddr_in structure specifies the address family, IP address, and port for the socket that is being bound.*/
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(PORTNUMBER);	/*htons() converts a port number in host byte order to a port number in network byte order*/
	server_address.sin_addr.s_addr = INADDR_ANY;
	
	
	/***Setting up connection to client***/
	client_address_length = sizeof(client_address);
	err_check = bind(socket_file_desc_1, (struct sockaddr *) &server_address, sizeof(server_address));	/*Binds socket to server IP address and port*/
	if(err_check < 0)
		printf("ERROR: Failure on binding listening socket.\n");
		
	listen(socket_file_desc_1, 5);	/*Server listens to the socket for connections*/
	
	
	while(1)	/*Server runs continuously*/
	{
		acceptsocket_file_desc_1 = accept(socket_file_desc_1,  (struct sockaddr *) &client_address, &client_address_length);	/*accept() pauses the server process until a client connection is made*/
		if(acceptsocket_file_desc_1 < 0)
			printf("ERROR: accept() command failed.\n");
			
		err_check = read(acceptsocket_file_desc_1, read_buffer, MAXCHARSIZE);
		if(err_check < 0)
		{
			printf("ERROR: Failure receiving data on socket.");
			close(acceptsocket_file_desc_1);	/*Closes the socket*/
			continue;
		}	/*end if*/

		/*CLIENT DATA SYNTAX: [STAGE		ID			NAME] */
		/*Read buffer will specify the stage, title ID, and title name between whitespace.  If client asserts stage 0, succeeding data is ignored.*/
		token = strtok(read_buffer, " \t");	
		if(token == NULL)
		{
			printf("ERROR: Tokenizing of read-in buffer (stage) failed.");
			close(acceptsocket_file_desc_1);
			continue;
		}	/*end if*/
		stage = atoi(token);	/*The first datum that the client passes should be an integer value for the stage the connection is in*/
		
		
		token = strtok(NULL, " \t");	
		if(token == NULL)
		{
			printf("ERROR: Tokenizing of read-in buffer (title_ID_number) failed.");
			close(acceptsocket_file_desc_1);
			continue;
		}	/*end if*/
		title_ID_number = atoi(token);	/*The second datum that the client passes should be an integer value for the title ID number*/
		
		
		token = strtok(NULL, " \t");	
		if(token == NULL)
		{
			printf("ERROR: Tokenizing of read-in buffer (title name) failed.");
			close(acceptsocket_file_desc_1);
			continue;
		}	/*end if*/
		strcpy(title_name, token);	/*The third datum that the client passes should be a string for the title name*/
		
		
	
		/*****STAGE 0*****/
		if(stage == 0)	/*Client has requested the title database*/
		{
			while(fgets(line, MAXCHARSIZE, in_file) != NULL)
			{
				//printf("%s",line);	/*Debugging purposes*/
				
				err_check = write(acceptsocket_file_desc_1, line, sizeof(line));
				
				if(err_check < 0)
					printf("ERROR: Failure writing on socket.");

			}	/*end while*/
			
			rewind(in_file);	/*rewind() clears any error flags on the file stream and allows the program to read the file stream again from the beginning*/
		}	/*end if*/

		
		/*****STAGE 1*****/
		else	/*No if() statement used here so that in the case of the variable 'stage' taking a value other than 0 or 1, the client does not deadlock from not receiving a response from the server*/
		{
			/*Read side (server) of pipe: burn_commands_pipe_file_desc[0]
			Write side (CL executable) of pipe: burn_commands_pipe_file_desc[1]*/
			
			sprintf(blank_DVD_pipe_string, "dvd+rw-format -blank /media/SuperMulti");	/*********COMPLETE THE SYNTAX FOR THE STRING-use df to find proper drive on Raspberry and replace with /cygdrive/g*************/
			
			//sprintf(burn_DVD_pipe_string, "growisofs -dvd-compat -Z /cygdrive/g=/home/Woods/Downloads/test.iso");	/*String for Cygwin debugging*/
			
			sprintf(burn_DVD_pipe_string, "growisofs -dvd-compat -Z /dev/sr0 -J -R /media/My\\ Passport/test.iso");	/*Backslash required between 'My' and 'Passport' since there is whitespace; double backslash required to indicate that first backslash isn't part of C-string escape sequence.*/
			
			
			/*Format DVD*/
			//child_process = popen(blank_DVD_pipe_string, "r");	/*Opening pipe to child process to format DVD; child_service is the stream to read to/write from the pipe*/
			
			//if(child_process == NULL)
			//	printf("ERROR: Pipe cannot be created.\n");

			//while(fgets(CL_output, MAXCHARSIZE, child_process) != NULL)	/*Print results of command*/
			//{
			//	printf("%s", CL_output);
			//}	/*end while*/
			
			//sleep(10);	/***************FIGURE OUT DELAY NECESSARY FOR PREVIOUS COMMAND********************/
			//err_check = pclose(child_process);
			//if(err_check < 0)
			//	printf("ERROR: Child process for DVD formatting did not close properly.\n");
				
				
			/*Burn DVD*/
			child_process = popen(burn_DVD_pipe_string, "r");	/*Opening pipe to child process to burn DVD*/
			
			if(child_process == NULL)
				printf("ERROR: Pipe cannot be created.\n");
			
			while(fgets(CL_output, MAXCHARSIZE, child_process) != NULL)	/*Print results of command*/
			{
				printf("%s", CL_output);
			}	/*end while*/
			
			
			if(strcmp(CL_output, "/dev/sr0/:  reloading tray"))	/*Disc is ejecting*/
			{
				usleep(10);	/*Wait for full ejection*/
				sprintf(status_message_to_client, "Burn complete.  Disc is ready.");
			}	/*end while*/
			else
			{
				/****TELL CLIENT THAT SOMETHING WENT WRONG****/
				sprintf(status_message_to_client, "Burn failed.");
			}	/*end else*/

			err_check = pclose(child_process);
			if(err_check < 0)
				printf("ERROR: Child process for DVD formatting/burning did not close properly.\n");
				
			
			
			/*******SEND MESSAGE TO CLIENT THAT DISC IS READY********/
			err_check = write(acceptsocket_file_desc_1, status_message_to_client, sizeof(status_message_to_client));
			if(err_check < 0)
				printf("ERROR: Failure writing on socket (after burn operation).");
			
			
			
			
		}	/*end else if*/
		
		
		close(acceptsocket_file_desc_1);
	
		
			
	}	/*end while*/
	
}	/*end server()*/



int main(void)
{
	int thread_error1 = 0;
	
	pthread_t server_thread;
	
	/*Creating threads to execute functions*/
	thread_error1 = pthread_create(&server_thread, NULL, &server, NULL);
	
	if(thread_error1)
		printf("Error creating server thread.  ErrNo: %d", thread_error1);
		
	/*Waiting until threads complete before main() resumes.  Otherwise, main() could exit before threads complete.*/
	
	while(1)
	{
		pthread_join(server_thread, NULL);
	}	/*end while*/
}	/*end main*/
	
	
	
	
	
	
	
