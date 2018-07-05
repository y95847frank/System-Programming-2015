/*b03902052 顏廷宇*/
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

int main(int argc, char *argv[])
{
    char Astring[100], Bstring[100], Cstring[100], Dstring[100], Hstring[100];

    sprintf(Astring, "host%s_A.FIFO", argv[1]);
    sprintf(Bstring, "host%s_B.FIFO", argv[1]);
    sprintf(Cstring, "host%s_C.FIFO", argv[1]);
    sprintf(Dstring, "host%s_D.FIFO", argv[1]);
    sprintf(Hstring, "host%s.FIFO", argv[1]);
    unlink(Astring);
    unlink(Bstring);
    unlink(Cstring);
    unlink(Dstring);
    unlink(Hstring);
    mkfifo(Astring, 0777);  //make all fifo!!!
    mkfifo(Bstring, 0777);
    mkfifo(Cstring, 0777);
    mkfifo(Dstring, 0777);
    mkfifo(Hstring, 0777);  //make host fifo!!!
    pid_t pid[4];
    char a[100], b[100], c[100], d[100];
    scanf("%s %s %s %s", a, b, c, d);  //get player from bidding_system
    int player[4][2];  //four player data
    
    int afd, bfd, cfd, dfd, hfd;
    FILE *afp, *bfp, *cfp, *dfp, *hfp;
    
    while(1) {          //start do something
        if(strcmp(a, "-1") == 0){   //end!!!
            remove(Astring);
            remove(Bstring);
            remove(Cstring);
            remove(Dstring);
            remove(Hstring);
            exit(0);   
        }

        for(int i = 0; i < 4; i++){  //init player data!!!
            player[i][0] = i+1;
            player[i][1] = 0;
        }
        //fork four children to exec player!
        if((pid[0] = fork()) == (pid_t)0) {             
            execl("player", "player", argv[1], "A", "302", (char*)0);  //exec  ./player 1 A 302
        }
        else{
            afd = open(Astring, O_WRONLY);  
            afp = fdopen(afd, "w");
        }
        if((pid[1] = fork()) == (pid_t)0) {
            execl("player", "player", argv[1], "B", "2015", (char*)0);
        }
        else{
            bfd = open(Bstring, O_WRONLY);
            bfp = fdopen(bfd, "w");
        }

        if((pid[2] = fork()) == (pid_t)0) {
            execl("player", "player", argv[1], "C", "1126", (char*)0);
        }
        else{
            cfd = open(Cstring, O_WRONLY);
            cfp = fdopen(cfd, "w");
        }

        if((pid[3] = fork()) == (pid_t)0) {
            execl("player", "player", argv[1], "D", "65535", (char*)0);
        }
        else{
            dfd = open(Dstring, O_WRONLY);
            dfp = fdopen(dfd, "w");
        }
        hfp = fopen(Hstring, "r");

        char money[100];  //pass money char
        int m[4];         //player now money
        for(int i = 0; i < 4; i++)          //set player now money = 0
            m[i] = 0;

        for(int i = 0; i < 10; i++) {       //ten competiton
            for(int k = 0; k < 4; k++)
                m[k] += 1000;               //each player +1000
            sprintf(money, "%d %d %d %d\n", m[0], m[1], m[2], m[3]);
            //pass money to each host[x]_[x].FIFO
            fputs(money, afp); 
            fflush(afp);
            fputs(money, bfp);
            fflush(bfp);
            fputs(money, cfp);
            fflush(cfp);
            fputs(money, dfp);
            fflush(dfp);
        
            char getmoney[100];
            int who[4] = {0};
            for(int k = 0; k < 4; k++) {        //get message from host[x].FIFO
                fflush(hfp);
                fgets(getmoney, 100, hfp);
                fflush(hfp);
                char id[10], random[100];
                int gmoney;   //passed money 
                sscanf(getmoney, "%s %s %d\n", id, random, &gmoney);  //analysis getmoney
                if(strcmp(id, "A") == 0 && strcmp(random, "302") == 0) {   //confirm is A money
                    who[0] = gmoney;
                }
                else if(strcmp(id, "B") == 0 && strcmp(random, "2015") == 0) {   //confirm is A money
                    who[1] = gmoney;
                }
                else if(strcmp(id, "C") == 0 && strcmp(random, "1126") == 0) {   //confirm is A money
                    who[2] = gmoney;
                }
                else if(strcmp(id, "D") == 0 && strcmp(random, "65535") == 0) {   //confirm is A money
                    who[3] = gmoney;
                }
            }
            for(int r = 0; r < 3; r++){
                int ccc = 0;
                for(int l = r+1; l < 4; l++){
                    if(who[r] == who[l]){
                        who[l] = 0;
                        ccc++;
                    }
                }
                if(ccc > 0)
                    who[r] = 0;
            }
            if(who[0] > who[1] && who[0] > who[2] && who[0] > who[3]){
                player[0][1]++;
                m[0] -= who[0];
            }
            else if(who[1] > who[0] && who[1] > who[2] && who[1] > who[3]){
                player[1][1]++;
                m[1] -= who[1];
            }
            else if(who[2] > who[0] && who[2] > who[1] && who[2] > who[3]){
                player[2][1]++;
                m[2] -= who[2];
            }
            else if(who[3] > who[0] && who[3] > who[2] && who[3] > who[1]){
                player[3][1]++;
                m[3] -= who[3];
            }
            else
                continue;
        }
        //process rank
        int rank[4] = {0};
        for(int i = 0; i < 4; i++){
            for(int k = 0; k < 4; k++){
                if(i == k)
                    continue;
                if(player[i][1] >= player[k][1])
                    rank[i]++;
            }
        }
        //print rank!(pass message to bidding_system)
        printf("%s %d\n", a, (4-rank[0]));
        printf("%s %d\n", b, (4-rank[1]));
        printf("%s %d\n", c, (4-rank[2]));
        printf("%s %d\n", d, (4-rank[3]));
        fflush(stdout); 
        int status;
        for(int i = 0; i < 4; i++)  //wait four children died~
            wait(&status);

        scanf("%s %s %s %s", a, b, c, d);  //get next message!
    }

    return 0;
}

