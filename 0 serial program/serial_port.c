/************************************************************

http://www.tldp.org/HOWTO/Serial-Programming-HOWTO/
http://wiki.kldp.org/wiki.php/Serial-Programming-HOWTO

v1.0, 5 June 2012 Dongwoo Lee, dongwoolee@dal.snu.ac.kr 

*************************************************************/


#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include <pthread.h>

/* Baudrate 설정은 <asm/termbits.h>에 정의되어 있다. */
/* <asm/termbits.h>는 <termios.h>에서 include된다. */
#define BAUDRATE B57600
/* 여기의 포트 장치 파일을 바꾼다. COM1="/dev/ttyS1, COM2="/dev/ttyS2 */
#define MODEMDEVICE "/dev/ttyUSB0"
#define _POSIX_SOURCE 1 /* POSIX 호환 소스 */

#define FALSE 0
#define TRUE 1

#define WRITE_STATE 0
#define READ_STATE 1

long long Begin_Time;
long long Diff_Time;

static long long GetTime(void)
{
    struct timeval tv;
    gettimeofday(&tv,0);
    return tv.tv_sec * 1000000LL + tv.tv_usec;
}


volatile int STOP=FALSE;

int main( int argc, char *argv[])
{

	/* read data from input file*/
	char *input_file;
	char *output_file;
	char **data_array;

	if(argc!=3) {
		printf(" the number of arguments is incorret\n");
		return -1;
	}
	input_file = argv[1];
	output_file = argv[2];

	FILE *rd;
	FILE *wd;
	int line_num=0;
	
	rd = fopen(input_file, "r");
	fscanf( rd, "%d", &line_num);
	printf("line num: %d\n", line_num);

	int i;	
	data_array = (char **) malloc(line_num * sizeof(char*));
	for(i=0; i<line_num; i++){
		data_array[i] = (char*) malloc(256*sizeof(char));
	}
	
	for(i=0; i<line_num; i++) {
		fscanf(rd, "%s", data_array[i]);	
	}
	fclose(rd);

	/* open file for write */
	wd = fopen(output_file, "w");


	/*serial port*/

	int fd,c, res;
	struct termios oldtio,newtio;
	char buf[255];

	/* 읽기/쓰기 모드로 모뎀 장치를 연다.(O_RDWR)
	   데이터 전송 시에 <CTRL>-C 문자가 오면 프로그램이 종료되지 않도록
	   하기 위해 controlling tty가 안되도록 한다.(O_NOCTTY)
	 */
	fd = open(MODEMDEVICE, O_RDWR | O_NOCTTY | O_NONBLOCK );

	fcntl(fd, F_SETFL, 0);

	if (fd <0) {perror(MODEMDEVICE); return -1; }
	else { printf("port open\n\n");}

	tcgetattr(fd,&oldtio); /* save current serial port settings */
	bzero(&newtio, sizeof(newtio)); /* clear struct for new port settings */
	
	/*
		BAUDRATE: 전송 속도. cfsetispeed() 및 cfsetospeed() 함수로도 세팅 가능
		CRTSCTS : 하드웨어 흐름 제어. (시리얼 케이블이 모든 핀에 연결되어 있는
		경우만 사용하도록 한다. Serial-HOWTO의 7장을 참조할 것.)
		CS8     : 8N1 (8bit, no parity, 1 stopbit)
		CLOCAL  : Local connection. 모뎀 제어를 하지 않는다.
		CREAD   : 문자 수신을 가능하게 한다.
	 */
	newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;

	/*
		IGNPAR   : Parity 에러가 있는 문자 바이트를 무시한다.
		ICRNL    : CR 문자를 NL 문자로 변환 처리한다. (이 설정을 안하면 다른
		컴퓨터는 CR 문자를 한 줄의 종료문자로 인식하지 않을 수 있다.)
		otherwise make device raw (no other input processing)
	 */
	newtio.c_iflag = IGNPAR | ICRNL;

	/*
	   Raw output.
	 */
	newtio.c_oflag = 0;
	/*
		ICANON   : canonical 입력을 가능하게 한다.
		disable all echo functionality, and don't send signals to calling program
	 */
	newtio.c_lflag = ~(ICANON|ECHO|ECHOE|ISIG);

	/*
	   모든 제어 문자들을 초기화한다.
	   디폴트 값은 <termios.h> 헤더 파일에서 찾을 수 있다. 여기 comment에도
	   추가로 달아놓았다.
	 */
	newtio.c_cc[VINTR]    = 0;     /* Ctrl-c */
	newtio.c_cc[VQUIT]    = 0;     /* Ctrl-\ */
	newtio.c_cc[VERASE]   = 0;     /* del */
	newtio.c_cc[VKILL]    = 0;     /* @ */
	newtio.c_cc[VEOF]     = 4;     /* Ctrl-d */
	newtio.c_cc[VTIME]    = 1;     /* inter-character timer unused */
	newtio.c_cc[VMIN]     = 1;     /* blocking read until 1 character arrives */
	newtio.c_cc[VSWTC]    = 0;     /* '\0' */
	newtio.c_cc[VSTART]   = 0;     /* Ctrl-q */
	newtio.c_cc[VSTOP]    = 0;     /* Ctrl-s */
	newtio.c_cc[VSUSP]    = 0;     /* Ctrl-z */
	newtio.c_cc[VEOL]     = 0;     /* '\0' */
	newtio.c_cc[VREPRINT] = 0;     /* Ctrl-r */
	newtio.c_cc[VDISCARD] = 0;     /* Ctrl-u */
	newtio.c_cc[VWERASE]  = 0;     /* Ctrl-w */
	newtio.c_cc[VLNEXT]   = 0;     /* Ctrl-v */
	newtio.c_cc[VEOL2]    = 0;     /* '\0' */

	/*
	   이제 modem 라인을 초기화하고 포트 세팅을 마친다.
	 */
	tcflush(fd, TCIFLUSH);
	tcsetattr(fd,TCSANOW,&newtio);

	int count =0;
	int state = WRITE_STATE;
	while (STOP==FALSE ) {  

		if(state == WRITE_STATE) {
			sleep(1);
			res=write(fd, data_array[count], strlen(data_array[count]));
			Begin_Time = GetTime();
			printf("send data: %s\n", data_array[count]);
			printf("recieve waitng ...\n");
			state = READ_STATE;
		}
		else if(state == READ_STATE) {
			char buf2[255];
			memset(buf2,0,255);
			
			res = 0;
			/* read serial port until '!' comes in*/
			while(strchr(buf2, '!') == NULL){
				res+=read(fd, buf2+res, 255);

				Diff_Time = GetTime()-Begin_Time;
				usleep(1);				
			}
	
			
			fprintf(wd,"%s %s %lld\n",data_array[count], buf2, Diff_Time);	
			printf("recieve: %s \n", buf2);
			printf("time: %.3f ms\n\n", Diff_Time*1E-3);
			state = WRITE_STATE;
			count++;
		}	

		if(count==line_num) STOP = TRUE;

	}

	/* close write file */
	fclose(wd);
	
	/* restore the old port settings */
	tcsetattr(fd,TCSANOW,&oldtio);

	/*free data array*/
	for(i=0; i<line_num; i++) {
		free(data_array[i]);		
	}
	free(data_array);
}

