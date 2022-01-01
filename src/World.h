#pragma once
#include "ChunkStack.h"
#include "Player.h"
#include <map>
#include <memory>
#include <stdint.h>

/**
 * Blocks are just u8 - 255 max.
 */
typedef uint8_t block_t;

// 0 - air
// 1 - grass
// 2 - stone
// 3 - dirt
// 4 - wood
// 5 - cobble
// 6 - sand
// 7 - water
// 8 - gravel
// 9 - leaf
// 10 - sapling
// 11 - dandelion
// 12 - rose
// 13 - b mush
// 14 - r mush
// 15 - glass
// 16 - gold block
// 17 - coal
// 18 - iron
// 19 - gold
// 20 - sponge
// 21 - red
// 22 - orange
// 23 - yellow
// 24 - lime
// 25 - green
// 26 - turqois ?
// 27 - aqua
// 28 - blue
// 29 - dark blue
// 30 - purple
// 31 - magenta
// 32 - fuschia
// 33 - pink
// 34 - grey
// 35 - light grey
// 36 - white
// 37 - bedrock

class Player;
class ChunkStack;

struct ChunkMeshMeta {
    bool isEmpty;
    bool isFull;
};

/**
 * This contains our entire world.
 */
class World {
  public:
    World(std::shared_ptr<Player> p);
    ~World();

    /**
     * Performs an update... does the job of tick updates too.
     */
    void update(double dt);

    /**
     * Performs a draw.
     */
    void draw();

    block_t getBlock(int x, int y, int z);

    /**
     * The world of blocks.
     */
    block_t worldData[128 * 128 * 128];

    /**
     * World Metadata.
     */
    ChunkMeshMeta metaData[8 * 8 * 8];

    /**
     * Texture ID of terrain atlas.
     */
    unsigned int terrain_atlas;

    /**
     * Chunk Stacks for our mesh.
     */
    std::map<Vector3i, ChunkStack *> mesh;
    std::shared_ptr<Player> player;
    glm::ivec2 lastPlayerPos;
    std::vector<Vector3i> remainingGeneration;
    int updatesTilNext;
};
