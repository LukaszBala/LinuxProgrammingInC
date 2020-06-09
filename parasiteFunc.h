#pragma once

#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


int sigRtNum;
int howManyReminders;
int howManySuccessfulRemainders;
int howManyLastRemainders;
int howManySuccessfulDemands;
float demandValue;
int PID;
int switchReceived;
typedef enum{false,true} bool;
bool end;
bool fulfilled;

void sigSetup(int);
void onSignal();
void onRealTime();
void onSigPipe();
void sigRTSetup();
void sigRTClear();