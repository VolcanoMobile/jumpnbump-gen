#pragma once

#include <genesis.h>

#define JNB_VERSION "1.60"

#define JNB_WIDTH 400
#define JNB_HEIGHT 256

#define JNB_MAX_PLAYERS 4

#define JNB_END_SCORE 100

#define KEY_PL1_LEFT	0xa0
#define KEY_PL1_RIGHT	0xa1
#define KEY_PL1_JUMP	0xa2
#define KEY_PL2_LEFT	0xb0
#define KEY_PL2_RIGHT	0xb1
#define KEY_PL2_JUMP	0xb2
#define KEY_PL3_LEFT	0xc0
#define KEY_PL3_RIGHT	0xc1
#define KEY_PL3_JUMP	0xc2
#define KEY_PL4_LEFT	0xd0
#define KEY_PL4_RIGHT	0xd1
#define KEY_PL4_JUMP	0xd2

#define OBJ_SPRING 0
#define OBJ_SPLASH 1
#define OBJ_SMOKE 2
#define OBJ_YEL_BUTFLY 3
#define OBJ_PINK_BUTFLY 4
#define OBJ_FUR 5
#define OBJ_FLESH 6
#define OBJ_FLESH_TRACE 7

#define OBJ_ANIM_SPRING 0
#define OBJ_ANIM_SPLASH 1
#define OBJ_ANIM_SMOKE 2
#define OBJ_ANIM_YEL_BUTFLY_RIGHT 3
#define OBJ_ANIM_YEL_BUTFLY_LEFT 4
#define OBJ_ANIM_PINK_BUTFLY_RIGHT 5
#define OBJ_ANIM_PINK_BUTFLY_LEFT 6
#define OBJ_ANIM_FLESH_TRACE 7

#define BAN_VOID	0
#define BAN_SOLID	1
#define BAN_WATER	2
#define BAN_ICE		3
#define BAN_SPRING	4

// #define NUM_OBJECTS 200
#define NUM_OBJECTS 70

struct {
    u16 joy;
    u16 changed;
    u16 state;
} input;

typedef struct {
    int action_left,action_up,action_right;
    int enabled, dead_flag;
    int bumps;
    int bumped[JNB_MAX_PLAYERS];
    int x, y;
    int x_add, y_add;
    int direction, jump_ready, jump_abort, in_water;
    int anim, frame, frame_tick, image;
} player_t;

typedef struct {
    u16 num_frames;
    u16 restart_frame;
    struct {
        u16 image;
        u16 ticks;
    } frame[4];
} player_anim_t;

/* main.c */

extern u16 VDPTilesFilled;
extern u16 palette[];
extern player_t player[];
extern player_anim_t player_anims[];
extern const u16 (*ban_map)[32];
extern u16 endscore_reached;
extern bool ai[];

// SAT pointer
extern VDPSprite* vdpSprite;
// VDP sprite index (for link field)
extern u8 vdpSpriteInd;

u16 rnd(u16 max);

/* interrpt.c */

void initKeyb();
int key_pressed(int key);
void addkey(unsigned int key);

/* input.c */

void update_player_actions();
