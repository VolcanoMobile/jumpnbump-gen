#include <genesis.h>

#include "bank.h"
#include "global.h"
#include "objects.h"
#include "resources.h"

#define FULL_UPDATE_BUDGET 40

static Bank* objects_bank;
static object_t* firstObject = NULL;

object_t springs[8];
u16 springsCount;

#define RANDOM_SPAWNER_SIZE  256
static bool spawner[RANDOM_SPAWNER_SIZE];
static u16 randomSpawnerInd = 0;
static u16 randomSpawnerOffset = 1;

// from -20 to 420
static const u16 x_correction[] = { 112, 113, 114, 115, 116, 116, 117, 118, 119, 120, 120, 121, 122, 123, 124, 124, 125, 126, 127, 128, 128, 128, 129, 130, 131, 132, 132, 133, 134, 135, 136, 136, 137, 138, 139, 140, 140, 141, 142, 143, 144, 144, 145, 146, 147, 148, 148, 149, 150, 151, 152, 152, 153, 154, 155, 156, 156, 157, 158, 159, 160, 160, 161, 162, 163, 164, 164, 165, 166, 167, 168, 168, 169, 170, 171, 172, 172, 173, 174, 175, 176, 176, 177, 178, 179, 180, 180, 181, 182, 183, 184, 184, 185, 186, 187, 188, 188, 189, 190, 191, 192, 192, 193, 194, 195, 196, 196, 197, 198, 199, 200, 200, 201, 202, 203, 204, 204, 205, 206, 207, 208, 208, 209, 210, 211, 212, 212, 213, 214, 215, 216, 216, 217, 218, 219, 220, 220, 221, 222, 223, 224, 224, 225, 226, 227, 228, 228, 229, 230, 231, 232, 232, 233, 234, 235, 236, 236, 237, 238, 239, 240, 240, 241, 242, 243, 244, 244, 245, 246, 247, 248, 248, 249, 250, 251, 252, 252, 253, 254, 255, 256, 256, 257, 258, 259, 260, 260, 261, 262, 263, 264, 264, 265, 266, 267, 268, 268, 269, 270, 271, 272, 272, 273, 274, 275, 276, 276, 277, 278, 279, 280, 280, 281, 282, 283, 284, 284, 285, 286, 287, 288, 288, 289, 290, 291, 292, 292, 293, 294, 295, 296, 296, 297, 298, 299, 300, 300, 301, 302, 303, 304, 304, 305, 306, 307, 308, 308, 309, 310, 311, 312, 312, 313, 314, 315, 316, 316, 317, 318, 319, 320, 320, 321, 322, 323, 324, 324, 325, 326, 327, 328, 328, 329, 330, 331, 332, 332, 333, 334, 335, 336, 336, 337, 338, 339, 340, 340, 341, 342, 343, 344, 344, 345, 346, 347, 348, 348, 349, 350, 351, 352, 352, 353, 354, 355, 356, 356, 357, 358, 359, 360, 360, 361, 362, 363, 364, 364, 365, 366, 367, 368, 368, 369, 370, 371, 372, 372, 373, 374, 375, 376, 376, 377, 378, 379, 380, 380, 381, 382, 383, 384, 384, 385, 386, 387, 388, 388, 389, 390, 391, 392, 392, 393, 394, 395, 396, 396, 397, 398, 399, 400, 400, 401, 402, 403, 404, 404, 405, 406, 407, 408, 408, 409, 410, 411, 412, 412, 413, 414, 415, 416, 416, 417, 418, 419, 420, 420, 421, 422, 423, 424, 424, 425, 426, 427, 428, 428, 429, 430, 431, 432, 432, 433, 434, 435, 436, 436, 437, 438, 439, 440, 440, 441, 442, 443, 444, 444, 445, 446, 447, 448, 448, 449, 450, 451, 452, 452, 453, 454, 455, 456, 456, 457, 458, 459, 460, 460, 461, 462, 463 };
static const u16* px = &x_correction[20];

// from -130 to 260
static const u16 y_correction[] = { 15, 16, 16, 17, 18, 19, 20, 21, 22, 23, 23, 24, 25, 26, 27, 28, 29, 30, 30, 31, 32, 33, 34, 35, 36, 37, 37, 38, 39, 40, 41, 42, 43, 44, 44, 45, 46, 47, 48, 49, 50, 51, 51, 52, 53, 54, 55, 56, 57, 58, 58, 59, 60, 61, 62, 63, 64, 65, 65, 66, 67, 68, 69, 70, 71, 72, 72, 73, 74, 75, 76, 77, 78, 79, 79, 80, 81, 82, 83, 84, 85, 86, 86, 87, 88, 89, 90, 91, 92, 93, 93, 94, 95, 96, 97, 98, 99, 100, 100, 101, 102, 103, 104, 105, 106, 107, 107, 108, 109, 110, 111, 112, 113, 114, 114, 115, 116, 117, 118, 119, 120, 121, 121, 122, 123, 124, 125, 126, 127, 128, 128, 128, 129, 130, 131, 132, 133, 134, 135, 135, 136, 137, 138, 139, 140, 141, 142, 142, 143, 144, 145, 146, 147, 148, 149, 149, 150, 151, 152, 153, 154, 155, 156, 156, 157, 158, 159, 160, 161, 162, 163, 163, 164, 165, 166, 167, 168, 169, 170, 170, 171, 172, 173, 174, 175, 176, 177, 177, 178, 179, 180, 181, 182, 183, 184, 184, 185, 186, 187, 188, 189, 190, 191, 191, 192, 193, 194, 195, 196, 197, 198, 198, 199, 200, 201, 202, 203, 204, 205, 205, 206, 207, 208, 209, 210, 211, 212, 212, 213, 214, 215, 216, 217, 218, 219, 219, 220, 221, 222, 223, 224, 225, 226, 226, 227, 228, 229, 230, 231, 232, 233, 233, 234, 235, 236, 237, 238, 239, 240, 240, 241, 242, 243, 244, 245, 246, 247, 247, 248, 249, 250, 251, 252, 253, 254, 254, 255, 256, 257, 258, 259, 260, 261, 261, 262, 263, 264, 265, 266, 267, 268, 268, 269, 270, 271, 272, 273, 274, 275, 275, 276, 277, 278, 279, 280, 281, 282, 282, 283, 284, 285, 286, 287, 288, 289, 289, 290, 291, 292, 293, 294, 295, 296, 296, 297, 298, 299, 300, 301, 302, 303, 303, 304, 305, 306, 307, 308, 309, 310, 310, 311, 312, 313, 314, 315, 316, 317, 317, 318, 319, 320, 321, 322, 323, 324, 324, 325, 326, 327, 328, 329, 330, 331, 331, 332, 333, 334, 335, 336, 337, 338, 338, 339, 340, 341, 342, 343, 344, 345, 345, 346, 347, 348, 349, 350, 351, 352, 352, 353, 354 };
static const u16* py = &y_correction[130];

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
    firstObject = NULL;
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

static bool update_splash(void* this, const bool _unused) {
    object_t* currentObject = (object_t*) this;

    currentObject->ticks--;
    if (currentObject->ticks <= 0) {
        currentObject->frame++;
        if (currentObject->frame >= object_anims[OBJ_ANIM_SPLASH].num_frames) {
            BANK_free(objects_bank, (void*)currentObject);
            return TRUE;
        } else {
            currentObject->ticks = object_anims[OBJ_ANIM_SPLASH].frame[currentObject->frame].ticks;
            currentObject->image = object_anims[OBJ_ANIM_SPLASH].frame[currentObject->frame].image;
        }
    }

    u16 imageInd = currentObject->image;
    s16 x = divu(mulu((currentObject->x >> 16), 320), JNB_WIDTH) - objects_frame_hs[OBJ_SPLASH][imageInd].x;
    s16 y = (mulu((currentObject->y >> 16), 224) / JNB_HEIGHT) - objects_frame_hs[OBJ_SPLASH][imageInd].y;

    vdpSprite->y = y + 0x80;
    vdpSprite->x = x + 0x80;
    vdpSprite->size = SPRITE_SIZE(3, 2);
    vdpSprite->link = vdpSpriteInd++;
    vdpSprite->attribut = TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, spritesTileInd[OBJ_SPLASH][imageInd]);
    vdpSprite++;

    return FALSE;
}

static bool update_smoke(void* this, const bool _unused) {
    object_t* currentObject = (object_t*) this;

    currentObject->x += currentObject->x_add;
    currentObject->y += currentObject->y_add;
    currentObject->ticks--;
    if (currentObject->ticks <= 0) {
        currentObject->frame++;
        if (currentObject->frame >= object_anims[OBJ_ANIM_SMOKE].num_frames) {
            BANK_free(objects_bank, (void*)currentObject);
            return TRUE;
        } else {
            currentObject->ticks = object_anims[OBJ_ANIM_SMOKE].frame[currentObject->frame].ticks;
            currentObject->image = object_anims[OBJ_ANIM_SMOKE].frame[currentObject->frame].image;
        }
    }

    u16 imageInd = currentObject->image;
    vdpSprite->y = py[currentObject->y >> 16] - objects_frame_hs[OBJ_SMOKE][imageInd].y;
    vdpSprite->x = px[currentObject->x >> 16] - objects_frame_hs[OBJ_SMOKE][imageInd].x;
    vdpSprite->size = SPRITE_SIZE(1, 1);
    vdpSprite->link = vdpSpriteInd++;
    vdpSprite->attribut = TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, spritesTileInd[OBJ_SMOKE][imageInd]);
    vdpSprite++;

    return FALSE;
}

static bool update_butfly(void* this, const bool _unused) {
    object_t* currentObject = (object_t*) this;

    currentObject->x_acc += (random() % 128) - 64;
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
    currentObject->y_acc += (random() % 64) - 32;
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
    vdpSprite->y = py[currentObject->y >> 16] - objects_frame_hs[OBJ_YEL_BUTFLY][imageInd].y;
    vdpSprite->x = px[currentObject->x >> 16] - objects_frame_hs[OBJ_YEL_BUTFLY][imageInd].x;
    vdpSprite->size = SPRITE_SIZE(1, 1);
    vdpSprite->link = vdpSpriteInd++;
    vdpSprite->attribut = TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, spritesTileInd[currentObject->type][imageInd]);
    vdpSprite++;

    return FALSE;
}

static bool update_fur(void* this, const bool partial_update) {
    object_t* currentObject = (object_t*) this;

    if (partial_update) {
        currentObject->x += currentObject->x_add;
        currentObject->y += currentObject->y_add;

        u16 imageInd = currentObject->frame + ((currentObject->x >> 16) % 8);
        vdpSprite->y = py[currentObject->y >> 16];
        vdpSprite->x = px[currentObject->x >> 16];
        vdpSprite->size = SPRITE_SIZE(1, 1);
        vdpSprite->link = vdpSpriteInd++;
        vdpSprite->attribut = TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, spritesTileInd[OBJ_FUR][imageInd]);
        vdpSprite++;

        return FALSE;
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
        return TRUE;
    }

    if (spawner[randomSpawnerInd])
        add_object(OBJ_FLESH_TRACE, currentObject->x >> 16, currentObject->y >> 16, 0, 0, OBJ_ANIM_FLESH_TRACE, 0);
    randomSpawnerInd = (randomSpawnerInd + randomSpawnerOffset) % RANDOM_SPAWNER_SIZE;
    
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
                        return TRUE;
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

    u16 imageInd = currentObject->frame + ((currentObject->x >> 16) % 8);
    vdpSprite->y = py[currentObject->y >> 16];
    vdpSprite->x = px[currentObject->x >> 16];
    vdpSprite->size = SPRITE_SIZE(1, 1);
    vdpSprite->link = vdpSpriteInd++;
    vdpSprite->attribut = TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, spritesTileInd[OBJ_FUR][imageInd]);
    vdpSprite++;

    return FALSE;
}

static bool update_flesh(void* this, const bool partial_update) {
    object_t* currentObject = (object_t*) this;

    if (partial_update == TRUE) {
        currentObject->x += currentObject->x_add;
        currentObject->y += currentObject->y_add;

        u16 imageInd = currentObject->frame;
        vdpSprite->y = py[currentObject->y >> 16];
        vdpSprite->x = px[currentObject->x >> 16];
        vdpSprite->size = SPRITE_SIZE(1, 1);
        vdpSprite->link = vdpSpriteInd++;
        vdpSprite->attribut = TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, spritesTileInd[OBJ_FLESH][imageInd]);
        vdpSprite++;

        return FALSE;
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
        return TRUE;
    }

    if (spawner[randomSpawnerInd]) {
        if (currentObject->frame == 0)
            add_object(OBJ_FLESH_TRACE, currentObject->x >> 16, currentObject->y >> 16, 0, 0, OBJ_ANIM_FLESH_TRACE, 1);
        else if (currentObject->frame == 1)
            add_object(OBJ_FLESH_TRACE, currentObject->x >> 16, currentObject->y >> 16, 0, 0, OBJ_ANIM_FLESH_TRACE, 2);
        else if (currentObject->frame == 2)
            add_object(OBJ_FLESH_TRACE, currentObject->x >> 16, currentObject->y >> 16, 0, 0, OBJ_ANIM_FLESH_TRACE, 3);
    }
    randomSpawnerInd = (randomSpawnerInd + randomSpawnerOffset) % RANDOM_SPAWNER_SIZE;

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
                        return TRUE;
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
    vdpSprite->y = py[currentObject->y >> 16];
    vdpSprite->x = px[currentObject->x >> 16];
    vdpSprite->size = SPRITE_SIZE(1, 1);
    vdpSprite->link = vdpSpriteInd++;
    vdpSprite->attribut = TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, spritesTileInd[OBJ_FLESH][imageInd]);
    vdpSprite++;

    return FALSE;
}

static bool update_flesh_trace(void* this, const bool _unused) {
    object_t* currentObject = (object_t*) this;

    currentObject->ticks--;
    if (currentObject->ticks <= 0) {
        currentObject->frame++;
        if (currentObject->frame >= object_anims[OBJ_ANIM_FLESH_TRACE].num_frames) {
            BANK_free(objects_bank, (void*)currentObject);
            return TRUE;
        } else {
            currentObject->ticks = object_anims[OBJ_ANIM_FLESH_TRACE].frame[currentObject->frame].ticks;
            currentObject->image = object_anims[OBJ_ANIM_FLESH_TRACE].frame[currentObject->frame].image;
        }
    }

    u16 imageInd = currentObject->image;
    vdpSprite->y = py[currentObject->y >> 16];
    vdpSprite->x = px[currentObject->x >> 16];
    vdpSprite->size = SPRITE_SIZE(1, 1);
    vdpSprite->link = vdpSpriteInd++;
    vdpSprite->attribut = TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, spritesTileInd[OBJ_FLESH_TRACE][imageInd]);
    vdpSprite++;

    return FALSE;
}

static bool (*updateFns[8])(void*, const bool) = {
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
    object_t* currentSpring = &springs[0];
    u16 currentSpringInd = springsCount;

    randomSpawnerInd = random() % RANDOM_SPAWNER_SIZE;
    randomSpawnerOffset = (random() % (RANDOM_SPAWNER_SIZE / 2)) + 1; 

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
        vdpSprite->y = py[currentSpring->y >> 16] - objects_frame_hs[OBJ_SPRING][imageInd].y;
        vdpSprite->x = px[currentSpring->x >> 16] - objects_frame_hs[OBJ_SPRING][imageInd].x;
        vdpSprite->size = SPRITE_SIZE(2, 2);
        vdpSprite->link = vdpSpriteInd++;
        vdpSprite->attribut = TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, spritesTileInd[OBJ_SPRING][imageInd]);
        vdpSprite++;

        currentSpring++;
    }

    object_t *current, *next, *new_first;
    bool stop;
    s16 i;

    for(i = FULL_UPDATE_BUDGET, stop = FALSE, new_first = current = firstObject, next = current ? (object_t *)BANK_getNext(objects_bank, current) : (object_t *)BANK_getFirst(objects_bank);
        !stop;
        current = next, next = current ? (object_t *)BANK_getNext(objects_bank, current) : (object_t *)BANK_getFirst(objects_bank))
    {
        if(next == firstObject)
        {
            stop = TRUE;
        }
        if(current && current->update_ptr(current, i-- <= 0))
        {
            if(current == firstObject)
            {
                firstObject = next;
            }
            if(current == new_first)
            {
                new_first = next;
            }
        }
        if(i == 0)
        {
            new_first = next;
        }
    }
    firstObject = new_first;    
}
