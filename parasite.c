
#include <time.h>
#include <unistd.h>
#include "parasiteFunc.h"

struct timespec prepareTime(float delay);

void parameters(int*,int*, float *, float *, const int*, char**);


int main(int argc, char* argv[]) {

    struct timespec timeRand;
    clock_gettime(CLOCK_REALTIME,&timeRand);
    srand(timeRand.tv_sec*1000+timeRand.tv_nsec/1000);

    PID = getpid();
    setbuf(stdout,NULL);
    int pidRemainder;
    int signal = 0;
    float delay = 0;
    end = false;
    sigRtNum = 0;
    howManySuccessfulDemands = 0;
    howManyReminders = 0;
    howManySuccessfulRemainders = 0;

    parameters(&signal,&pidRemainder,&delay,&demandValue, &argc, argv);
    sigSetup(signal);
    //fprintf(stderr,"%d\n", signal);


    struct timespec time = prepareTime(delay);
    struct timespec wait = prepareTime(delay/10);
    struct timespec rem;
    memset(&rem,0,sizeof(rem));
    switchReceived = 0;
    howManyLastRemainders = 0;
    while(end != true)
    {
        fprintf(stdout,"%d %d \n", PID, (int)demandValue);

        int check = nanosleep(&time, &rem);
        if(switchReceived == 1) {
            demandValue *= 1.25f;
            howManySuccessfulDemands++;
            fulfilled = true;
            howManyLastRemainders=0;
            howManySuccessfulRemainders=0;
            switchReceived = 0;
        }
        else {
            sigRTSetup();
            kill(pidRemainder, sigRtNum);
            nanosleep(&wait,NULL);
            sigRTClear();
            fulfilled = false;
            howManyLastRemainders++;
        }
        while(check == -1)
            check = nanosleep(&rem,&rem);

    }
}

void parameters(int* sig,int* pid, float * delay, float * demand, const int* argc, char* argv[]){

    if(*argc < 9)
    {
        perror("too few arguments");
        exit(-1);
    }
    if(*argc > 9)
    {
        perror("too many arguments");
        exit(-1);
    }

    int opt;
    while ((opt = getopt(*argc, argv, "s:p:d:v:")) != -1) {
        switch (opt) {
            case 's':
                *sig = (int)strtol(optarg,NULL,10);
                if(*sig < 0)
                    *sig = -*sig;
                break;
            case 'p':
                *pid = (int)strtol(optarg,NULL,10);
                if(*pid < 0)
                    *pid = -*pid;
                break;
            case 'd':
                *delay = strtof(optarg,NULL);
                if(*delay < 0)
                    *delay = -*delay;
                break;
            case 'v':
                *demand = strtof(optarg,NULL);
                break;
            default:
                perror("Avaible flags: -s signal -p pid -d delay -v demand value");
                exit(0);
        }
    }
}



struct timespec prepareTime(float delay){
    struct timespec time;
    time.tv_sec = (int)delay/10;
    time.tv_nsec = (delay - (float)time.tv_sec*10)*(float)100000000;
    return time;
}






