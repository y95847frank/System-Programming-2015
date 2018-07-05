#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/select.h>
#include <signal.h>
#include <time.h>
#include <setjmp.h>

FILE *his;
int cus, mem, vip;
int o_cus, o_mem, o_vip;

void sig_usr1(int signo);
void sig_int(int signo);
void sig_usr2(int signo);
void sig_alarm(int signo);

int main(int argc, char *argv[])
{
    int who;
    double when;
    double last = 0;
    char buf[100];
    struct itimerval timer;

    FILE *fp = fopen(argv[1], "r");
    his = fopen("customer_log", "w");
    pid_t parent = getppid();
    
    /*struct sigaction sig_mem, sig_cus, sig_vip, sig_A;
    
    sigemptyset(&sig_mem.sa_mask);
    sigemptyset(&sig_cus.sa_mask);
    sigemptyset(&sig_vip.sa_mask);
    sigemptyset(&sig_A.sa_mask);

    sigaddset(&sig_vip.sa_mask, SIGUSR1);
    sigaddset(&sig_cus.sa_mask, SIGUSR1);
    sigaddset(&sig_vip.sa_mask, SIGINT);
    sigaddset(&sig_mem.sa_mask, SIGINT);
    sigaddset(&sig_mem.sa_mask, SIGUSR2);
    sigaddset(&sig_cus.sa_mask, SIGUSR2);
    sigaddset(&sig_A.sa_mask, SIGUSR2);
    sigaddset(&sig_A.sa_mask, SIGUSR1);
    sigaddset(&sig_A.sa_mask, SIGINT);

    sig_mem.sa_handler = sig_usr1;
    sig_vip.sa_handler = sig_usr2;
    sig_cus.sa_handler = sig_int;
    sig_A.sa_handler = sig_alarm;

    sigaction(SIGUSR1, &sig_mem, NULL);
    sigaction(SIGUSR2, &sig_vip, NULL);
    sigaction(SIGINT, &sig_cus, NULL);
    sigaction(SIGALRM, &sig_A, NULL);*/
    
    signal(SIGUSR1, sig_usr1);
    signal(SIGINT, sig_int);
    signal(SIGUSR2, sig_usr2);
    signal(SIGALRM, sig_alarm);

    while(1){
        char *num = fgets(buf, 100, fp);  //get line from test_data~
        if(feof(fp))
            break;
        sscanf(buf, "%d %lf\n", &who, &when);  //analyze line

        char message[100];
            /*timer.it_value.tv_sec = 0;
            timer.it_value.tv_usec = 300000;
            timer.it_interval.tv_sec = 0;
            timer.it_interval.tv_usec = 0;*/
            //setitimer(ITIMER_VIRTUAL, &timer, NULL);
        int minus = when - last;
        struct timespec tim, rem;
        tim.tv_sec = minus;
        tim.tv_nsec = (when - last - minus)*1000000000L;
        //fprintf(stderr, "sleep: %d %d\n", tim.tv_sec, tim.tv_nsec);
        while(nanosleep(&tim, &rem) != 0) {
            tim.tv_sec = rem.tv_sec;
            tim.tv_nsec = rem.tv_nsec;
            //fprintf(stderr, "back to sleep: %d %d\n", tim.tv_sec, tim.tv_nsec);
        }
        if(who == 0) {
            cus++;
            sprintf(message, "send %d %d\n", who, cus);
            fputs(message, his);
            //fflush(his);
            //fflush(his);    
            printf("ordinary\n");
            fflush(stdout);
        }
        else if(who == 1) {
            mem++;
            sprintf(message, "send %d %d\n", who, mem);
            fputs(message, his);
            //fflush(his);    
            //fflush(his);    
            kill(parent, SIGUSR1);
            alarm(1);
        }
        else if(who == 2){
            vip++;
            sprintf(message, "send %d %d\n", who, vip);
            fputs(message, his);
            //fflush(his);
            //fflush(his);
            kill(parent, SIGUSR2);
        }

        last = when;
    }
    while(1) {
        if(cus == o_cus && o_mem == mem && o_vip == vip)
            break;
    }
    fflush(his);
    close(1);
    return 0;
}

void sig_int(int signo){
    //fprintf(stderr, "cus finish~\n");
    o_cus++;
    char message[100];
    sprintf(message, "finish 0 %d\n", o_cus);
    fputs(message, his);
    //fflush(his);
    //fflush(his);
}

void sig_usr1(int signo){
    alarm(0);
    //fprintf(stderr, "mem finish~\n");
    o_mem++;
    char message[100];
    sprintf(message, "finish 1 %d\n", o_mem);
    fputs(message, his);
    //fflush(his);
    //fflush(his);
}

void sig_usr2(int signo){
    //fprintf(stderr, "VIP finish~\n");
    o_vip++;
    char message[100];
    sprintf(message, "finish 2 %d\n", o_vip);
    fputs(message, his);
    //fflush(his);
    //fflush(his);
}

void sig_alarm(int signo){
    //fprintf(stderr, "ring!!!\n");
    o_mem++;
    char message[100];
    sprintf(message, "timeout 1 %d\n", o_mem);
    fputs(message, his);
    //fflush(his);
    fflush(his);
    close(1);
    exit(0);
}


