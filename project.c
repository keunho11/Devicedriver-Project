#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <time.h>
#include <string.h>

#define MAX_BUFF 32
#define LINE_BUFF 16
#define MAX_BUTTON 9
#define PUSH_DEVICE "/dev/fpga_push_switch"
#define TEXT_DEVICE "/dev/fpga_text_lcd"
#define BUZZER_DEVICE "/dev/fpga_buzzer"

unsigned char quit = 0;
int hour, min , sec = 0;

void user_signal1(int sig)
{
	quit = 1;
}

void timer()
{
	sec++;
	if(sec == 60)
	{
		sec = 0;
		min ++;
	} 
	if(min == 60)
	{
		min =0;
		hour++;
	}	


	printf("nowtime %d:%d:%d \n", hour, min, sec);	
}

int createTimer(timer_t *timerID, int sec, int msec)
{
	struct sigevent te;
	struct itimerspec its;
	struct sigaction sa;
	int sigNo = SIGRTMIN;

	sa.sa_flags = SA_SIGINFO;
	sa.sa_sigaction = timer;
	sigemptyset(&sa.sa_mask);

	if(sigaction(sigNo, &sa, NULL)== -1)
	{
		printf("sigactino erroe\n");
		return -1;	
	}

	te.sigev_notify = SIGEV_SIGNAL;
	te.sigev_signo = sigNo;
	te.sigev_value.sival_ptr = timerID;
	timer_create(CLOCK_REALTIME, &te, timerID);

	its.it_interval.tv_sec = sec;
	its.it_interval.tv_nsec = msec *1000000;
	its.it_value.tv_sec = sec;

	its.it_value.tv_nsec = msec * 1000000;
	timer_settime(*timerID, 0, &its, NULL);

	return 0;

}

void Game(int dev_push, int  dev_text ,int  dev_buzzer)
{
	int ran1, ran2;
	int result;
	int cnt=0;
	int ans = 0;
	int ans_cnt =0;
	int ans_10, ans_1;
	int j =0;
	int flg =0;
	unsigned char string[33]="                                ";	
	unsigned char string2[33]="score:                          ";
	unsigned char string3[33]="restart                         ";	
	unsigned char string4[33]="                                ";	
	
	int buff_size;
	unsigned char push_sw_buff[MAX_BUTTON];
	(void)signal(SIGINT, user_signal1);
	buff_size = sizeof(push_sw_buff);
	printf("Press <ctrl+c> to quit\n");
	
	memset(push_sw_buff, 0 , sizeof(push_sw_buff));
	while(cnt != 3)
	{
	
	if(flg ==0)
	{	
		string[4] = ' ';
		string[5] = ' ';
		
		do{
		srand(time(NULL));
		ran1 = (rand()%9)+1;
		ran2 = (rand()%9)+1;
		result = ran1*ran2;
		}while((result%10 ==0) || (result<=10));

		printf("result: %d\n", result);
		string[0] = ran1 +48;
		string[2] = ran2 +48;
		string[1] = '*';
		string[3] = '=';
		write(dev_text, string, MAX_BUFF);
		
		flg =1;
	}
	if(flg==1)
	{
		usleep(300000);
		read(dev_push, &push_sw_buff, buff_size);
		
		for(j=0; j<MAX_BUTTON;  j++)
		{
		if(push_sw_buff[j] == 1)
		{
			if(ans_cnt ==0){
			ans_10 = j+1;
			ans_cnt = 1;
			printf("ans_10: %d\n", ans_10);
			}
			else{
			ans_1=j+1;
			ans_cnt=0;
			printf("ans_0: %d\n", ans_1);
			flg = 2;
			}
		}
		}
	}
	if(flg ==2)
	{
		ans = ans_10*10 + ans_1;
		printf("answer : %d \n", ans);
		string[4] = ans_10 +48;
		string[5] = ans_1 +48;
		write(dev_text,string,MAX_BUFF);
		sleep(2);		

		if(result == ans){
		cnt++;
		string2[6] = cnt+48;
		write(dev_text, string2, MAX_BUFF);
		sleep(2);
		printf("cnt : %d\n", cnt);
		
		flg = 0;
		}
		else{	
		write(dev_text, string3, MAX_BUFF);
		write(dev_text, string4, MAX_BUFF);
		write(dev_text, string, MAX_BUFF);
		sleep(2);
 
		
		flg = 0;
		}
		
	}	
	}
	
}

int main(void)
{
	timer_t timerID;
	int dev_push, dev_text, dev_buzzer;

	int buff_size;
	unsigned char push_sw_buff[MAX_BUTTON];

	unsigned char string[33]="00:00:00                         ";
	unsigned char string2[33]="00:00:00   arlam                ";
	unsigned char string3[33] = "                                ";
	unsigned char string4[33] = "arlam ON                        ";
	unsigned char string5[33] = "END GAME                      ";
	unsigned char string6[33]="00:00:00                         ";
	
	int j = 0;
	int i = 0;
	int setnum =0;
	int lcdtime[6]={0,0,0,0,0,0};
	int lcdtime_a[6]={0,0,0,0,0,0};
	int sec_a, min_a, hour_a =0;
	int data;

	////////////////////////////////
	dev_push = open(PUSH_DEVICE,O_RDWR);
	if(dev_push <0){
		printf("Push Open Error\n");
		close(dev_push);
		return -1;
		}

	dev_text = open(TEXT_DEVICE,O_WRONLY);
        if(dev_text <0){
                printf("Text Open Error\n");
                close(dev_text);
                return -1;
                }
	
	dev_buzzer = open(BUZZER_DEVICE, O_RDWR);
	if(dev_buzzer <0){
		printf("Buzzer Open Error\n");
		exit(1);
	}

	(void)signal(SIGINT, user_signal1);
	buff_size = sizeof(push_sw_buff);
	printf("Press <ctrl+c> to quit\n");

	////////////////////////////
	
	write(dev_text,string3,MAX_BUFF);	

	while(i != 6)
	{

		usleep(270000);
		read(dev_push, &push_sw_buff, buff_size);
		
		for(j=0; j<MAX_BUFF;  j++)
		{
		if(push_sw_buff[j] == 1)
		{
			if(j==1)
			{			
				if(i == 0){
				if(lcdtime[i] == 2)
					lcdtime[i] =2;
				else
					lcdtime[i]++;
				}
				else if(i ==1){
				if(lcdtime[i] == 3)
					lcdtime[i] =3;
				else
					lcdtime[i]++;
				}
				else if(i==3 || i ==5){
				if(lcdtime[i] == 9)
					lcdtime[i] =9;
				else
					lcdtime[i]++;	
				} 	
				else{
				if(lcdtime[i] == 5)
					lcdtime[i] =5;
				else
					lcdtime[i]++;			
				}
			}
			if(j==7)
			{		
				if(lcdtime[i] == 0)
					lcdtime[i] =0;
				else
					lcdtime[i]--;
			}
			if(j==3)
			{
				if(i==0)
				i=0;
				else
				i--;
			}
			if(j==5)
			{		
				i++;
			}

		}
		}
		string[0]= lcdtime[0]+48;
		string[1]= lcdtime[1]+48;
		string[3]= lcdtime[2]+48;
		string[4]= lcdtime[3]+48;
		string[6]= lcdtime[4]+48;
		string[7]= lcdtime[5]+48;
			write(dev_text,string,MAX_BUFF);
		hour = lcdtime[0]*10 +lcdtime[1];
		min  = lcdtime[2]*10 +lcdtime[3];
		sec  = lcdtime[4]*10 +lcdtime[5];
		
	}
	
	createTimer(&timerID, 1, 0);
	j =0 ;
	i =0 ;
	memset(lcdtime,0,6*sizeof(int));

	while(i != 6)
	{
		usleep(270000);
		read(dev_push, &push_sw_buff, buff_size);
		
		for(j=0; j<MAX_BUFF;  j++)
		{
		if(push_sw_buff[j] == 1)
		{
			if(j==1)
			{			
				if(i == 0){
				if(lcdtime[i] == 2)
					lcdtime[i] =2;
				else
					lcdtime[i]++;
				}
				else if(i ==1){
				if(lcdtime[i] == 3)
					lcdtime[i] =3;
				else
					lcdtime[i]++;
				}
				else if(i==3 || i ==5){
				if(lcdtime[i] == 9)
					lcdtime[i] =9;
				else
					lcdtime[i]++;	
				} 	
				else{
				if(lcdtime[i] == 5)
					lcdtime[i] =5;
				else
					lcdtime[i]++;			
				}
			}
			if(j==7)
			{		
				if(lcdtime[i] == 0)
					lcdtime[i] =0;
				else
					lcdtime[i]--;
			}
			if(j==3)
			{
				if(i==0)
				i=0;
				else
				i--;
			}
			if(j==5)
			{		
				i++;
			}
		}
		}
		string2[0]= lcdtime[0]+48;
		string2[1]= lcdtime[1]+48;
		string2[3]= lcdtime[2]+48;
		string2[4]= lcdtime[3]+48;
		string2[6]= lcdtime[4]+48;
		string2[7]= lcdtime[5]+48;
			write(dev_text,string2,MAX_BUFF);
		hour_a = lcdtime[0]*10 +lcdtime[1];
		min_a  = lcdtime[2]*10 +lcdtime[3];
		sec_a  = lcdtime[4]*10 +lcdtime[5];
		
	}

	usleep(400000);	
	printf("arlam :%d:%d:%d \n", hour_a, min_a, sec_a);
	write(dev_text,string3,MAX_BUFF);
	
	while(!quit)
	{	
		string6[0] = (hour/10)+48;
		string6[1] = (hour%10)+48;
		string6[3] = (min/10)+48;
		string6[4] = (min%10)+48;
		string6[6] = (sec/10)+48;
		string6[7] = (sec%10)+48;
		write(dev_text,string6,MAX_BUFF);

		if(sec == sec_a)
			if(min == min_a)
				if(hour == hour_a){	
					data =1 ;
					write(dev_buzzer, &data, 1);
					usleep(1000000);

					write(dev_text,string4,MAX_BUFF);
					sleep(2);

					Game(dev_push, dev_text, dev_buzzer);
					write(dev_text,string5,MAX_BUFF);
					
					data =0;
					write(dev_buzzer, &data, 1);
					     }
	
	}

	close(dev_push);
	close(dev_text);
	close(dev_buzzer);
	return(0);
}
