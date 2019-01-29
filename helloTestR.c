#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

int main(){
	char fdStr[4],buff[20];
	int fd,NewFD, err;
	
	fd = open("/sys/module/epoll/parameters/test",O_RDWR);
	err = read(fd,fdStr,4);
	sscanf(fdStr,"%d",&NewFD);
	printf("READ %d:-> %s\n",NewFD,fdStr);
	close(fd);
	printf("READ Before READ\n");
	err = read(NewFD,buff,20);
	printf("READ After READ\n");
	if(err > 0)
		printf("Read data from FD %d %s %d\n",NewFD,buff,err);
	else
		printf("Read data from FD %d %d\n",NewFD,err);
	close(NewFD);

	return 0;

}
