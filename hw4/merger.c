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
#include <pthread.h>

pthread_mutex_t m;
int t;
int hehe;
int *pass[50000000];

void *divide(void *vptr);

void *merge(void *vptr);

int compare(const void *a, const void *b)
{
    int c = *(int *) a;
    int d = *(int *) b;
    if(c < d)
        return -1;
    else
        return 1;
}

int main(int argc, char *argv[])
{
    pthread_mutex_init(&m, NULL);
    long long int size = atoi(argv[1]);
    long long int num;
    scanf("%lld\n", &num);
    int team = num / size;
    if(num % size != 0) {
        team++;
    }

    pthread_t *id = malloc(sizeof(pthread_t)*(team+1));
    int count = 0;   
    int gone = 0;
    for(long long int i = 0; i < team-1; i++){
        int *arr = malloc(sizeof(int)*(size+1));
        arr[0] = size;
        for(int k = 0; k < size; k++) {
            scanf("%d", &arr[k+1]);
        }
        pthread_create(&id[i], NULL, divide, (void *)arr);
        count++;
        if(count == 500) {
            for(int k = 0; k < count; k++){
                int *tmp_pass;
                pthread_join(id[k+500*gone], (void *)&tmp_pass);
                pass[k+500*gone] = (int *)tmp_pass;
            }
            count = 0;
            gone++;
            free(arr);
        }
    }
    int tmp = size * (team-1);
    tmp = num - tmp;
    int *arr = malloc(sizeof(int)*(tmp+1));
    arr[0] = tmp;
    for(int i = 0; i < tmp; i++)
        scanf("%d", &arr[i+1]);
    pthread_create(&id[team-1], NULL, divide, (void *)arr);
    //pthread_join(id[team-1], (void *)&tmp_pass);
    //pass[team-1] = (int *)tmp_pass;
    
    for(int i = 0; i < (((team-1)%500)+1); i++) {
        int *tmp_pass;
        pthread_join(id[i+gone*500], (void *)&tmp_pass);
        //pass[i] = malloc(sizeof(int)*(size+1));
        pass[i+gone*500] = (int *)tmp_pass;
        //printf("%d how many %d\n", id[i], pass[i][0]);
    }
    //printf("end!\n");
    //printf("hehe~ %d %d\n", pass[399][0], pass[399][1]);

    int more = 0;
    long long int process = team;
    while( process > 1) {
        long long int woo = process;
        if(process % 2 == 1){
            more = 1;
        }
        else
            more = 0;

        process /= 2;
        pthread_t *tmp_id = malloc(sizeof(pthread_t)*process);
        int c = 0;
        int g = 0;
        for(int i = 0; i < process; i++) {
            int *second = malloc(sizeof(int)*(pass[i*2][0]+pass[i*2+1][0]+2));
            //printf("haha : %d %d\n", pass[i*2][1], pass[i*2+1][1]);
            second[0] = pass[i*2][0];
            second[1] = pass[i*2+1][0];
            for(long long int k = 0; k < pass[i*2][0]; k++)
                second[k+2] = pass[i*2][k+1];
            for(long long int k = 0; k < pass[i*2+1][0]; k++)
                second[pass[i*2][0]+k+2] = pass[i*2+1][k+1];
            //printf("check : ");
            //for(int k = 0; k < pass[i*2][0]+pass[i*2+1][0]+2; k++)
            //    printf("%d ", second[k]);
            //printf("\n");
            pthread_create(&tmp_id[i], NULL, merge, (void *)second);
            c++;
            if(c == 500) {
                for(int k = 0; k < 500; k++) {
                    int *tmp_p;
                    pthread_join(tmp_id[k+g*500], (void *)&tmp_p);
                    pass[k+g*500] = (int *)tmp_p;
                }
                c = 0;
                g++;
            }
            //printf("hehe~ %d %d\n", pass[400][0], pass[400][1]);
            //printf("%d w = %d %d fuck!!!!!!!!!!!!!!  %d %d %d\n",process, second[2], second[3], i, c,  g);
        }
        for(int i = 0; i < (process%500); i++){
            int *tmp_p;
            pthread_join(tmp_id[i+g*500], (void *)&tmp_p);
            pass[i+g*500] = (int *)tmp_p;
            //printf("end: ");
            //for(int k = 0; k < pass[i][0]; k++)
            //    printf("%d ", pass[i][k+1]);
            //printf("\n");
        }
        if(more){
            pass[process] = pass[woo-1];
            process++;
        }
    }
    for(long long int i = 0; i < pass[0][0]-1; i++)
        printf("%d ", pass[0][i+1]);
    printf("%d\n", pass[0][pass[0][0]]);
    printf("\n");
    return 0;
}

void *divide(void *vptr)
{
    pthread_mutex_lock(&m);
    int *arr = (int *)vptr;
    printf("Handling elements:\n");
    for(long long int i = 0; i < arr[0]-1; i++){
        printf("%d ", arr[i+1]);
    }
    printf("%d", arr[arr[0]]);
    printf("\nSorted %d elements.\n", arr[0]);
    pthread_mutex_unlock(&m);
    qsort(arr+1, arr[0], sizeof(int), compare);
      
    pthread_exit((void *)arr);
}

void *merge(void *vptr)
{
    int *arr = (int *)vptr;
    pthread_mutex_lock(&m);
    //int *first = malloc(sizeof(int)*arr[0]);
    //int *second = malloc(sizeof(int)*arr[1]);
    int *deliver = malloc(sizeof(int)*(arr[0]+arr[1]+2));
    printf("Handling elements:\n");
    for(long long int i = 0; i < arr[0]; i++){
        printf("%d ", arr[i+2]);
        //first[i] = arr[i+2];
    }
    for(long long int i = 0; i < arr[1]-1; i++){
        printf("%d ", arr[arr[0]+i+2]);
        //second[i] = arr[arr[0]+i+2];
    }
    printf("%d\n", arr[arr[0]+arr[1]+1]);
    //second[arr[1]-1] = arr[arr[0]+arr[1]+1];
    printf("Merged %d and %d elements with ", arr[0], arr[1]);
    deliver[0] = (arr[0]+arr[1]);
    int s = 0;
    int f = 0;
    int dup = 0;
    for(long long int i = 0; i < (arr[0]+arr[1]); i++) {
        if(f == arr[0]){
            //deliver[i+1] = second[s];
            deliver[i+1] = arr[arr[0]+2+s];
            s++;
            //printf("s %d ", deliver[i+1]);
            continue;
        }
        if(s == arr[1]) {
            //deliver[i+1] = first[f];
            deliver[i+1] = arr[2+f];
            f++;
            //printf("f %d ", deliver[i+1]);
            continue;
        }

        if(arr[2+f] < arr[arr[0]+2+s]){
            deliver[i+1] = arr[2+f];
            //deliver[i+1] = first[f];
            f++;
            //printf("f %d ", deliver[i+1]);
        }
        else if(arr[2+f] > arr[arr[0]+2+s]) {
            deliver[i+1] = arr[arr[0]+2+s];
            //deliver[i+1] = second[s];
            s++;
            //printf("s %d ", deliver[i+1]);
        }
        else {
            deliver[i+1] = arr[2+f];
            //deliver[i+1] = first[f];
            f++;
            //printf("f %d ", deliver[i+1]);
            dup++;
        }
    }
    printf("%d duplicates.\n", dup);
    //printf("bug? : %d %d\n", f, s);
    //hehe++;
    //printf("%d\n", hehe);
    pthread_mutex_unlock(&m);
    //free(first);
    //free(second);
    //free(arr);
    pthread_exit((void *)deliver);
}

