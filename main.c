#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#define LENGTH 128
#define BUF_LENGTH 1024

int main(int argc, char* argv[]) //-nkey -l <number>
{
	int PID;
	int pipefd[2];
	int fd;
	char kk[LENGTH+1];
	char mbuffer[BUF_LENGTH];

	if (pipe(pipefd) < 0)
	{
		perror("Pipe error");
		exit(1);
	}

	if ((PID = fork()) < 0)
	{
		perror("Fork error");
		exit(1);	
	}
	
	if (PID == 0) //generator
	{
		if ((fd = open("key", O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) < 0)
		{
			//key file isn't found
			close(fd);
			remove("key");
			printf("Key file isn't found.\nGenerating...\n");
			//no file, create it
			if ((fd = open("key", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) < 0)
			{
				perror("Key file error");
				close(fd);
				_exit(1);
			}
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
			printf("Key has been written to the file.\n");
		}
		else
		{
			//read our key from file
			read(fd, kk, LENGTH);
			printf("Key file is found and read from the file...\n");
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
		close(pipefd[0]);
	}

	//create another pipe

	if (pipe(pipefd) < 0)
	{
		perror("Pipe error");
		exit(1);
	}
	
	if ((PID = fork()) < 0)
	{
		perror("Fork error");
		exit(1);	
	}
	
	if (PID == 0) //text
	{
		char tbuffer[BUF_LENGTH];
		if ((fd = open("stext", O_RDWR)) < 0)
		{
			perror("Source text file error");
			close(fd);
			_exit(1);
		}
		int hh;
		close(pipefd[0]); //close read
		while ((hh = read(fd, tbuffer, BUF_LENGTH)) > 0)
			write(pipefd[1], tbuffer, hh);
		printf("Text file has been read.\n");
		close(fd);
		close(pipefd[1]);
		_exit(0);
	}
	else
	{
		close(pipefd[1]); //close write
		remove("entext");
		int fd1;
		if ((fd1 = open("entext", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) < 0)
		{
			perror("Encrypted text file error");
			remove("entext");
			close(fd1);
			exit(1);
		}
		int hh;
		int t_cnt;
		while ((hh = read(pipefd[0], mbuffer, BUF_LENGTH)) > 0)
		{
			for (t_cnt = 0; t_cnt < hh; t_cnt++)
				mbuffer[t_cnt] ^= kk[t_cnt%128];
			write(fd1, mbuffer, hh);
		}
		printf("Text has been encrypted.\n");
		close(pipefd[0]);
		close(fd1);
		wait(NULL);
		exit(0);
	}
	return 1;
}
