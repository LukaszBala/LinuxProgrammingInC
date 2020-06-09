//
// Created by lukas on 26.05.2020.
//


#include <stdio.h>
#include <unistd.h>
#include "providerFunc.h"

void parameters(int*,float*, float *, float *,float *, const int*, char**);

int main(int argc, char* argv[]) {

    struct timespec timeRand;
    clock_gettime(CLOCK_REALTIME,&timeRand);
    srand(timeRand.tv_sec*1000+timeRand.tv_nsec/1000);

    int sig;
    float delay;
    float noKill;
    float answer;
    resourceVal = 0.0f;
    struct itimerval timer;
    int pid;
    demandValue = 0;
    char buff[128];
    memset(buff,0,sizeof(buff));
    int status;
    char* readPointer;
    counter = 0;


    parameters(&sig,&addValue,&delay,&noKill,&answer,&argc,argv);
    sigSetup(noKill,answer);
    setTimer(&timer,delay);

//    printf("%d, %f, %f, %f, %f\n",sig,addValue,delay, noKill, answer);
//    printf("%d\n", getpid());
//    printf("%ld,%ld\n",timer.it_value.tv_sec,timer.it_interval.tv_usec);

//    setbuf(stdout,NULL);
    
    setitimer(ITIMER_REAL,&timer,NULL);
    while(1)
    {

        status = read(0,buff,sizeof(buff));
        if(status == -1)
            continue;
        demandValue = 0;
        pid = (int)strtol(buff,&readPointer,10);
        demandValue = strtof(readPointer,NULL);
        if(demandValue > 0 && pid != 0)
        {
            counter++;
            if(resourceVal - demandValue >= 0) {
                resourceVal -= demandValue;
                kill(pid, sig);
                demandValue = 0;
            }
        }
        if(demandValue < 0 && pid != 0) {
            counter++;
            if(resourceVal + demandValue >= 0) {
                resourceVal += demandValue;
                kill(pid, sig);
                demandValue = 0;
            }
        }
        if(resourceVal<0)
            break;
    }

    return 1;
}

void parameters(int* sig,float* add, float * delay, float * noKill, float * answer,const int* argc, char* argv[]){

    if(*argc < 6)
    {
        perror("too few arguments");
        exit(-1);
    }
    if(*argc > 6)
    {
        perror("too many arguments");
        exit(-1);
    }

    int opt;
    while ((opt = getopt(*argc, argv, "s:h:")) != -1) {
        switch (opt) {
            case 's':
                *sig = (int)strtol(optarg,NULL,10);
                if(*sig < 0)
                    *sig = -*sig;
                break;
            case 'h':
                *add = strtof(optarg,&optarg);
                if(*add < 0)
                    *add = -*add;
                if(*optarg == '/') {
                    *delay = strtof(optarg + 1, NULL);
                    if(*delay < 0)
                        *delay= -*delay;
                }
                break;
            default:
                perror("Avaible flags: -s <int> -h <float>/<float> positional arg <float>/<float>  ");
                exit(0);
        }
        if (optind < *argc ) {
            *noKill = strtof(argv[optind], &optarg);
            if(*optarg =='/')
                *answer = strtof(optarg + 1, NULL);
            if (*noKill > 100)
                *noKill = 100;
            if (*noKill < 0)
                *noKill = 0;
            if (*answer > 100)
                *answer = 100;
            if (*answer < 0)
                *answer = 0;
        }

    }
}