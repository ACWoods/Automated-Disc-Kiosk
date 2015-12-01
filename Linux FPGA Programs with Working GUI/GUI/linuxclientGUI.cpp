

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
#include <fcntl.h>
#include <sys/mman.h>


#define MAXCHARSIZE	255
#define NUM_OF_TITLES 100
#define NUM_OF_DISC_SLOTS 105
#define MAXBACKLOG 5
#define PORTNUMBER 2200		/* Arbitrary port assignment*/
#define APPPORTNUMBER 2015

//"127.0.0.1"		Local IP address for testing
//"192.168.0.100		Raspberry Pi IP address
//"192.168.0.110		Zybo IP address


/*Carousel control definitions*/
#define MAP_SIZE 4096UL
#define MAP_MASK (MAP_SIZE -1)
#define MORE0 0x7BE00000
#define MORE1 0x7BE20000
#define MORE2 0x7BE40000
#define MORE3 0x7BE60000
#define CORE0 0x7FC00000
#define CORE1 0x7FC20000
#define CORE2 0x7FC40000
#define CORE3 0x7FC60000
#define BASE0 0x41200000
#define DATA 0x00
#define TRIS 0x04

#define LO 14
#define HI 15

#define RESET 0b00000000
/*#define ONE 0b00010000
#define TWO 0b01010000
#define THREE 0b01000000
#define FOUR 0b01100000
#define FIVE 0b00100000
#define SIX 0b10100000
#define SEVEN 0b10000000
#define EIGHT 0b10010000*/

#define ONE 0b01010000
#define TWO 0b01010000
#define THREE 0b01100000
#define FOUR 0b01100000
#define FIVE 0b10100000
#define SIX 0b10100000
#define SEVEN 0b10010000
#define EIGHT 0b10010000





//Global Variables for Memory Mapping
int memfd;
off_t dev_b0 = BASE0;//GPIO 0 base address
off_t dev_data0 = BASE0 + DATA;//GPIO 0 Channel 1 data reg
off_t dev_tris0 = BASE0 + TRIS;//GPIO 0 Channel 1 tristate reg
off_t dev_c0 = CORE0;//servo core 0 base address
off_t dev_c1 = CORE1;//servo core 1 base address
off_t dev_c2 = CORE2;//servo core 2 base address
off_t dev_c3 = CORE3;//servo core 3 base address
off_t dev_m0 = MORE0;//motor core 0 base address
off_t dev_m1 = MORE1;//motor core 1 base address
off_t dev_m2 = MORE2;//motor core 2 base address
off_t dev_m3 = MORE3;//motor core 3 base address

void *base0;
void *datareg0;
void *trisreg0;
void *cbase0;
void *cbase1;
void *cbase2;
void *cbase3;
void *mbase0;
void *mbase1;
void *mbase2;
void *mbase3;	



/**************************************************************************************************	
*Carousel command code list-'D' refers to commands used primarily (or solely) for debugging
*		0-104 Align carousel to slot
*
*		150 Load disc into DVD burner
*
*		151 Unload disc from DVD Burner
*		
*		152 Eject disc through service slot
*
*		153 Insert disc through service slot
*
*	D	160 Set Index to 'x'
*			->Enter 'x' (new index)
*
*	D	161 Print Index
*
*	D	200 Manually rotate carousel 'x' slots
*			->Enter 'x' (number of slots to move)
*
*	D	250 Manually set servo motor steps to x
*			->Enter 'x' (number of servo steps)
*
*	D	300 Motor control (wheel motor)
*			->Enter motor command #
*
*	D	400 Motor control (lateral disc load motor)
*			->Enter motor command #
*
*	D	500 Motor control (wheel arm)
*			->Enter motor command #
*
*	D	600 Motor control (lower arm)
*			->Enter motor command #
*
*	D	700 Motor control (upper arm)
*			->Enter motor command #
*
*	D	800	Motor control (front arm)
*			->Enter motor command #
*
*	D	998 Panic
*
*		999-Exit from function
***************************************************************************************************/


int device_control(int inp, int ind, int serv, int mot)
{
	int input = inp;
	int motin = mot;
	int servin = serv;
	int i = 0;
	int counter = 0;
	
	int state = 0;
	int nate = 0;
	int index = 0;
	
	int steps = 0;
	int slots = 0;
	int temp = 0;

	
	bool panic = true;

	state = 1;
	nate = 1;
	index = ind;

	//Output Loop
	*((volatile int *)trisreg0) = 0b00000000;//set GPIO 0 tristates to output
	*((volatile int *)datareg0) = RESET;//set all channels to output 0
	
	temp = input;
	panic = true;
	while(panic == true)
	{
		
		if(temp == 998)
		{
			panic = true;
			printf("Enter new input: \r\n");
			scanf("%d",&input);
			printf("Entered: %d\r\n",input);
		}	/*end if*/
		else
			panic = false;
		
		
		servin = 0;
		motin = 0;

	
		printf("input: %d\r\n",input);
		printf("index: %d\r\n",index);
		printf("servin: %d\r\n",servin);
		printf("motin: %d\r\n",motin);

		if(input>=0 && input <=104)//index
		{
			if(index-input>0)
			{
				if((104-index)+input<index-input)//forward
				{
					slots = (105-index)+input;
					steps = (int)((slots*30.476)+0.5);

					for(i=0; i<steps;i++)
					{				
						switch(state)
						{
							case 1:
								*((volatile int *)datareg0) = ONE;
								nate = 8;
							break;
							case 2:
								*((volatile int *)datareg0) = TWO;
								nate = 1;
							break;
							case 3:
								*((volatile int *)datareg0) = THREE;
								nate = 2;
							break;
							case 4:
								*((volatile int *)datareg0) = FOUR;
								nate = 3;
							break;
							case 5:
								*((volatile int *)datareg0) = FIVE;
								nate = 4;
							break;
							case 6:
								*((volatile int *)datareg0) = SIX;
								nate = 5;
							break;
							case 7:
								*((volatile int *)datareg0) = SEVEN;
								nate = 6;
							break;
							case 8:
								*((volatile int *)datareg0) = EIGHT;
								nate = 7;
							break;
						}
						state = nate;
						usleep(7000);
					}
					slots = 0;
					steps = 0;
				}
				else//reverse
				{
					slots = index-input;
					steps = (int)((slots*30.476)+0.5);

					for(i=0; i<steps;i++)
					{				
						switch(state)
						{
							case 1:
								*((volatile int *)datareg0) = ONE;
								nate = 2;
							break;
							case 2:
								*((volatile int *)datareg0) = TWO;
								nate = 3;
							break;
							case 3:
								*((volatile int *)datareg0) = THREE;
								nate = 4;
							break;
							case 4:
								*((volatile int *)datareg0) = FOUR;
								nate = 5;
							break;
							case 5:
								*((volatile int *)datareg0) = FIVE;
								nate = 6;
							break;
							case 6:
								*((volatile int *)datareg0) = SIX;
								nate = 7;
							break;
							case 7:
								*((volatile int *)datareg0) = SEVEN;
								nate = 8;
							break;
							case 8:
								*((volatile int *)datareg0) = EIGHT;
								nate = 1;
							break;
						}
						state = nate;
						usleep(7000);
					}
					slots = 0;
					steps = 0;										
				}
			}
			else if(index-input<=0)
			{
				if(index+(104-input)<input-index)//reverse
				{
					slots = index+(105-input);
					steps = (int)((slots*30.476)+0.5);

					for(i=0; i<steps;i++)
					{				
						switch(state)
						{
							case 1:
								*((volatile int *)datareg0) = ONE;
								nate = 2;
							break;
							case 2:
								*((volatile int *)datareg0) = TWO;
								nate = 3;
							break;
							case 3:
								*((volatile int *)datareg0) = THREE;
								nate = 4;
							break;
							case 4:
								*((volatile int *)datareg0) = FOUR;
								nate = 5;
							break;
							case 5:
								*((volatile int *)datareg0) = FIVE;
								nate = 6;
							break;
							case 6:
								*((volatile int *)datareg0) = SIX;
								nate = 7;
							break;
							case 7:
								*((volatile int *)datareg0) = SEVEN;
								nate = 8;
							break;
							case 8:
								*((volatile int *)datareg0) = EIGHT;
								nate = 1;
							break;
						}
						state = nate;
						usleep(7000);
					}
					slots = 0;
					steps = 0;
				}
				else//forward
				{
					slots = input-index;
					steps = (int)((slots*30.476)+0.5);

					for(i=0; i<steps;i++)
					{				
						switch(state)
						{
							case 1:
								*((volatile int *)datareg0) = ONE;
								nate = 8;
							break;
							case 2:
								*((volatile int *)datareg0) = TWO;
								nate = 1;
							break;
							case 3:
								*((volatile int *)datareg0) = THREE;
								nate = 2;
							break;
							case 4:
								*((volatile int *)datareg0) = FOUR;
								nate = 3;
							break;
							case 5:
								*((volatile int *)datareg0) = FIVE;
								nate = 4;
							break;
							case 6:
								*((volatile int *)datareg0) = SIX;
								nate = 5;
							break;
							case 7:
								*((volatile int *)datareg0) = SEVEN;
								nate = 6;
							break;
							case 8:
								*((volatile int *)datareg0) = EIGHT;
								nate = 7;
							break;
						}
						state = nate;
						usleep(7000);
					}
					slots = 0;
					steps = 0;
				}
			}
			index=input;
			printf("Done moving.\r\n");
		}
		else if(input==150)//load disc
		{
			printf("Loading disc.\r\n");

			*((volatile int *)mbase2) = 0;
			*((volatile int *)mbase3) = 270;

			sleep(2);

			*((volatile int *)mbase2) = 0;
			*((volatile int *)mbase3) = 0;			

			sleep(1);

			//raise lower arm
			*((volatile int *)cbase1) = 35;

			sleep(1);

			//lower upper arm
			*((volatile int *)cbase3) = 28;

			sleep(1);

			//raise upper arm
			*((volatile int *)cbase3) = 12;

			//reset under arm
			*((volatile int *)cbase1) = 44;

			sleep(1);

			*((volatile int *)mbase2) = 270;
			*((volatile int *)mbase3) = 0;

			sleep(2);

			*((volatile int *)mbase2) = 0;
			*((volatile int *)mbase3) = 0;			


		}
		else if(input==151)//unload disc
		{
			//spin motor reverse
			*((volatile int *)mbase0) = 0;
			*((volatile int *)mbase1) = 90;

			sleep(1);

			//lower wheel arm
			*((volatile int *)cbase0) = 25;	

			*((volatile int *)mbase0) = 0;
			*((volatile int *)mbase1) = 270;

			sleep(2);

			//turn off motor
			*((volatile int *)mbase0) = 0;
			*((volatile int *)mbase1) = 0;			

			//raise arm
			*((volatile int *)cbase0) = 45;

		}
		else if(input==152)//eject disc
		{
			steps = (int)((54*30.476)+0.5);
			for(i=0; i<steps;i++)//move forward 54 slots
			{				
				switch(state)
				{
					case 1:
						*((volatile int *)datareg0) = ONE;
						nate = 8;
					break;
					case 2:
						*((volatile int *)datareg0) = TWO;
						nate = 1;
					break;
					case 3:
						*((volatile int *)datareg0) = THREE;
						nate = 2;
					break;
					case 4:
						*((volatile int *)datareg0) = FOUR;
						nate = 3;
					break;
					case 5:
						*((volatile int *)datareg0) = FIVE;
						nate = 4;
					break;
					case 6:
						*((volatile int *)datareg0) = SIX;
						nate = 5;
					break;
					case 7:
						*((volatile int *)datareg0) = SEVEN;
						nate = 6;
					break;
					case 8:
						*((volatile int *)datareg0) = EIGHT;
						nate = 7;
					break;
				}
				state = nate;
				usleep(7000);
			}

			*((volatile int *)cbase2) = 41;

			sleep(5);

			*((volatile int *)cbase2) = 26;

			sleep(1);

			for(i=0; i<steps;i++)//move reverse 54 slots
			{				
				switch(state)
				{
					case 1:
						*((volatile int *)datareg0) = ONE;
						nate = 2;
					break;
					case 2:
						*((volatile int *)datareg0) = TWO;
						nate = 3;
					break;
					case 3:
						*((volatile int *)datareg0) = THREE;
						nate = 4;
					break;
					case 4:
						*((volatile int *)datareg0) = FOUR;
						nate = 5;
					break;
					case 5:
						*((volatile int *)datareg0) = FIVE;
						nate = 6;
					break;
					case 6:
						*((volatile int *)datareg0) = SIX;
						nate = 7;
					break;
					case 7:
						*((volatile int *)datareg0) = SEVEN;
						nate = 8;
					break;
					case 8:
						*((volatile int *)datareg0) = EIGHT;
						nate = 1;
					break;
				}
				state = nate;
				usleep(7000);
			}			

		}
		else if(input==153)//insert disc
		{
			steps = (int)((54*30.476)+0.5);
			for(i=0; i<steps;i++)//move forward 54 slots
			{				
				switch(state)
				{
					case 1:
						*((volatile int *)datareg0) = ONE;
						nate = 8;
					break;
					case 2:
						*((volatile int *)datareg0) = TWO;
						nate = 1;
					break;
					case 3:
						*((volatile int *)datareg0) = THREE;
						nate = 2;
					break;
					case 4:
						*((volatile int *)datareg0) = FOUR;
						nate = 3;
					break;
					case 5:
						*((volatile int *)datareg0) = FIVE;
						nate = 4;
					break;
					case 6:
						*((volatile int *)datareg0) = SIX;
						nate = 5;
					break;
					case 7:
						*((volatile int *)datareg0) = SEVEN;
						nate = 6;
					break;
					case 8:
						*((volatile int *)datareg0) = EIGHT;
						nate = 7;
					break;
				}
				state = nate;
				usleep(7000);
			}

			sleep(10);

			for(i=0; i<steps;i++)//move reverse 54 slots
			{				
				switch(state)
				{
					case 1:
						*((volatile int *)datareg0) = ONE;
						nate = 2;
					break;
					case 2:
						*((volatile int *)datareg0) = TWO;
						nate = 3;
					break;
					case 3:
						*((volatile int *)datareg0) = THREE;
						nate = 4;
					break;
					case 4:
						*((volatile int *)datareg0) = FOUR;
						nate = 5;
					break;
					case 5:
						*((volatile int *)datareg0) = FIVE;
						nate = 6;
					break;
					case 6:
						*((volatile int *)datareg0) = SIX;
						nate = 7;
					break;
					case 7:
						*((volatile int *)datareg0) = SEVEN;
						nate = 8;
					break;
					case 8:
						*((volatile int *)datareg0) = EIGHT;
						nate = 1;
					break;
				}
				state = nate;
				usleep(7000);
			}			

		}
		else if(input==160)//set index
		{
			printf("Enter new index: \r\n");
			scanf("%d",&index);			
		}
		else if(input==161)//print index
		{
			printf("Index: %d\r\n",index);		
		}
		else if(input==200)//manual slots
		{
			while(servin != 999)
			{
				printf("Enter number of slots to move: [999 to exit]\r\n");
				scanf("%d",&servin);
				steps = (int)((servin*30.476)+0.5);

				if(servin == 999)//exit
				{
					//do nothing
				}
				else if(steps>=1)//forward
				{
					for(i=0; i<steps;i++)
					{				
						printf("i = %d\r\n",i);
						switch(state)
						{
							case 1:
								*((volatile int *)datareg0) = ONE;
								nate = 8;
							break;
							case 2:
								*((volatile int *)datareg0) = TWO;
								nate = 1;
							break;
							case 3:
								*((volatile int *)datareg0) = THREE;
								nate = 2;
							break;
							case 4:
								*((volatile int *)datareg0) = FOUR;
								nate = 3;
							break;
							case 5:
								*((volatile int *)datareg0) = FIVE;
								nate = 4;
							break;
							case 6:
								*((volatile int *)datareg0) = SIX;
								nate = 5;
							break;
							case 7:
								*((volatile int *)datareg0) = SEVEN;
								nate = 6;
							break;
							case 8:
								*((volatile int *)datareg0) = EIGHT;
								nate = 7;
							break;
						}
						state = nate;
						usleep(7000);
					}

				}
				else if(steps<=-1)//reverse
				{
					for(i=0; i<(steps*(-1));i++)
					{				
						printf("i = %d\r\n",i);
						switch(state)
						{
							case 1:
								*((volatile int *)datareg0) = ONE;
								nate = 2;
							break;
							case 2:
								*((volatile int *)datareg0) = TWO;
								nate = 3;
							break;
							case 3:
								*((volatile int *)datareg0) = THREE;
								nate = 4;
							break;
							case 4:
								*((volatile int *)datareg0) = FOUR;
								nate = 5;
							break;
							case 5:
								*((volatile int *)datareg0) = FIVE;
								nate = 6;
							break;
							case 6:
								*((volatile int *)datareg0) = SIX;
								nate = 7;
							break;
							case 7:
								*((volatile int *)datareg0) = SEVEN;
								nate = 8;
							break;
							case 8:
								*((volatile int *)datareg0) = EIGHT;
								nate = 1;
							break;
						}
						state = nate;
						usleep(7000);
					}
				}
				//servin = 0;
				steps = 0;
			}
			printf("Leaving carousel control mode.\r\n");	
		}
		else if(input==250)//manual steps
		{
			while(servin != 999)
			{
				printf("Enter number of steps to move: [999 to exit]\r\n");
				scanf("%d",&servin);
				steps = servin;

				if(servin == 999)//exit
				{
					//do nothing
				}
				else if(steps>=1)//forward
				{
					for(i=0; i<steps;i++)
					{				
						printf("i = %d\r\n",i);
						switch(state)
						{
							case 1:
								*((volatile int *)datareg0) = ONE;
								nate = 8;
							break;
							case 2:
								*((volatile int *)datareg0) = TWO;
								nate = 1;
							break;
							case 3:
								*((volatile int *)datareg0) = THREE;
								nate = 2;
							break;
							case 4:
								*((volatile int *)datareg0) = FOUR;
								nate = 3;
							break;
							case 5:
								*((volatile int *)datareg0) = FIVE;
								nate = 4;
							break;
							case 6:
								*((volatile int *)datareg0) = SIX;
								nate = 5;
							break;
							case 7:
								*((volatile int *)datareg0) = SEVEN;
								nate = 6;
							break;
							case 8:
								*((volatile int *)datareg0) = EIGHT;
								nate = 7;
							break;
						}
						state = nate;
						usleep(7000);
					}

				}
				else if(steps<=-1)//reverse
				{
					for(i=0; i<(steps*(-1));i++)
					{				
						printf("i = %d\r\n",i);
						switch(state)
						{
							case 1:
								*((volatile int *)datareg0) = ONE;
								nate = 2;
							break;
							case 2:
								*((volatile int *)datareg0) = TWO;
								nate = 3;
							break;
							case 3:
								*((volatile int *)datareg0) = THREE;
								nate = 4;
							break;
							case 4:
								*((volatile int *)datareg0) = FOUR;
								nate = 5;
							break;
							case 5:
								*((volatile int *)datareg0) = FIVE;
								nate = 6;
							break;
							case 6:
								*((volatile int *)datareg0) = SIX;
								nate = 7;
							break;
							case 7:
								*((volatile int *)datareg0) = SEVEN;
								nate = 8;
							break;
							case 8:
								*((volatile int *)datareg0) = EIGHT;
								nate = 1;
							break;
						}
						state = nate;
						usleep(7000);
					}
				}
				//servin = 0;
				steps = 0;
			}
			printf("Leaving carousel control mode.\r\n");	
		}
		else if(input==300)//motor control mode
		{
			while(motin != 999)
			{
				printf("Enter motor command: [999 to exit]\r\n");
				scanf("%d",&motin);
				printf("Entered: %d\r\n",motin);
				if(motin==0)//stop motor
				{
					printf("Stop motor.\r\n");
					*((volatile int *)mbase0) = 0;
					*((volatile int *)mbase1) = 0;
				}
				else if(motin==1)//spin forward
				{
					printf("Spin forward.\r\n");
					*((volatile int *)mbase0) = 180;
					*((volatile int *)mbase1) = 0;
				}
				else if(motin==2)//spin reverse
				{
					printf("Spin reverse.\r\n");
					*((volatile int *)mbase0) = 0;
					*((volatile int *)mbase1) = 180;
				}
				else if(motin==999)//exit
				{
					//do nothing
				}
				else//unknown, stop
				{
					printf("Unknown command, stopping.\r\n");
					*((volatile int *)mbase0) = 0;
					*((volatile int *)mbase1) = 0;
				}
			}
			printf("Leaving motor control mode.\r\n");
		}
		else if(input==400)//CD motor
		{
			while(motin != 999)
			{
				printf("Enter motor command: [999 to exit]\r\n");
				scanf("%d",&motin);
				printf("Entered: %d\r\n",motin);
				if(motin==0)//stop motor
				{
					printf("Stop motor.\r\n");
					*((volatile int *)mbase2) = 0;
					*((volatile int *)mbase3) = 0;
				}
				else if(motin==1)//spin forward
				{
					printf("Spin forward.\r\n");
					*((volatile int *)mbase2) = 0;
					*((volatile int *)mbase3) = 180;
				}
				else if(motin==2)//spin reverse
				{
					printf("Spin reverse.\r\n");
					*((volatile int *)mbase2) = 180;
					*((volatile int *)mbase3) = 0;
				}
				else if(motin==999)//exit
				{
					//do nothing
				}
				else//unknown, stop
				{
					printf("Unknown command, stopping.\r\n");
					*((volatile int *)mbase2) = 0;
					*((volatile int *)mbase3) = 0;
				}
			}
			printf("Leaving motor control mode.\r\n");
		}
		else if(input==500)//wheel servo control mode
		{
			while(servin != 999)
			{
				printf("Enter servo 0 command: [999 to exit]\r\n");
				scanf("%d",&servin);
				printf("Entered: %d\r\n",servin);
				if(servin>=0 && servin <= 360)//pass command
				{
					*((volatile int *)cbase0) = servin;
				}
				else if(servin==999)//exit
				{
					//do nothing
				}
				else//unknown, stop
				{
					printf("Unknown command.\r\n");
				}
			}
			printf("Leaving servo control mode.\r\n");
		}
		else if(input==600)//lower servo control mode
		{
			while(servin != 999)
			{
				printf("Enter servo 1 command: [999 to exit]\r\n");
				scanf("%d",&servin);
				printf("Entered: %d\r\n",servin);
				if(servin>=0 && servin <= 360)//pass command
				{
					*((volatile int *)cbase1) = servin;
				}
				else if(servin==999)//exit
				{
					//do nothing
				}
				else//unknown, stop
				{
					printf("Unknown command.\r\n");
				}
			}
			printf("Leaving servo control mode.\r\n");
		}
		else if(input==700)//upper servo control mode
		{
			while(servin != 999)
			{
				printf("Enter servo 3 command: [999 to exit]\r\n");
				scanf("%d",&servin);
				printf("Entered: %d\r\n",servin);
				if(servin>=0 && servin <= 360)//pass command
				{
					*((volatile int *)cbase3) = servin;
				}
				else if(servin==999)//exit
				{
					//do nothing
				}
				else//unknown, stop
				{
					printf("Unknown command.\r\n");
				}
			}
			printf("Leaving servo control mode.\r\n");
		}
		else if(input==800)//front servo control mode
		{
			while(servin != 999)
			{
				printf("Enter servo 2 command: [999 to exit]\r\n");
				scanf("%d",&servin);
				printf("Entered: %d\r\n",servin);
				if(servin>=0 && servin <= 360)//pass command
				{
					*((volatile int *)cbase2) = servin;
				}
				else if(servin==999)//exit
				{
					//do nothing
				}
				else//unknown, stop
				{
					printf("Unknown command.\r\n");
				}
			}
			printf("Leaving servo control mode.\r\n");
		}
		else if(input==999)//exit-set carousel back to 0 slot
		{
			input = 0;
			
			if(input>=0 && input <=104)//index
					{
						if(index-input>0)
						{
							if((104-index)+input<index-input)//forward
							{
								slots = (105-index)+input;
								steps = (int)((slots*30.476)+0.5);

								for(i=0; i<steps;i++)
								{				
									switch(state)
									{
										case 1:
											*((volatile int *)datareg0) = ONE;
											nate = 8;
										break;
										case 2:
											*((volatile int *)datareg0) = TWO;
											nate = 1;
										break;
										case 3:
											*((volatile int *)datareg0) = THREE;
											nate = 2;
										break;
										case 4:
											*((volatile int *)datareg0) = FOUR;
											nate = 3;
										break;
										case 5:
											*((volatile int *)datareg0) = FIVE;
											nate = 4;
										break;
										case 6:
											*((volatile int *)datareg0) = SIX;
											nate = 5;
										break;
										case 7:
											*((volatile int *)datareg0) = SEVEN;
											nate = 6;
										break;
										case 8:
											*((volatile int *)datareg0) = EIGHT;
											nate = 7;
										break;
									}
									state = nate;
									usleep(7000);
								}
								slots = 0;
								steps = 0;
							}
							else//reverse
							{
								slots = index-input;
								steps = (int)((slots*30.476)+0.5);

								for(i=0; i<steps;i++)
								{				
									switch(state)
									{
										case 1:
											*((volatile int *)datareg0) = ONE;
											nate = 2;
										break;
										case 2:
											*((volatile int *)datareg0) = TWO;
											nate = 3;
										break;
										case 3:
											*((volatile int *)datareg0) = THREE;
											nate = 4;
										break;
										case 4:
											*((volatile int *)datareg0) = FOUR;
											nate = 5;
										break;
										case 5:
											*((volatile int *)datareg0) = FIVE;
											nate = 6;
										break;
										case 6:
											*((volatile int *)datareg0) = SIX;
											nate = 7;
										break;
										case 7:
											*((volatile int *)datareg0) = SEVEN;
											nate = 8;
										break;
										case 8:
											*((volatile int *)datareg0) = EIGHT;
											nate = 1;
										break;
									}
									state = nate;
									usleep(7000);
								}
								slots = 0;
								steps = 0;										
							}
						}
						else if(index-input<=0)
						{
							if(index+(104-input)<input-index)//reverse
							{
								slots = index+(105-input);
								steps = (int)((slots*30.476)+0.5);

								for(i=0; i<steps;i++)
								{				
									switch(state)
									{
										case 1:
											*((volatile int *)datareg0) = ONE;
											nate = 2;
										break;
										case 2:
											*((volatile int *)datareg0) = TWO;
											nate = 3;
										break;
										case 3:
											*((volatile int *)datareg0) = THREE;
											nate = 4;
										break;
										case 4:
											*((volatile int *)datareg0) = FOUR;
											nate = 5;
										break;
										case 5:
											*((volatile int *)datareg0) = FIVE;
											nate = 6;
										break;
										case 6:
											*((volatile int *)datareg0) = SIX;
											nate = 7;
										break;
										case 7:
											*((volatile int *)datareg0) = SEVEN;
											nate = 8;
										break;
										case 8:
											*((volatile int *)datareg0) = EIGHT;
											nate = 1;
										break;
									}
									state = nate;
									usleep(7000);
								}
								slots = 0;
								steps = 0;
							}
							else//forward
							{
								slots = input-index;
								steps = (int)((slots*30.476)+0.5);

								for(i=0; i<steps;i++)
								{				
									switch(state)
									{
										case 1:
											*((volatile int *)datareg0) = ONE;
											nate = 8;
										break;
										case 2:
											*((volatile int *)datareg0) = TWO;
											nate = 1;
										break;
										case 3:
											*((volatile int *)datareg0) = THREE;
											nate = 2;
										break;
										case 4:
											*((volatile int *)datareg0) = FOUR;
											nate = 3;
										break;
										case 5:
											*((volatile int *)datareg0) = FIVE;
											nate = 4;
										break;
										case 6:
											*((volatile int *)datareg0) = SIX;
											nate = 5;
										break;
										case 7:
											*((volatile int *)datareg0) = SEVEN;
											nate = 6;
										break;
										case 8:
											*((volatile int *)datareg0) = EIGHT;
											nate = 7;
										break;
									}
									state = nate;
									usleep(7000);
								}
								slots = 0;
								steps = 0;
							}
						}
						index=input;
						printf("Done moving.\r\n");
						//exit(1);
						break;
					}
		}
		else//invalid
		{
			printf("Invalid input.\r\n");
		}
	}	/*end while*/
	printf("Exiting program.\r\n");
	return index;
}






int main(void)
{
	int stage = 0;	/*In stage 0 the server sends the client the list of titles.  In stage 1 the server burns a specific title*/
	int temp = 0, current_index = 0;
	int mobile_app_address_length = 0;
	bool user_input_error = false;
	int i = 0, j = 0, socket_file_desc_1 = 0, socket_file_desc_2 = 0, acceptsocket_file_desc_1 = 0, err_check = 0, title_ID_number = 0, disc_burn = 0, disc_return = 0;
	char *token;
	char read_buffer[MAXCHARSIZE], write_buffer[MAXCHARSIZE], user_input[MAXCHARSIZE], title_name[NUM_OF_TITLES];
	char mobile_app_instruction_1[MAXCHARSIZE], mobile_app_instruction_2[MAXCHARSIZE]; 
	char user_instruction, database[NUM_OF_TITLES][MAXCHARSIZE] = {0}, temp_buffer[MAXCHARSIZE], status_message_from_server[MAXCHARSIZE];
	bool carousel_is_empty = false, carousel_is_full = false;
	
	bool disc_occupancy[NUM_OF_DISC_SLOTS];

	struct sockaddr_in server_address, mobile_app_address;		/*sockaddr_in is a structure that contains Internet address information*/
	struct hostent *server, *app;		/*'server' is a pointer to the hostent structure, which defines a host computer on the Internet (address information in struct members)*/
	
	
	
	//Memory Mapping
	memfd = open("/dev/mem", O_RDWR | O_SYNC);

	if(memfd == -1)
	{
		printf("Error: cannot open memory.");
		return 0;
	}

	base0 = mmap(0, MAP_SIZE,PROT_READ | PROT_WRITE, MAP_SHARED, memfd, dev_b0 & ~MAP_MASK);
	cbase0 = mmap(0, MAP_SIZE,PROT_READ | PROT_WRITE, MAP_SHARED, memfd, dev_c0 & ~MAP_MASK);
	cbase1 = mmap(0, MAP_SIZE,PROT_READ | PROT_WRITE, MAP_SHARED, memfd, dev_c1 & ~MAP_MASK);
	cbase2 = mmap(0, MAP_SIZE,PROT_READ | PROT_WRITE, MAP_SHARED, memfd, dev_c2 & ~MAP_MASK);
	cbase3 = mmap(0, MAP_SIZE,PROT_READ | PROT_WRITE, MAP_SHARED, memfd, dev_c3 & ~MAP_MASK);
	mbase0 = mmap(0, MAP_SIZE,PROT_READ | PROT_WRITE, MAP_SHARED, memfd, dev_m0 & ~MAP_MASK);
	mbase1 = mmap(0, MAP_SIZE,PROT_READ | PROT_WRITE, MAP_SHARED, memfd, dev_m1 & ~MAP_MASK);
	mbase2 = mmap(0, MAP_SIZE,PROT_READ | PROT_WRITE, MAP_SHARED, memfd, dev_m2 & ~MAP_MASK);
	mbase3 = mmap(0, MAP_SIZE,PROT_READ | PROT_WRITE, MAP_SHARED, memfd, dev_m3 & ~MAP_MASK);		

	if(base0 == (void*)-1||cbase0 == (void*)-1||cbase1 == (void*)-1||cbase2 == (void*)-1||cbase3 == (void*)-1||mbase0 == (void*)-1||mbase1 == (void*)-1||mbase2 == (void*)-1||mbase3 == (void*)-1)
	{
		printf("Error: cannot map memory.");
		return 0;
	}

	datareg0 = base0 + (dev_data0 & MAP_MASK);
	trisreg0 = base0 + (dev_tris0 & MAP_MASK);
	
	
	*((volatile int *)datareg0) = ONE;
	//End Memory Mapping
	
	
	

	socket_file_desc_2 = socket(AF_INET, SOCK_STREAM, 0);
	if(socket_file_desc_2 < 0)
		printf("ERROR: Mobile application socket could not be opened.\n");
	//setsockopt(socket_file_desc_2, SOL_SOCKET, SO_REUSEADDR, (char*)&iSetOption, sizeof(iSetOption));	/*Violates TCP/IP protocol but allows the program to be restarted without TIME_WAIT socket delay*/
		
	bzero(&mobile_app_address, sizeof(mobile_app_address));	/*Initializing the mobile address buffer to 0*/

	/***Setting the fields in mobile_app_address***/
	/*The sockaddr_in structure specifies the address family, IP address, and port for the socket that is being bound.*/
	mobile_app_address.sin_family = AF_INET;
	mobile_app_address.sin_port = htons(APPPORTNUMBER);	/*htons() converts a port number in host byte order to a port number in network byte order*/
	mobile_app_address.sin_addr.s_addr = INADDR_ANY;
	
	
	/***Setting up connection to mobile application***/
	mobile_app_address_length = sizeof(mobile_app_address);
	err_check = bind(socket_file_desc_2, (struct sockaddr *) &mobile_app_address, sizeof(mobile_app_address));	/*Binds socket to server IP address and port*/
	if(err_check < 0)
		perror("ERROR: Failure on binding listening socket.");
	
	
	
	

	memset(disc_occupancy, false, NUM_OF_DISC_SLOTS);	/*Assuming all slots are empty initially*/

	while(1)
	{
		socket_file_desc_1 = socket(AF_INET, SOCK_STREAM, 0);		/*Creating the socket: AF_INET for Internet address domain, SOCK_STREAM for continuous stream socket type, and 0 to choose TCP as protocol for stream*/
		if(socket_file_desc_1 < 0)
			printf("ERROR: Server socket could not be opened.\n");
			
		server = NULL;	
		while(server == NULL)
		{		
			server = gethostbyname("192.168.0.100");		/*Takes name of server and returns a pointer to a hostent structure (defined 'server') containing information about the host*/
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
				memset(&read_buffer, '\0', sizeof(read_buffer));	
				printf("Waiting for instructions from mobile application...\n");
				listen(socket_file_desc_2, 5);	/*Server listens to the socket for connections*/
				

				acceptsocket_file_desc_1 = accept(socket_file_desc_2,  (struct sockaddr *) &mobile_app_address, &mobile_app_address_length);	/*accept() pauses the server process until a client connection is made*/
				if(acceptsocket_file_desc_1 < 0)
					printf("ERROR: accept() command failed.\n");
					
				err_check = read(acceptsocket_file_desc_1, read_buffer, MAXCHARSIZE);
				if(err_check < 0)
				{
					perror("ERROR: Failure receiving data on socket.");
					close(acceptsocket_file_desc_1);	/*Closes the socket*/
					continue;
				}	/*end if*/			

				
				
				/*Tokenizing app command and image number*/
				//strtok(temp_buffer, " ");	/*Resetting strtok() function*/
				token = strtok(read_buffer, " ");
				strcpy(mobile_app_instruction_1, token);
				token = strtok(NULL, " ");	/*NULL allows strok to continue scanning from where it previously left off*/
				strtok(NULL, " ");
				strcpy(mobile_app_instruction_2, token);
				title_ID_number = atoi(mobile_app_instruction_2);
				
				printf("instruction 1 = %s", mobile_app_instruction_1);
				printf("instruction 2 = %d", title_ID_number);
				
				if((!strcmp(mobile_app_instruction_1, "BURN") || !strcmp(mobile_app_instruction_1, "RETURN") || !strcmp(mobile_app_instruction_1, "PANIC")) && (title_ID_number >= 0 && title_ID_number <= 24))
					user_input_error = false;
				else
					printf("ERROR: Command from mobile application is invalid.\n");
				
			}	/*end while*/
			
			if(!strcmp(mobile_app_instruction_1, "RETURN"))
			{
				printf("Please wait while disc carousel rotates in position...\n");
				for(i = 0; i < NUM_OF_DISC_SLOTS; i++)	/*Checking for the first available slot*/
				{
					carousel_is_full = true;
					if(i == 0 || i == 101 || i == 102 || i == 104)	/*These particular slots on the carousel are covered by plastic and can't be used to store discs*/
						continue;
							
					else if(disc_occupancy[i] == false)
					{	
						disc_return = i;	/*'disc_return' will hold the value of the slot that needs to point out to the customer for disc return*/
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
				{
					current_index = device_control(disc_return, current_index, 0, 0);
					current_index = device_control(153, current_index, 0, 0);	/*User has 5 seconds to return disc*/
				}
					
				continue;				
			}	/*end if*/
			
			if(!strcmp(mobile_app_instruction_1, "PANIC"))
			{
				printf("PANIC!\n");
				current_index = device_control(998, current_index , 0, 0);
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
			
			
			/*Instructing server to burn particular image*/
			printf("Desired title is image %d.\n", title_ID_number);				
			if(title_ID_number < 0 || title_ID_number > NUM_OF_TITLES)
			{
				printf("ERROR:  Entered value is invalid.\n");
				continue;
			}	/*end if*/
			
			
			
			for(i = 0; i < NUM_OF_DISC_SLOTS; i++)	/*Checking for the first available disc*/
			{
				carousel_is_empty = true;
				if(i == 0 || i == 101 || i == 102 || i == 104);	/*These particular slots on the carousel are covered by plastic and can't be used to store discs*/
				

				else if(disc_occupancy[i] == true)
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
				//disc_carousel_rotation(disc_burn);
				//insert_disc();	/*Inserts disc into DVD burner*/
				printf("disc_burn = %d\n", disc_burn);
				current_index = device_control(disc_burn, current_index, 0, 0);	/*Rotates disc carousel into position to load DVD*/
				current_index = device_control(150, current_index, 0, 0);	/*Loads disc into DVD burner*/
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
			//Wait for message from server that burning operation is done.  Then return disc to customer.
			err_check = read(socket_file_desc_1, status_message_from_server, MAXCHARSIZE);
			if(err_check < 0)
			{
				printf("ERROR: Failure writing burn command on socket.\n");
				close(socket_file_desc_1);
				continue;
			}	/*end if*/
			

			/*if(!strcmp(status_message_from_server, "Burn complete.  Disc is ready."));
			
			else if(!strcmp(status_message_from_server, "Burn failed."))
			{
				printf("Burn failed.\n");
				break;
			}	/*end if*/
			
			/*else
				printf("Invalid message sent back from server.\n");*/
			
			current_index = device_control(151, current_index, 0, 0);	/*Unload disc from DVD burner to place DVD back into slot*/
			current_index = device_control(152, current_index, 0, 0);	/*Rotate carousel and eject disc from service slot*/
		
			
			stage = 0;
		}	/*end else if*/
		
		close(socket_file_desc_1);

	}	/*end while*/
	
	
	return(0);
}	/*end main*/
