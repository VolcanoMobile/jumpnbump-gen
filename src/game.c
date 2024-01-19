#include <genesis.h>

#include "bank.h"

#include "global.h"
#include "objects.h"
#include "rabbits.h"
#include "sfx.h"
#include "resources.h"

#define GET_BAN_MAP_XY(x,y) ban_map[(u16)((y) >> 4)][(u16)((x) >> 4)]
#define GET_BAN_MAP_IN_WATER(s1, s2) (GET_BAN_MAP_XY((s1), ((s2) + 7)) == BAN_VOID || GET_BAN_MAP_XY(((s1) + 15), ((s2) + 7)) == BAN_VOID) && (GET_BAN_MAP_XY((s1), ((s2) + 8)) == BAN_WATER || GET_BAN_MAP_XY(((s1) + 15), ((s2) + 8)) == BAN_WATER)

int pogostick, bunnies_in_space, jetpack, lord_of_the_flies, blood_is_thicker_than_water;

static const u16 level_map[32][32] = {
    {1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0},
    {1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1},
    {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1},
    {1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 1, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 1, 1, 1, 1, 0, 0, 0, 0, 3, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1},
    {2, 2, 2, 2, 2, 2, 2, 2, 1, 4, 0, 0, 0, 0, 0, 1, 3, 3, 3, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};

#define EXPLOSION_RANDOM_VALUES_SIZE 128
static s32 explosions_values[EXPLOSION_RANDOM_VALUES_SIZE][4];

static void position_player(u32 player_num)
{
    u32 c1;
    s16 s1, s2;

    while (1) {
        while (1) {
            s1 = rnd(22);
            s2 = rnd(16);
            if (ban_map[s2][s1] == BAN_VOID && (ban_map[s2 + 1][s1] == BAN_SOLID || ban_map[s2 + 1][s1] == BAN_ICE))
                break;
        }
        for (c1 = 0; c1 < JNB_MAX_PLAYERS; c1++) {
            if (c1 != player_num && player[c1].enabled == 1) {
                if (abs((s1 << 4) - (player[c1].x >> 16)) < 32 && abs((s2 << 4) - (player[c1].y >> 16)) < 32)
                    break;
            }
        }
        if (c1 == JNB_MAX_PLAYERS) {
            player[player_num].x = (long) s1 << 20;
            player[player_num].y = (long) s2 << 20;
            player[player_num].x_add = player[player_num].y_add = 0;
            player[player_num].direction = 0;
            player[player_num].jump_ready = 1;
            player[player_num].in_water = 0;
            player[player_num].anim = 0;
            player[player_num].frame = 0;
            player[player_num].frame_tick = 0;
            player[player_num].image = player_anims[player[player_num].anim].frame[player[player_num].frame].image;
            player[player_num].dead_flag = 0;
            break;
        }
    }
}

void unloadLevel() {
    SYS_disableInts();
    VDP_clearPlane(BG_B, TRUE);
    VDP_clearPlane(BG_A, TRUE);
    SYS_enableInts();

    springsCount = 0;
    VDPTilesFilled -= image_level.tileset->numTile;
}

void loadLevel() {
    memcpy(&palette[0], image_level.palette->data, 16 * 2);
    palette[0] = RGB3_3_3_TO_VDPCOLOR(0, 0, 0);

    VDP_loadTileSet(image_level.tileset, VDPTilesFilled, DMA);
    VDP_setTileMapEx(BG_B, image_level.tilemap, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, VDPTilesFilled), 0, 0, 0, 0, 40, 28, DMA);
    VDP_setTileMapEx(BG_A, image_level.tilemap, TILE_ATTR_FULL(PAL0, TRUE, FALSE, FALSE, VDPTilesFilled), 0, 0, 0, 28, 40, 28, DMA);
    VDPTilesFilled += image_level.tileset->numTile;

    SYS_doVBlankProcess();

    VDP_loadFont(&level_font, CPU);
    // re-pack memory as VDP_lontFont allocate memory to unpack font
    MEM_pack();
}

void initLevel() {
    ban_map = (u16 (*)[32])level_map;

    for (u32 c1 = 0; c1 < JNB_MAX_PLAYERS; c1++) {
        if (player[c1].enabled == 1) {
            player[c1].bumps = 0;
            for (u32 c2 = 0; c2 < JNB_MAX_PLAYERS; c2++)
                player[c1].bumped[c2] = 0;
            position_player(c1);
            VDP_drawText("00", 36, 4 + c1 * 7);
        }
    }

    clear_objects();

    for(u32 c1 = 0; c1 < EXPLOSION_RANDOM_VALUES_SIZE; c1++) {
        explosions_values[c1][0] = rnd(5);
        explosions_values[c1][1] = rnd(5);
        explosions_values[c1][2] = (rnd(65535) - 32768) * 3;
        explosions_values[c1][3] = (rnd(65535) - 32768) * 3;
    }

    springsCount = 0;
    for (u32 c1 = 0; c1 < 16; c1++) {
        for (u32 c2 = 0; c2 < 22; c2++) {
            if (ban_map[c1][c2] == BAN_SPRING)
                add_spring(c2 << 4, c1 << 4, 0, 0, 5);
        }
    }

    while (1) {
        u32 s1 = rnd(22);
        u32 s2 = rnd(16);
        if (ban_map[s2][s1] == BAN_VOID) {
            add_object(OBJ_YEL_BUTFLY, (s1 << 4) + 8, (s2 << 4) + 8, (rnd(65535) - 32768) * 2, (rnd(65535) - 32768) * 2, 0, 0);
            break;
        }
    }
    while (1) {
        u16 s1 = rnd(22);
        u16 s2 = rnd(16);
        if (ban_map[s2][s1] == BAN_VOID) {
            add_object(OBJ_YEL_BUTFLY, (s1 << 4) + 8, (s2 << 4) + 8, (rnd(65535) - 32768) * 2, (rnd(65535) - 32768) * 2, 0, 0);
            break;
        }
    }
    while (1) {
        u32 s1 = rnd(22);
        u32 s2 = rnd(16);
        if (ban_map[s2][s1] == BAN_VOID) {
            add_object(OBJ_PINK_BUTFLY, (s1 << 4) + 8, (s2 << 4) + 8, (rnd(65535) - 32768) * 2, (rnd(65535) - 32768) * 2, 0, 0);
            break;
        }
    }
    while (1) {
        u32 s1 = rnd(22);
        u32 s2 = rnd(16);
        if (ban_map[s2][s1] == BAN_VOID) {
            add_object(OBJ_PINK_BUTFLY, (s1 << 4) + 8, (s2 << 4) + 8, (rnd(65535) - 32768) * 2, (rnd(65535) - 32768) * 2, 0, 0);
            break;
        }
    }

    lord_of_the_flies = bunnies_in_space = jetpack = pogostick = blood_is_thicker_than_water = 0;
}

static void player_action_left(int c1)
{
    s16 s1 = 0, s2 = 0;
    u32 below_left, below, below_right;

    s1 = (player[c1].x >> 16);
    s2 = (player[c1].y >> 16);
    below_left = GET_BAN_MAP_XY(s1, s2 + 16);
    below = GET_BAN_MAP_XY(s1 + 8, s2 + 16);
    below_right = GET_BAN_MAP_XY(s1 + 15, s2 + 16);

    if (below == BAN_ICE) {
        if (player[c1].x_add > 0)
            player[c1].x_add -= 1024;
        else
            player[c1].x_add -= 768;
    } else if ((below_left != BAN_SOLID && below_right == BAN_ICE) || (below_left == BAN_ICE && below_right != BAN_SOLID)) {
        if (player[c1].x_add > 0)
            player[c1].x_add -= 1024;
        else
            player[c1].x_add -= 768;
    } else {
        if (player[c1].x_add > 0) {
            player[c1].x_add -= 16384;
            if (player[c1].x_add > -98304L && player[c1].in_water == 0 && below == BAN_SOLID)
                add_object(OBJ_SMOKE, (player[c1].x >> 16) + 2 + rnd(9), (player[c1].y >> 16) + 13 + rnd(5), 0, -16384 - rnd(8192), OBJ_ANIM_SMOKE, 0);
        } else
            player[c1].x_add -= 12288;
    }
    if (player[c1].x_add < -98304L)
        player[c1].x_add = -98304L;
    player[c1].direction = 1;
    if (player[c1].anim == 0) {
        player[c1].anim = 1;
        player[c1].frame = 0;
        player[c1].frame_tick = 0;
        player[c1].image = player_anims[player[c1].anim].frame[player[c1].frame].image + player[c1].direction * 9;
    }
}


static void player_action_right(int c1)
{
    s16 s1 = 0, s2 = 0;
    u32 below_left, below, below_right;

    s1 = (player[c1].x >> 16);
    s2 = (player[c1].y >> 16);
    below_left = GET_BAN_MAP_XY(s1, s2 + 16);
    below = GET_BAN_MAP_XY(s1 + 8, s2 + 16);
    below_right = GET_BAN_MAP_XY(s1 + 15, s2 + 16);

    if (below == BAN_ICE) {
        if (player[c1].x_add < 0)
            player[c1].x_add += 1024;
        else
            player[c1].x_add += 768;
    } else if ((below_left != BAN_SOLID && below_right == BAN_ICE) || (below_left == BAN_ICE && below_right != BAN_SOLID)) {
        if (player[c1].x_add > 0)
            player[c1].x_add += 1024;
        else
            player[c1].x_add += 768;
    } else {
        if (player[c1].x_add < 0) {
            player[c1].x_add += 16384;
            if (player[c1].x_add < 98304L && player[c1].in_water == 0 && below == BAN_SOLID)
                add_object(OBJ_SMOKE, (player[c1].x >> 16) + 2 + rnd(9), (player[c1].y >> 16) + 13 + rnd(5), 0, -16384 - rnd(8192), OBJ_ANIM_SMOKE, 0);
        } else
            player[c1].x_add += 12288;
    }
    if (player[c1].x_add > 98304L)
        player[c1].x_add = 98304L;
    player[c1].direction = 0;
    if (player[c1].anim == 0) {
        player[c1].anim = 1;
        player[c1].frame = 0;
        player[c1].frame_tick = 0;
        player[c1].image = player_anims[player[c1].anim].frame[player[c1].frame].image + player[c1].direction * 9;
    }
}

static int map_tile(int pos_x, int pos_y)
{
    u16 tile;

    pos_x = pos_x >> 4;
    pos_y = pos_y >> 4;

    if(pos_x < 0 || pos_x >= 17 || pos_y < 0 || pos_y >= 22)
        return BAN_VOID;

    tile = ban_map[(u16)pos_y][(u16)pos_x];
    return tile;
}

static void cpu_move(void)
{
    int lm, rm, jm;
    int cur_posx, cur_posy, tar_posx, tar_posy;
    int players_distance;
    player_t* target = NULL;
    int nearest_distance = -1;

    for (u32 i = 0; i < JNB_MAX_PLAYERS; i++)
    {
        nearest_distance = -1;
        if(ai[i] && player[i].enabled)		// this player is a computer
        {		// get nearest target
            for (u32 j = 0; j < JNB_MAX_PLAYERS; j++)
            {
                int deltax, deltay;

                if(i == j || !player[j].enabled)
                    continue;

                deltax = player[j].x - player[i].x;
                deltay = player[j].y - player[i].y;
                players_distance = deltax*deltax + deltay*deltay;

                if (players_distance < nearest_distance || nearest_distance == -1)
                {
                    target = &player[j];
                    nearest_distance = players_distance;
                }
            }

            if(target == NULL)
                continue;

            cur_posx = player[i].x >> 16;
            cur_posy = player[i].y >> 16;
            tar_posx = target->x >> 16;
            tar_posy = target->y >> 16;

            /** nearest player found, get him */
            /* here goes the artificial intelligence code */

            /* X-axis movement */
            if(tar_posx > cur_posx)       // if true target is on the right side
            {    // go after him
                lm=0;
                rm=1;
            }
            else    // target on the left side
            {
                lm=1;
                rm=0;
            }

            if(cur_posy - tar_posy < 32 && cur_posy - tar_posy > 0 &&
                tar_posx - cur_posx < 32+8 && tar_posx - cur_posx > -32)
            {
                lm = !lm;
                rm = !rm;
            }
            else if(tar_posx - cur_posx < 4+8 && tar_posx - cur_posx > -4)
            {      // makes the bunnies less "nervous"
                lm=0;
                lm=0;
            }

            /* Y-axis movement */
            if(map_tile(cur_posx, cur_posy+16) != BAN_VOID &&
                ((i == 0 && key_pressed(KEY_PL1_JUMP)) ||
                (i == 1 && key_pressed(KEY_PL2_JUMP)) ||
                (i == 2 && key_pressed(KEY_PL3_JUMP)) ||
                (i == 3 && key_pressed(KEY_PL4_JUMP))))
                    jm=0;   // if we are on ground and jump key is being pressed,
                                    //first we have to release it or else we won't be able to jump more than once

            else if(map_tile(cur_posx, cur_posy-8) != BAN_VOID &&
                map_tile(cur_posx, cur_posy-8) != BAN_WATER)
                    jm=0;   // don't jump if there is something over it

            else if(map_tile(cur_posx-(lm*8)+(rm*16), cur_posy) != BAN_VOID &&
                map_tile(cur_posx-(lm*8)+(rm*16), cur_posy) != BAN_WATER &&
                cur_posx > 16 && cur_posx < 352-16-8)  // obstacle, jump
                    jm=1;   // if there is something on the way, jump over it

            else if(((i == 0 && key_pressed(KEY_PL1_JUMP)) ||
                            (i == 1 && key_pressed(KEY_PL2_JUMP)) ||
                            (i == 2 && key_pressed(KEY_PL3_JUMP)) ||
                            (i == 3 && key_pressed(KEY_PL4_JUMP))) &&
                            (map_tile(cur_posx-(lm*8)+(rm*16), cur_posy+8) != BAN_VOID &&
                            map_tile(cur_posx-(lm*8)+(rm*16), cur_posy+8) != BAN_WATER))
                    jm=1;   // this makes it possible to jump over 2 tiles

            else if(cur_posy - tar_posy < 32 && cur_posy - tar_posy > 0 &&
                tar_posx - cur_posx < 32+8 && tar_posx - cur_posx > -32)  // don't jump - running away
                jm=0;

            else if(tar_posy <= cur_posy)   // target on the upper side
                jm=1;
            else   // target below
                jm=0;

            /** Artificial intelligence done, now apply movements */
            if(lm)
            {
                unsigned int key;
                if(i == 0)
                    key = KEY_PL1_LEFT;
                else if(i == 1)
                    key = KEY_PL2_LEFT;
                else if(i == 2)
                    key = KEY_PL3_LEFT;
                else
                    key = KEY_PL4_LEFT;

                key &= 0x7fff;
                addkey(key);
            }
            else
            {
                unsigned int key;
                if(i == 0)
                    key = KEY_PL1_LEFT;
                else if(i == 1)
                    key = KEY_PL2_LEFT;
                else if(i == 2)
                    key = KEY_PL3_LEFT;
                else
                    key = KEY_PL4_LEFT;

                key &= 0x7fff;
                addkey(key | 0x8000);
            }

            if(rm)
            {
                unsigned int key;
                if(i == 0)
                    key = KEY_PL1_RIGHT;
                else if(i == 1)
                    key = KEY_PL2_RIGHT;
                else if(i == 2)
                    key = KEY_PL3_RIGHT;
                else
                    key = KEY_PL4_RIGHT;

                key &= 0x7fff;
                addkey(key);
            }
            else
            {
                unsigned int key;
                if(i == 0)
                    key = KEY_PL1_RIGHT;
                else if(i == 1)
                    key = KEY_PL2_RIGHT;
                else if(i == 2)
                    key = KEY_PL3_RIGHT;
                else
                    key = KEY_PL4_RIGHT;

                key &= 0x7fff;
                addkey(key | 0x8000);
            }

            if(jm)
            {
                unsigned int key;
                if(i == 0)
                    key = KEY_PL1_JUMP;
                else if(i == 1)
                    key = KEY_PL2_JUMP;
                else if(i == 2)
                    key = KEY_PL3_JUMP;
                else
                    key = KEY_PL4_JUMP;

                key &= 0x7fff;
                addkey(key);
            }
            else
            {
                unsigned int key;
                if(i == 0)
                    key = KEY_PL1_JUMP;
                else if(i == 1)
                    key = KEY_PL2_JUMP;
                else if(i == 2)
                    key = KEY_PL3_JUMP;
                else
                    key = KEY_PL4_JUMP;

                key &= 0x7fff;
                addkey(key | 0x8000);
            }
        }
    }
}

static void steer_players(void)
{
    s16 s1 = 0, s2 = 0;

    cpu_move();
    update_player_actions();

    for (u32 c1 = 0; c1 < JNB_MAX_PLAYERS; c1++) {
        if (player[c1].enabled == 1) {
            if (player[c1].dead_flag == 0) {
                if (player[c1].action_left && player[c1].action_right) {
                    if (player[c1].direction == 0) {
                        if (player[c1].action_right) {
                            player_action_right(c1);
                        }
                    } else {
                        if (player[c1].action_left) {
                            player_action_left(c1);
                        }
                    }
                } else if (player[c1].action_left) {
                    player_action_left(c1);
                } else if (player[c1].action_right) {
                    player_action_right(c1);
                } else if ((!player[c1].action_left) && (!player[c1].action_right)) {
                    u32 below_left, below, below_right;

                    s1 = (player[c1].x >> 16);
                    s2 = (player[c1].y >> 16);
                    below_left = GET_BAN_MAP_XY(s1, s2 + 16);
                    below = GET_BAN_MAP_XY(s1 + 8, s2 + 16);
                    below_right = GET_BAN_MAP_XY(s1 + 15, s2 + 16);
                    if (below == BAN_SOLID || below == BAN_SPRING || (((below_left == BAN_SOLID || below_left == BAN_SPRING) && below_right != BAN_ICE) || (below_left != BAN_ICE && (below_right == BAN_SOLID || below_right == BAN_SPRING)))) {
                        if (player[c1].x_add < 0) {
                            player[c1].x_add += 16384;
                            if (player[c1].x_add > 0)
                                player[c1].x_add = 0;
                        } else {
                            player[c1].x_add -= 16384;
                            if (player[c1].x_add < 0)
                                player[c1].x_add = 0;
                        }
                        if (player[c1].x_add != 0 && GET_BAN_MAP_XY((s1 + 8), (s2 + 16)) == BAN_SOLID)
                            add_object(OBJ_SMOKE, (player[c1].x >> 16) + 2 + rnd(9), (player[c1].y >> 16) + 13 + rnd(5), 0, -16384 - rnd(8192), OBJ_ANIM_SMOKE, 0);
                    }
                    if (player[c1].anim == 1) {
                        player[c1].anim = 0;
                        player[c1].frame = 0;
                        player[c1].frame_tick = 0;
                        player[c1].image = player_anims[player[c1].anim].frame[player[c1].frame].image + player[c1].direction * 9;
                    }
                }
                if (jetpack == 0) {
                    /* no jetpack */
                    if (pogostick == 1 || (player[c1].jump_ready == 1 && player[c1].action_up)) {
                        s1 = (player[c1].x >> 16);
                        s2 = (player[c1].y >> 16);
                        if (s2 < -16)
                            s2 = -16;
                        /* jump */
                        if (GET_BAN_MAP_XY(s1, (s2 + 16)) == BAN_SOLID || GET_BAN_MAP_XY(s1, (s2 + 16)) == BAN_ICE || GET_BAN_MAP_XY((s1 + 15), (s2 + 16)) == BAN_SOLID || GET_BAN_MAP_XY((s1 + 15), (s2 + 16)) == BAN_ICE) {
                            player[c1].y_add = -280000L;
                            player[c1].anim = 2;
                            player[c1].frame = 0;
                            player[c1].frame_tick = 0;
                            player[c1].image = player_anims[player[c1].anim].frame[player[c1].frame].image + player[c1].direction * 9;
                            player[c1].jump_ready = 0;
                            player[c1].jump_abort = 1;
                            if (pogostick == 0)
                                playJumpSfx();
                            else
                                playSpringSfx();
                        }
                        /* jump out of water */
                        if (GET_BAN_MAP_IN_WATER(s1, s2)) {
                            player[c1].y_add = -196608L;
                            player[c1].in_water = 0;
                            player[c1].anim = 2;
                            player[c1].frame = 0;
                            player[c1].frame_tick = 0;
                            player[c1].image = player_anims[player[c1].anim].frame[player[c1].frame].image + player[c1].direction * 9;
                            player[c1].jump_ready = 0;
                            player[c1].jump_abort = 1;
                            if (pogostick == 0)
                                playJumpSfx();
                            else
                                playSpringSfx();
                        }
                    }
                    /* fall down by gravity */
                    if (pogostick == 0 && (!player[c1].action_up)) {
                        player[c1].jump_ready = 1;
                        if (player[c1].in_water == 0 && player[c1].y_add < 0 && player[c1].jump_abort == 1) {
                            if (bunnies_in_space == 0)
                                /* normal gravity */
                                player[c1].y_add += 32768;
                            else
                                /* light gravity */
                                player[c1].y_add += 16384;
                            if (player[c1].y_add > 0)
                                player[c1].y_add = 0;
                        }
                    }
                } else {
                    /* with jetpack */
                    if (player[c1].action_up) {
                        player[c1].y_add -= 16384;
                        if (player[c1].y_add < -400000L)
                            player[c1].y_add = -400000L;
                        if (GET_BAN_MAP_IN_WATER(s1, s2))
                            player[c1].in_water = 0;
                        if (rnd(100) < 50)
                            add_object(OBJ_SMOKE, (player[c1].x >> 16) + 6 + rnd(5), (player[c1].y >> 16) + 10 + rnd(5), 0, 16384 + rnd(8192), OBJ_ANIM_SMOKE, 0);
                    }
                }

                player[c1].x += player[c1].x_add;
                if ((player[c1].x >> 16) < 0) {
                    player[c1].x = 0;
                    player[c1].x_add = 0;
                }
                if ((player[c1].x >> 16) + 15 > 351) {
                    player[c1].x = 336L << 16;
                    player[c1].x_add = 0;
                }
                {
                    if (player[c1].y > 0) {
                        s2 = (player[c1].y >> 16);
                    } else {
                        /* check top line only */
                        s2 = 0;
                    }

                    s1 = (player[c1].x >> 16);
                    if (GET_BAN_MAP_XY(s1, s2) == BAN_SOLID || GET_BAN_MAP_XY(s1, s2) == BAN_ICE || GET_BAN_MAP_XY(s1, s2) == BAN_SPRING || GET_BAN_MAP_XY(s1, (s2 + 15)) == BAN_SOLID || GET_BAN_MAP_XY(s1, (s2 + 15)) == BAN_ICE || GET_BAN_MAP_XY(s1, (s2 + 15)) == BAN_SPRING) {
                        player[c1].x = (((s1 + 16) & 0xfff0)) << 16;
                        player[c1].x_add = 0;
                    }

                    s1 = (player[c1].x >> 16);
                    if (GET_BAN_MAP_XY((s1 + 15), s2) == BAN_SOLID || GET_BAN_MAP_XY((s1 + 15), s2) == BAN_ICE || GET_BAN_MAP_XY((s1 + 15), s2) == BAN_SPRING || GET_BAN_MAP_XY((s1 + 15), (s2 + 15)) == BAN_SOLID || GET_BAN_MAP_XY((s1 + 15), (s2 + 15)) == BAN_ICE || GET_BAN_MAP_XY((s1 + 15), (s2 + 15)) == BAN_SPRING) {
                        player[c1].x = (((s1 + 16) & 0xfff0) - 16) << 16;
                        player[c1].x_add = 0;
                    }
                }

                player[c1].y += player[c1].y_add;

                s1 = (player[c1].x >> 16);
                s2 = (player[c1].y >> 16);
                if (s2 < 0)
                    s2 = 0;
                if (GET_BAN_MAP_XY((s1 + 8), (s2 + 15)) == BAN_SPRING || ((GET_BAN_MAP_XY(s1, (s2 + 15)) == BAN_SPRING && GET_BAN_MAP_XY((s1 + 15), (s2 + 15)) != BAN_SOLID) || (GET_BAN_MAP_XY(s1, (s2 + 15)) != BAN_SOLID && GET_BAN_MAP_XY((s1 + 15), (s2 + 15)) == BAN_SPRING))) {
                    player[c1].y = ((player[c1].y >> 16) & 0xfff0) << 16;
                    player[c1].y_add = -400000L;
                    player[c1].anim = 2;
                    player[c1].frame = 0;
                    player[c1].frame_tick = 0;
                    player[c1].image = player_anims[player[c1].anim].frame[player[c1].frame].image + player[c1].direction * 9;
                    player[c1].jump_ready = 0;
                    player[c1].jump_abort = 0;

                    object_t* currentSpring = &springs[0];
                    u16 currentSpringInd = springsCount;

                    while (currentSpringInd--)
                    {
                        if (GET_BAN_MAP_XY((s1 + 8), (s2 + 15)) == BAN_SPRING) {
                            if ((currentSpring->x >> 20) == ((s1 + 8) >> 4) && (currentSpring->y >> 20) == ((s2 + 15) >> 4)) {
                                currentSpring->frame = 0;
                                currentSpring->ticks = object_anims[OBJ_ANIM_SPRING].frame[currentSpring->frame].ticks;
                                currentSpring->image = object_anims[OBJ_ANIM_SPRING].frame[currentSpring->frame].image;
                                break;
                            }
                        } else {
                            if (GET_BAN_MAP_XY(s1, (s2 + 15)) == BAN_SPRING) {
                                if ((currentSpring->x >> 20) == (s1 >> 4) && (currentSpring->y >> 20) == ((s2 + 15) >> 4)) {
                                    currentSpring->frame = 0;
                                    currentSpring->ticks = object_anims[OBJ_ANIM_SPRING].frame[currentSpring->frame].ticks;
                                    currentSpring->image = object_anims[OBJ_ANIM_SPRING].frame[currentSpring->frame].image;
                                    break;
                                }
                            } else if (GET_BAN_MAP_XY((s1 + 15), (s2 + 15)) == BAN_SPRING) {
                                if ((currentSpring->x >> 20) == ((s1 + 15) >> 4) && (currentSpring->y >> 20) == ((s2 + 15) >> 4)) {
                                    currentSpring->frame = 0;
                                    currentSpring->ticks = object_anims[OBJ_ANIM_SPRING].frame[currentSpring->frame].ticks;
                                    currentSpring->image = object_anims[OBJ_ANIM_SPRING].frame[currentSpring->frame].image;
                                    break;
                                }
                            }
                        }
                        
                        currentSpring++;
                    }
                    playSpringSfx();
                }
                s1 = (player[c1].x >> 16);
                s2 = (player[c1].y >> 16);
                if (s2 < 0)
                    s2 = 0;
                if (GET_BAN_MAP_XY(s1, s2) == BAN_SOLID || GET_BAN_MAP_XY(s1, s2) == BAN_ICE || GET_BAN_MAP_XY(s1, s2) == BAN_SPRING || GET_BAN_MAP_XY((s1 + 15), s2) == BAN_SOLID || GET_BAN_MAP_XY((s1 + 15), s2) == BAN_ICE || GET_BAN_MAP_XY((s1 + 15), s2) == BAN_SPRING) {
                    player[c1].y = (((s2 + 16) & 0xfff0)) << 16;
                    player[c1].y_add = 0;
                    player[c1].anim = 0;
                    player[c1].frame = 0;
                    player[c1].frame_tick = 0;
                    player[c1].image = player_anims[player[c1].anim].frame[player[c1].frame].image + player[c1].direction * 9;
                }
                s1 = (player[c1].x >> 16);
                s2 = (player[c1].y >> 16);
                if (s2 < 0)
                    s2 = 0;
                if (GET_BAN_MAP_XY((s1 + 8), (s2 + 8)) == BAN_WATER) {
                    if (player[c1].in_water == 0) {
                        /* falling into water */
                        player[c1].in_water = 1;
                        player[c1].anim = 4;
                        player[c1].frame = 0;
                        player[c1].frame_tick = 0;
                        player[c1].image = player_anims[player[c1].anim].frame[player[c1].frame].image + player[c1].direction * 9;
                        if (player[c1].y_add >= 32768) {
                            add_object(OBJ_SPLASH, (player[c1].x >> 16) + 8, ((player[c1].y >> 16) & 0xfff0) + 15, 0, 0, OBJ_ANIM_SPLASH, 0);
                            playSplashSfx();
                        }
                    }
                    /* slowly move up to water surface */
                    player[c1].y_add -= 1536;
                    if (player[c1].y_add < 0 && player[c1].anim != 5) {
                        player[c1].anim = 5;
                        player[c1].frame = 0;
                        player[c1].frame_tick = 0;
                        player[c1].image = player_anims[player[c1].anim].frame[player[c1].frame].image + player[c1].direction * 9;
                    }
                    if (player[c1].y_add < -65536L)
                        player[c1].y_add = -65536L;
                    if (player[c1].y_add > 65535L)
                        player[c1].y_add = 65535L;
                    if (GET_BAN_MAP_XY(s1, (s2 + 15)) == BAN_SOLID || GET_BAN_MAP_XY(s1, (s2 + 15)) == BAN_ICE || GET_BAN_MAP_XY((s1 + 15), (s2 + 15)) == BAN_SOLID || GET_BAN_MAP_XY((s1 + 15), (s2 + 15)) == BAN_ICE) {
                        player[c1].y = (((s2 + 16) & 0xfff0) - 16) << 16;
                        player[c1].y_add = 0;
                    }
                } else if (GET_BAN_MAP_XY(s1, (s2 + 15)) == BAN_SOLID || GET_BAN_MAP_XY(s1, (s2 + 15)) == BAN_ICE || GET_BAN_MAP_XY(s1, (s2 + 15)) == BAN_SPRING || GET_BAN_MAP_XY((s1 + 15), (s2 + 15)) == BAN_SOLID || GET_BAN_MAP_XY((s1 + 15), (s2 + 15)) == BAN_ICE || GET_BAN_MAP_XY((s1 + 15), (s2 + 15)) == BAN_SPRING) {
                    player[c1].in_water = 0;
                    player[c1].y = (((s2 + 16) & 0xfff0) - 16) << 16;
                    player[c1].y_add = 0;
                    if (player[c1].anim != 0 && player[c1].anim != 1) {
                        player[c1].anim = 0;
                        player[c1].frame = 0;
                        player[c1].frame_tick = 0;
                        player[c1].image = player_anims[player[c1].anim].frame[player[c1].frame].image + player[c1].direction * 9;
                    }
                } else {
                    if (player[c1].in_water == 0) {
                        if (bunnies_in_space == 0)
                            player[c1].y_add += 12288;
                        else
                            player[c1].y_add += 6144;
                        if (player[c1].y_add > 327680L)
                            player[c1].y_add = 327680L;
                    } else {
                        player[c1].y = (player[c1].y & 0xffff0000) + 0x10000;
                        player[c1].y_add = 0;
                    }
                    player[c1].in_water = 0;
                }
                if (player[c1].y_add > 36864 && player[c1].anim != 3 && player[c1].in_water == 0) {
                    player[c1].anim = 3;
                    player[c1].frame = 0;
                    player[c1].frame_tick = 0;
                    player[c1].image = player_anims[player[c1].anim].frame[player[c1].frame].image + player[c1].direction * 9;
                }

            }

            player[c1].frame_tick++;
            if (player[c1].frame_tick >= player_anims[player[c1].anim].frame[player[c1].frame].ticks) {
                player[c1].frame++;
                if (player[c1].frame >= player_anims[player[c1].anim].num_frames) {
                    if (player[c1].anim != 6)
                        player[c1].frame = player_anims[player[c1].anim].restart_frame;
                    else
                        position_player(c1);
                }
                player[c1].frame_tick = 0;
            }
            player[c1].image = player_anims[player[c1].anim].frame[player[c1].frame].image + player[c1].direction * 9;

        }
    }
}

static void player_kill(u32 c1, u32 c2)
{
	if (player[c1].y_add >= 0) {
        int x = player[c2].x;
        int y = player[c2].y;

        player[c1].y_add = -player[c1].y_add;
        if (player[c1].y_add > -262144L)
            player[c1].y_add = -262144L;
        player[c1].jump_abort = 1;
        player[c2].dead_flag = 1;
        if (player[c2].anim != 6) {
            player[c2].anim = 6;
            player[c2].frame = 0;
            player[c2].frame_tick = 0;
            player[c2].image = player_anims[player[c2].anim].frame[player[c2].frame].image + player[c2].direction * 9;

            u16 rndValuesInd = rnd(EXPLOSION_RANDOM_VALUES_SIZE);
            u16 offset = rnd(EXPLOSION_RANDOM_VALUES_SIZE) + 1;

            s32* values = &explosions_values[rndValuesInd][0];
            add_object(OBJ_FUR, (x >> 16) + 6 + values[0], (y >> 16) + 6 + values[1], values[2], values[3], 0, c2 * 8);
            rndValuesInd = modu(rndValuesInd + offset, EXPLOSION_RANDOM_VALUES_SIZE);
            values = &explosions_values[rndValuesInd][0];
            add_object(OBJ_FUR, (x >> 16) + 6 + values[0], (y >> 16) + 6 + values[1], values[2], values[3], 0, c2 * 8);
            rndValuesInd = modu(rndValuesInd + offset, EXPLOSION_RANDOM_VALUES_SIZE);
            values = &explosions_values[rndValuesInd][0];
            add_object(OBJ_FUR, (x >> 16) + 6 + values[0], (y >> 16) + 6 + values[1], values[2], values[3], 0, c2 * 8);
            rndValuesInd = modu(rndValuesInd + offset, EXPLOSION_RANDOM_VALUES_SIZE);
            values = &explosions_values[rndValuesInd][0];
            add_object(OBJ_FUR, (x >> 16) + 6 + values[0], (y >> 16) + 6 + values[1], values[2], values[3], 0, c2 * 8);
            // rndValuesInd = modu(rndValuesInd + offset, EXPLOSION_RANDOM_VALUES_SIZE);
            // values = &explosions_values[rndValuesInd][0];
            // add_object(OBJ_FUR, (x >> 16) + 6 + values[0], (y >> 16) + 6 + values[1], values[2], values[3], 0, c2 * 8);
            // rndValuesInd = modu(rndValuesInd + offset, EXPLOSION_RANDOM_VALUES_SIZE);
            // values = &explosions_values[rndValuesInd][0];
            // add_object(OBJ_FUR, (x >> 16) + 6 + values[0], (y >> 16) + 6 + values[1], values[2], values[3], 0, c2 * 8);

            rndValuesInd = modu(rndValuesInd + offset, EXPLOSION_RANDOM_VALUES_SIZE);
            values = &explosions_values[rndValuesInd][0];
            add_object(OBJ_FLESH, (x >> 16) + 6 + values[0], (y >> 16) + 6 + values[1], values[2], values[3], 0, 0);
            rndValuesInd = modu(rndValuesInd + offset, EXPLOSION_RANDOM_VALUES_SIZE);
            values = &explosions_values[rndValuesInd][0];
            add_object(OBJ_FLESH, (x >> 16) + 6 + values[0], (y >> 16) + 6 + values[1], values[2], values[3], 0, 0);
            rndValuesInd = modu(rndValuesInd + offset, EXPLOSION_RANDOM_VALUES_SIZE);
            values = &explosions_values[rndValuesInd][0];
            add_object(OBJ_FLESH, (x >> 16) + 6 + values[0], (y >> 16) + 6 + values[1], values[2], values[3], 0, 0);
            rndValuesInd = modu(rndValuesInd + offset, EXPLOSION_RANDOM_VALUES_SIZE);
            values = &explosions_values[rndValuesInd][0];
            add_object(OBJ_FLESH, (x >> 16) + 6 + values[0], (y >> 16) + 6 + values[1], values[2], values[3], 0, 0);
            // rndValuesInd = modu(rndValuesInd + offset, EXPLOSION_RANDOM_VALUES_SIZE);
            // values = &explosions_values[rndValuesInd][0];
            // add_object(OBJ_FLESH, (x >> 16) + 6 + values[0], (y >> 16) + 6 + values[1], values[2], values[3], 0, 0);
            // rndValuesInd = modu(rndValuesInd + offset, EXPLOSION_RANDOM_VALUES_SIZE);
            // values = &explosions_values[rndValuesInd][0];
            // add_object(OBJ_FLESH, (x >> 16) + 6 + values[0], (y >> 16) + 6 + values[1], values[2], values[3], 0, 0);

            rndValuesInd = modu(rndValuesInd + offset, EXPLOSION_RANDOM_VALUES_SIZE);
            values = &explosions_values[rndValuesInd][0];
            add_object(OBJ_FLESH, (x >> 16) + 6 + values[0], (y >> 16) + 6 + values[1], values[2], values[3], 0, 1);
            rndValuesInd = modu(rndValuesInd + offset, EXPLOSION_RANDOM_VALUES_SIZE);
            values = &explosions_values[rndValuesInd][0];
            add_object(OBJ_FLESH, (x >> 16) + 6 + values[0], (y >> 16) + 6 + values[1], values[2], values[3], 0, 1);
            rndValuesInd = modu(rndValuesInd + offset, EXPLOSION_RANDOM_VALUES_SIZE);
            values = &explosions_values[rndValuesInd][0];
            add_object(OBJ_FLESH, (x >> 16) + 6 + values[0], (y >> 16) + 6 + values[1], values[2], values[3], 0, 1);
            rndValuesInd = modu(rndValuesInd + offset, EXPLOSION_RANDOM_VALUES_SIZE);
            values = &explosions_values[rndValuesInd][0];
            add_object(OBJ_FLESH, (x >> 16) + 6 + values[0], (y >> 16) + 6 + values[1], values[2], values[3], 0, 1);
            // rndValuesInd = modu(rndValuesInd + offset, EXPLOSION_RANDOM_VALUES_SIZE);
            // values = &explosions_values[rndValuesInd][0];
            // add_object(OBJ_FLESH, (x >> 16) + 6 + values[0], (y >> 16) + 6 + values[1], values[2], values[3], 0, 1);
            // rndValuesInd = modu(rndValuesInd + offset, EXPLOSION_RANDOM_VALUES_SIZE);
            // values = &explosions_values[rndValuesInd][0];
            // add_object(OBJ_FLESH, (x >> 16) + 6 + values[0], (y >> 16) + 6 + values[1], values[2], values[3], 0, 1);

            rndValuesInd = modu(rndValuesInd + offset, EXPLOSION_RANDOM_VALUES_SIZE);
            values = &explosions_values[rndValuesInd][0];
            add_object(OBJ_FLESH, (x >> 16) + 6 + values[0], (y >> 16) + 6 + values[1], values[2], values[3], 0, 2);
            rndValuesInd = modu(rndValuesInd + offset, EXPLOSION_RANDOM_VALUES_SIZE);
            values = &explosions_values[rndValuesInd][0];
            add_object(OBJ_FLESH, (x >> 16) + 6 + values[0], (y >> 16) + 6 + values[1], values[2], values[3], 0, 2);
            rndValuesInd = modu(rndValuesInd + offset, EXPLOSION_RANDOM_VALUES_SIZE);
            values = &explosions_values[rndValuesInd][0];
            add_object(OBJ_FLESH, (x >> 16) + 6 + values[0], (y >> 16) + 6 + values[1], values[2], values[3], 0, 2);
            rndValuesInd = modu(rndValuesInd + offset, EXPLOSION_RANDOM_VALUES_SIZE);
            values = &explosions_values[rndValuesInd][0];
            add_object(OBJ_FLESH, (x >> 16) + 6 + values[0], (y >> 16) + 6 + values[1], values[2], values[3], 0, 2);
            rndValuesInd = modu(rndValuesInd + offset, EXPLOSION_RANDOM_VALUES_SIZE);
            values = &explosions_values[rndValuesInd][0];
            add_object(OBJ_FLESH, (x >> 16) + 6 + values[0], (y >> 16) + 6 + values[1], values[2], values[3], 0, 2);
            // rndValuesInd = modu(rndValuesInd + offset, EXPLOSION_RANDOM_VALUES_SIZE);
            // values = &explosions_values[rndValuesInd][0];
            // add_object(OBJ_FLESH, (x >> 16) + 6 + values[0], (y >> 16) + 6 + values[1], values[2], values[3], 0, 2);
            // rndValuesInd = modu(rndValuesInd + offset, EXPLOSION_RANDOM_VALUES_SIZE);
            // values = &explosions_values[rndValuesInd][0];
            // add_object(OBJ_FLESH, (x >> 16) + 6 + values[0], (y >> 16) + 6 + values[1], values[2], values[3], 0, 2);
            // rndValuesInd = modu(rndValuesInd + offset, EXPLOSION_RANDOM_VALUES_SIZE);
            // values = &explosions_values[rndValuesInd][0];
            // add_object(OBJ_FLESH, (x >> 16) + 6 + values[0], (y >> 16) + 6 + values[1], values[2], values[3], 0, 2);

            rndValuesInd = modu(rndValuesInd + offset, EXPLOSION_RANDOM_VALUES_SIZE);
            values = &explosions_values[rndValuesInd][0];
            add_object(OBJ_FLESH, (x >> 16) + 6 + values[0], (y >> 16) + 6 + values[1], values[2], values[3], 0, 3);
            rndValuesInd = modu(rndValuesInd + offset, EXPLOSION_RANDOM_VALUES_SIZE);
            values = &explosions_values[rndValuesInd][0];
            add_object(OBJ_FLESH, (x >> 16) + 6 + values[0], (y >> 16) + 6 + values[1], values[2], values[3], 0, 3);
            rndValuesInd = modu(rndValuesInd + offset, EXPLOSION_RANDOM_VALUES_SIZE);
            values = &explosions_values[rndValuesInd][0];
            add_object(OBJ_FLESH, (x >> 16) + 6 + values[0], (y >> 16) + 6 + values[1], values[2], values[3], 0, 3);
            rndValuesInd = modu(rndValuesInd + offset, EXPLOSION_RANDOM_VALUES_SIZE);
            values = &explosions_values[rndValuesInd][0];
            add_object(OBJ_FLESH, (x >> 16) + 6 + values[0], (y >> 16) + 6 + values[1], values[2], values[3], 0, 3);
            rndValuesInd = modu(rndValuesInd + offset, EXPLOSION_RANDOM_VALUES_SIZE);
            values = &explosions_values[rndValuesInd][0];
            add_object(OBJ_FLESH, (x >> 16) + 6 + values[0], (y >> 16) + 6 + values[1], values[2], values[3], 0, 3);
            rndValuesInd = modu(rndValuesInd + offset, EXPLOSION_RANDOM_VALUES_SIZE);
            values = &explosions_values[rndValuesInd][0];
            add_object(OBJ_FLESH, (x >> 16) + 6 + values[0], (y >> 16) + 6 + values[1], values[2], values[3], 0, 3);
            // rndValuesInd = modu(rndValuesInd + offset, EXPLOSION_RANDOM_VALUES_SIZE);
            // values = &explosions_values[rndValuesInd][0];
            // add_object(OBJ_FLESH, (x >> 16) + 6 + values[0], (y >> 16) + 6 + values[1], values[2], values[3], 0, 3);
            // rndValuesInd = modu(rndValuesInd + offset, EXPLOSION_RANDOM_VALUES_SIZE);
            // values = &explosions_values[rndValuesInd][0];
            // add_object(OBJ_FLESH, (x >> 16) + 6 + values[0], (y >> 16) + 6 + values[1], values[2], values[3], 0, 3);
            // rndValuesInd = modu(rndValuesInd + offset, EXPLOSION_RANDOM_VALUES_SIZE);
            // values = &explosions_values[rndValuesInd][0];
            // add_object(OBJ_FLESH, (x >> 16) + 6 + values[0], (y >> 16) + 6 + values[1], values[2], values[3], 0, 3);
            // rndValuesInd = modu(rndValuesInd + offset, EXPLOSION_RANDOM_VALUES_SIZE);
            // values = &explosions_values[rndValuesInd][0];
            // add_object(OBJ_FLESH, (x >> 16) + 6 + values[0], (y >> 16) + 6 + values[1], values[2], values[3], 0, 3);

            playDeathSfx();
            player[c1].bumps++;
            if (player[c1].bumps >= JNB_END_SCORE) {
                endscore_reached = 1;
            }
            player[c1].bumped[c2]++;

            char buf[3];
            uintToStr(modu(player[c1].bumps, 100), buf, 2);
            VDP_drawText(buf, 36, 4 + c1 * 7);
        }
	} else {
		if (player[c2].y_add < 0)
			player[c2].y_add = 0;
	}
}

static void collision_check(void)
{
    u32 c1 = 0, c2 = 0;
    int l1;

    /* collision check */
    for (u32 c3 = 0; c3 < 6; c3++) {
        if (c3 == 0) {
            c1 = 0;
            c2 = 1;
        } else if (c3 == 1) {
            c1 = 0;
            c2 = 2;
        } else if (c3 == 2) {
            c1 = 0;
            c2 = 3;
        } else if (c3 == 3) {
            c1 = 1;
            c2 = 2;
        } else if (c3 == 4) {
            c1 = 1;
            c2 = 3;
        } else if (c3 == 5) {
            c1 = 2;
            c2 = 3;
        }
        if (player[c1].enabled == 1 && player[c2].enabled == 1) {
            if (abs(player[c1].x - player[c2].x) < (12L << 16) && abs(player[c1].y - player[c2].y) < (12L << 16)) {
                if ((abs(player[c1].y - player[c2].y) >> 16) > 5) {
                    if (player[c1].y < player[c2].y) {
                        player_kill(c1,c2);
                    } else {
                        player_kill(c2,c1);
                    }
                } else {
                    if (player[c1].x < player[c2].x) {
                        if (player[c1].x_add > 0)
                            player[c1].x = player[c2].x - (12L << 16);
                        else if (player[c2].x_add < 0)
                            player[c2].x = player[c1].x + (12L << 16);
                        else {
                            player[c1].x -= player[c1].x_add;
                            player[c2].x -= player[c2].x_add;
                        }
                        l1 = player[c2].x_add;
                        player[c2].x_add = player[c1].x_add;
                        player[c1].x_add = l1;
                        if (player[c1].x_add > 0)
                            player[c1].x_add = -player[c1].x_add;
                        if (player[c2].x_add < 0)
                            player[c2].x_add = -player[c2].x_add;
                    } else {
                        if (player[c1].x_add > 0)
                            player[c2].x = player[c1].x - (12L << 16);
                        else if (player[c2].x_add < 0)
                            player[c1].x = player[c2].x + (12L << 16);
                        else {
                            player[c1].x -= player[c1].x_add;
                            player[c2].x -= player[c2].x_add;
                        }
                        l1 = player[c2].x_add;
                        player[c2].x_add = player[c1].x_add;
                        player[c1].x_add = l1;
                        if (player[c1].x_add < 0)
                            player[c1].x_add = -player[c1].x_add;
                        if (player[c2].x_add > 0)
                            player[c2].x_add = -player[c2].x_add;
                    }
                }
            }
        }
    }
}


u16 gameFrame() {

    steer_players();
    collision_check();
    updateRabbitsSprites(FALSE);
    update_objects();

    return endscore_reached;
}
