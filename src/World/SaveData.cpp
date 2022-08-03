#include "SaveData.hpp"
#include <zlib.h>
#include "../Utils.hpp"

namespace CrossCraft
{
    auto SaveData::load_world(World *wrld) -> bool
    {
        gzFile save_file = gzopen((PLATFORM_FILE_PREFIX + "save.ccc").c_str(), "rb");
        gzrewind(save_file);

        int version = 0;
        gzread(save_file, &version, sizeof(int) * 1);

        SC_APP_DEBUG("READING FILE -- VERSION {}", version);

        if (version == 1) {
            gzread(save_file, wrld->worldData, 256 * 64 * 256);
            gzclose(save_file);
        }
        else if (version == 2) {
            gzread(save_file, &wrld->world_size, sizeof(wrld->world_size));

            wrld->worldData = (block_t*)realloc(
                wrld->worldData,
                wrld->world_size.x * wrld->world_size.y * wrld->world_size.z);
            wrld->lightData = (uint16_t*)realloc(
                wrld->lightData, wrld->world_size.x *
                (wrld->world_size.y / 16 + 1) *
                wrld->world_size.z * sizeof(uint16_t));
            wrld->chunksMeta = (ChunkMeta*)realloc(
                wrld->chunksMeta, wrld->world_size.x / 16 *
                (wrld->world_size.y / 16 + 1) *
                wrld->world_size.z / 16 * sizeof(ChunkMeta));

            gzread(save_file, wrld->worldData, wrld->world_size.x * wrld->world_size.y * wrld->world_size.z);
            gzclose(save_file);
        }
        else
            return false;


        // Update Lighting
        for (int x = 0; x < wrld->world_size.x; x++)
        {
            for (int z = 0; z < wrld->world_size.z; z++)
            {
                wrld->update_lighting(x, z);
            }
        }
        wrld->generate_meta();

        return true;
    }

    auto SaveData::save(std::any p) -> void
    {
        auto wrld = std::any_cast<World *>(p);
        if (wrld->client == nullptr)
        {
            SC_APP_DEBUG("SAVING!");

            gzFile save_file = gzopen((PLATFORM_FILE_PREFIX + "save.ccc").c_str(), "wb");

            if (save_file != nullptr)
            {
                const int save_version = 2;
                gzwrite(save_file, &save_version, 1 * sizeof(int));
                gzwrite(save_file, &wrld->world_size, sizeof(wrld->world_size));
                gzwrite(save_file, wrld->worldData, 256 * 64 * 256);

                gzclose(save_file);
            }
        }
    }

}