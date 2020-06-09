//
// Created by lukas on 22.05.2020.
//

#include "parasiteFunc.h"



void sigSetup(int sigNum){
//    fprintf(stderr,"sig: %d\n", sigNum);
    struct sigaction actionUsr, actionUsr2;
    memset(&actionUsr, 0, sizeof(struct sigaction));
    actionUsr.sa_handler = onSignal;
    sigaction(sigNum, &actionUsr, NULL);
    memset(&actionUsr2, 0, sizeof(struct sigaction));
    actionUsr2.sa_handler = onSigPipe;
    sigaction(SIGPIPE, &actionUsr2, NULL);
}

void onSignal(){
    switchReceived = 1;
//    fprintf(stderr,"GETIT\n");
}

void onRealTime(){
    demandValue*=0.8f;
    howManySuccessfulRemainders++;
}

void onSigPipe(){
    fprintf(stderr,"\nPID %d\n", PID);
    fprintf(stderr,"FulfilledDemands %d\n", howManySuccessfulDemands);
    fprintf(stderr,"SentRemainders %d\n", howManyReminders);
    fprintf(stderr,"DemandValue %f\n", demandValue);
    fprintf(stderr,"LastDemandRemainders %d\n", howManyLastRemainders);
    fprintf(stderr,"FulfilledLastRemainder %d\n", howManySuccessfulRemainders);
    fprintf(stderr,"WasItDone %d\n",fulfilled);
    end = true;
    exit(141);
}

void sigRTSetup(){
    struct sigaction actionUsr;
    int diff = SIGRTMAX - SIGRTMIN +1;
    sigRtNum = rand() %diff +SIGRTMIN;
    memset(&actionUsr, 0, sizeof(struct sigaction));
    actionUsr.sa_handler = onRealTime;
    sigaction(sigRtNum, &actionUsr, NULL);
    howManyReminders++;

}
void sigRTClear(){
    struct sigaction actionUsr;
    if (sigRtNum != 0)
    {
        actionUsr.sa_handler = SIG_DFL;
        sigaction(sigRtNum, &actionUsr, NULL);
        sigRtNum = 0;
    }
}