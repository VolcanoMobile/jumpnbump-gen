#pragma once

#include <genesis.h>

#include "bank.h"

typedef struct {
	u32 type;
	int x, y;
	int x_add, y_add;
	int x_acc, y_acc;
	u16 anim;
	u16 frame;
    s16 ticks;
	u16 image;
    bool (*update_ptr)(void*, const bool);
} object_t;

typedef struct {
    u16 num_frames;
    u16 restart_frame;
    struct {
        u16 image;
        u16 ticks;
    } frame[10];
} object_anim_t;

extern const object_anim_t object_anims[];

extern u16 springsCount;
extern object_t springs[];

void init_objects();
void clear_objects();
void load_objects_sprites();
void add_object(int type, int x, int y, int x_add, int y_add, int anim, int frame);
void update_objects();
void add_spring(int x, int y, int x_add, int y_add, int frame);
