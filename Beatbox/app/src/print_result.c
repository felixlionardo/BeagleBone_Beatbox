#include "hal/audio_mixer.h"
#include "hal/general_command.h"
#include "hal/beatbox.h"
#include "hal/gpio.h"
#include "hal/joystick.h"
#include "hal/accelerometer.h"
#include "period_timer.h"

#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

static pthread_t printingThread;
static bool shutdown = false;


static void printCurrentInfo(){
    printf("M%d  %3dbpm  vol:%3d", 
     Beatbox_getPatternNumber(), Beatbox_getBpm(), AudioMixer_getVolume());
}

static void printTimingJitter(){
    Period_statistics_t pStats;

    Period_getStatisticsAndClear(PERIOD_EVENT_REFILL_AUDIO_PLAYBACK_BUFFER, &pStats);
    printf("\tAudio[%6.3f, %6.3f] avg %.3f/%d", pStats.minPeriodInMs, pStats.maxPeriodInMs,
     pStats.avgPeriodInMs, pStats.numSamples);  

    Period_getStatisticsAndClear(PERIOD_EVENT_SAMPLE_ACCELEROMETER, &pStats);
    printf("\tAccel[%6.3f, %6.3f] avg %.3f/%d", pStats.minPeriodInMs, pStats.maxPeriodInMs,
     pStats.avgPeriodInMs, pStats.numSamples); 

    printf("\n");
}

void *printing(void *args){
    (void)args;
    
    while(!shutdown){

    printCurrentInfo();
    printTimingJitter();

    sleepForMs(1000);
    } 

    return NULL;
}

void createPrintingThread(){
    pthread_create(&printingThread, NULL, printing, NULL);
}

void joinPrintingThread(){
    pthread_join(printingThread, NULL);
}

void shutdownPrintingThread(){
    shutdown = true;
}