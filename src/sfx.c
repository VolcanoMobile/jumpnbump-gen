#include <genesis.h>

#include "global.h"
#include "sfx.h"
#include "resources.h"

static u32 nextPcmChannel = SOUND_PCM_CH1;

void SFX_init(void)
{
    XGM_setPCM(SFX_JUMP_1, jump_1_sfx, sizeof(jump_1_sfx));
    XGM_setPCM(SFX_JUMP_2, jump_2_sfx, sizeof(jump_2_sfx));
    XGM_setPCM(SFX_JUMP_3, jump_3_sfx, sizeof(jump_3_sfx));
    XGM_setPCM(SFX_JUMP_4, jump_3_sfx, sizeof(jump_3_sfx));
    XGM_setPCM(SFX_DEATH_1, death_1_sfx, sizeof(death_1_sfx));
    XGM_setPCM(SFX_DEATH_2, death_2_sfx, sizeof(death_2_sfx));
    XGM_setPCM(SFX_DEATH_3, death_3_sfx, sizeof(death_3_sfx));
    XGM_setPCM(SFX_DEATH_4, death_3_sfx, sizeof(death_3_sfx));
    XGM_setPCM(SFX_SPLASH_1, splash_1_sfx, sizeof(splash_1_sfx));
    XGM_setPCM(SFX_SPLASH_2, splash_2_sfx, sizeof(splash_2_sfx));
    XGM_setPCM(SFX_SPLASH_3, splash_3_sfx, sizeof(splash_3_sfx));
    XGM_setPCM(SFX_SPLASH_4, splash_3_sfx, sizeof(splash_3_sfx));
    XGM_setPCM(SFX_SPRING_1, spring_1_sfx, sizeof(spring_1_sfx));
    XGM_setPCM(SFX_SPRING_2, spring_2_sfx, sizeof(spring_2_sfx));
    XGM_setPCM(SFX_SPRING_3, spring_3_sfx, sizeof(spring_3_sfx));
    XGM_setPCM(SFX_SPRING_4, spring_3_sfx, sizeof(spring_3_sfx));
}

void playJumpSfx()
{
    XGM_startPlayPCM(SFX_JUMP_1 + (random() % 4), 1, nextPcmChannel++);
    if (nextPcmChannel > SOUND_PCM_CH4) {
        nextPcmChannel = SOUND_PCM_CH1;
    }
}

void playDeathSfx()
{
    XGM_startPlayPCM(SFX_DEATH_1 + (random() % 4), 1, nextPcmChannel++);
    if (nextPcmChannel > SOUND_PCM_CH4) {
        nextPcmChannel = SOUND_PCM_CH1;
    }
}

void playSplashSfx()
{
    XGM_startPlayPCM(SFX_SPLASH_1 + (random() % 4), 1, nextPcmChannel++);
    if (nextPcmChannel > SOUND_PCM_CH4) {
        nextPcmChannel = SOUND_PCM_CH1;
    }
}

void playSpringSfx()
{
    XGM_startPlayPCM(SFX_SPRING_1 + (random() % 4), 1, nextPcmChannel++);
    if (nextPcmChannel > SOUND_PCM_CH4) {
        nextPcmChannel = SOUND_PCM_CH1;
    }
}
