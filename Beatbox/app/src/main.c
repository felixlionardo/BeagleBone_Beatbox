#include "hal/audio_mixer.h"
#include "hal/general_command.h"
#include "hal/beatbox.h"
#include "hal/gpio.h"
#include "hal/joystick.h"
#include "hal/accelerometer.h"
#include "shutdown.h"

#include <stdio.h>
#include <stdlib.h>
#include <alsa/asoundlib.h>


#define SOURCE_FILE1 "wave-files/100060__menegass__gui-drum-splash-hard.wav"
#define SOURCE_FILE2 "wave-files/100053__menegass__gui-drum-cc.wav"
#define SOURCE_FILE3 "wave-files/100051__menegass__gui-drum-bd-hard.wav"

#define SAMPLE_RATE   44100
#define NUM_CHANNELS  1
#define SAMPLE_SIZE   (sizeof(short))


int main(void)
{
    createThreads();

    waitShutdown();
    
    joinThreads();
    

    return 0;
}