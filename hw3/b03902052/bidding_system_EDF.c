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
#include <errno.h>

#define SIGUSR3 SIGWINCH

FILE *his;
pid_t pid;
static sigjmp_buf jmpbuf;
struct timespec tim, rem;
int slept;
int ord, pat, imp;

void sig_usr1(int signo);

void sig_usr2(int signo);

void sig_usr3(int signo);

int main(int argc, char *argv[])
{
    int pfd1[2];
    int pfd2[2];
    char buf[100];
    FILE *fp = fopen(argv[1], "r");
    his = fopen("bidding_system_log", "w");
    pipe(pfd1);
    pipe(pfd2);
    FILE *pipe_fp = fdopen(pfd1[0], "r");
    if((pid = fork()) == 0){
        dup2(pfd1[0], 0);
        dup2(pfd1[1], 1);
        close(pfd2[1]);
        char ggg[100];
        read(pfd2[0], ggg, 100);
        close(pfd2[0]);
        execl("customer_EDF", "customer_EDF", argv[1], (char*)0);
    }
    else {
        char c[100];
        close(pfd1[1]);
        /*sigaction(SIGUSR1, &sig_ord, NULL);
        sigaction(SIGUSR2, &sig_pat, NULL);
        sigaction(SIGUSR3, &sig_imp, NULL);*/
        signal(SIGUSR1, sig_usr1);
        signal(SIGUSR2, sig_usr2);
        signal(SIGUSR3, sig_usr3);
        FILE *ppp = fdopen(pfd2[1], "w");
        fputs("haha", ppp);
        fflush(ppp);
        close(pfd2[0]);
        close(pfd2[1]);

        while(1){
            char* n = fgets(c, 100, pipe_fp);
            if(feof(pipe_fp))
                break;
            fflush(his);
        }
        fputs("terminate\n", his);
        fflush(his);
    }
    int status;
    wait(&status);
    return 0;
}

void sig_usr1(int signo)  //block sigusr2   //0.5sec
{
    sigset_t block;
    sigemptyset(&block);
    sigaddset(&block, SIGUSR2);
    sigprocmask(SIG_SETMASK, &block, NULL);
    char message[100];
    if(slept == 0) {
        slept = 1;
        ord++;
        sprintf(message, "receive 0 %d\n", ord);
        fputs(message, his);
        tim.tv_sec = 0;
        tim.tv_nsec = 500000000L;
        nanosleep(&tim, &rem);
        if(slept == 1){   
            kill(pid, SIGUSR1);
            sprintf(message, "finish 0 %d\n", ord);
            fputs(message, his);
        }
        slept = 0;
    }
    else if(slept == 2){
        if(rem.tv_nsec < 500000000L) {
            tim = rem;
            nanosleep(&tim, &rem);
            if(slept == 2){
                kill(pid, SIGUSR2);
                sprintf(message, "finish 1 %d\n", pat);
                fputs(message, his);
            }
            slept = 1;
            ord++;
            sprintf(message, "receive 0 %d\n", ord);
            fputs(message, his);
            tim.tv_sec = 0;
            tim.tv_nsec = 500000000L;
            nanosleep(&tim, &rem);
            if(slept == 1){
                kill(pid, SIGUSR1);
                sprintf(message, "finish 0 %d\n", ord);
                fputs(message, his);
            }
            slept = 0;
        }
        else {
            slept = 1;
            struct timespec tmp = rem;
            ord++;
            sprintf(message, "receive 0 %d\n", ord);
            fputs(message, his);
            tim.tv_sec = 0;
            tim.tv_nsec = 500000000L;
            nanosleep(&tim, &rem);
            if(slept == 1){
                kill(pid, SIGUSR1);
                sprintf(message, "finish 0 %d\n", ord);
                fputs(message, his);
            }
            slept = 2;
            tim = tmp;
            nanosleep(&tim, &rem);
            if(slept == 2){
                kill(pid, SIGUSR2);
                sprintf(message, "finish 1 %d\n", pat);
                fputs(message, his);
            }
            slept = 0;
        }
    }
}

void sig_usr2(int signo)  //pat 1sec
{
    char message[100];
    if(slept == 0) {
        slept = 2;
        pat++;
        sprintf(message, "receive 1 %d\n", pat);
        fputs(message, his);
        tim.tv_sec = 1;
        tim.tv_nsec = 0L;
        nanosleep(&tim, &rem);
        if(slept == 2){
            kill(pid, SIGUSR2);
            sprintf(message, "finish 1 %d\n", pat);
            fputs(message, his);
        }
        slept = 0;
    }
    else if(slept == 1){
        tim = rem;
        printf("%d %d\n", tim.tv_sec, tim.tv_nsec);
        nanosleep(&tim, &rem);
        if(slept == 1){
            kill(pid, SIGUSR1);
            sprintf(message, "finish 0 %d\n", ord);
            fputs(message, his);
        }
        slept = 2;
        pat++;
        tim.tv_sec = 1;
        tim.tv_nsec = 0L;
        nanosleep(&tim, &rem);
        if(slept == 2){
            kill(pid, SIGUSR2);
            sprintf(message, "finish 1 %d\n", pat);
            fputs(message, his);
        }
        slept = 0;
    }
    else if(slept == 3) {
        tim = rem;
        nanosleep(&tim, &rem);
        if(slept == 3){
            kill(pid, SIGUSR3);
            sprintf(message, "finish 2 %d\n", imp);
            fputs(message, his);
        }
        slept = 2;
        pat++;
        tim.tv_sec = 1;
        tim.tv_nsec = 0L;
        nanosleep(&tim, &rem);
        if(slept == 2){
            kill(pid, SIGUSR2);
            sprintf(message, "finish 1 %d\n", pat);
            fputs(message, his);
        }
        slept = 0;
    }
}

void sig_usr3(int signo)  //have to block all signal   //0.2sec
{
    sigset_t block;
    sigemptyset(&block);
    sigaddset(&block, SIGUSR1);
    sigaddset(&block, SIGUSR2);
    sigprocmask(SIG_SETMASK, &block, NULL);
    char message[100];
    if(slept == 0) {
        imp++;
        sprintf(message, "receive 2 %d\n", imp);
        fputs(message, his);
        tim.tv_sec = 0;
        tim.tv_nsec = 200000000L;
        nanosleep(&tim, &rem);
        kill(pid, SIGUSR3);
        sprintf(message, "finish 2 %d\n", imp);
        fputs(message, his);
    }
    else if(slept == 1){
        if(rem.tv_nsec < 200000000L) {
            tim = rem;
            nanosleep(&tim, &rem);
            kill(pid, SIGUSR1);
            sprintf(message, "finish 0 %d\n", ord);
            fputs(message, his);
            imp++;
            sprintf(message, "receive 2 %d\n", imp);
            fputs(message, his);
            tim.tv_sec = 0;
            tim.tv_nsec = 200000000L;
            nanosleep(&tim, &rem);
            kill(pid, SIGUSR3);
            sprintf(message, "finish 2 %d\n", imp);
            fputs(message, his);
        }
        else {
            struct timespec tmp = rem;
            imp++;
            sprintf(message, "receive 2 %d\n", imp);
            fputs(message, his);
            tim.tv_sec = 0;
            tim.tv_nsec = 200000000L;
            nanosleep(&tim, &rem);
            kill(pid, SIGUSR3);
            sprintf(message, "finish 2 %d\n", imp);
            fputs(message, his);
            tim = tmp;
            nanosleep(&tim, &rem);
            kill(pid, SIGUSR1);
            sprintf(message, "finish 0 %d\n", ord);
            fputs(message, his);
        }
        slept = 0;
    }
    else if(slept == 2){
        if(rem.tv_nsec < 200000000L) {
            tim = rem;
            nanosleep(&tim, &rem);
            kill(pid, SIGUSR2);
            sprintf(message, "finish 1 %d\n", ord);
            fputs(message, his);
            imp++;
            sprintf(message, "receive 2 %d\n", imp);
            fputs(message, his);
            tim.tv_sec = 0;
            tim.tv_nsec = 200000000L;
            nanosleep(&tim, &rem);
            kill(pid, SIGUSR3);
            sprintf(message, "finish 2 %d\n", imp);
            fputs(message, his);
        }
        else {
            struct timespec tmp = rem;
            imp++;
            sprintf(message, "receive 2 %d\n", imp);
            fputs(message, his);
            tim.tv_sec = 0;
            tim.tv_nsec = 200000000L;
            nanosleep(&tim, &rem);
            kill(pid, SIGUSR3);
            sprintf(message, "finish 2 %d\n", imp);
            fputs(message, his);
            tim = tmp;
            nanosleep(&tim, &rem);
            kill(pid, SIGUSR2);
            sprintf(message, "finish 1 %d\n", ord);
            fputs(message, his);
        }
        slept = 0;
    }
}

