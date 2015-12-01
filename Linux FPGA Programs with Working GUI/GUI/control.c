#include "stdio.h"
#include "stdlib.h"
#include "fcntl.h"
#include "sys/mman.h"

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



int main(void)
{
	int input = 0;
	int motin = 0;
	int servin = 0;
	int i = 0;
	int counter = 0;
	int state = 1;
	int nate = 1;
	int steps = 0;
	int slots = 0;

	int index = 0;
	int outdex = 0;//change this


	//Memory Mapping
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

	//End Memory Mapping

	//Output Loop
	*((volatile int *)trisreg0) = 0b00000000;//set GPIO 0 tristates to output
	*((volatile int *)datareg0) = RESET;//set all channels to output 0
	while(input != 999)
	{
		
		servin = 0;
		motin = 0;

		printf("Enter command: [999 to exit]\r\n");
		scanf("%d",&input);
		printf("Entered: %d\r\n",input);

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
		else if(input==999)//exit
		{
			//nothing
		}
		else//invalid
		{
			printf("Invalid input.\r\n");
		}
	}
	printf("Exiting program.\r\n");
	return 1;
}