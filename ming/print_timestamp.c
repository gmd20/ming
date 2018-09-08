#include <sys/time.h>
#include <time.h>
#include <stdio.h>


time_t last_timestamp = 0;
char timestamp[32] = {0};

void print_timestamp()
{
	struct timeval tv;
	struct tm * now;
	unsigned int miliseconds;
	int a, b, c;
	gettimeofday(&tv, NULL);
	if (last_timestamp != timestamp) {
		now = localtime(&tv.tv_sec);
		strftime(timestamp, 32, "%Y-%m-%d %H:%M:%S.", now);
		miliseconds = (unsigned int)(tv.tv_usec/1000);
		a = miliseconds/100;
		b = miliseconds % 100;
		c = b % 10;
		b = b / 10;
		timestamp[20] = '0' + a;
		timestamp[21] = '0' + b;
		timestamp[22] = '0' + c;
		timestamp[23] = '\0';
		last_timestamp = tv.tv_sec;
	}
	puts(timestamp);
}
