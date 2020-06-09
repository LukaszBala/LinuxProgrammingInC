//
// Created by lukas on 27.05.2020.
//

#include "providerFunc.h"

void setTimer (struct itimerval* timer,float delay){
    timer->it_value.tv_sec = (int)delay;
    timer->it_value.tv_usec= (delay-timer->it_value.tv_sec)*1000000;
    timer->it_interval.tv_sec = (int)delay;
    timer->it_interval.tv_usec= (delay-timer->it_value.tv_sec)*1000000;

}

void sigSetup(float noKill, float answer){
        struct sigaction actionUsr, actionUsr2, actionUsr3;

        memset(&actionUsr, 0, sizeof(struct sigaction));
        actionUsr.sa_handler = onAlarm;
        sigaction(SIGALRM,&actionUsr,NULL);

        bool working[SIGRTMAX-SIGRTMIN+1];
        bool sending[SIGRTMAX-SIGRTMIN+1];

        memset(working,0,sizeof(working));
        memset(sending,0,sizeof(working));
        int i = 0;
        int howMany = (int)((float)(SIGRTMAX-SIGRTMIN+1)*noKill/100.0);
        memset(&actionUsr2, 0, sizeof(struct sigaction));
        actionUsr2.sa_handler = SIG_IGN;

        while(i < howMany)
        {
            for(int j = SIGRTMIN; j <= SIGRTMAX; j++)
            {
                if(working[j-SIGRTMIN]==true)
                    continue;
                if(i>=howMany)
                    break;
                if(rand()%2 == 1)
                {
                    sigaction(j,&actionUsr2,NULL);
                    working[j-SIGRTMIN]=true;
                    i++;
                }

            }
        }

        howMany = (int)((float)howMany*answer/100.0);
        i = 0;
        memset(&actionUsr3, 0, sizeof(struct sigaction));
        actionUsr3.sa_flags = SA_SIGINFO;
        actionUsr3.sa_sigaction = sigAnswer;
        while(i < howMany)
        {
            for(int j = SIGRTMIN; j <= SIGRTMAX; j++)
            {
                if(sending[j-SIGRTMIN] == true || working[j-SIGRTMIN] == false)
                    continue;
                if(i>=howMany)
                    break;
                if(rand()%2 == 1)
                {
                    sigaction(j,&actionUsr3,NULL);
                    sending[j-SIGRTMIN]=true;
                    i++;
                }
            }
        }

}

void sigAnswer(int sig, siginfo_t *info, void *ucontext){
    kill(info->si_pid,sig);
}

void onAlarm(){
    resourceVal+=addValue;
    if(PRINT == 1)
        printf("res: %f, demand: %f, received: %d\n", resourceVal, demandValue, counter);
}
