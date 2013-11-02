#include "Shader.h"
#include "Buffer.h"
#include "Chunk.h"
#include "World.h"
#include "Image.h"
#include "Texture.h"
#include "Window.h"
#include "Renderer.h"
#include "perlin.h"
#include <unistd.h>
#include <iostream>
#include <tuple>
#include <boost/asio.hpp>
#include <chrono>
#include <random>
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
        stone.setAllFaceTextureNums(0);

        dirt.setAllFaceTextureNums(1);

        grass.setAllFaceTextureNums(3);
        grass.setFaceTextureNum(Face::TOP, 2);
        grass.setFaceTextureNum(Face::BOTTOM, 1);    
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
    Window window{800, 600};

    Image img = Image::loadPNG("blocks.png");
    ArrayTexture tex{img, 4};

    Sampler sampler{Sampler::NEAREST};

    Shader vert{Shader::Type::VERTEX, "vert.glsl"};
    Shader frag{Shader::Type::FRAGMENT, "frag.glsl"};
    ShaderProgram prgm{vert, frag};

    std::default_random_engine rand;
    TestWorldGenerator gen;
    World world(gen);

    auto regenWorld = [&]() {
        world.getChunks().clearAllChunks();
        gen.reseed(rand());
        for (int x=-2; x<=2; x++) {
            for (int y=-2; y<=2; y++) {
                world.generateChunk(glm::ivec3{x, y, 0});
            }
        }
    };
    regenWorld();

    PerspectiveProjection projection;
    projection.aspect = window.getAspectRatio();

    Renderer renderer;
    renderer.setWindow(window);
    renderer.setProjection(projection);
    renderer.setProgram(prgm);
    renderer.setTexture(0, tex, sampler);

    RPYCameraManipulator camera_manipulator{.002, 5};
    RPYCamera camera;
    camera.pos.z = 40;

    static const auto rate = boost::posix_time::seconds(1/60.0);
    boost::asio::io_service io;
    boost::asio::deadline_timer timer(io, rate);

    std::function<void (const boost::system::error_code &)> render =
        [&](const boost::system::error_code &err) -> void {
        if (window.isClosed()) {
            io.stop();
        }

        if (camera_manipulator.update(camera, window, 1/50.0)) {
            renderer.setCamera(camera);
        }

        if (window.isKeyPressed('r')) {
            regenWorld();
        }

        if (window.isKeyPressed('p')) {
            glm::vec3 pos, dir;
            std::tie(pos, dir) = renderer.unproject(
                window.getNDCPos(window.getMousePos()));
            auto pick = world.getChunks().pick(pos, dir, 10);
            if (pick) {
                std::cout << "Picked "
                          << pick->x << " "
                          << pick->y << " "
                          << pick->z << std::endl;
            } else {
                std::cout << "No pick" << std::endl;
            }
        }

        if (window.isMousePressed(MouseButton::RIGHT)) {
            glm::vec3 pos, dir;
            std::tie(pos, dir) = renderer.unproject(
                window.getNDCPos(window.getMousePos()));
            auto pick = world.getChunks().pick(pos, dir, 10);
            if (pick) {
                glm::ivec3 chunkpos, blockpos;
                std::tie(chunkpos, blockpos) = ChunkGrid::posToChunkBlock(*pick);
                std::unique_ptr<Chunk> newchunk{
                    new Chunk(*world.getChunks().getChunk(chunkpos))};
                newchunk->getBlock(blockpos) = Block::air();
                world.getChunks().setChunk(chunkpos, std::move(newchunk));
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
            if (world.generateChunk(chunkpos))
                break;
        }
        
        window.clear();

        for (const auto &meshpos : world.getChunks().getMeshPoses()) {
            glm::mat4 model{1};
            model = glm::translate(model, glm::vec3{32*meshpos.first});
            renderer.render(model, meshpos.second);
        }

        window.swapBuffers();

        timer.expires_at(timer.expires_at() + rate);
        timer.async_wait(render);
    };

    timer.async_wait(render);
    io.run();

    return 0;
}
