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
	int fd, fd1;
	int len; //key length
	char kk[LENGTH+1];
	char buffer[1024], mbuffer[1024];

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
		if ((fd = open("key", O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) < 0)
		{
			//key file isn't found
			close(fd);
			remove("key");
			//no file, create it
			if ((fd = open("key", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) < 0)
			{
				perror("Key file error");
				close(fd);
				_exit(0);
			}
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
				kk[i] = random()%128;
			//write key to the file;
			write(fd, kk, LENGTH);
		}
		else
		{
			//read our key from file
			read(fd, kk, LENGTH);
		}
		close(fd);
		close(pipefd[0]); //close read
		//send it
		write(pipefd[1], kk, LENGTH);
		close(pipefd[1]);
		_exit(0);
	}
	else //main program
	{
		close(pipefd[1]); //close write
		read(pipefd[0], kk, LENGTH);
		printf("Key: %s\n\n", kk);
		close(pipefd[0]);
		//exit(0);
	}

	//create another pipe

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

	if (PID == 0) //text
	{
		if (fd = open("stext", O_RDONLY) < 0)
		{
			perror("Source text file error");
			close(fd);
			_exit(0);
		}
		int hh;
		while ((hh = read(fd, buffer, 1024)) > 0) 
			write(pipefd[1], buffer, hh);
		close(pipefd[0]); //close read
		close(fd);
		close(pipefd[1]);
		_exit(0);
	}
	else
	{
		close(pipefd[1]); //close write
		if ((fd1 = open("entext", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) < 0)
		{
			perror("Encrypted text file error");
			remove("key");
			close(fd);
			exit(0);
		}
		int hh;
		int t_cnt, k_cnt;
		while ((hh = read(pipefd[0], mbuffer, 1024)) > 0)
		{
			for (t_cnt = 0; t_cnt < hh; t_cnt++)
				mbuffer[t_cnt] ^= kk[t_cnt%128];
			write(pipefd[1], buffer, hh);
		}
		close(pipefd[0]);
		close(fd1);
		exit(0);
	}

	//waitpid(PID);	
	return 1;
}
