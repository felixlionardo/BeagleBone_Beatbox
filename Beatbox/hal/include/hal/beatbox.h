#ifndef _BEATBOX_H_
#define _BEATBOX_H_

#include <stdint.h>

#define BEATBOX_MAX_BPM 300
#define BEATBOX_MIN_BPM 40

//Module to manage the beatbox application

void Beatbox_queueTestSound(int index);
void Beatbox_changeBpm(int newBpm);
void Beatbox_incrementBpm();
void Beatbox_decrementBpm();
int Beatbox_getBpm();
int Beatbox_getPatternNumber();
void Beatbox_init();
void Beatbox_changePattern(int patternNumber);
void Beatbox_incrementPattern();
void Beatbox_cleanup();


#endif