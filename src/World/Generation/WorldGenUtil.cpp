#include "WorldGenUtil.hpp"

namespace CrossCraft {

auto WorldGenUtil::make_tree(World *wrld, int x, int z, int h) -> void {

    int tree_height = rand() % 3 + 4;

    for (int i = 0; i < tree_height + 1; i++) {
        // lower layer
        if (i > tree_height - 4 && i < tree_height - 1) {
            for (int tx = -2; tx < 3; tx++)
                for (int tz = -2; tz < 3; tz++)
                    wrld->worldData[wrld->getIdx(x + tx, h + i, z + tz)] =
                        Block::Leaves;
        } else if (i >= tree_height - 1 && i < tree_height) {
            for (int tx = -1; tx < 2; tx++)
                for (int tz = -1; tz < 2; tz++)
                    wrld->worldData[wrld->getIdx(x + tx, h + i, z + tz)] =
                        Block::Leaves;
        } else if (i == tree_height) {
            wrld->worldData[wrld->getIdx(x - 1, h + i, z)] = Block::Leaves;
            wrld->worldData[wrld->getIdx(x + 1, h + i, z)] = Block::Leaves;
            wrld->worldData[wrld->getIdx(x, h + i, z)] = Block::Leaves;
            wrld->worldData[wrld->getIdx(x, h + i, z + 1)] = Block::Leaves;
            wrld->worldData[wrld->getIdx(x, h + i, z - 1)] = Block::Leaves;
        }

        // write the log
        if (i < tree_height) {
            wrld->worldData[wrld->getIdx(x, h + i, z)] = Block::Logs;
        }
    }
}

} // namespace CrossCraft