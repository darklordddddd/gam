#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#define GEN_LENGTH 128
#define LENGTH 128

int main(int argc, char* argv[])
{
	char key_mas[LENGTH+1];
	int opt, fd;
	char path[20];
	while ((opt = getopt(argc, argv, ":f:g:")) != -1) {
		switch(opt) {
			case 'f':
				sscanf(optarg,"%s",path);
				if ((fd = open(path, O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) < 0) { perror(""); exit(0); }
				else { if (read(fd, key_mas, LENGTH) < 0) { perror(""); close(fd); exit(0); } }
				close(fd);
			break;
			case 'g':
				sscanf(optarg,"%s",path);
				if ((fd = open(path, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) < 0) { perror(""); exit(0); }
				int i;
				
				//random init
				time_t ti;
				char st[GEN_LENGTH];
				(void) time(&ti);
				(void) initstate((unsigned int)(ti + getpid()), st, sizeof(st));
				(void) setstate(st);
				
				//and generate!
				for (i = 0; i < LENGTH; i++) key_mas[i] = random()%32 + 30;
		
				//write key to the file;
				if (write(fd, key_mas, LENGTH) < 0) { perror(""); close(fd); exit(0); }
				close(fd);
			break;
		}
	}
	
	int i = 0;
	while (1)
	{
		printf("%c", key_mas[i]);
		if (i < LENGTH) i++;
		else i = 0;
	}

	return 0;
}