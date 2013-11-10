#include "Lua.h"
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
#include <boost/asio.hpp>
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

struct TestWorldBlocks {
    BlockType stone;
    BlockType dirt;
    BlockType grass;

    TestWorldBlocks() {
        stone.setAllFaceTextureNums(3);

        dirt.setAllFaceTextureNums(2);

        grass.setAllFaceTextureNums(0);
        grass.setFaceTextureNum(Face::TOP, 1);
        grass.setFaceTextureNum(Face::BOTTOM, 2);    
    }
};

class TestWorldGenerator : public WorldGenerator {
public:
    TestWorldGenerator() : seed(0) { }

    std::unique_ptr<Chunk> generateChunk(const glm::ivec3 &chunkpos) const {
        std::unique_ptr<Chunk> chunk{new Chunk{}};

        for (auto i = begin(*chunk); i != end(*chunk); ++i) {
            glm::vec3 pos = glm::vec3{chunkpos} + glm::vec3{i.getPos()}/32.0f;

            float thresh = pos.z;
            thresh -= 2*perlin3(pos/5.0f, seed ^ 0x1);
            if (thresh < 0) {
                thresh = 0;
            }

            float val = perlin3(pos, seed);

            if (val > thresh) {
                *i = blocks.stone;
            } else {
                *i = Block::air();
            }
        }

        for (int x=0; x<Chunk::XSize; x++) {
            for (int y=0; y<Chunk::YSize; y++) {
                int ctr = 0;

                for (int z=Chunk::ZSize-1; z>=0; z--) {
                    Block &b = (*chunk)[glm::ivec3{x, y, z}];
                    if (&b.getType() == &blocks.stone) {
                        if (ctr == 0) {
                            b = blocks.grass;
                        } else {
                            b = blocks.dirt;
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

    const TestWorldBlocks &getBlocks() const { return blocks; }
    
private:
    int seed;
    TestWorldBlocks blocks;
};

int main(int argc, char **argv) {
    boost::asio::io_service main_io;
    boost::asio::io_service work_io;
    boost::asio::io_service::work work_io_work{work_io};
    
    Lua lua;
    lua.runFile("game.lua");

    TestWorldGenerator gen;
    World world(gen, main_io, work_io);

    auto regenWorld = [&]() {
        world.getChunks().clearAllChunks();
        gen.reseed(rand());
    };
    regenWorld();

    GraphicsSystem gfx{world, main_io, work_io};
    gfx.getCamera().pos.z = 40;
    
    RPYCameraManipulator camera_manipulator{.002, 5};

    gfx.setInputCallback([&]() {
        Window &window = gfx.getWindow();
        RPYCamera &camera = gfx.getCamera();
        
        if (window.isClosed()) {
            main_io.stop();
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
                newchunk->getBlock(blockpos) = Block::air();
                world.getChunks().setChunk(chunkpos, std::move(newchunk));
                gfx.regenerateChunkMesh(chunkpos);
            }
        }

        static constexpr int range = 8;
        glm::ivec3 camera_chunkpos = ChunkGrid::posToChunkBlock(
            glm::ivec3{floorVec(camera.pos)}).first;
        
        for (int i=0; i<10; i++) {
            glm::ivec3 chunkpos = camera_chunkpos;
            chunkpos.x += (rand() % range) - range/2;
            chunkpos.y += (rand() % range) - range/2;
            chunkpos.z = 0;
            world.asyncGenerateChunk(chunkpos);
        }
    });

    std::vector<std::thread> work_threads;

    for (int i=0; i<4; i++) {
        work_threads.emplace_back([&work_io]() {
            work_io.run();
        });
    }

    main_io.run();
    work_io.stop();
    for (auto &thread : work_threads) {
        thread.join();
    }
    
    return 0;
}
