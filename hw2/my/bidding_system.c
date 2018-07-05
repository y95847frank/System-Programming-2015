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

void create_game(int a, int game, int m[a][4])
{
    int i, j, k, l;
    int time = 0;
    for(i = 0; i < game-3; i++)
        for(j = i+1; j < game-2; j++)
            for(k = j+1; k < game-1; k++)
                for(l = k+1; l < game; l++){
                    m[time][0] = i+1;
                    m[time][1] = j+1;
                    m[time][2] = k+1;
                    m[time][3] = l+1;
                    time++;
                    if(time == a)
                        return ;
                }
    return ;
}

int main(int argc, char *argv[])
{
    fd_set master;
    //FD_ZERO(&master);
    for(int i = 0; i < 100; i++)
        FD_CLR(i, &master);
    struct timeval time;
    time.tv_sec = 0;
    time.tv_usec = 0;
    
    int ppp[1000] = {0};
    int wpfd[12][2];
    int rpfd[12][2];
    int host_num = atoi(argv[1]);     //host num
    int player_num = atoi(argv[2]);   //player num
    for(int i = 0; i < host_num; i++){
        pipe(wpfd[i]);   //wpfd[1] bid-write ~ wpfd[0] host-read
        pipe(rpfd[i]);   //rpfd[1] host-write ~ rpfd[0] bid-read
    }
    int player[player_num][2];             
    for(int i = 0; i < player_num; i++){             //construct player data
        player[i][0] = i+1;
        player[i][1] = 0;
    }
    pid_t pid[host_num];
    
    int game = player_num * (player_num-1) * (player_num -2) * (player_num -3) / 24;        //how many games
    int mix[game][4];
    create_game(game, player_num, mix); 
    
    for(int i = 0; i < host_num; i++){    //fork first round
        if((pid[i] = fork()) == (pid_t)0) {
            dup2(wpfd[i][0], 0);
            dup2(rpfd[i][1], 1);
            char buf[2];
            sprintf(buf, "%d", i+1);
            execl("host", "host", buf, (char*)0);
        }
        FD_SET(rpfd[i][0], &master);
    }

    int tmp = game;

    while(game > 0) {
        fd_set readfd;
        memcpy(&readfd, &master, sizeof(master));
        for(int i = 0; i < host_num; i++) {
            if(game == 0)
                break;
            int s = select(100, &readfd, NULL, NULL, &time);
            if(s == 0){
                if(tmp == 0){
                    break;
                }
                //FD_SET(rpfd[i][0], &master);
                char buf[100];
                sprintf(buf, "%d %d %d %d\n", mix[tmp-1][0], mix[tmp-1][1], mix[tmp-1][2], mix[tmp-1][3]);
                FILE *fp;
                fp = fdopen(wpfd[i][1], "w");
                fputs(buf, fp);
                fflush(fp);
                //printf("%s", buf);
                ppp[rpfd[i][0]]++;
                tmp--;
            }
            else if (s > 0){
                int j;
                for(j = 0; j < 100; j++) {
                    if(FD_ISSET(j, &readfd)) {
                        break;
                    }
                }
                char buf[100];
                FILE *getfp = fdopen(j, "r");
                fflush(getfp);
                int t = 0;
                while(1){
                    if(ppp[j] == 0)
                        break;
                    fgets(buf, 100, getfp);
                    //if(strcmp(buf, "/") == 0)
                    //    break;
                    //printf("%s", buf);
                    t++;
                    int id, rank;
                    sscanf(buf, "%d %d\n", &id, &rank);
                    player[id-1][1] += rank;
                    if(t == 4) {
                        t = 0;
                        ppp[j]--;
                        game--;
                    }
                }
                fflush(getfp);
                FD_CLR(j, &readfd);
                //game--;
                //printf("game left : %d\n",game); 
            }
        }
    }

    char *buf = "-1 -1 -1 -1\n";
    for(int i = 0; i < host_num; i++){
        FILE *fp = fdopen(wpfd[i][1], "w");
        fputs(buf, fp);
        fflush(fp);
    }    
    
       
    int status;
    pid_t child;
    int c = host_num;
    while(c > 0){    //wait children
        child = wait(&status);
        c--;
    }
    
    int ttt[player_num];
    for(int i = 0; i < player_num; i++)
        ttt[i] = 0;
    for(int k = 0; k < player_num; k++) {
        for(int i = 0; i < player_num; i++){
            if(k == i)
                continue;
            if(player[k][1] <= player[i][1])
                ttt[k]++;
        }
    }
    for(int i = 0; i < player_num; i++)
        printf("%d %d\n", i+1, (player_num-ttt[i]));
    


    return 0;
}

