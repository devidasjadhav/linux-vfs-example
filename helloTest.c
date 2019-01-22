#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

int main(){
	char fdStr[4],buff[20];
	int fd,NewFD, err;
	
	fd = open("/sys/module/hello/parameters/test",O_RDWR);
	err = read(fd,fdStr,4);
	sscanf(fdStr,"%d",&NewFD);
	printf("READ %d:-> %s\n",NewFD,fdStr);
	close(fd);
	err = write(NewFD,"Test12345",9);
	err = read(NewFD,buff,20);
	printf("Read data from FD %d %s\n",NewFD,buff);

	close(NewFD);

	return 0;

}
