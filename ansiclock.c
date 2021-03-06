
#include <string.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

//#include "term.h"

#ifndef __TERM_EXAMPLE__
#define __TERM_EXAMPLE__

#define ESC "\033"

//Format text
#define RESET 		0
#define BRIGHT 		1
#define DIM			2
#define UNDERSCORE	3
#define BLINK		4
#define REVERSE		5
#define HIDDEN		6

//Foreground Colours (text)

#define F_BLACK 	30
#define F_RED		31
#define F_GREEN		32
#define F_YELLOW	33
#define F_BLUE		34
#define F_MAGENTA 	35
#define F_CYAN		36
#define F_WHITE		37

//Background Colours
#define B_BLACK 	40
#define B_RED		41
#define B_GREEN		42
#define B_YELLOW	44
#define B_BLUE		44
#define B_MAGENTA 	45
#define B_CYAN		46
#define B_WHITE		47

#endif /*__TERM_EXAMPLE__*/
/*ANSI/VT100 Terminal using example */


#define home() 			printf(ESC "[H") //Move cursor to the indicated row, column (origin at 1,1)

#define clrscr()		printf(ESC "[2J") //clear the screen, move to (1,1)

#define gotoxy(x,y)		printf(ESC "[%d;%dH", y, x);
#define visible_cursor() printf(ESC "[?251");

/* 
Set Display Attribute Mode	<ESC>[{attr1};...;{attrn}m
*/
#define resetcolor() printf(ESC "[0m")
#define set_display_atrib(color) 	printf(ESC "[%dm",color)


void frame_draw () 
{
	home();
	set_display_atrib(B_BLUE);
                //            123456789012345678901
           	//puts(	"┌─────────┐┌─────────┐\n" //0

			//"├─────────┤\n" //4
           	puts(	"┌─────────┐\n" //0
			"│         │\n" //1
			"│         │\n" //2
			"│         │\n" //3
			"└─────────┘"); //9

/*
			"│         │┌─────────┐\n" //4
			"│         ││         │\n" //1
			"│         ││         │\n" //2
			"│         ││         │\n" //3
			"│         │├─────────┤\n" //4
			"│         ││         │\n" //5
			"│         ││         │\n" //6
			"│         ││         │\n" //7
			"│         ││         │\n" //8
			"└─────────┘└─────────┘\n" //9
			"┌────────────────────┐\n" //10
			"│                    │\n" //11
			"└────────────────────┘");  //12
     */

	resetcolor();
}

void print_port_bits (unsigned char port) {
	int i;
	unsigned char maxPow = 1<<(8-1);
	set_display_atrib(B_BLUE);
	set_display_atrib(BRIGHT);
	for(i=0;i<8;++i){
		// print last bit and shift left.
		gotoxy(2,2 + i);
		if (port & maxPow) {
			set_display_atrib(F_GREEN);
			printf("pin%d= on ",i);
		} else {
			set_display_atrib(F_RED);
			printf("pin%d= off",i);			
		}
		port = port<<1;
	}
	resetcolor();
}


void print_accelerometer(float * a) {
	int i;
	const char * coordinate = "XYZ";
	set_display_atrib(B_BLUE);
	set_display_atrib(BRIGHT);
	int display_atrib = F_RED;
	set_display_atrib(display_atrib);
	for (i = 0; i < 3; i++) {
		set_display_atrib(display_atrib + i);
		gotoxy(13,2+i);
		printf("A%c=%+1.3f", coordinate[i], a[i]);
	}
	

	resetcolor();
}



void print_alarm (int alarm_error) 
{
	gotoxy(2,12);
	gotoxy(1,12-6);
	if (alarm_error) {
		set_display_atrib(BRIGHT);
		set_display_atrib(B_RED);
		printf("        ERROR       ");
	} else {
		set_display_atrib(BRIGHT);
		set_display_atrib(B_GREEN);
		//printf("         OK         ");
		printf(" | TIME |  ");
	}
}


void print_time_date (struct tm* tm_info) 
{
	char buffer[12];
	set_display_atrib(BRIGHT);
	set_display_atrib(B_BLUE);
	strftime(buffer, 12, "%d.%m.%y", tm_info);
	gotoxy(13,7)
	gotoxy(2,2);
	puts(buffer);
	strftime(buffer, 10, "%H:%M:%S", tm_info);
	gotoxy(13,8)
	gotoxy(2,3);
	puts(buffer);
	resetcolor();
}

unsigned char rol(unsigned char  a)
{
	return (a << 1) | (a >> 7);
}



void controller_emulator () 
{
	unsigned char  port = 0x01;
	int alarm_counter = 0;
	time_t timer;
	struct tm* tm_info;
	srand(time(NULL));  
	while (1) {
		float a[3] = {0.0, 0.0, 0.0};
		int i;
		for (i=0; i<3; i++) {
			int r = rand() % 101 + (-50);
			a[i] += (float)r / 1000;
		}
		if (alarm_counter < 30) {
			print_alarm(0);
		}
		if ((alarm_counter < 60) && (alarm_counter >= 30)) {
			//print_alarm(1);
		}
		if (alarm_counter > 60) {
			alarm_counter = 0;
		}
		alarm_counter++;
		time(&timer);
		tm_info = localtime(&timer);
//********************************//
		//print_accelerometer(a);
		//print_port_bits(port);
		print_time_date(tm_info);
//********************************//
		port = rol(port);
		gotoxy(1,18);
		fflush(stdout);
		usleep(100000);
	}
}


int main (void) 
{
	home();
	clrscr();
	frame_draw();
	controller_emulator();
	return 0;
}
