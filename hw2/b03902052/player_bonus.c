/*b03902052 顏廷宇*/
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

int main(int argc, char *argv[])
{
    if(argc < 3)
        fprintf(stderr, "player doesn't have enough file\n");
    char string[100];
    sprintf(string, "host%s_%s.FIFO", argv[1], argv[2]);
    int a_fd = open(string, O_RDONLY);          //make a fifo!!!
    char s[100];
    sprintf(s, "host%s.FIFO", argv[1]);
    int p_fd = open(s, O_WRONLY);
    FILE *fp = fdopen(p_fd, "w");

    for(int time = 0; time < 10; time++) {
        char money[100] = {0};
        read(a_fd, money, 100);
        int m;
        if(strcmp(argv[2], "A") == 0){
            int m;
            sscanf(money, "%d %*s %*s %*s\n", &m);
            char pass[100];

            if(time == 0 || time == 3 || time == 6)
                sprintf(pass, "%s %s %d\n", argv[2], argv[3], m-3);
            else if(time == 1 || time == 4 || time == 7)
                sprintf(pass, "%s %s %d\n", argv[2], argv[3], m-2);
            else if(time == 2 || time == 5 || time == 8)
                sprintf(pass, "%s %s %d\n", argv[2], argv[3], m-1);
            else if(time == 9)
                sprintf(pass, "%s %s %d\n", argv[2], argv[3], m);

            fputs(pass, fp);
            fflush(fp);
        }
        else if(strcmp(argv[2], "B") == 0){
            int m;
            sscanf(money, "%*s %d %*s %*s\n", &m);
            int tmp = 0;
            char pass[100];

            if(time == 0 || time == 3 || time == 6)
                sprintf(pass, "%s %s %d\n", argv[2], argv[3], m-3);
            else if(time == 1 || time == 4 || time == 7)
                sprintf(pass, "%s %s %d\n", argv[2], argv[3], m-2);
            else if(time == 2 || time == 5 || time == 8)
                sprintf(pass, "%s %s %d\n", argv[2], argv[3], m-1);
            else if(time == 9)
                sprintf(pass, "%s %s %d\n", argv[2], argv[3], m);

            fputs(pass, fp);
            fflush(fp);
        }
        else if(strcmp(argv[2], "C") == 0){
            int m;
            sscanf(money, "%*s %*s %d %*s\n", &m);
            int tmp = 0;
            char pass[100];

            if(time == 0 || time == 3 || time == 6)
                sprintf(pass, "%s %s %d\n", argv[2], argv[3], m-3);
            else if(time == 1 || time == 4 || time == 7)
                sprintf(pass, "%s %s %d\n", argv[2], argv[3], m-2);
            else if(time == 2 || time == 5 || time == 8)
                sprintf(pass, "%s %s %d\n", argv[2], argv[3], m-1);
            else if(time == 9)
                sprintf(pass, "%s %s %d\n", argv[2], argv[3], m);

            fputs(pass, fp);
            fflush(fp);
        }
        else if(strcmp(argv[2], "D") == 0){
            int m;
            sscanf(money, "%*s %*s %*s %d", &m);
            int tmp = 0;
            char pass[100];

            if(time == 0 || time == 3 || time == 6)
                sprintf(pass, "%s %s %d\n", argv[2], argv[3], m-3);
            else if(time == 1 || time == 4 || time == 7)
                sprintf(pass, "%s %s %d\n", argv[2], argv[3], m-2);
            else if(time == 2 || time == 5 || time == 8)
                sprintf(pass, "%s %s %d\n", argv[2], argv[3], m-1);
            else if(time == 9)
                sprintf(pass, "%s %s %d\n", argv[2], argv[3], m);

            fputs(pass, fp);
            fflush(fp);
        }
        else fprintf(stderr, "fuck\n");
    }
    exit(0);

    return 0;
}

