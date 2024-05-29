#include "udp.h"
#include "hal/beatbox.h"
#include "hal/audio_mixer.h"
#include "shutdown.h"

#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>

#define FILE_UPTIME "/proc/uptime"
#define MSG_MAX_LEN 200000
#define PORT 3000
#define RPORT 12345

static pthread_t UDPThread;

static double uptime = 0;
static int hours, minutes, seconds;


void *displayUDPcommands(void *args)
{
    (void)args;
    struct sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;                // Connection may be from network
    sin.sin_addr.s_addr = htonl(INADDR_ANY); // Host to   Network long
    sin.sin_port = htons(RPORT);              // Host to Network short

    // Create the socket for UDP
    int socketDescriptor = socket(PF_INET, SOCK_DGRAM, 0);

    // Bind the socket to the port (PORT) that we specify
    if (bind(socketDescriptor, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    while (1)
    {
        // Get the data (blocking)
        // Will change sin (the address) to be the address of the client.
        // Note: sin passes information in and out of call!
        struct sockaddr_in sinRemote;

        unsigned int sin_len = sizeof(sinRemote);

        char messageRx[MSG_MAX_LEN];

        // Pass buffer size - 1 for max # bytes so room for the null (string data)
        int bytesRx = recvfrom(socketDescriptor,
                               messageRx, MSG_MAX_LEN - 1, 0,
                               (struct sockaddr *)&sinRemote, &sin_len);

        // Check for errors (-1)

        // Make it null terminated (so string functions work)
        // - recvfrom given max size - 1, so there is always room for the null
        messageRx[bytesRx] = 0;


        // change mode to "mode_none"
        if (strncmp(messageRx, "mode_none", strlen("mode_none")) == 0)
        {
            Beatbox_changePattern(0);
        }
        // change mode to "mode_one"
        else if (strncmp(messageRx, "mode_one", strlen("mode_one")) == 0)
        {
            Beatbox_changePattern(1);
        }
        // change mode to "mode_two"
        else if (strncmp(messageRx, "mode_two", strlen("mode_two")) == 0)
        {
            Beatbox_changePattern(2);
        }
    
        //make the volume go up by 5
        else if (strncmp(messageRx, "volume_up", strlen("volume_up")) == 0)
        {
            AudioMixer_incrementVolume();
        }
        //make the volume go down by 5
        else if (strncmp(messageRx, "volume_down", strlen("volume_down")) == 0)
        {
            AudioMixer_decrementVolume();
        }

        //make the BPM go up by 5
        else if (strncmp(messageRx, "bpm_up", strlen("bpm_up")) == 0)
        {
            Beatbox_incrementBpm();
        }
        //make the BPM go down by 5
        else if (strncmp(messageRx, "bpm_down", strlen("bpm_down")) == 0)
        {
            Beatbox_decrementBpm();
        }

        //command to queue a "hi_hat" sound
        else if (strncmp(messageRx, "hi_hat", strlen("hi_hat")) == 0)
        {
            Beatbox_queueTestSound(3);
        }
        //command to queue a "snare" sound
        else if (strncmp(messageRx, "snare", strlen("snare")) == 0)
        {
            Beatbox_queueTestSound(1);
        }
        //command to queue a "bass" sound
        else if (strncmp(messageRx, "bass", strlen("bass")) == 0)
        {
            Beatbox_queueTestSound(0);
        }
        // command for stop
        else if (strncmp(messageRx, "stop", strlen("stop")) == 0)
        {
            signalShutdown();
            break;
        }
        //get information about the current state of the program
        else if (strncmp(messageRx, "info", strlen("info")) == 0)
        {
            FILE *fp = fopen(FILE_UPTIME, "r");
            if (fp == NULL)
            {
                perror("Error opening file");
                return NULL;
            }
            if (fscanf(fp, "%lf", &uptime) < 1) {
                fprintf(stderr, "Failed to read uptime from /proc/uptime\n");
                fclose(fp);
                return NULL;
            }
            fclose(fp);

            hours = (int)uptime / 3600;
            minutes = ((int)uptime % 3600) / 60;
            seconds = (int)uptime % 60;
            
            char messageTx[MSG_MAX_LEN];
            sprintf(messageTx, "BPM:%d\nVolume:%d\nPattern:%d\n Hours: %d Minutes: %d Seconds: %d\n", Beatbox_getBpm(), AudioMixer_getVolume(), Beatbox_getPatternNumber(), hours, minutes, seconds);
            sin_len = sizeof(sinRemote);
            sendto(socketDescriptor, messageTx, strlen(messageTx), 0, (struct sockaddr *)&sinRemote, sin_len);
        }

        // command for any other command
        else
        {
            char messageTx[MSG_MAX_LEN];
            sprintf(messageTx, "Command not recognized\n");
            sin_len = sizeof(sinRemote);
            sendto(socketDescriptor, messageTx, strlen(messageTx), 0, (struct sockaddr *)&sinRemote, sin_len);
        }

    }
    close(socketDescriptor);
    return NULL;
}

void createUDPThread()
{
    pthread_create(&UDPThread, NULL, displayUDPcommands, NULL);
}

void joinUDPThread()
{
    pthread_join(UDPThread, NULL);
}