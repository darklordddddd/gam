#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>

#define MAX_ARG_COUNT 20

char arg[MAX_ARG_COUNT][100];
char *argp[MAX_ARG_COUNT];

int GEN_PID; 

void parsim(char *text)
{
    char c;
    int arg_count=0;
    char buf[1000];
    char *cp = buf;
    while (arg_count < MAX_ARG_COUNT){
        c = *(text++);
        if (c == '\0' || c == ' ' || c == '\t') {
            *cp = '\0';
            strcpy(arg[arg_count++], buf);
            while (c == ' ' || c == '\t') c = *(text++);
            if (c == '\0') break;
            cp = buf;
        }
        *(cp++) = c;
    }

    for(c=0; c< arg_count; c++)
            argp[c] = arg[c];
        argp[arg_count] = NULL;
}

int main(int argc, char* argv[])
{
    int pipe_cat[2];
    int pipe_gen[2];
    
    char mas_gen[1000];
    char mas_cat[1000];
    
    int opt;
    while ((opt = getopt(argc, argv, ":g:c:")) != -1) {
        switch(opt) {
            case 'g':
                strcpy(mas_gen, optarg);
                break;
            case 'c':
                strcpy(mas_cat, optarg);
                break;
            default:
                break;
        }
    }

    if ((strlen(mas_cat) < 1) || (strlen(mas_gen) < 1)) {
        printf("nope\n"); 
        exit(1);
    }
    
    if (pipe(pipe_gen) != 0) {
        printf("Ошибка создания pipe");        
        exit(1);
    }
    if (pipe(pipe_cat) != 0) {
        printf("Ошибка создания pipe");        
        exit(1);
    }

    if ((GEN_PID = fork()) == 0){
        close(1);
        dup(pipe_gen[1]);
        close(pipe_gen[1]);
        close(pipe_cat[1]);

        parsim(mas_gen);
        execvp(argp[0], argp);
        _exit(0);
    }

    if (fork() == 0){
        close(1);
        dup(pipe_cat[1]);
        close(pipe_cat[1]);
        close(pipe_gen[1]);

        parsim(mas_cat);
        execvp(argp[0], argp);
        _exit(0);
    }
    
    close(pipe_cat[1]);

    char buf[1]; char b[1];
    char res;
    //int i = 0;
    while (read(pipe_cat[0], buf, 1) != 0){    
        read(pipe_gen[0], b, 1);
        res = b[0] ^ buf[0];
        printf("%c",res);
    }
    kill(GEN_PID, SIGKILL);

    return 0;
}
