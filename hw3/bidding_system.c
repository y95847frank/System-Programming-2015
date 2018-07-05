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

int costumer, mem, vip;
FILE *his;
pid_t pid;

static sigjmp_buf jmpbuf_2, jmpbuf_1;

void sig_usr1(int signo);

void sig_usr2(int signo);

int main(int argc, char *argv[])
{
    struct sigaction sig_vip, sig_mem;
    sigemptyset(&sig_mem.sa_mask);
    sigemptyset(&sig_vip.sa_mask);
    sigaddset(&sig_vip.sa_mask, SIGUSR1);
    //sig_mem.sa_flags = SA_RESTART;
    //sig_vip.sa_flags = SA_RESTART;
    sig_mem.sa_handler = sig_usr1;
    sig_vip.sa_handler = sig_usr2;

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
        execl("customer", "customer", argv[1], (char*)0);
    }
    else {
        close(pfd1[1]);
        char message[100];
        sigaction(SIGUSR1, &sig_mem, NULL);
        sigaction(SIGUSR2, &sig_vip, NULL);
        signal(SIGUSR1, sig_usr1);
        signal(SIGUSR2, sig_usr2);
        FILE *ppp = fdopen(pfd2[1], "w");
        fputs("haha", ppp);
        fflush(ppp);
        close(pfd2[0]);
        close(pfd2[1]);

        while(1){
            int stop = 0;
            char* num = fgets(buf, 100, pipe_fp);
            while(num == NULL){
                if(feof(pipe_fp)){
                    stop = 1;
                    break;
                }
                num = fgets(buf, 100, pipe_fp);
            }
            if(stop == 1)
                break;

            if(strcmp(buf, "ordinary\n") == 0){
                costumer++;
                sprintf(message, "receive 0 %d\n", costumer);
                fputs(message, his);
                //fflush(his);

                struct timespec tim, rem;
                tim.tv_sec = 1;
                tim.tv_nsec = 0L;
                int c;
                while((c = nanosleep(&tim, &rem)) != 0){
                    //tim.tv_sec = rem.tv_sec;
                    //tim.tv_nsec = rem.tv_nsec;
                    tim = rem;
                    //printf("ord back to sleep : %d %d\n", tim.tv_sec, tim.tv_nsec);
                }

                kill(pid, SIGINT);
                sprintf(message, "finish 0 %d\n", costumer);
                fputs(message, his);
                //fflush(his);
            }
        }
        fputs("terminate\n", his);
        fflush(his);
    }
    int status;
    wait(&status);
    return 0;
}

void sig_usr1(int signo)
{
    //printf("set sigusr1\n");
    mem++;
    char message[100];
    sprintf(message, "receive 1 %d\n", mem);
    fputs(message, his);
    //fflush(his);
    struct timespec tim, rem;
    tim.tv_sec = 0;
    tim.tv_nsec = 500000000L;
    int c;
    while((c = nanosleep(&tim, &rem)) != 0){
        tim = rem;
        //printf("sig1 back to sleep : %d %d\n", tim.tv_sec, tim.tv_nsec);
    }

    kill(pid, SIGUSR1);
    sprintf(message, "finish 1 %d\n", mem);
    fputs(message, his);
    //fflush(his);
}

void sig_usr2(int signo)
{
    //printf("set sigusr2\n");
    sigset_t block;
    sigemptyset(&block);
    sigaddset(&block, SIGUSR1);
    sigprocmask(SIG_SETMASK, &block, NULL);
    vip++;
    char message[100];
    sprintf(message, "receive 2 %d\n", vip);
    fputs(message, his);
    //fflush(his);
    int c = usleep(200000);
    kill(pid, SIGUSR2);
    sprintf(message, "finish 2 %d\n", vip);
    fputs(message, his);
    //fflush(his);
}

