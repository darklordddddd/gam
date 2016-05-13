#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#define DEFAULT_SUPER_SECRET_KEY 3522
#define LENGTH 128

int getN(int numb)
{
    int n = 0;
    while (numb>0){
        numb /= 10;
        n++;
    }
    return n;
}

long ppow(int x, int y)
{
    int i;
    long res = 1; 
    for(i=0; i<y; i++) res*=x;
    return res;
}

int main(int argc, char* argv[])
{
    int key = DEFAULT_SUPER_SECRET_KEY;
    char key_mas[LENGTH+1];
    int opt, fd;
    char path[20];
    while ((opt = getopt(argc, argv, ":f:g:")) != -1) {
        switch(opt) {
            case 'f':
                sscanf(optarg,"%s",path);
		//printf("%s\n", path);
		if ((fd = open(path, O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) < 0) { printf("Key open error\n"); exit(0); }
		else { read(fd, key_mas, LENGTH); }
		close(fd);
                break;
	    case 'g':
		//Generate
		sscanf(optarg,"%s",path);
		//printf("%s\n", path);
		fd = open(path, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
		int i;
		//random init
		time_t ti;
		char st[128];
		(void) time(&ti);
		(void) initstate((unsigned int)(ti + getpid()), st, sizeof(st));
		(void) setstate(st);
		//and generate!
		for (i = 0; i < LENGTH; i++) key_mas[i] = random()%32 + 30;
		//write key to the file;
		write(fd, key_mas, LENGTH);
		close(fd);
		break;
            default:
		printf("%c\n", opt);
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

    /*int i = 0,right;
    long p;
    int N = getN(key);
    int new_N;
    int temp_key;
    long int cur_key = key;
    
    while (1){
        p = cur_key*cur_key;
        new_N = getN(p);
        right = new_N / 2;
        cur_key = p / ppow(10, right);
        cur_key = cur_key %  ppow(10, N);

        temp_key = (int) cur_key;
        do{
            printf("%c", (char)temp_key);
            temp_key>>=sizeof(char)*8;
            i++;
        }while(temp_key != 0);
        
        if (cur_key == 0) {cur_key = key;}
    }
    printf("\n");*/

    return 0;
}

