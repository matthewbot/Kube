#include "Lua.h"
#include "Block.h"
#include "util/ThreadManager.h"
#include "gfx/Shader.h"
#include "gfx/Buffer.h"
#include "gfx/GraphicsSystem.h"
#include "Chunk.h"
#include "World.h"
#include "gfx/Image.h"
#include "gfx/Texture.h"
#include "gfx/Window.h"
#include "gfx/Renderer.h"
#include "perlin.h"
#include "gfx/Font.h"
#include <unistd.h>
#include <iostream>
#include <tuple>
#include <chrono>
#include <random>
#include <thread>
#include <GL/glew.h>
#include <GL/glfw.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

const float pi = static_cast<float>(M_PI);

void registerBlockTypes(BlockTypeRegistry &types) {
    BlockTypeInfo stone;
    stone.setAllFaceTextureNums(3);
    types.makeType("stone", stone);

    BlockTypeInfo dirt;
    dirt.setAllFaceTextureNums(2);
    types.makeType("dirt", dirt);

    BlockTypeInfo grass;
    grass.setAllFaceTextureNums(0);
    grass.setFaceTextureNum(Face::TOP, 1);
    grass.setFaceTextureNum(Face::BOTTOM, 2);
    types.makeType("grass", grass);
}

class TestWorldGenerator : public WorldGenerator {
public:
    TestWorldGenerator() : seed(0) { }

    bool solid(glm::vec3 pos) const {
        float val = 2*perlin3(pos, seed);

        glm::vec3 threshpos{pos.x/20, pos.y/20, 0};
        float thresh = pos.z + 5*perlin3(threshpos, seed ^ 0x1);

        return val > thresh;
    }

    std::unique_ptr<Chunk> generateChunk(
        const glm::ivec3 &chunkpos,
        const BlockTypeRegistry &blocktypes) const
    {
        const BlockType *grass = blocktypes.getType("grass");
        const BlockType *dirt = blocktypes.getType("dirt");
        const BlockType *stone = blocktypes.getType("stone");

        std::unique_ptr<Chunk> chunk{new Chunk{blocktypes}};
        chunk->fill(Block::air());

        for (auto &pos : ChunkIndex::Range) {
            glm::vec3 worldpos = static_cast<glm::vec3>(chunkpos) +
                static_cast<glm::vec3>(pos.getVec())/32.0f;
            chunk->setBlock(pos, solid(worldpos) ? *stone : Block::air());
        }

        for (int x=0; x<Chunk::XSize; x++) {
            for (int y=0; y<Chunk::YSize; y++) {
                int ctr = 0;

                glm::vec3 pos_above{chunkpos.x + x/32.0f,
                                    chunkpos.y + y/32.0f,
                                    chunkpos.z+1};
                if (solid(pos_above))
                    continue;

                for (int z=Chunk::ZSize-1; z>=0; z--) {
                    ChunkIndex idx{x, y, z};
                    Block b = chunk->getBlock(idx);
                    if (&b.getType() == stone) {
                        if (ctr == 0) {
                            chunk->setBlock(idx, Block{*grass});
                        } else {
                            chunk->setBlock(idx, Block{*dirt});
                        }

                        if (++ctr >= 3) {
                            break;
                        }
                    }
                }
            }
        }

        return chunk;
    }

    void reseed(int seed) { this->seed = seed; }

private:
    int seed;
};

int main(int argc, char **argv) {
    ThreadManager tm;

    Lua lua;
    lua.runFile("game.lua");

    BlockTypeRegistry blocktypes;
    registerBlockTypes(blocktypes);

    TestWorldGenerator gen;
    World world(blocktypes, gen, tm);

    auto regenWorld = [&]() {
        world.getChunks().clearAllChunks();
        gen.reseed(rand());
    };
    regenWorld();

    GraphicsSystem gfx{world, tm};
    gfx.getCamera().pos.z = 40;

    RPYCameraManipulator camera_manipulator{.002, 5};

    // TODO
    gfx.runRenderLoop([&]() -> bool {
        Window &window = gfx.getWindow();
        RPYCamera &camera = gfx.getCamera();

        if (window.isClosed()) {
	    return false;
        }

        camera_manipulator.update(camera, window, 1/50.0);

        if (window.isKeyPressed('r')) {
            regenWorld();
        }

        if (window.isMousePressed(MouseButton::RIGHT)) {
            glm::vec3 pos, dir;
            std::tie(pos, dir) = unproject(
                gfx.getPerspectiveProjection().getMatrix(),
                camera.getMatrix(),
                window.getNDCPos(window.getMousePos()));
            auto pick = world.getChunks().pick(pos, dir, 10);
            if (pick) {
                glm::ivec3 chunkpos, blockpos;
                std::tie(chunkpos, blockpos) = ChunkGrid::posToChunkBlock(*pick);
                std::unique_ptr<Chunk> newchunk{
                    new Chunk(*world.getChunks().getChunk(chunkpos))};
                newchunk->setBlock(ChunkIndex{blockpos}, Block::air());
                world.getChunks().setChunk(chunkpos, std::move(newchunk));
            }
        }

        glm::ivec3 camera_chunkpos = ChunkGrid::posToChunkBlock(
            glm::ivec3{floorVec(camera.pos)}).first;

        static constexpr int range = 8;
        static constexpr int zrange = 4;
        for (int i=0; i<10; i++) {
            glm::ivec3 chunkpos = camera_chunkpos;
            chunkpos.x += (rand() % range) - range/2;
            chunkpos.y += (rand() % range) - range/2;
            chunkpos.z = (rand() % zrange) - zrange/2;
            world.asyncGenerateChunk(chunkpos);
        }

	return true;
    });

    return 0;
}
