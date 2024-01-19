#include <genesis.h>

#include "bank.h"
#include "global.h"
#include "objects.h"
#include "resources.h"

static Bank* objects_bank;

object_t springs[8];
u16 springsCount;

#define RANDOM_SPAWNER_SIZE  256
static bool spawner[RANDOM_SPAWNER_SIZE];
static u16 randomSpawnerInd = 0;
static u16 randomSpawnerOffset = 1;

const object_anim_t object_anims[8] = {
    {
        6, 0, // OBJ_ANIM_SPRING (spring.bmp)
        { {0, 3}, {1, 3}, {2, 3}, {3, 3}, {4, 3}, {5, 3}, {0, 0}, {0, 0},{0, 0},{0, 0} }
    }, {
        9, 0, // OBJ_ANIM_SPLASH (splash.bmp)
        { {0, 2}, {1, 2}, {2, 2}, {3, 2}, {4, 2}, {5, 2}, {6, 2}, {7, 2}, {8, 2}, {0, 0} }
    }, {
        5, 0, // OBJ_ANIM_SMOKE (smoke.bmp)
        { {0, 3}, {1, 3}, {1, 3}, {2, 3}, {3, 3}, {4, 3}, {0, 0}, {0, 0}, {0, 0},{0, 0} }
    }, {
        10, 0, // OBJ_ANIM_YEL_BUTFLY_RIGHT
        { {0, 2}, {1, 2}, {2, 2}, {3, 2}, {4, 2}, {5, 2}, {4, 2}, {3, 2}, {2, 2}, {1, 2} }
    }, {
        10, 0, // OBJ_ANIM_YEL_BUTFLY_LEFT
        { {6, 2}, {7, 2}, {8, 2}, {9, 2}, {10, 2}, {11, 2}, {10, 2}, {9, 2}, {8, 2}, {7, 2} }
    }, {
        10, 0, // OBJ_ANIM_PINK_BUTFLY_RIGHT
        { {0, 2}, {1, 2}, {2, 2}, {3, 2}, {4, 2}, {5, 2}, {4, 2}, {3, 2}, {2, 2}, {1, 2} }
    }, {
        10, 0, // OBJ_ANIM_PINK_BUTFLY_LEFT
        { {6, 2}, {7, 2}, {8, 2}, {9, 2}, {10, 2}, {11, 2}, {10, 2}, {9, 2}, {8, 2}, {7, 2} }
    }, {
        4, 0, // OBJ_ANIM_FLESH_TRACE
        { {0, 4}, {1, 4}, {2, 4}, {3, 4}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0} }
    }
};

static const struct {
    s16 x, y;
} objects_frame_hs[4][32] = {
    { {0, 3}, {0, 4}, {0, 8}, {0, 6}, {0, 4}, {0, 3} }, // OBJ_SPRING
    { {4, 4}, {6, 6}, {8, 8}, {10, 9}, {11, 10}, {12, 9}, {13, 7}, {14, 4}, {15, 1} }, // OBJ_SPLASH
    { {0, 0}, {1, 1}, {2, 2}, {2, 2}, {2, 2} }, // OBJ_SMOKE
    { {4, 3}, {5, 2}, {5, 1}, {5, 0}, {5, 0}, {4, 0}, {2, 3}, {2, 2}, {3, 1}, {2, 0}, {2, 0}, {2, 0} } // OBJ_YEL_BUTFLY && OBJ_PINK_BUTFLY
};

static u16 spritesTileInd[8][32];

void init_objects() {
    objects_bank = BANK_create(NUM_OBJECTS, sizeof(object_t));
    for(u16 i = 0; i < RANDOM_SPAWNER_SIZE; i++)
        spawner[i] = rnd(100) < 30;
}

void clear_objects() {
    BANK_clear(objects_bank);
}

void load_objects_sprites() {
    // load objects palette
    memcpy(&palette[16], sprite_pallette.data, 16 * 2);

    for(u16 c1 = 0; c1 < spring_sprite.animations[0]->numFrame; c1++) {
        spritesTileInd[OBJ_SPRING][c1] = VDPTilesFilled;
        const AnimationFrame* animationFrame = spring_sprite.animations[0]->frames[c1];
        const u32* tiles   = animationFrame->tileset->tiles;
        const u16  numTile = animationFrame->tileset->numTile;
        VDP_loadTileData(tiles, VDPTilesFilled, numTile, DMA);
        VDPTilesFilled += numTile;
    }

    for(u16 c1 = 0; c1 < splash_sprite.animations[0]->numFrame; c1++) {
        spritesTileInd[OBJ_SPLASH][c1] = VDPTilesFilled;
        const AnimationFrame* animationFrame = splash_sprite.animations[0]->frames[c1];
        const u32* tiles   = animationFrame->tileset->tiles;
        const u16  numTile = animationFrame->tileset->numTile;
        VDP_loadTileData(tiles, VDPTilesFilled, numTile, DMA);
        VDPTilesFilled += numTile;
    }

    for(u16 c1 = 0; c1 < smoke_sprite.animations[0]->numFrame; c1++) {
        spritesTileInd[OBJ_SMOKE][c1] = VDPTilesFilled;
        const AnimationFrame* animationFrame = smoke_sprite.animations[0]->frames[c1];
        const u32* tiles   = animationFrame->tileset->tiles;
        const u16  numTile = animationFrame->tileset->numTile;
        VDP_loadTileData(tiles, VDPTilesFilled, numTile, DMA);
        VDPTilesFilled += numTile;
    }

    for(u16 c1 = 0; c1 < yel_butterfly_sprite.animations[0]->numFrame; c1++) {
        spritesTileInd[OBJ_YEL_BUTFLY][c1] = VDPTilesFilled;
        const AnimationFrame* animationFrame = yel_butterfly_sprite.animations[0]->frames[c1];
        const u32* tiles   = animationFrame->tileset->tiles;
        const u16  numTile = animationFrame->tileset->numTile;
        VDP_loadTileData(tiles, VDPTilesFilled, numTile, DMA);
        VDPTilesFilled += numTile;
    }

    for(u16 c1 = 0; c1 < pink_butterfly_sprite.animations[0]->numFrame; c1++) {
        spritesTileInd[OBJ_PINK_BUTFLY][c1] = VDPTilesFilled;
        const AnimationFrame* animationFrame = pink_butterfly_sprite.animations[0]->frames[c1];
        const u32* tiles   = animationFrame->tileset->tiles;
        const u16  numTile = animationFrame->tileset->numTile;
        VDP_loadTileData(tiles, VDPTilesFilled, numTile, DMA);
        VDPTilesFilled += numTile;
    }

    for(u16 c1 = 0; c1 < fur_sprite.animations[0]->numFrame; c1++) {
        spritesTileInd[OBJ_FUR][c1] = VDPTilesFilled;
        const AnimationFrame* animationFrame = fur_sprite.animations[0]->frames[c1];
        const u32* tiles   = animationFrame->tileset->tiles;
        const u16  numTile = animationFrame->tileset->numTile;
        VDP_loadTileData(tiles, VDPTilesFilled, numTile, DMA);
        VDPTilesFilled += numTile;
    }

    for(u16 c1 = 0; c1 < flesh_sprite.animations[0]->numFrame; c1++) {
        spritesTileInd[OBJ_FLESH][c1] = VDPTilesFilled;
        spritesTileInd[OBJ_FLESH_TRACE][c1] = VDPTilesFilled;
        const AnimationFrame* animationFrame = flesh_sprite.animations[0]->frames[c1];
        const u32* tiles   = animationFrame->tileset->tiles;
        const u16  numTile = animationFrame->tileset->numTile;
        VDP_loadTileData(tiles, VDPTilesFilled, numTile, DMA);
        VDPTilesFilled += numTile;
    }
}

static void update_splash(void* this, const bool _unused) {
    object_t* currentObject = (object_t*) this;

    currentObject->ticks--;
    if (currentObject->ticks <= 0) {
        currentObject->frame++;
        if (currentObject->frame >= object_anims[OBJ_ANIM_SPLASH].num_frames) {
            BANK_free(objects_bank, (void*)currentObject);
            return;
        } else {
            currentObject->ticks = object_anims[OBJ_ANIM_SPLASH].frame[currentObject->frame].ticks;
            currentObject->image = object_anims[OBJ_ANIM_SPLASH].frame[currentObject->frame].image;
        }
    }

    u16 imageInd = currentObject->image;
    s16 x = divu(mulu((currentObject->x >> 16), 320), JNB_WIDTH) - objects_frame_hs[OBJ_SPLASH][imageInd].x;
    s16 y = divu(mulu((currentObject->y >> 16), 224), JNB_HEIGHT) - objects_frame_hs[OBJ_SPLASH][imageInd].y;

    vdpSprite->y = y + 0x80;
    vdpSprite->x = x + 0x80;
    vdpSprite->size = SPRITE_SIZE(3, 2);
    vdpSprite->link = vdpSpriteInd++;
    vdpSprite->attribut = TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, spritesTileInd[OBJ_SPLASH][imageInd]);
    vdpSprite++;
}

static void update_smoke(void* this, const bool _unused) {
    object_t* currentObject = (object_t*) this;

    currentObject->x += currentObject->x_add;
    currentObject->y += currentObject->y_add;
    currentObject->ticks--;
    if (currentObject->ticks <= 0) {
        currentObject->frame++;
        if (currentObject->frame >= object_anims[OBJ_ANIM_SMOKE].num_frames) {
            BANK_free(objects_bank, (void*)currentObject);
            return;
        } else {
            currentObject->ticks = object_anims[OBJ_ANIM_SMOKE].frame[currentObject->frame].ticks;
            currentObject->image = object_anims[OBJ_ANIM_SMOKE].frame[currentObject->frame].image;
        }
    }

    u16 imageInd = currentObject->image;
    s16 x = divu(mulu((currentObject->x >> 16), 320), JNB_WIDTH) - objects_frame_hs[OBJ_SMOKE][imageInd].x;
    s16 y = divu(mulu((currentObject->y >> 16), 224), JNB_HEIGHT) - objects_frame_hs[OBJ_SMOKE][imageInd].y;

    vdpSprite->y = y + 0x80;
    vdpSprite->x = x + 0x80;
    vdpSprite->size = SPRITE_SIZE(1, 1);
    vdpSprite->link = vdpSpriteInd++;
    vdpSprite->attribut = TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, spritesTileInd[OBJ_SMOKE][imageInd]);
    vdpSprite++;
}

static void update_butfly(void* this, const bool _unused) {
    object_t* currentObject = (object_t*) this;

    currentObject->x_acc += rnd(128) - 64;
    if (currentObject->x_acc < -1024)
        currentObject->x_acc = -1024;
    if (currentObject->x_acc > 1024)
        currentObject->x_acc = 1024;
    currentObject->x_add += currentObject->x_acc;
    if (currentObject->x_add < -32768)
        currentObject->x_add = -32768;
    if (currentObject->x_add > 32768)
        currentObject->x_add = 32768;
    currentObject->x += currentObject->x_add;
    if ((currentObject->x >> 16) < 16) {
        currentObject->x = 16 << 16;
        currentObject->x_add = -currentObject->x_add >> 2;
        currentObject->x_acc = 0;
    } else if ((currentObject->x >> 16) > 350) {
        currentObject->x = 350 << 16;
        currentObject->x_add = -currentObject->x_add >> 2;
        currentObject->x_acc = 0;
    }
    if (ban_map[currentObject->y >> 20][currentObject->x >> 20] != BAN_VOID) {
        if (currentObject->x_add < 0) {
            currentObject->x = (((currentObject->x >> 16) + 16) & 0xfff0) << 16;
        } else {
            currentObject->x = ((((currentObject->x >> 16) - 16) & 0xfff0) + 15) << 16;
        }
        currentObject->x_add = -currentObject->x_add >> 2;
        currentObject->x_acc = 0;
    }
    currentObject->y_acc += rnd(64) - 32;
    if (currentObject->y_acc < -1024)
        currentObject->y_acc = -1024;
    if (currentObject->y_acc > 1024)
        currentObject->y_acc = 1024;
    currentObject->y_add += currentObject->y_acc;
    if (currentObject->y_add < -32768)
        currentObject->y_add = -32768;
    if (currentObject->y_add > 32768)
        currentObject->y_add = 32768;
    currentObject->y += currentObject->y_add;
    if ((currentObject->y >> 16) < 0) {
        currentObject->y = 0;
        currentObject->y_add = -currentObject->y_add >> 2;
        currentObject->y_acc = 0;
    } else if ((currentObject->y >> 16) > 255) {
        currentObject->y = 255 << 16;
        currentObject->y_add = -currentObject->y_add >> 2;
        currentObject->y_acc = 0;
    }
    if (ban_map[currentObject->y >> 20][currentObject->x >> 20] != BAN_VOID) {
        if (currentObject->y_add < 0) {
            currentObject->y = (((currentObject->y >> 16) + 16) & 0xfff0) << 16;
        } else {
            currentObject->y = ((((currentObject->y >> 16) - 16) & 0xfff0) + 15) << 16;
        }
        currentObject->y_add = -currentObject->y_add >> 2;
        currentObject->y_acc = 0;
    }
    if (currentObject->type == OBJ_YEL_BUTFLY) {
        if (currentObject->x_add < 0 && currentObject->anim != OBJ_ANIM_YEL_BUTFLY_LEFT) {
            currentObject->anim = OBJ_ANIM_YEL_BUTFLY_LEFT;
            currentObject->frame = 0;
            currentObject->ticks = object_anims[currentObject->anim].frame[currentObject->frame].ticks;
            currentObject->image = object_anims[currentObject->anim].frame[currentObject->frame].image;
        } else if (currentObject->x_add > 0 && currentObject->anim != OBJ_ANIM_YEL_BUTFLY_RIGHT) {
            currentObject->anim = OBJ_ANIM_YEL_BUTFLY_RIGHT;
            currentObject->frame = 0;
            currentObject->ticks = object_anims[currentObject->anim].frame[currentObject->frame].ticks;
            currentObject->image = object_anims[currentObject->anim].frame[currentObject->frame].image;
        }
    } else {
        if (currentObject->x_add < 0 && currentObject->anim != OBJ_ANIM_PINK_BUTFLY_LEFT) {
            currentObject->anim = OBJ_ANIM_PINK_BUTFLY_LEFT;
            currentObject->frame = 0;
            currentObject->ticks = object_anims[currentObject->anim].frame[currentObject->frame].ticks;
            currentObject->image = object_anims[currentObject->anim].frame[currentObject->frame].image;
        } else if (currentObject->x_add > 0 && currentObject->anim != OBJ_ANIM_PINK_BUTFLY_RIGHT) {
            currentObject->anim = OBJ_ANIM_PINK_BUTFLY_RIGHT;
            currentObject->frame = 0;
            currentObject->ticks = object_anims[currentObject->anim].frame[currentObject->frame].ticks;
            currentObject->image = object_anims[currentObject->anim].frame[currentObject->frame].image;
        }
    }
    
    currentObject->ticks--;
    if (currentObject->ticks <= 0) {
        currentObject->frame++;
        if (currentObject->frame >= object_anims[currentObject->anim].num_frames)
            currentObject->frame = object_anims[currentObject->anim].restart_frame;
        else {
            currentObject->ticks = object_anims[currentObject->anim].frame[currentObject->frame].ticks;
            currentObject->image = object_anims[currentObject->anim].frame[currentObject->frame].image;
        }
    }

    u16 imageInd = currentObject->image;
    s16 x = divu(mulu((currentObject->x >> 16), 320), JNB_WIDTH) - objects_frame_hs[OBJ_YEL_BUTFLY][imageInd].x;
    s16 y = divu(mulu((currentObject->y >> 16), 224), JNB_HEIGHT) - objects_frame_hs[OBJ_YEL_BUTFLY][imageInd].y;

    vdpSprite->y = y + 0x80;
    vdpSprite->x = x + 0x80;
    vdpSprite->size = SPRITE_SIZE(1, 1);
    vdpSprite->link = vdpSpriteInd++;
    vdpSprite->attribut = TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, spritesTileInd[currentObject->type][imageInd]);
    vdpSprite++;
}

static void update_fur(void* this, const bool partial_update) {
    object_t* currentObject = (object_t*) this;

    if (partial_update) {
        currentObject->x += currentObject->x_add;
        currentObject->y += currentObject->y_add;

        u16 imageInd = currentObject->frame + modu(currentObject->x >> 16, 8);
        s16 x = divu(mulu((currentObject->x >> 16), 320), JNB_WIDTH);
        s16 y = divu(mulu((currentObject->y >> 16), 224), JNB_HEIGHT);

        vdpSprite->y = y + 0x80;
        vdpSprite->x = x + 0x80;
        vdpSprite->size = SPRITE_SIZE(1, 1);
        vdpSprite->link = vdpSpriteInd++;
        vdpSprite->attribut = TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, spritesTileInd[OBJ_FUR][imageInd]);
        vdpSprite++;

        return;
    }

    u16 tile = ban_map[currentObject->y >> 20][currentObject->x >> 20];
    if (tile == BAN_VOID) {
        currentObject->y_add += 3072;
        if (currentObject->y_add > 196608L)
            currentObject->y_add = 196608L;
    } else if (tile == BAN_WATER) {
        if (currentObject->x_add < 0) {
            if (currentObject->x_add < -65536L)
                currentObject->x_add = -65536L;
            currentObject->x_add += 1024;
            if (currentObject->x_add > 0)
                currentObject->x_add = 0;
        } else {
            if (currentObject->x_add > 65536L)
                currentObject->x_add = 65536L;
            currentObject->x_add -= 1024;
            if (currentObject->x_add < 0)
                currentObject->x_add = 0;
        }
        currentObject->y_add += 1024;
        if (currentObject->y_add < -65536L)
            currentObject->y_add = -65536L;
        if (currentObject->y_add > 65536L)
            currentObject->y_add = 65536L;
    }
    currentObject->x += currentObject->x_add;

    if ((currentObject->y >> 16) > 0) {
        tile = ban_map[currentObject->y >> 20][currentObject->x >> 20];
        if (tile == BAN_SOLID || tile == BAN_ICE) {
            if (currentObject->x_add < 0) {
                currentObject->x = (((currentObject->x >> 16) + 16) & 0xfff0) << 16;
                currentObject->x_add = -currentObject->x_add >> 2;
            } else {
                currentObject->x = ((((currentObject->x >> 16) - 16) & 0xfff0) + 15) << 16;
                currentObject->x_add = -currentObject->x_add >> 2;
            }
        }
    }
    currentObject->y += currentObject->y_add;

    if ((currentObject->x >> 16) < -5 || (currentObject->x >> 16) > 405 || (currentObject->y >> 16) > 260) {
        BANK_free(objects_bank, (void*)currentObject);
        return;
    }

    if (spawner[randomSpawnerInd])
        add_object(OBJ_FLESH_TRACE, currentObject->x >> 16, currentObject->y >> 16, 0, 0, OBJ_ANIM_FLESH_TRACE, 0);
    randomSpawnerInd = modu(randomSpawnerInd + randomSpawnerOffset, RANDOM_SPAWNER_SIZE);
    
    if ((currentObject->y >> 16) > 0) {
        tile = ban_map[currentObject->y >> 20][currentObject->x >> 20];
        if (tile != BAN_VOID) {
            if (currentObject->y_add < 0) {
                if (tile != BAN_WATER) {
                    currentObject->y = (((currentObject->y >> 16) + 16) & 0xfff0) << 16;
                    currentObject->x_add >>= 2;
                    currentObject->y_add = -currentObject->y_add >> 2;
                }
            } else {
                if (tile == BAN_SOLID) {
                    if (currentObject->y_add > 131072L) {
                        currentObject->y = ((((currentObject->y >> 16) - 16) & 0xfff0) + 15) << 16;
                        currentObject->x_add >>= 2;
                        currentObject->y_add = -currentObject->y_add >> 2;
                    } else {
                        BANK_free(objects_bank, (void*)currentObject);
                        return;
                    }
                } else if (tile == BAN_ICE) {
                    currentObject->y = ((((currentObject->y >> 16) - 16) & 0xfff0) + 15) << 16;
                    if (currentObject->y_add > 131072L)
                        currentObject->y_add = -currentObject->y_add >> 2;
                    else
                        currentObject->y_add = 0;
                }
            }
        }
    }

    if (currentObject->x_add < 0 && currentObject->x_add > -16384)
        currentObject->x_add = -16384;
    if (currentObject->x_add > 0 && currentObject->x_add < 16384)
        currentObject->x_add = 16384;

    u16 imageInd = currentObject->frame + modu(currentObject->x >> 16, 8);
    s16 x = divu(mulu((currentObject->x >> 16), 320), JNB_WIDTH);
    s16 y = divu(mulu((currentObject->y >> 16), 224), JNB_HEIGHT);

    vdpSprite->y = y + 0x80;
    vdpSprite->x = x + 0x80;
    vdpSprite->size = SPRITE_SIZE(1, 1);
    vdpSprite->link = vdpSpriteInd++;
    vdpSprite->attribut = TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, spritesTileInd[OBJ_FUR][imageInd]);
    vdpSprite++;
}

static void update_flesh(void* this, const bool partial_update) {
    object_t* currentObject = (object_t*) this;

    if (partial_update == TRUE) {

        currentObject->x += currentObject->x_add;
        currentObject->y += currentObject->y_add;

        u16 imageInd = currentObject->frame;
        s16 x = divu(mulu((currentObject->x >> 16), 320), JNB_WIDTH);
        s16 y = divu(mulu((currentObject->y >> 16), 224), JNB_HEIGHT);

        vdpSprite->y = y + 0x80;
        vdpSprite->x = x + 0x80;
        vdpSprite->size = SPRITE_SIZE(1, 1);
        vdpSprite->link = vdpSpriteInd++;
        vdpSprite->attribut = TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, spritesTileInd[OBJ_FLESH][imageInd]);
        vdpSprite++;

        return;
    }

    u16 tile = ban_map[currentObject->y >> 20][currentObject->x >> 20];
    if (tile == BAN_VOID) {
        currentObject->y_add += 3072;
        if (currentObject->y_add > 196608L)
            currentObject->y_add = 196608L;
    } else if (tile == BAN_WATER) {
        if (currentObject->x_add < 0) {
            if (currentObject->x_add < -65536L)
                currentObject->x_add = -65536L;
            currentObject->x_add += 1024;
            if (currentObject->x_add > 0)
                currentObject->x_add = 0;
        } else {
            if (currentObject->x_add > 65536L)
                currentObject->x_add = 65536L;
            currentObject->x_add -= 1024;
            if (currentObject->x_add < 0)
                currentObject->x_add = 0;
        }
        currentObject->y_add += 1024;
        if (currentObject->y_add < -65536L)
            currentObject->y_add = -65536L;
        if (currentObject->y_add > 65536L)
            currentObject->y_add = 65536L;
    }
    currentObject->x += currentObject->x_add;

    if ((currentObject->y >> 16) > 0) {
        tile = ban_map[currentObject->y >> 20][currentObject->x >> 20];
        if (tile == BAN_SOLID || tile == BAN_ICE) {
            if (currentObject->x_add < 0) {
                currentObject->x = (((currentObject->x >> 16) + 16) & 0xfff0) << 16;
                currentObject->x_add = -currentObject->x_add >> 2;
            } else {
                currentObject->x = ((((currentObject->x >> 16) - 16) & 0xfff0) + 15) << 16;
                currentObject->x_add = -currentObject->x_add >> 2;
            }
        }
    }
    currentObject->y += currentObject->y_add;
    if ((currentObject->x >> 16) < -5 || (currentObject->x >> 16) > 405 || (currentObject->y >> 16) > 260) {
        BANK_free(objects_bank, (void*)currentObject);
        return;
    }

    if (spawner[randomSpawnerInd]) {
        if (currentObject->frame == 0)
            add_object(OBJ_FLESH_TRACE, currentObject->x >> 16, currentObject->y >> 16, 0, 0, OBJ_ANIM_FLESH_TRACE, 1);
        else if (currentObject->frame == 1)
            add_object(OBJ_FLESH_TRACE, currentObject->x >> 16, currentObject->y >> 16, 0, 0, OBJ_ANIM_FLESH_TRACE, 2);
        else if (currentObject->frame == 2)
            add_object(OBJ_FLESH_TRACE, currentObject->x >> 16, currentObject->y >> 16, 0, 0, OBJ_ANIM_FLESH_TRACE, 3);
    }
    randomSpawnerInd = modu(randomSpawnerInd + randomSpawnerOffset, RANDOM_SPAWNER_SIZE);

    if ((currentObject->y >> 16) > 0) {
        tile = ban_map[currentObject->y >> 20][currentObject->x >> 20];
        if (tile != BAN_VOID) {
            if (currentObject->y_add < 0) {
                if (tile != BAN_WATER) {
                    currentObject->y = (((currentObject->y >> 16) + 16) & 0xfff0) << 16;
                    currentObject->x_add >>= 2;
                    currentObject->y_add = -currentObject->y_add >> 2;
                }
            } else {
                if (tile == BAN_SOLID) {
                    if (currentObject->y_add > 131072L) {
                        currentObject->y = ((((currentObject->y >> 16) - 16) & 0xfff0) + 15) << 16;
                        currentObject->x_add >>= 2;
                        currentObject->y_add = -currentObject->y_add >> 2;
                    } else {
                        // if (rnd(100) < 10) {
                        // 	s1 = rnd(4) - 2;
                        // 	add_leftovers(0, currentObject->x >> 16, (currentObject->y >> 16) + s1, currentObject->frame, &object_gobs);
                        // 	add_leftovers(1, currentObject->x >> 16, (currentObject->y >> 16) + s1, currentObject->frame, &object_gobs);
                        // }
                        BANK_free(objects_bank, (void*)currentObject);
                        return;
                    }
                } else if (tile == BAN_ICE) {
                    currentObject->y = ((((currentObject->y >> 16) - 16) & 0xfff0) + 15) << 16;
                    if (currentObject->y_add > 131072L)
                        currentObject->y_add = -currentObject->y_add >> 2;
                    else
                        currentObject->y_add = 0;
                }
            }
        }
    }

    if (currentObject->x_add < 0 && currentObject->x_add > -16384)
        currentObject->x_add = -16384;
    if (currentObject->x_add > 0 && currentObject->x_add < 16384)
        currentObject->x_add = 16384;

    u16 imageInd = currentObject->frame;
    s16 x = divu(mulu((currentObject->x >> 16), 320), JNB_WIDTH);
    s16 y = divu(mulu((currentObject->y >> 16), 224), JNB_HEIGHT);

    vdpSprite->y = y + 0x80;
    vdpSprite->x = x + 0x80;
    vdpSprite->size = SPRITE_SIZE(1, 1);
    vdpSprite->link = vdpSpriteInd++;
    vdpSprite->attribut = TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, spritesTileInd[OBJ_FLESH][imageInd]);
    vdpSprite++;
}

static void update_flesh_trace(void* this, const bool _unused) {
    object_t* currentObject = (object_t*) this;

    currentObject->ticks--;
    if (currentObject->ticks <= 0) {
        currentObject->frame++;
        if (currentObject->frame >= object_anims[OBJ_ANIM_FLESH_TRACE].num_frames) {
            BANK_free(objects_bank, (void*)currentObject);
            return;
        } else {
            currentObject->ticks = object_anims[OBJ_ANIM_FLESH_TRACE].frame[currentObject->frame].ticks;
            currentObject->image = object_anims[OBJ_ANIM_FLESH_TRACE].frame[currentObject->frame].image;
        }
    }

    u16 imageInd = currentObject->image;
    s16 x = divu(mulu((currentObject->x >> 16), 320), JNB_WIDTH);
    s16 y = divu(mulu((currentObject->y >> 16), 224), JNB_HEIGHT);

    vdpSprite->y = y + 0x80;
    vdpSprite->x = x + 0x80;
    vdpSprite->size = SPRITE_SIZE(1, 1);
    vdpSprite->link = vdpSpriteInd++;
    vdpSprite->attribut = TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, spritesTileInd[OBJ_FLESH_TRACE][imageInd]);
    vdpSprite++;
}

static void (*updateFns[8])(void*, const bool) = {
    NULL, &update_splash, &update_smoke, &update_butfly, &update_butfly,
    &update_fur, &update_flesh, &update_flesh_trace
};

void add_object(int type, int x, int y, int x_add, int y_add, int anim, int frame)
{
    object_t *currentObject = (object_t *) BANK_allocate(objects_bank);
    if (currentObject != NULL) {
            currentObject->type = type;
            currentObject->x = (long) x << 16;
            currentObject->y = (long) y << 16;
            currentObject->x_add = x_add;
            currentObject->y_add = y_add;
            currentObject->x_acc = 0;
            currentObject->y_acc = 0;
            currentObject->anim = anim;
            currentObject->frame = frame;
            currentObject->ticks = object_anims[anim].frame[frame].ticks;
            currentObject->image = object_anims[anim].frame[frame].image;
            currentObject->update_ptr = updateFns[type];
    }
}

void add_spring(int x, int y, int x_add, int y_add, int frame) {
    springs[springsCount].x = (long) x << 16;
    springs[springsCount].y = (long) y << 16;
    springs[springsCount].x_add = x_add;
    springs[springsCount].y_add = y_add;
    springs[springsCount].x_acc = 0;
    springs[springsCount].y_acc = 0;
    springs[springsCount].anim = OBJ_ANIM_SPRING;
    springs[springsCount].frame = frame;
    springs[springsCount].ticks = object_anims[OBJ_ANIM_SPRING].frame[frame].ticks;
    springs[springsCount].image = object_anims[OBJ_ANIM_SPRING].frame[frame].image;
    springsCount++;
}

void update_objects(void)
{
    static bool partial_update = FALSE;

    object_t* currentSpring = &springs[0];
    u16 currentSpringInd = springsCount;

    randomSpawnerInd = rnd(RANDOM_SPAWNER_SIZE);
    randomSpawnerOffset = rnd(RANDOM_SPAWNER_SIZE - 1) + 1; 

    while (currentSpringInd--)
    {
        currentSpring->ticks--;
        if (currentSpring->ticks <= 0) {
            currentSpring->frame++;
            if (currentSpring->frame >= object_anims[OBJ_ANIM_SPRING].num_frames) {
                currentSpring->frame--;
                currentSpring->ticks = object_anims[OBJ_ANIM_SPRING].frame[currentSpring->frame].ticks;
            } else {
                currentSpring->ticks = object_anims[OBJ_ANIM_SPRING].frame[currentSpring->frame].ticks;
                currentSpring->image = object_anims[OBJ_ANIM_SPRING].frame[currentSpring->frame].image;
            }
        }

        u16 imageInd = currentSpring->image;
        s16 x = divu(mulu((currentSpring->x >> 16), 320), JNB_WIDTH) - objects_frame_hs[OBJ_SPRING][imageInd].x;
        s16 y = divu(mulu((currentSpring->y >> 16), 224), JNB_HEIGHT) - objects_frame_hs[OBJ_SPRING][imageInd].y;

        vdpSprite->y = y + 0x80;
        vdpSprite->x = x + 0x80;
        vdpSprite->size = SPRITE_SIZE(2, 2);
        vdpSprite->link = vdpSpriteInd++;
        vdpSprite->attribut = TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, spritesTileInd[OBJ_SPRING][imageInd]);
        vdpSprite++;

        currentSpring++;
    }

    object_t* currentObject = (object_t*) BANK_getFirst(objects_bank);
    while(currentObject != NULL) {
        object_t* nextObject = (object_t*) BANK_getNext(objects_bank, (void*) currentObject);
        currentObject->update_ptr(currentObject, partial_update);
        currentObject = nextObject;
        partial_update = !partial_update;
    }
}
