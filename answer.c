#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>

void prepareSignal();
void onSignal();
int PID;


int main (int argc, char* argv[]){
    prepareSignal();
    char buff[128];
    int state;
    int val1;
    float val2;
    char* pointer;
    memset(buff,0,sizeof(buff));
    while(1) {

        state = read(0,buff,sizeof(buff));
        if(state == -1)
            continue;
        val1 = (int)strtol(buff,&pointer,10);
        val2 = strtof(pointer,NULL);
//        char c = 1;
//        int i = 0;
//        while(i< sizeof(buff) && (status = read(0,&c,1) > 0) && c!=0 && c!='\n')
//        {
//            buff[i] = c;
//            i++;
//        }
        printf("%s %d %d %f\n",buff, state, val1, val2);
        memset(buff,0,sizeof(buff));
        if (PID < 0)
            break;
    }

}

void prepareSignal(){
    struct sigaction actionUsr;
    memset(&actionUsr,0,sizeof(struct sigaction));
    actionUsr.sa_handler = onSignal;
    sigaction(3,&actionUsr,NULL);
}

void onSignal(int sig, siginfo_t *info, void *ucontext){
    printf("dżem\n");
}