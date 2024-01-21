#include <genesis.h>

#include "bank.h"
#include "global.h"
#include "objects.h"
#include "rabbits.h"
#include "sfx.h"
#include "resources.h"

static u8 new_game_flag;
static u32 cur_message;
static u16 textTimeOut = 0;
static u16 fadeTextPalette[64];

static const char * const message[] = {
	"Jump 'n Bump "JNB_VERSION,
	"by Brainchild Design in 1998.",

	"Code by Mattias Brynervall.", "",

	"Graphics by Martin Magnusson",
	"and Andreas Brynervall.",

	"Music by Anders Nilsson.", "",

	"Linux port by Chuck Mason.", "",

	"New SDL port by Florian Schulze.", "",

	"Port to SDL2 by Come Chilliet.", "",

    "Demake for Genesis by Philippe Simons", "",

	"http://www.icculus.org/jumpnbump/", "",

	"Ryan C. Gordon made networking",
    "possible again!",

	"Visit our homepage at:",
	"http://www.algonet.se/~mattiasb",

	"Jump 'n Bump is e-mailware.", "",

	"That means you're supposed",
    "to send us an e-mail!",

	"Write for example where you're from",
	"and what you thought about this game.",

	"If you do that, you will greatly",
	"encourage us to make more games for U!",

	"Send your e-mail to:", " matbr656@student.liu.se",
	"Oh, and by the way,", "there're a lot of secrets!",

	"If you can't find them,", "you'd better ask us...",
	"If you'd like to contact", "a specific member of BCD,",

	"these are their e-mail addresses:", "",

	"Andreas Brynervall:"," andreasb@acc.umu.se",
	"Mattias Brynervall:"," matbr656@student.liu.se",
	"Martin Magnusson:"," marma102@student.liu.se",
	"Anders Nilsson:"," equel@swipnet.se"
};

static const u16 menu_map[32][32] = {
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
    {2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 0, 0, 0, 0, 0, 1, 3, 3, 3, 1, 1, 1},
    {2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};

#define NUM_MESSAGES (sizeof(message)/sizeof(char *))

void initMenu() {
    for (u16 c1 = 0; c1 < JNB_MAX_PLAYERS; c1++) {
        player[c1].enabled = 0;
        player[c1].x = (long) rnd(150) << 16;
        player[c1].y = (160L + c1 * 2) << 16;
        player[c1].x_add = 0;
        player[c1].y_add = 0;
        player[c1].direction = rnd(2);
        player[c1].jump_ready = 1;
        player[c1].anim = 0;
        player[c1].frame = 0;
        player[c1].frame_tick = 0;
        player[c1].image = player_anims[0].frame[0].image;

        ai[c1] = FALSE; // set AI to false
    }

    ban_map = (u16 (*)[32])menu_map;

    /* After a game, we have to release the keys, cause AI player
        * can still be using them */
    addkey((KEY_PL1_LEFT & 0x7fff) | 0x8000);
    addkey((KEY_PL2_LEFT & 0x7fff) | 0x8000);
    addkey((KEY_PL3_LEFT & 0x7fff) | 0x8000);
    addkey((KEY_PL4_LEFT & 0x7fff) | 0x8000);

    addkey((KEY_PL1_RIGHT & 0x7fff) | 0x8000);
    addkey((KEY_PL2_RIGHT & 0x7fff) | 0x8000);
    addkey((KEY_PL3_RIGHT & 0x7fff) | 0x8000);
    addkey((KEY_PL4_RIGHT & 0x7fff) | 0x8000);

    addkey((KEY_PL1_JUMP & 0x7fff) | 0x8000);
    addkey((KEY_PL2_JUMP & 0x7fff) | 0x8000);
    addkey((KEY_PL3_JUMP & 0x7fff) | 0x8000);
    addkey((KEY_PL4_JUMP & 0x7fff) | 0x8000);

    clear_objects();

    new_game_flag = 0;
    cur_message = 0;
    textTimeOut = 60;
}

void loadMenu() {
    memcpy(&palette[0], image_menu.palette->data, 16 * 2);
    palette[0] = RGB3_3_3_TO_VDPCOLOR(0, 0, 0);

    VDP_loadTileSet(image_menu.tileset, VDPTilesFilled, DMA);
    VDP_setTileMapEx(BG_B, image_menu.tilemap, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, VDPTilesFilled), 0, 0, 0, 0, 40, 28, DMA);
    VDP_setTileMapEx(BG_A, image_menu.tilemap, TILE_ATTR_FULL(PAL0, TRUE, FALSE, FALSE, VDPTilesFilled), 0, 16, 0, 28, 40, 6, DMA);
    VDPTilesFilled += image_menu.tileset->numTile;

    SYS_doVBlankProcess();

    VDP_loadFont(&font_default, CPU);
    // re-pack memory as VDP_lontFont allocate memory to unpack font
    MEM_pack();

    VDP_setTextPalette(PAL1);
    palette[31] = RGB24_TO_VDPCOLOR(0xffffff);
    memcpy(&fadeTextPalette[0], &palette[0], 64 * 2);
    fadeTextPalette[31] = RGB24_TO_VDPCOLOR(0);
}

void unloadMenu() {
    SYS_disableInts();
    VDP_clearPlane(BG_B, TRUE);
    VDP_clearPlane(BG_A, TRUE);
    SYS_enableInts();

    VDPTilesFilled -= image_menu.tileset->numTile;
    palette[31] = sprite_pallette.data[15];
}

u16 menuFrame() {

    update_player_actions();

    for (s32 c1 = 0; c1 < JNB_MAX_PLAYERS; c1++) {
        if (new_game_flag == 1) {
            if ((player[c1].x >> 16) > (165 + c1 * 2)) {
                if (player[c1].x_add < 0)
                    player[c1].x_add += 16384;
                else
                    player[c1].x_add += 12288;
                if (player[c1].x_add > 98304L)
                    player[c1].x_add = 98304L;
                player[c1].direction = 0;
                if (player[c1].anim == 0) {
                    player[c1].anim = 1;
                    player[c1].frame = 0;
                    player[c1].frame_tick = 0;
                    player[c1].image = player_anims[player[c1].anim].frame[player[c1].frame].image + player[c1].direction * 9;
                }
                player[c1].enabled = 1;
            }
            if (!player[c1].action_up) {
                if (player[c1].y_add < 0) {
                    player[c1].y_add += 32768;
                    if (player[c1].y_add > 0)
                        player[c1].y_add = 0;
                }
            }
            player[c1].y_add += 12288;
            if (player[c1].y_add > 36864 && player[c1].anim != 3) {
                player[c1].anim = 3;
                player[c1].frame = 0;
                player[c1].frame_tick = 0;
                player[c1].image = player_anims[player[c1].anim].frame[player[c1].frame].image + player[c1].direction * 9;
            }
            player[c1].y += player[c1].y_add;
            if ((player[c1].x >> 16) <= (165 + c1 * 2) || (player[c1].x >> 16) >= (208 + c1 * 2)) {
                if ((player[c1].y >> 16) > (160 + c1 * 2)) {
                    player[c1].y = (160L + c1 * 2) << 16;
                    player[c1].y_add = 0;
                    if (player[c1].anim != 0 && player[c1].anim != 1) {
                        player[c1].anim = 0;
                        player[c1].frame = 0;
                        player[c1].frame_tick = 0;
                        player[c1].image = player_anims[player[c1].anim].frame[player[c1].frame].image + player[c1].direction * 9;
                    }
                }
            } else {
                if ((player[c1].y >> 16) > (138 + c1 * 2)) {
                    player[c1].y = (138L + c1 * 2) << 16;
                    player[c1].y_add = 0;
                    if (player[c1].anim != 0 && player[c1].anim != 1) {
                        player[c1].anim = 0;
                        player[c1].frame = 0;
                        player[c1].frame_tick = 0;
                        player[c1].image = player_anims[player[c1].anim].frame[player[c1].frame].image + player[c1].direction * 9;
                    }
                    if (!player[c1].action_up)
                        player[c1].jump_ready = 1;
                }
            }
            player[c1].x += player[c1].x_add;
            if ((player[c1].y >> 16) > (138 + c1 * 2)) {
                if ((player[c1].x >> 16) > (165 + c1 * 2) && (player[c1].x >> 16) < (190 + c1 * 2)) {
                    player[c1].x = (165L + c1 * 2) << 16;
                    player[c1].x_add = 0;
                }
                if ((player[c1].x >> 16) > (190 + c1 * 2) && (player[c1].x >> 16) < (208 + c1 * 2)) {
                    player[c1].x = (208L + c1 * 2) << 16;
                    player[c1].x_add = 0;
                }
            }
        } else {
            if (player[c1].action_left && player[c1].action_right) {
                if (player[c1].direction == 1) {
                    if ((player[c1].x >> 16) <= (165 + c1 * 2) || (player[c1].x >> 16) >= (208 + c1 * 2)) {
                        if (player[c1].x_add > 0) {
                            player[c1].x_add -= 16384;
                            if ((player[c1].y >> 16) >= (160 + c1 * 2))
                                add_object(OBJ_SMOKE, (player[c1].x >> 16) + 2 + rnd(9), (player[c1].y >> 16) + 13 + rnd(5), 0, -16384 - rnd(8192), OBJ_ANIM_SMOKE, 0);
                        } else
                            player[c1].x_add -= 12288;
                    }
                    if ((player[c1].x >> 16) > (165 + c1 * 2) && (player[c1].x >> 16) < (208 + c1 * 2)) {
                        if (player[c1].x_add > 0) {
                            player[c1].x_add -= 16384;
                            if ((player[c1].y >> 16) >= (138 + c1 * 2))
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
                } else {
                    if ((player[c1].x >> 16) <= (165 + c1 * 2) || (player[c1].x >> 16) >= (208 + c1 * 2)) {
                        if (player[c1].x_add < 0) {
                            player[c1].x_add += 16384;
                            if ((player[c1].y >> 16) >= (160 + c1 * 2))
                                add_object(OBJ_SMOKE, (player[c1].x >> 16) + 2 + rnd(9), (player[c1].y >> 16) + 13 + rnd(5), 0, -16384 - rnd(8192), OBJ_ANIM_SMOKE, 0);
                        } else
                            player[c1].x_add += 12288;
                    }
                    if ((player[c1].x >> 16) > (165 + c1 * 2) && (player[c1].x >> 16) < (208 + c1 * 2)) {
                        if (player[c1].x_add < 0) {
                            player[c1].x_add += 16384;
                            if ((player[c1].y >> 16) >= (138 + c1 * 2))
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
            } else if (player[c1].action_left) {
                if ((player[c1].x >> 16) <= (165 + c1 * 2) || (player[c1].x >> 16) >= (208 + c1 * 2)) {
                    if (player[c1].x_add > 0) {
                        player[c1].x_add -= 16384;
                        if ((player[c1].y >> 16) >= (160 + c1 * 2))
                            add_object(OBJ_SMOKE, (player[c1].x >> 16) + 2 + rnd(9), (player[c1].y >> 16) + 13 + rnd(5), 0, -16384 - rnd(8192), OBJ_ANIM_SMOKE, 0);
                    } else
                        player[c1].x_add -= 12288;
                }
                if ((player[c1].x >> 16) > (165 + c1 * 2) && (player[c1].x >> 16) < (208 + c1 * 2)) {
                    if (player[c1].x_add > 0) {
                        player[c1].x_add -= 16384;
                        if ((player[c1].y >> 16) >= (138 + c1 * 2))
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
            } else if (player[c1].action_right) {
                if ((player[c1].x >> 16) <= (165 + c1 * 2) || (player[c1].x >> 16) >= (208 + c1 * 2)) {
                    if (player[c1].x_add < 0) {
                        player[c1].x_add += 16384;
                        if ((player[c1].y >> 16) >= (160 + c1 * 2))
                            add_object(OBJ_SMOKE, (player[c1].x >> 16) + 2 + rnd(9), (player[c1].y >> 16) + 13 + rnd(5), 0, -16384 - rnd(8192), OBJ_ANIM_SMOKE, 0);
                    } else
                        player[c1].x_add += 12288;
                }
                if ((player[c1].x >> 16) > (165 + c1 * 2) && (player[c1].x >> 16) < (208 + c1 * 2)) {
                    if (player[c1].x_add < 0) {
                        player[c1].x_add += 16384;
                        if ((player[c1].y >> 16) >= (138 + c1 * 2))
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
            } else {
                if (((player[c1].x >> 16) <= (165 + c1 * 2) || (player[c1].x >> 16) >= (208 + c1 * 2)) && (player[c1].y >> 16) >= (160 + c1 * 2)) {
                    if (player[c1].x_add < 0) {
                        player[c1].x_add += 16384;
                        if (player[c1].x_add > 0)
                            player[c1].x_add = 0;
                        add_object(OBJ_SMOKE, (player[c1].x >> 16) + 2 + rnd(9), (player[c1].y >> 16) + 13 + rnd(5), 0, -16384 - rnd(8192), OBJ_ANIM_SMOKE, 0);
                    } else if (player[c1].x_add > 0) {
                        player[c1].x_add -= 16384;
                        if (player[c1].x_add < 0)
                            player[c1].x_add = 0;
                        add_object(OBJ_SMOKE, (player[c1].x >> 16) + 2 + rnd(9), (player[c1].y >> 16) + 13 + rnd(5), 0, -16384 - rnd(8192), OBJ_ANIM_SMOKE, 0);
                    }
                }
                if ((((player[c1].x >> 16) > (165 + c1 * 2) && (player[c1].x >> 16) < (208 + c1 * 2)) && (player[c1].y >> 16) >= (138 + c1 * 2))) {
                    if (player[c1].x_add < 0) {
                        player[c1].x_add += 16384;
                        if (player[c1].x_add > 0)
                            player[c1].x_add = 0;
                        add_object(OBJ_SMOKE, (player[c1].x >> 16) + 2 + rnd(9), (player[c1].y >> 16) + 13 + rnd(5), 0, -16384 - rnd(8192), OBJ_ANIM_SMOKE, 0);
                    } else if (player[c1].x_add > 0) {
                        player[c1].x_add -= 16384;
                        if (player[c1].x_add < 0)
                            player[c1].x_add = 0;
                        add_object(OBJ_SMOKE, (player[c1].x >> 16) + 2 + rnd(9), (player[c1].y >> 16) + 13 + rnd(5), 0, -16384 - rnd(8192), OBJ_ANIM_SMOKE, 0);
                    }
                }
                if (player[c1].anim == 1) {
                    player[c1].anim = 0;
                    player[c1].frame = 0;
                    player[c1].frame_tick = 0;
                    player[c1].image = player_anims[player[c1].anim].frame[player[c1].frame].image + player[c1].direction * 9;
                }
            }
            if ((player[c1].jump_ready == 1) && player[c1].action_up) {
                if ((player[c1].x >> 16) <= (165 + c1 * 2) || (player[c1].x >> 16) >= (208 + c1 * 2)) {
                    if ((player[c1].y >> 16) >= (160 + c1 * 2)) {
                        player[c1].y_add = -280000L;
                        player[c1].anim = 2;
                        player[c1].frame = 0;
                        player[c1].frame_tick = 0;
                        player[c1].image = player_anims[player[c1].anim].frame[player[c1].frame].image + player[c1].direction * 9;
                        player[c1].jump_ready = 0;
                        playJumpSfx();
                    }
                } else {
                    if ((player[c1].y >> 16) >= (138 + c1 * 2)) {
                        player[c1].y_add = -280000L;
                        player[c1].anim = 2;
                        player[c1].frame = 0;
                        player[c1].frame_tick = 0;
                        player[c1].image = player_anims[player[c1].anim].frame[player[c1].frame].image + player[c1].direction * 9;
                        player[c1].jump_ready = 0;
                        playJumpSfx();
                    }
                }
            }
            if (!player[c1].action_up) {
                if (player[c1].y_add < 0) {
                    player[c1].y_add += 32768;
                    if (player[c1].y_add > 0)
                        player[c1].y_add = 0;
                }
            }
            if (!player[c1].action_up)
                player[c1].jump_ready = 1;
            player[c1].y_add += 12288;
            if (player[c1].y_add > 36864 && player[c1].anim != 3) {
                player[c1].anim = 3;
                player[c1].frame = 0;
                player[c1].frame_tick = 0;
                player[c1].image = player_anims[player[c1].anim].frame[player[c1].frame].image + player[c1].direction * 9;
            }
            player[c1].y += player[c1].y_add;
            if ((player[c1].x >> 16) <= (165 + c1 * 2) || (player[c1].x >> 16) >= (208 + c1 * 2)) {
                if ((player[c1].y >> 16) > (160 + c1 * 2)) {
                    player[c1].y = (160L + c1 * 2) << 16;
                    player[c1].y_add = 0;
                    if (player[c1].anim != 0 && player[c1].anim != 1) {
                        player[c1].anim = 0;
                        player[c1].frame = 0;
                        player[c1].frame_tick = 0;
                        player[c1].image = player_anims[player[c1].anim].frame[player[c1].frame].image + player[c1].direction * 9;
                    }
                }
            } else {
                if ((player[c1].y >> 16) > (138 + c1 * 2)) {
                    player[c1].y = (138L + c1 * 2) << 16;
                    player[c1].y_add = 0;
                    if (player[c1].anim != 0 && player[c1].anim != 1) {
                        player[c1].anim = 0;
                        player[c1].frame = 0;
                        player[c1].frame_tick = 0;
                        player[c1].image = player_anims[player[c1].anim].frame[player[c1].frame].image + player[c1].direction * 9;
                    }
                }
            }
            player[c1].x += player[c1].x_add;
            if ((player[c1].x >> 16) < 0) {
                player[c1].x = 0;
                player[c1].x_add = 0;
            }
            if ((player[c1].x >> 16) > JNB_WIDTH) {
                new_game_flag = 1;
            }
            if ((player[c1].y >> 16) > (138 + c1 * 2)) {
                if ((player[c1].x >> 16) > (165 + c1 * 2) && (player[c1].x >> 16) < (190 + c1 * 2)) {
                    player[c1].x = (165L + c1 * 2) << 16;
                    player[c1].x_add = 0;
                }
                if ((player[c1].x >> 16) > (190 + c1 * 2) && (player[c1].x >> 16) < (208 + c1 * 2)) {
                    player[c1].x = (208L + c1 * 2) << 16;
                    player[c1].x_add = 0;
                }
            }
        }
        player[c1].frame_tick++;
        if (player[c1].frame_tick >= player_anims[player[c1].anim].frame[player[c1].frame].ticks) {
            player[c1].frame++;
            if (player[c1].frame >= player_anims[player[c1].anim].num_frames)
                player[c1].frame = player_anims[player[c1].anim].restart_frame;
            player[c1].frame_tick = 0;
        }
        player[c1].image = player_anims[player[c1].anim].frame[player[c1].frame].image + player[c1].direction * 9;
    }

    updateRabbitsSprites(TRUE);
    update_objects();

    if (textTimeOut == 0) {
        VDP_clearTextLine(23);
        VDP_clearTextLine(24);
        VDP_drawText(message[cur_message], 1, 23);
        VDP_drawText(message[cur_message + 1], 1, 24);

        cur_message += 2;
        if (cur_message >= NUM_MESSAGES)
            cur_message = 0;

        PAL_fade(0, 63, fadeTextPalette, palette, 20, TRUE);
        textTimeOut = 180;
    }
    if (textTimeOut == 30) {
        PAL_fade(0, 63, palette, fadeTextPalette, 20, TRUE);
    }

    textTimeOut--;
    
    return new_game_flag;
}
