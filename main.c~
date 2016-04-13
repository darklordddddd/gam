#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#define LENGTH 128

int main(int argc, char* argv[]) //-nkey -l <number>
{
	int PID;
	int pipefd[2];
	int fd;
	int len; //key length
	char kk[LENGTH+1];

	if (pipe(pipefd) < 0)
	{
		perror("Pipe error");
		exit(0);
	}

	if ((PID = fork()) < 0)
	{
		perror("Fork error");
		exit(0);	
	}

	if (PID == 0) //generator
	{
		close(pipefd[0]); //close read
		char * gkey = (char*) malloc(LENGTH + 1);
		//generate a key
		int i;
		//random init
		time_t ti;
		char st[128];
		(void) time(&ti);
		(void) initstate((unsigned int)(ti + getpid()), st, sizeof(st));
		(void) setstate(st);
		//and generate!
		for (i = 0; i < LENGTH; i++)
			gkey[i] = random()%128;
		//send it
		write(pipefd[1], gkey, LENGTH);
		close(pipefd[1]);
		free(gkey);
		_exit(0);
	}
	else //main program
	{
		//if (-nkey) flag - new key requested
		//send information to gen
		char * rkey = (char*) malloc(LENGTH + 1);
		close(pipefd[1]); //close write
		read(pipefd[0], rkey, LENGTH);
		printf("Key: %s\n\n", rkey);
		close(pipefd[0]);
		free(rkey);
		exit(0);
	}
	/*
	close(fd);
	remove("key");
	//no file, create it
	if ((fd = open("key", O_RDWR | O_CREAT)) < 0)
	{
		perror("Key error");
		close(fd);
		_exit(0);
	}
	close(fd);
	*/
	
	//waitpid(PID);	
	return 1;
}
