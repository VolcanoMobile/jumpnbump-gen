#pragma once

#define SFX_JUMP_1            64
#define SFX_JUMP_2            65
#define SFX_JUMP_3            66
#define SFX_DEATH_1           67
#define SFX_DEATH_2           68
#define SFX_DEATH_3           69
#define SFX_SPLASH_1          70
#define SFX_SPLASH_2          71
#define SFX_SPLASH_3          72
#define SFX_SPRING_1          73
#define SFX_SPRING_2          74
#define SFX_SPRING_3          75


void SFX_init(void);

void playJumpSfx();
void playDeathSfx();
void playSplashSfx();
void playSpringSfx();
