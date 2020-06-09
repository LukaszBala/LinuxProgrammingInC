//
// Created by lukas on 08.06.2020.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <string.h>

#define PRINTARGS 0

int set_cloexec_flag (int desc, int value);
void parameters(int* sig, float* add, float * delay, float * noKill, float * answer, float* parasite, int argc, char* argv[]);



int main(int argc, char* argv[]){
    int sigArg = 0;
    for (int i = 0; i < argc; i++)
    {
        if(*(argv[i]) == '-'){
            if(*(argv[i]+1) == 's')
                sigArg = 1;
        }
    }
    int paraNum;
    if(sigArg == 1) {
        if (argc > 7)
            paraNum = (argc - 7) * 2;
        else {
            fprintf(stderr, "Error: Not enough arguments - Flags: -s <int> -h <float>/<float> -r <float>/<float> and at least one positional <float>:<float>\n");
            exit(EXIT_FAILURE);
        }
    }
    else{
        if (argc > 5)
            paraNum = (argc - 5) * 2;
        else {
            fprintf(stderr, "Error: Not enough arguments - Flags -s <int> -h <float>/<float> -r <float>/<float> and at least one positional <float>:<float>\n");
            exit(EXIT_FAILURE);
        }
    }
    float parasite[paraNum];
    float* point = parasite;
    int sig;
    float add, delay, noKill, answer;
    parameters(&sig, &add, &delay, &noKill, &answer, point,argc,argv);

    if(PRINTARGS == 1) {
        printf("signal: %d, addValue: %f, Delay: %f, noKill: %f, answer: %f\n", sig, add, delay, noKill, answer);
        for (int i = 0; i < paraNum; i += 2) {
            printf("parasite args: %f, %f\n", parasite[i], parasite[i + 1]);
        }
    }

    int pipefd[2];
    if(pipe(pipefd) == -1)
    {
        fprintf(stderr,"Error: Creating a pipe\n");
        exit(EXIT_FAILURE);
    }

    if(set_cloexec_flag(pipefd[0],1) < 0){
        fprintf(stderr,"Error: Setting flags to file Descriptor\n");
        exit(errno);
    }
    if(set_cloexec_flag(pipefd[1],1) < 0){
        fprintf(stderr,"Error: Setting flags to file Descriptor\n");
        exit(errno);
    }

    char providerArgs[3][32];
    memset(providerArgs, 0, sizeof(providerArgs));
    sprintf(providerArgs[0], "%d ", sig);
    sprintf(providerArgs[1], "%f/%f ", add,delay);
    sprintf(providerArgs[2], "%f/%f ", noKill,answer);
    int grandpaPid;
    grandpaPid = getpid();
    pid_t pidParent = fork();
    if (pidParent == 0) {
        for (int i = 0; i < paraNum; i += 2) {
            char paraArgs[4][32];
            memset(paraArgs, 0, sizeof(paraArgs));
            sprintf(paraArgs[0], "%d", sig);
            sprintf(paraArgs[1], "%d", grandpaPid);
            sprintf(paraArgs[2], "%f", parasite[i]);
            sprintf(paraArgs[3], "%f", parasite[i + 1]);
            pid_t pidGrandChild = fork();
            if (pidGrandChild == 0) {
                if(dup2(pipefd[1], STDOUT_FILENO) == -1){
                    fprintf(stderr,"Error: Changing StdOut failed\n");
                    exit(errno);
                }
                if(execv("parasite", (char *[]) {"parasite", "-s", paraArgs[0], "-p", paraArgs[1], "-d", paraArgs[2], "-v", paraArgs[3], NULL}) == -1)
                {
                    fprintf(stderr,"Error: Running Parasite Error\n");
                    exit(errno);
                }
            }
        }
        exit(0);
    } else
        waitpid(pidParent,0,0);
    if(dup2(pipefd[0], STDIN_FILENO)== -1){
        fprintf(stderr,"Error: Changing StdIn failed\n");
        exit(errno);
    }
    if(execv("provider", (char *[]) {"provider","-s", providerArgs[0], "-h", providerArgs[1],providerArgs[2], NULL}) == -1){
        fprintf(stderr,"Error: Running Provider Failed\n");
        exit(errno);
    }
    return 1;
}


int set_cloexec_flag (int desc, int value)
{
    int oldflags = fcntl (desc, F_GETFD, 0);
    if (oldflags < 0)
        return oldflags;
    if (value != 0)
        oldflags |= FD_CLOEXEC;
    else
        oldflags &= ~FD_CLOEXEC;
    return fcntl (desc, F_SETFD, oldflags);
}



void parameters(int* sig, float* add, float * delay, float * noKill, float * answer, float* parasite, int argc, char* argv[]){

    int opt;
    char *endptr;
    char *secendptr;
    int i = 1;
    *add = -1;
    *noKill = -1;
    *delay = -1;
    *answer = -1;

    *sig = SIGHUP;

    opterr = 0;
    int optHelp;

    while(*argv[i] != '-')
    {
        *parasite = strtof(argv[i],&endptr);
        if(*parasite == 0 && (errno != 0 || endptr == argv[i])) {
            fprintf(stderr, "Error: input positional is not a valid float <float>:<float>\n");
            exit(EXIT_FAILURE);
        }
        parasite++;
        if(*endptr == ':')
            *parasite = strtof(endptr + 1, &secendptr);
        else{
            fprintf(stderr,"Error: Correct format of positional <float>:<float>\n");
            exit(EXIT_FAILURE);
        }
        if(*parasite == 0 && (errno != 0 || endptr + 1 == secendptr)) {
            fprintf(stderr, "Error: input positional is not a valid float <float>:<float>\n");
            exit(EXIT_FAILURE);
        }
        parasite++;
        //printf("BEFORE: %s bulbulator\n",argv[i]);
        i++;
    }
    while ((opt = getopt(argc, argv, "s:h:r:")) != -1) {
        switch (opt) {
            case 's':
                *sig = (int)strtol(optarg,&endptr,10);
                if(*sig < 0)
                    *sig = -*sig;
                if(*sig == 0 && (errno != 0 || endptr == optarg)) {
                    fprintf(stderr, "Error: input -s is not a valid int \n");
                    exit(EXIT_FAILURE);
                }
                optHelp = 0;
                break;
            case 'h':
                *add = strtof(optarg,&endptr);
                if(*add < 0)
                    *add = -*add;
                if(*add == 0 && (errno != 0 || endptr == optarg)) {
                    fprintf(stderr, "Error: input -h is not a valid float <float>/<float>\n");
                    exit(EXIT_FAILURE);
                }
                if(*endptr == '/') {
                    *delay = strtof(endptr + 1, &secendptr);
                    if(*delay < 0)
                        *delay= -*delay;
                }
                else{
                    fprintf(stderr,"Error: Correct format of -h flag <float>/<float>\n");
                    exit(EXIT_FAILURE);
                }
                if(*delay == 0 && (errno != 0 || endptr+1 == secendptr)) {
                    fprintf(stderr, "Error: input -h is not a valid float <float>/<float>\n");
                    exit(EXIT_FAILURE);
                }
                optHelp = 0;
                break;
            case 'r':
                *noKill = strtof(optarg, &endptr);
                if(*noKill == 0 && (errno != 0 || endptr == optarg)) {
                    fprintf(stderr, "Error: input -r is not a valid float <float>/<float>\n");
                    exit(EXIT_FAILURE);
                }
                if(*endptr =='/')
                    *answer = strtof(endptr + 1, &secendptr);
                else{
                    fprintf(stderr,"Error: Correct format of -r flag <float>/<float>\n");
                    exit(EXIT_FAILURE);
                }
                if(*answer == 0 && (errno != 0 || endptr+1 == secendptr)) {
                    fprintf(stderr, "Error: input -r is not a valid float <float>/<float>\n");
                    exit(EXIT_FAILURE);
                }
                if (*noKill > 100)
                    *noKill = 100;
                if (*noKill < 0)
                    *noKill = 0;
                if (*answer > 100)
                    *answer = 100;
                if (*answer < 0)
                    *answer = 0;
                optHelp = 0;
                break;
            default:
                if(optopt >= 48 &&  optopt <= 57) {
                    if(optopt == *(argv[optind]+1))
                        optHelp = 1;
                    else
                        optHelp = -1;
                    break;
                }
                if((optopt == ':' && *(argv[optind]+1) != optopt) || (optopt == '/' && *(argv[optind]+1) != optopt) || (optopt == '-' && *(argv[optind]+1) != optopt)){
                    optHelp = -1;
                    break;
                }
                fprintf(stderr,"Avaible flags: -s <int> -h <float>/<float> -r <float>/<float> positional arg <float>:<float>");
                exit(0);
        }

        if (optind < argc ) {
            if (*argv[optind] == '-'){
                if(*(argv[optind]+1) > 57 || *(argv[optind]+1) < 48)
                    continue;
            }
            //printf("NAME ARG: %s bulbulator\n", argv[optind]);
            i = optind;
            if(optHelp == 1){
                *parasite = strtof(argv[i], &endptr);
                if (*parasite == 0 && (errno != 0 || endptr == argv[i])) {
                    fprintf(stderr, "Error: input positional is not a valid float <float>:<float>\n");
                    exit(EXIT_FAILURE);
                }
                parasite++;
                if (*endptr == ':')
                    *parasite = strtof(endptr + 1, &secendptr);
                else {
                    fprintf(stderr, "Error: Correct format of positional <float>:<float>\n");
                    exit(EXIT_FAILURE);
                }
                if (*parasite == 0 && (errno != 0 || endptr + 1 == secendptr)) {
                    fprintf(stderr, "Error: input positional is not a valid float <float>:<float>\n");
                    exit(EXIT_FAILURE);
                }
                parasite++;
                i++;
                optHelp = 0;
                if (i >= argc)
                    break;
            }
            if(optHelp == 0) {
                while (*argv[i] != '-') {
                    *parasite = strtof(argv[i], &endptr);
                    if (*parasite == 0 && (errno != 0 || endptr == argv[i])) {
                        fprintf(stderr, "Error: input positional is not a valid float <float>:<float>\n");
                        exit(EXIT_FAILURE);
                    }
                    parasite++;
                    if (*endptr == ':')
                        *parasite = strtof(endptr + 1, &secendptr);
                    else {
                        fprintf(stderr, "Error: Correct format of positional <float>:<float>\n");
                        exit(EXIT_FAILURE);
                    }
                    if (*parasite == 0 && (errno != 0 || endptr + 1 == secendptr)) {
                        fprintf(stderr, "Error: input positional is not a valid float <float>:<float>\n");
                        exit(EXIT_FAILURE);
                    }
                    parasite++;
                    i++;
                    if (i >= argc)
                        break;
                }
            }
        }
    }
    if(*delay == -1 || *add == -1)
    {
        fprintf(stderr, "Error: No -h flag or valid fields for it <float>/<float> \n");
        exit(EXIT_FAILURE);
    }
    if(*answer == -1 || *noKill == -1)
    {
        fprintf(stderr, "Error: No -r flag or valid fields for it <float>/<float> \n");
        exit(EXIT_FAILURE);
    }
}

