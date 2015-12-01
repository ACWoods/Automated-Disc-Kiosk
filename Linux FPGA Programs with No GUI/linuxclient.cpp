

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>


#define MAXCHARSIZE	255
#define NUM_OF_TITLES 100
#define NUM_OF_DISC_SLOTS 105
#define MAXBACKLOG 5
#define PORTNUMBER 2200		/* Arbitrary port assignment*/

//"127.0.0.1"		Local IP address for testing
//"128.172.167.185"		Zybo IP address



int disc_carousel_rotation(int slot)
{
	//Send command to Zybo to rotate disc carousel.  Wait x seconds for rotation to finish or wait for 'finished' signal from Zybo.  Afterwards, continue with operation.
	/*ROTATE TO VALUE OF SLOT*/
	printf("Carousel is being aligned into position for disc burning...\n");
	return(0);
}	/*end disc_carousel_rotation()*/


int insert_disc(void)
{
	//Send command to Zybo to play disc.  Wait x seconds for rotation to finish or wait for 'finished' signal from Zybo.  Afterwards, continue with operation.
	printf("Disc is being loaded into authoring device.\n");
	return(0);
}	/*end insert_disc()*/

int return_disc(void)
{
	//Send command to Zybo to eject disc.  Wait x seconds for rotation to finish or wait for 'finished' signal from Zybo.  Afterwards, continue with operation.
	printf("Please wait while disc carousel aligns itself into position for disc delivery.\n");
	usleep(5);	//Simulating carousel rotation
	printf("Disc is ready for delivery.  Please remove disc.\n");
	return(0);
}	/*end return_disc()*/


int main(void)
{
	int stage = 0;	/*In stage 0 the server sends the client the list of titles.  In stage 1 the server burns a specific title*/
	bool user_input_error = false;
	int i = 0, j = 0, socket_file_desc_1 = 0, err_check = 0, title_ID_number = 0, disc_burn = 0, disc_return = 0;
	char read_buffer[MAXCHARSIZE], write_buffer[MAXCHARSIZE], user_input[MAXCHARSIZE], title_name[NUM_OF_TITLES]; 
	char user_instruction, database[NUM_OF_TITLES][MAXCHARSIZE] = {0}, temp_buffer[MAXCHARSIZE], status_message_from_server[MAXCHARSIZE];
	bool carousel_is_empty = false, carousel_is_full = false;
	
	bool disc_occupancy[NUM_OF_DISC_SLOTS];

	struct sockaddr_in server_address;		/*sockaddr_in is a structure that contains Internet address information*/
	struct hostent *server;		/*'server' is a pointer to the hostent structure, which defines a host computer on the Internet (address information in struct members)*/

	memset(disc_occupancy, true, NUM_OF_DISC_SLOTS);	/*Assuming all slots are occupied by discs*/

	while(1)
	{
		socket_file_desc_1 = socket(AF_INET, SOCK_STREAM, 0);		/*Creating the socket: AF_INET for Internet address domain, SOCK_STREAM for continuous stream socket type, and 0 to choose TCP as protocol for stream*/
		if(socket_file_desc_1 < 0)
			printf("ERROR: Socket could not be opened.\n");
			
		server = NULL;	
		while(server == NULL)
		{		
			server = gethostbyname("127.0.0.1");		/*Takes name of server and returns a pointer to a hostent structure (defined 'server') containing information about the host*/
		}	/*end while*/

		memset(&server_address, 0, sizeof(server_address));	/*Initializing the server address buffer to 0*/

		/***Setting the fields in server_address***/
		server_address.sin_family = AF_INET;
		memmove(&server_address.sin_addr.s_addr,
			server->h_addr,		/*h_addr is defined as the first address in the array h_addr_list*/
			server->h_length);

		server_address.sin_port = htons(PORTNUMBER);		/*htons() converts a port number in host byte order (little-endian) to a port number in network byte order (big-endian)*/
		
		
		/*Stage 0 prompts user for instruction and asks server for list of titles.  Stage 1 asks server to burn specific title.*/
		
		if(stage == 0)
		{
			/*****STAGE 0*****/
			/*User inputs value to either burn disc or return previously burned disc*/
			user_input_error = true;
			while(user_input_error == true)
			{
				printf("Enter 'b' to burn an image to disc or 'r' to return a previously burned disc.\n");
				
				/*Error checking user input-Using fgets() and sscanf() instead of scanf() prevents infinite loop caused by error flag when user inputs character instead of number*/
				fgets(temp_buffer, MAXCHARSIZE, stdin);	/*Replaced scanf() with fgets() and sscanf() to prevent buffer overruns*/
				err_check = sscanf(temp_buffer, "%c", &user_instruction);
				
				if(err_check < 1)
				{
					printf("ERROR:  Entered value is invalid.\n");
					continue;
				}	/*end if*/
				
				if(user_instruction == 'b' || user_instruction == 'r')
					user_input_error = false;
				else
					printf("ERROR:  Entered value is invalid.\n");
				
			}	/*end while*/
			
			if(user_instruction == 'r')
			{
				printf("Please wait while disc carousel rotates in position...\n");
				for(i = 0; i < NUM_OF_DISC_SLOTS; i++)	/*Checking for the first available slot*/
				{
					carousel_is_full = true;
					if(i == 0 || i == 101 || i == 102 || i == 104)	/*These particular slots on the carousel are covered by plastic and can't be used to store discs*/
						continue;
						
					if(disc_occupancy[i] == false)
					{
						disc_return = i;
						disc_occupancy[i] = true;
						carousel_is_full = false;
						break;
					}	/*end if*/				
				}	/*end for*/
			
				/*Rotate disc carousel into position*/
				if(carousel_is_full)
				{
					printf("ERROR: Unable to return disc.  Disc carousel is full.\n");
					continue;
				}
				else
					disc_carousel_rotation(disc_return);
				
				/*Error checking user input*/
				user_input_error = true;
				while(user_input_error == true)
				{
					printf("Enter 'd' when done inserting disc: \n");
					fgets(temp_buffer, MAXCHARSIZE, stdin);
					err_check = sscanf(temp_buffer, "%c", &user_instruction);	/*Reusing user_instruction variable to save space*/
					
					if(err_check < 1)
					{
						printf("ERROR:  Entered value is invalid.\n");
						continue;
					}	/*end if*/
					
					if(user_instruction == 'd')
						user_input_error = false;
					else
						printf("ERROR:  Entered value is invalid.\n");
				}	/*end while*/
				
				continue;				
			}	/*end if*/
			
			
			/*Client retrieves list of titles from server*/
			if(connect(socket_file_desc_1, (sockaddr *) &server_address, sizeof(server_address)) < 0)		/*Establishing connection to server with error checking*/
			{
				printf("ERROR: Socket could not be connected to server.\n");
				close(socket_file_desc_1);	/*If connect() fails, state of socket is undefined and so should be closed*/
				continue;
			}	/*end if*/
				
			strcpy(write_buffer, "0 0 NULL");
			err_check = write(socket_file_desc_1, write_buffer, sizeof(write_buffer));	/*Tells server to upload title database to client*/
			if(err_check < 0)
			{
				printf("ERROR: Failure writing database upload command on socket.");
				close(socket_file_desc_1);
			}	/*end if*/
			
			
			for(i = 0; i < NUM_OF_TITLES; i++)
			{
				err_check = read(socket_file_desc_1, read_buffer, sizeof(read_buffer));
				if(err_check < 0)
				{
					printf("ERROR: Failure reading title database from socket.\n");
					close(socket_file_desc_1);
					break;
				}	/*end if*/
			
				strcpy(database[i], read_buffer); 
				printf("%s", database[i]);
			}	/*end for*/
			
			user_input_error = true;
			while(user_input_error == true)
			{
				printf("Enter integer number for desired title to be burned [0 - %d]:  ", NUM_OF_TITLES - 1);
				
				/*Error checking user input*/
				fgets(temp_buffer, MAXCHARSIZE, stdin);
				err_check = sscanf(temp_buffer, "%d", &title_ID_number);
				
				if(err_check < 1)
				{
					printf("ERROR:  Entered value is invalid.\n");
					continue;
				}	/*end if*/
				
				if(title_ID_number < 0 || title_ID_number > NUM_OF_TITLES)
					printf("ERROR:  Entered value is invalid.\n");
				else
					user_input_error = false;
			}	/*end while*/
			
			
			
			for(i = 0; i < NUM_OF_DISC_SLOTS; i++)	/*Checking for the first available disc*/
			{
				carousel_is_empty = true;
				if(i == 0 || i == 101 || i == 102 || i == 104)	/*These particular slots on the carousel are covered by plastic and can't be used to store discs*/
					continue;
						
				if(disc_occupancy[i] == true)
				{
					disc_burn = i;
					disc_occupancy[i] = false;
					carousel_is_empty = false;
					break;
				}	/*end if*/				
			}	/*end for*/
			
			/*Rotate disc carousel into position*/
			if(carousel_is_empty)
			{
				printf("ERROR: Unable to burn disc.  Disc carousel is empty.\n");
				continue;
			}	/*end if*/
			else
			{
				disc_carousel_rotation(disc_burn);
				insert_disc();	/*Inserts disc into DVD burner*/
			}	/*end else*/
			
			stage = 1;
		}	/*end if*/
				
	/******************************************************************************************************************************************************************/		
			
		else if(stage == 1)
		{
			/*****STAGE 1*****/
			/*Client requests specific title to be burned*/
			
			
			/*CLIENT DATA SYNTAX: [STAGE		ID			NAME] */
			for(i = 0; i < MAXCHARSIZE; i++)	/*Checks for the first letter in the string which should be the beginning of the title name and copies until null character is met*/
			{
				if(isalpha(database[title_ID_number][i]))
				{
					for(j = 0; i < MAXCHARSIZE; i++)
					{					
						title_name[j] = database[title_ID_number][i];
						
						if(database[title_ID_number][i] == '\0')
							break;
							
						j++;
					}	/*end while*/
					
					break;
				}	/*end if*/
			}	/*end for*/
			
			sprintf(write_buffer, "1 %d %s", title_ID_number, title_name);
			
			if(connect(socket_file_desc_1, (sockaddr *) &server_address, sizeof(server_address)) < 0)		/*Establishing connection to server with error checking*/
			{
				printf("ERROR: Socket could not be connected to server.\n");
				close(socket_file_desc_1);	/*If connect() fails, state of socket is undefined and so should be closed*/
				continue;
			}	/*end if*/
			
			err_check = write(socket_file_desc_1, write_buffer, MAXCHARSIZE);	/*Writing burn command to the socket*/
			if(err_check < 0)
			{
				printf("ERROR: Failure writing burn command on socket.\n");
				close(socket_file_desc_1);
			}	/*end if*/
			
			
			/*****FUNCTION TO RETURN DISC TO CUSTOMER*****/
			//Wait for message from server that burning operation is done.  Then press 'stop' on front panel.
			while(!strcmp(status_message_from_server, "Burn complete.  Disc is ready."));

			return_disc();
			
		
			
			stage = 0;
		}	/*end else if*/
		
		close(socket_file_desc_1);

	}	/*end while*/
	
	
	return(0);
}	/*end main*/