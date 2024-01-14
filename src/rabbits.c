#include <genesis.h>

#include "global.h"
#include "resources.h"

u16 rabbitsTileInd[JNB_MAX_PLAYERS][18];

void loadRabbits() {
    // load Rabbits Sprites
    memcpy(&palette[32], rabbits_palette1.data, 16 * 2);
    memcpy(&palette[48], rabbits_palette2.data, 16 * 2);

    for(u16 c1 = 0; c1 < 18; c1++) {
        rabbitsTileInd[0][c1] = VDPTilesFilled;
        const AnimationFrame* animationFrame = rabbit1_sprite.animations[0]->frames[c1];
        const u32* tiles   = animationFrame->tileset->tiles;
        const u16  numTile = animationFrame->tileset->numTile;
        VDP_loadTileData(tiles, VDPTilesFilled, numTile, DMA);
        VDPTilesFilled += numTile;
    }

    for(u16 c1 = 0; c1 < 18; c1++) {
        rabbitsTileInd[1][c1] = VDPTilesFilled;
        const AnimationFrame* animationFrame = rabbit2_sprite.animations[0]->frames[c1];
        const u32* tiles   = animationFrame->tileset->tiles;
        const u16  numTile = animationFrame->tileset->numTile;
        VDP_loadTileData(tiles, VDPTilesFilled, numTile, DMA);
        VDPTilesFilled += numTile;
    }

    for(u16 c1 = 0; c1 < 18; c1++) {
        rabbitsTileInd[2][c1] = VDPTilesFilled;
        const AnimationFrame* animationFrame = rabbit3_sprite.animations[0]->frames[c1];
        const u32* tiles   = animationFrame->tileset->tiles;
        const u16  numTile = animationFrame->tileset->numTile;
        VDP_loadTileData(tiles, VDPTilesFilled, numTile, DMA);
        VDPTilesFilled += numTile;
    }

    for(u16 c1 = 0; c1 < 18; c1++) {
        rabbitsTileInd[3][c1] = VDPTilesFilled;
        const AnimationFrame* animationFrame = rabbit4_sprite.animations[0]->frames[c1];
        const u32* tiles   = animationFrame->tileset->tiles;
        const u16  numTile = animationFrame->tileset->numTile;
        VDP_loadTileData(tiles, VDPTilesFilled, numTile, DMA);
        VDPTilesFilled += numTile;
    }
}

static struct {
    s16 x, y;
} rabbit_hs[18] = {
    { -2, -1 },
    { -2,  0 },
    { -2,  2 },
    { -3,  2 },
    { -3, -2 },
    { -1,  1 },
    {  0,  0 },
    {  1,  0 },
    { -1, -3 },
    { -1, -1 },
    {  0,  0 },
    {  0,  2 },
    {  0,  2 },
    {  1, -2 },
    {  0,  1 },
    {  0,  0 },
    {  0,  0 },
    {  2, -3 }
};

void updateRabbitsSprites(bool forceAll) {
    u32 c1 = JNB_MAX_PLAYERS;
    while(c1--) {
        if(player[c1].enabled || forceAll) {

            u16 imageInd = player[c1].image;
            s16 x = divu(mulu((player[c1].x >> 16), 320), JNB_WIDTH) - rabbit_hs[imageInd].x;
            s16 y = divu(mulu((player[c1].y >> 16), 224), JNB_HEIGHT) - rabbit_hs[imageInd].y;

            vdpSprite->y = y + 0x80 ;
            vdpSprite->x = x + 0x80;
            vdpSprite->size = SPRITE_SIZE(2, 2);
            vdpSprite->link = vdpSpriteInd++;
            vdpSprite->attribut = TILE_ATTR_FULL(PAL2 + (c1 >> 1), FALSE, FALSE, FALSE, rabbitsTileInd[c1][imageInd]);
            vdpSprite++;
        }
    }
}
