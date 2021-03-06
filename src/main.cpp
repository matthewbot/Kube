#include "lua/Lua.h"
#include "lua/MetatableBuilder.h"
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
#include "perlin.h"
#include "gfx/Font.h"
#include "gfx/TextureArrayBuilder.h"
#include "gfx/WorldView.h"
#include "gfx/DebugView.h"
#include "gfx/SimpleBlockVisual.h"
#include "gfx/PlantBlockVisual.h"
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
#include <fstream>

const float pi = static_cast<float>(M_PI);

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
        const auto &air = blocktypes.getType("air");
        const auto &grass = blocktypes.getType("grass");
        const auto &dirt = blocktypes.getType("dirt");
        const auto &stone = blocktypes.getType("stone");
        const auto &tall_grass = blocktypes.getType("tall_grass");

        std::unique_ptr<Chunk> chunk{new Chunk{blocktypes}};
        chunk->fill(air);

        for (auto &pos : ChunkIndex::range) {
            glm::vec3 worldpos = static_cast<glm::vec3>(chunkpos) +
                static_cast<glm::vec3>(pos.getVec())/32.0f;
            chunk->setBlock(pos, solid(worldpos) ? stone : air);
        }

        for (int x=0; x<Chunk::XSize; x++) {
            for (int y=0; y<Chunk::YSize; y++) {
                int ctr = 0;

                glm::vec3 pos_above{chunkpos.x + x/32.0f,
                                    chunkpos.y + y/32.0f,
                                    chunkpos.z+1};
                if (solid(pos_above))
                    continue;

                bool has_tall_grass = perlin3(pos_above, seed ^ 0x02) > 0.2f;
                
                for (int z=Chunk::ZSize-1; z>=0; z--) {
                    ChunkIndex idx{x, y, z};
                    auto b = chunk->getBlock(idx);
                    if (b.getType() == stone) {
                        if (ctr == 0) {
                            chunk->setBlock(idx, has_tall_grass ? tall_grass : grass);
                        } else if (ctr == 1) {
                            chunk->setBlock(idx, has_tall_grass ? grass : dirt);
                        } else {
                            chunk->setBlock(idx, dirt);
                        }

                        if (++ctr >= 4) {
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

static void buildMetaTables(Lua &lua) {
    MetatableBuilder<FaceMap<unsigned int>>(lua, "FaceMapUInt")
        .index<Face, unsigned int>()
        .function("fill", &FaceMap<unsigned int>::fill);

    MetatableBuilder<FaceMap<std::string>>(lua, "FaceMapString")
        .index<Face, std::string>()
        .function("fill", &FaceMap<std::string>::fill);
    
    MetatableBuilder<BlockTypeInfo>(lua, "BlockTypeInfo")
        .constructor("new")
        .field("solid", &BlockTypeInfo::solid);

    MetatableBuilder<const BlockType>(lua, "ConstBlockType")
//        .field("solid", &BlockType::solid) // TODO, base class fields?
        .getter("id", &BlockType::id)
        .getter("name", &BlockType::name);
    
    MetatableBuilder<BlockTypeRegistry>(lua, "BlockTypeRegistry")
        .function_ref("makeType", &BlockTypeRegistry::makeType);

    MetatableBuilder<BlockVisualInfo>(lua, "BlockVisualInfo");
    
    MetatableBuilder<SimpleBlockVisualInfo>(lua, "SimpleBlockVisualInfo")
        .constructor("new")
        .constructor<std::string>("newFromFilename")
        .field_ref("face_tex_filenames", &SimpleBlockVisualInfo::face_tex_filenames)
        .downCast<BlockVisualInfo>("toBlockVisualInfo");

    MetatableBuilder<PlantBlockVisualInfo>(lua, "PlantBlockVisualInfo")
        .constructor("new")
        .field("tex_filename", &PlantBlockVisualInfo::tex_filename)
        .downCast<BlockVisualInfo>("toBlockVisualInfo");

    MetatableBuilder<const BlockVisual>(lua, "ConstBlockVisual");
    
    MetatableBuilder<BlockVisualRegistry>(lua, "BlockVisualRegistry")
        .function_ref("makeVisual", &BlockVisualRegistry::makeVisual);
}

static std::unique_ptr<View> buildWorldView(ThreadManager &tm,
                                            World &world,
                                            BlockVisualRegistry &blockvisuals) {
    Sampler sampler;
    sampler.setFilter(Sampler::NEAREST);

    Shader vert{Shader::Type::VERTEX, "vert.glsl"};
    Shader frag{Shader::Type::FRAGMENT, "frag.glsl"};
    ShaderProgram prgm{vert, frag};
    
    return std::unique_ptr<View>{
        new WorldView{
            tm,
            world,
            std::move(sampler),
            std::move(prgm),
            std::move(blockvisuals)}};
}

static std::unique_ptr<View> buildDebugView() {
    Font font{"font.fnt"};

    Shader vert{Shader::Type::VERTEX, "vert2d.glsl"};
    Shader frag{Shader::Type::FRAGMENT, "frag2d.glsl"};
    ShaderProgram prgm{vert, frag};

    return std::unique_ptr<View>{new DebugView{
        std::move(font), std::move(prgm)}};
}

int main(int argc, char **argv) {
    ThreadManager tm;

    tm.postWorkAll([](WorkerThread &th) {
        auto &lua = th.cacheLocal<Lua>("lua");
        buildMetaTables(lua);
        lua.doFile("game.lua");
    });

    BlockTypeRegistry blocktypes;
    BlockVisualRegistry blockvisuals{16};
    
    tm.postWork([&](WorkerThread &th) {
        auto &lua = th.cacheLocal<Lua>("lua");
        lua.call<void>("register_blocktypes", std::ref(blocktypes), std::ref(blockvisuals));
    });

    tm.syncWork();
    blocktypes.dump(std::cout);

    const auto &air = blocktypes.getType("air");

    TestWorldGenerator gen;
    World world(blocktypes, gen, tm);

    auto regenWorld = [&]() {
        world.getChunks().clearAllChunks();
        gen.reseed(rand());
    };
    regenWorld();

    GraphicsSystem gfx{tm};
    gfx.pushView(buildWorldView(tm, world, blockvisuals));
    gfx.pushView(buildDebugView());

    auto &worldview = gfx.getView<WorldView>(0);
//    auto &debugview = gfx.getView<DebugView>(1);

    worldview.getCamera().pos.z = 40;

    RPYCameraManipulator camera_manipulator{.002, 5};

    gfx.runRenderLoop([&]() -> bool {
        Window &window = gfx.getWindow();
        RPYCamera &camera = worldview.getCamera();

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
                worldview.getProjection(window).getMatrix(),
                camera.getMatrix(),
                window.getNDCPos(window.getMousePos()));
            auto pick = world.getChunks().pick(pos, dir, 10);
            if (pick) {
                glm::ivec3 chunkpos, blockpos;
                std::tie(chunkpos, blockpos) = ChunkGrid::posToChunkBlock(*pick);
                std::unique_ptr<Chunk> newchunk{
                    new Chunk(*world.getChunks().getChunk(chunkpos))};
                newchunk->setBlock(ChunkIndex{blockpos}, air);
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

    tm.stopThreads();
    
    return 0;
}
