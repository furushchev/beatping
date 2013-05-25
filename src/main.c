//
//  main.c
//  pingbeats
//
//  Created by Seki Inoue on 2012/09/29.
//  Copyright (c) 2012年 peroxyacyl. All rights reserved.
//

#include <ALUT/alut.h>
#include <math.h>
#include <stdio.h>
#include <pthread.h>

#include "beatping.h"

static float beat_interval = 1;
static float beat_duration = 0.15;
static ALboolean should_quit = 0;


void set_beat(float interval, float duration) {
    if (duration>=0) beat_duration = duration;
    if (interval>=0) beat_interval = log(interval*1000)/3;
}

void *sound_thread(void *info) {
    alutInit(NULL,NULL);
    ALuint buffer, source;
    alGenBuffers( 1, &buffer );
    alGenSources( 1, &source );
    const int freq = 225000 , Hz = 810;
	ALshort data[freq/Hz];
    for (int i = 0; i < freq/Hz ; ++i) {
        data[i] = 15000 * sin(i * M_PI * 2 * Hz / freq);
    }
    alBufferData( buffer, AL_FORMAT_MONO16, data, freq/Hz*sizeof(ALshort), freq );
    alSourcei( source, AL_BUFFER, buffer );
    alSourcei( source, AL_LOOPING, AL_TRUE );
    
    while (!should_quit) {
        ALint state;
        alGetSourcei(source, AL_SOURCE_STATE, &state);
        if (state != AL_PLAYING) {
            alSourcePlay(source);
        }

        alSourcef(source, AL_GAIN, 1.0);
        alutSleep (beat_duration);
        if (beat_interval>0) {
            for (int i = 1; i <= 100; i++) {
                alSourcef(source, AL_GAIN, 1.0-(float)i/100);
                alutSleep(0.001);
            }
            alutSleep (beat_interval);
        }
    }
    return NULL;
}

int main(int argc, const char * argv[])
{
    char ch = NULL;
    int silent = 0;
    while ((ch = getopt(argc, argv, "H")) != -1)
	{
		switch(ch) {
            case 'H':
                silent = 1;
                break;
            default:
                break;
        }
    }
    pthread_t thread;
    if (silent == 0) pthread_create( &thread, NULL, sound_thread, NULL);
    ping_main(argc, argv);
    should_quit = 1;
    if (silent == 0) pthread_join(thread, NULL);
    return 0;
}