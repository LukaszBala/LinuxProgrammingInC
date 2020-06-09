#pragma once

#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>

float addValue;
float resourceVal;
typedef enum {false,true} bool;
int counter;
float demandValue;

void setTimer (struct itimerval*,float delay);
void sigSetup(float, float);
void onAlarm();
void onRealTime();
void sigAnswer(int sig, siginfo_t *info, void *ucontext);