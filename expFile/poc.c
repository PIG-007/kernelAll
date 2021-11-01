#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/mman.h>
#include <assert.h>


int main(int argc, char *argv[])
{
	int fd;
	unsigned char payload[237] = {0};
	unsigned char *p           = payload;
	unsigned long memOffset;

	if ((fd = open("/dev/stack", O_RDWR)) < 0) {
		printf("    Can't open device file: /dev/stack\n");
		exit(1);
	}



	return 0;
}