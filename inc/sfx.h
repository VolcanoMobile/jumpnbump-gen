#pragma once

#define SFX_JUMP_1            64
#define SFX_JUMP_2            65
#define SFX_JUMP_3            66
#define SFX_JUMP_4            67

#define SFX_DEATH_1           68
#define SFX_DEATH_2           69
#define SFX_DEATH_3           70
#define SFX_DEATH_4           71

#define SFX_SPLASH_1          72
#define SFX_SPLASH_2          73
#define SFX_SPLASH_3          74
#define SFX_SPLASH_4          75

#define SFX_SPRING_1          76
#define SFX_SPRING_2          77
#define SFX_SPRING_3          78
#define SFX_SPRING_4          79


void SFX_init(void);

void playJumpSfx();
void playDeathSfx();
void playSplashSfx();
void playSpringSfx();
