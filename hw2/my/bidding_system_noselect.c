#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

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
    FD_ZERO(&master);
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
            FD_SET(rpfd[i][1], &master);
            execl("host", "host", buf, (char*)0);
        }
    }
    while(game > 0) {
        fd_set readfd;
        memcpy(&readfd, &master, sizeof(master));
        for(int i = 0; i < host_num; i++) {
            if(game == 0)
                break;
            char buf[100];
            sprintf(buf, "%d %d %d %d\n", mix[game-1][0], mix[game-1][1], mix[game-1][2], mix[game-1][3]);
            FILE *fp;
            fp = fdopen(wpfd[i][1], "w");
            fputs(buf, fp);
            fflush(fp);
            FILE *getfp = fdopen(rpfd[i][0], "r");
            fflush(getfp);
            for(int k = 0; k < 4; k++){
                fgets(buf, 100, getfp);
                int id, rank;
                sscanf(buf, "%d %d\n", &id, &rank);
                player[id-1][1] += rank;
            }
            game--;
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
        
    for(int i = 0; i < player_num; i++)
        printf("%d %d\n", player[i][0], player[i][1]);
    


    return 0;
}

