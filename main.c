#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#define LENGTH 128

int main(int argc, char* argv[]) //-nkey -l <number>
{
	int PID, PID1;
	int pipefd[2], pipefd1[2];
	int fd, fd1, fd2;
	int len; //key length
	char kk[LENGTH+1];
	char tbuffer[1024], mbuffer[1024];

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
			//no file, create it
			if ((fd = open("key", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) < 0)
			{
				perror("Key file error");
				close(fd);
				_exit(1);
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
		exit(0);
	}
	else //main program
	{
		close(pipefd[1]); //close write
		read(pipefd[0], kk, LENGTH);
		printf("Key: %s\n\n", kk);
		close(pipefd[0]);
		//exit(1);
	}

	//create another pipe

	if (pipe(pipefd1) < 0)
	{
		perror("Pipe error");
		exit(1);
	}
	
	if ((PID1 = fork()) < 0)
	{
		perror("Fork error");
		exit(1);	
	}
	
	if (PID1 == 0) //text
	{
		if ((fd2 = open("stext", O_RDWR)) < 0)
		{
			perror("Source text file error");
			close(fd2);
			_exit(1);
		}
		int hh;
		close(pipefd1[0]); //close read
		while ((hh = read(fd2, tbuffer, 1024)) > 0)
		{
			write(pipefd1[1], tbuffer, hh);
			printf("HH: %d\nBuf:\n%s\n\n", hh, tbuffer);
		}
		/*printf("Before read\n");
		hh = read(fd2, tbuffer, 1024);
		printf("After read\n");
		write(pipefd1[1], tbuffer, hh);
		printf("HH: %d\nBuf:\n%s\n\n", hh, tbuffer);*/
		close(fd2);
		close(pipefd1[1]);
		exit(0);
	}
	else
	{
		close(pipefd1[1]); //close write
		if ((fd1 = open("entext", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) < 0)
		{
			perror("Encrypted text file error");
			remove("entext");
			close(fd1);
			exit(1);
		}
		int hh;
		int t_cnt;
		while ((hh = read(pipefd1[0], mbuffer, 1024)) > 0)
		{
			for (t_cnt = 0; t_cnt < hh; t_cnt++)
				mbuffer[t_cnt] ^= kk[t_cnt%128];
			write(fd1, mbuffer, hh);
			printf("HH: %d\nEnc. buf:\n%s\n\n", hh, mbuffer);
			//waitpid(PID1);
		}
		/*hh = read(pipefd1[0], mbuffer, 1024);
		for (t_cnt = 0; t_cnt < hh; t_cnt++)
			mbuffer[t_cnt] ^= kk[t_cnt%128];
		write(fd1, mbuffer, hh);
		printf("HH: %d\nEnc. buf:\n%s\n\n", hh, mbuffer);*/
		close(pipefd1[0]);
		close(fd1);
		wait(NULL);
		exit(0);
	}

	//waitpid(PID);	
	return 1;
}
