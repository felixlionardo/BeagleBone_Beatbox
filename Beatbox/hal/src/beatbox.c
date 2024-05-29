#include "hal/beatbox.h"
#include "hal/audio_mixer.h"
#include "hal/general_command.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>



#define DRUM_BD_HARD "wave-files/100051__menegass__gui-drum-bd-hard.wav"
#define DRUM_BD_SOFT "wave-files/100052__menegass__gui-drum-bd-soft.wav"
#define DRUM_CC "wave-files/100053__menegass__gui-drum-cc.wav"
#define DRUM_CH "wave-files/100054__menegass__gui-drum-ch.wav"
#define DRUM_CO "wave-files/100055__menegass__gui-drum-co.wav"
#define DRUM_CYN_HARD "wave-files/100056__menegass__gui-drum-cyn-hard.wav"
#define DRUM_CYN_SOFT "wave-files/100057__menegass__gui-drum-cyn-soft.wav"
#define DRUM_SNARE_HARD "wave-files/100058__menegass__gui-drum-snare-hard.wav"
#define DRUM_SNARE_SOFT "wave-files/100059__menegass__gui-drum-snare-soft.wav"
#define DRUM_SPLASH_HARD "wave-files/100060__menegass__gui-drum-splash-hard.wav"
#define DRUM_SPLASH_SOFT "wave-files/100061__menegass__gui-drum-splash-soft.wav"
#define DRUM_TOM_HI_HARD "wave-files/100062__menegass__gui-drum-tom-hi-hard.wav"
#define DRUM_TOM_HI_SOFT "wave-files/100063__menegass__gui-drum-tom-hi-soft.wav"
#define DRUM_TOM_LO_HARD "wave-files/100064__menegass__gui-drum-tom-lo-hard.wav"
#define DRUM_TOM_LO_SOFT "wave-files/100065__menegass__gui-drum-tom-lo-soft.wav"
#define DRUM_TOM_MID_HARD "wave-files/100066__menegass__gui-drum-tom-mid-hard.wav"
#define DRUM_TOM_MID_SOFT "wave-files/100067__menegass__gui-drum-tom-mid-soft.wav"
#define RICK_ROLL "wave-files/rickroll.wav"

#define DEFAULT_BPM 120

wavedata_t drumBdHard;
wavedata_t drumCc;
wavedata_t drumCh;
wavedata_t drumSnareHard;
wavedata_t drumSplashHard;
wavedata_t rickRoll;

static int bpm = DEFAULT_BPM;
static bool shutdown = false;

static pthread_t beatboxThread;

typedef struct {
    wavedata_t* sound1;
    wavedata_t* sound2;
    double timeOffset;
} BeatPattern;

typedef struct {
    int patternNumber;
    BeatPattern* pattern;
    bool changeRequested;
    int size;
} PatternManager;

static BeatPattern beatNumber0[] = {
    {NULL, NULL, 0.5},
    {NULL, NULL, 0.5},
    {NULL, NULL, 0.5},
    {NULL, NULL, 0.5},
    {NULL, NULL, 0.5},
    {NULL, NULL, 0.5},
    {NULL, NULL, 0.5},
    {NULL, NULL, 0.5}
};

static BeatPattern beatNumber1[] = {
    {&drumCc, &drumBdHard, 0.5},
    {&drumCc, NULL, 0.5},
    {&drumCc, &drumSnareHard, 0.5},
    {&drumCc, NULL, 0.5},
    {&drumCc, &drumBdHard, 0.5},
    {&drumCc, NULL, 0.5},
    {&drumCc, &drumSnareHard, 0.5},
    {&drumCc, NULL, 0.5}
};

static BeatPattern beatNumber2[] = {
    {&drumSplashHard, &drumBdHard, 0.25},
    {&drumCh, NULL, 0.25},
    {&drumCh, NULL, 0.25},
    {&drumCh, NULL, 0.25},
    {NULL, &drumSnareHard, 0.5},
    {&drumCh, NULL, 0.25},
    {&drumCh, NULL, 0.25},
    {&drumCh, NULL, 0.25},
    {&drumCh, &drumBdHard, 0.25},
    {&drumCh, NULL, 0.25},
    {&drumCh, &drumSnareHard, 0.5},
    {NULL, NULL, 0.5},
    {&drumCh, &drumBdHard, 0.25},
    {&drumCh, NULL, 0.25},
    {&drumCh, NULL, 0.25},
    {&drumCh, NULL, 0.25},
    {NULL, &drumSnareHard, 0.5},
    {&drumCh, NULL, 0.25},
    {&drumCh, NULL, 0.25},
    {&drumCh, NULL, 0.25},
    {&drumCh, &drumBdHard, 0.25},
    {&drumCh, NULL, 0.25},
    {&drumCh, &drumSnareHard, 0.5},
    {NULL, NULL, 0.5}
};

// Manage Beat Patterns
static PatternManager patternManager = {
    .patternNumber = 0,
    .pattern = beatNumber0,
    .changeRequested = false,
    .size = sizeof(beatNumber0) / sizeof(beatNumber0[0])
};

void *Beatbox_startThread(void *arg);

void Beatbox_queueTestSound(int index) {

    switch (index) {
        case 0: AudioMixer_queueSound(&drumBdHard); break; //bass
        case 1: AudioMixer_queueSound(&drumSnareHard); break; //snare
        case 2: AudioMixer_queueSound(&drumSplashHard); break; // splash
        case 3: AudioMixer_queueSound(&drumCh); break; // hi-hat
        default: break;
    }
}

void Beatbox_changeBpm(int newBpm) {
    bpm = newBpm;
}

void Beatbox_incrementBpm() {
    int newBpm = bpm + 5;
    if (newBpm <= BEATBOX_MAX_BPM) {
        bpm = newBpm;
    }
}

void Beatbox_decrementBpm() {
    int newBpm = bpm - 5;
    if (newBpm >= BEATBOX_MIN_BPM) {
        bpm = newBpm;
    }
}

int Beatbox_getBpm() {
    return bpm;
}

static double Beatbox_halfBeat(int bpm) {
    //Time For Half Beat [sec] = 60 [sec/min] /  BPM / 2 [half-beats per beat]
    //Calculate the time for a half beat in milliseconds
    return (60.0 / bpm) * 1000;
}

void Beatbox_init() {
    AudioMixer_readWaveFileIntoMemory(DRUM_BD_HARD, &drumBdHard);
    AudioMixer_readWaveFileIntoMemory(DRUM_CC, &drumCc);
    AudioMixer_readWaveFileIntoMemory(DRUM_CH, &drumCh);
    AudioMixer_readWaveFileIntoMemory(DRUM_SNARE_HARD, &drumSnareHard);
    AudioMixer_readWaveFileIntoMemory(DRUM_SPLASH_HARD, &drumSplashHard);

    pthread_create(&beatboxThread, NULL, Beatbox_startThread, NULL);
}



void Beatbox_changePattern(int patternNumber) {
    switch (patternNumber) {
        case 0:
            patternManager.patternNumber = 0;
            patternManager.pattern = beatNumber0;
            patternManager.size = sizeof(beatNumber0) / sizeof(beatNumber0[0]);
            break;
        case 1:
                    patternManager.patternNumber = 1;

            patternManager.pattern = beatNumber1;
            patternManager.size = sizeof(beatNumber1) / sizeof(beatNumber1[0]);
            break;
        case 2:
                    patternManager.patternNumber = 2;

            patternManager.pattern = beatNumber2;
            patternManager.size = sizeof(beatNumber2) / sizeof(beatNumber2[0]);
            break;
        default:
                    patternManager.patternNumber = 0;

            patternManager.pattern = beatNumber0;
            patternManager.size = sizeof(beatNumber0) / sizeof(beatNumber0[0]);
            break;
    }
    patternManager.changeRequested = true;
}

int Beatbox_getPatternNumber() {
    return patternManager.patternNumber;
}

void Beatbox_incrementPattern() {
    Beatbox_changePattern((Beatbox_getPatternNumber() + 1) % 3);
}


void *Beatbox_startThread(void *arg) {
    (void)arg;
    while(!shutdown){
        BeatPattern* pattern = patternManager.pattern;
        patternManager.changeRequested = false;
        int i = 0;
        while (i < patternManager.size && !patternManager.changeRequested) {
            if (pattern[i].sound1 != NULL)
                AudioMixer_queueSound(pattern[i].sound1);
            if (pattern[i].sound2 != NULL) 
                AudioMixer_queueSound(pattern[i].sound2);
            
            sleepForMs(Beatbox_halfBeat(bpm) * (pattern[i].timeOffset));
            i++;
        }
    }
    return NULL;

}

void Beatbox_cleanup() {

    shutdown = true;
    patternManager.changeRequested = true;
    pthread_join(beatboxThread, NULL);

    AudioMixer_freeWaveFileData(&drumBdHard);
    AudioMixer_freeWaveFileData(&drumCc);
    AudioMixer_freeWaveFileData(&drumCh);
    AudioMixer_freeWaveFileData(&drumSnareHard);
    AudioMixer_freeWaveFileData(&drumSplashHard);

    
    


}