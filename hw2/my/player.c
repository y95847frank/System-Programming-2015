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
        //fprintf(stderr, "run %d money => %s",time, money);
        if(strcmp(argv[2], "A") == 0){
            int m;
            sscanf(money, "%d %*s %*s %*s\n", &m);
            //fprintf(stderr, "A = %d\n", m);
            char pass[100];
            if(time == 0 || time == 4 || time == 8){
                //tmp = 1 + sizeof(argv[3]) + 4;
                sprintf(pass, "%s %s %d\n", argv[2], argv[3], m);
            //fprintf(stderr, "A: %s total =  %d\n", pass, sizeof(pass));
                //fprintf(stderr, "%s", pass);
                fputs(pass, fp);
                fflush(fp);
                //write(p_fd, pass, sizeof(pass));
            }
            else {
                //tmp = 1+ sizeof(argv[3]) + 1;
                sprintf(pass, "%s %s 0\n", argv[2], argv[3]);
                //fprintf(stderr, "%s", pass);
                fputs(pass, fp);
                fflush(fp);
                //write(p_fd, pass, sizeof(pass));
            }
            //fprintf(stderr, "A: %s total =  %d\n", pass, sizeof(pass));
        }
        else if(strcmp(argv[2], "B") == 0){
            int m;
            sscanf(money, "%*s %d %*s %*s\n", &m);
            //fprintf(stderr, "B = %d\n", m);
            int tmp = 0;
            char pass[100];
            if(time == 1 || time == 5 || time == 9) {
                //tmp = 1 + sizeof(argv[3]) + 4;
                sprintf(pass, "%s %s %d\n", argv[2], argv[3], m);
                fputs(pass, fp);
                fflush(fp);
            }
            else {
                //tmp = 1 + sizeof(argv[3]) + 1;
                sprintf(pass, "%s %s 0\n", argv[2], argv[3]);
                fputs(pass, fp);
                fflush(fp);
            }
            //fprintf(stderr, "B: %s\n", pass);
        }
        else if(strcmp(argv[2], "C") == 0){
            int m;
            sscanf(money, "%*s %*s %d %*s\n", &m);
            //fprintf(stderr, "C = %d\n", m);
            int tmp = 0;
            char pass[100];
            if(time == 2 || time == 6) {
                //tmp = 1 + sizeof(argv[3]) + 4;
                sprintf(pass, "%s %s %d\n", argv[2], argv[3], m);
                fputs(pass, fp);
                fflush(fp);
            }
            else {
                //tmp = 1 + sizeof(argv[3]) + 1;
                sprintf(pass, "%s %s 0\n", argv[2], argv[3]);
                fputs(pass, fp);
                fflush(fp);
            }
            //fprintf(stderr, "C: %s\n", pass);
        }
        else if(strcmp(argv[2], "D") == 0){
            int m;
            sscanf(money, "%*s %*s %*s %d", &m);
            //fprintf(stderr, "D = %d\n", m);
            int tmp = 0;
            char pass[100];
            if(time == 3 || time == 7) {
                //tmp = 1 + sizeof(argv[3]) + 4;
                sprintf(pass, "%s %s %d\n", argv[2], argv[3], m);
                fputs(pass, fp);
                fflush(fp);
            }
            else {
                //tmp = 1 + sizeof(argv[3]) + 1;
                sprintf(pass, "%s %s 0\n", argv[2], argv[3]);
                fputs(pass, fp);
                fflush(fp);
            }
            //fprintf(stderr, "D: %s\n", pass);

        }
        else fprintf(stderr, "fuck\n");
    }
    exit(0);

    return 0;
}

