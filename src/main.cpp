#include "Shader.h"
#include "Buffer.h"
#include "Chunk.h"
#include "Image.h"
#include "Texture.h"
#include "Window.h"
#include "Renderer.h"
#include <unistd.h>
#include <iostream>
#include <tuple>
#include <GL/glew.h>
#include <GL/glfw.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

const float pi = static_cast<float>(M_PI);

int main(int argc, char **argv) {
    Window window{800, 600};

    Image img = Image::loadPNG("blocks.png");
    ArrayTexture tex{img, 4};

    Sampler sampler{Sampler::NEAREST};

    Shader vert{Shader::Type::VERTEX, "vert.glsl"};
    Shader frag{Shader::Type::FRAGMENT, "frag.glsl"};
    ShaderProgram prgm{vert, frag};

    BlockType stone;
    stone.setAllFaceTextureNums(0);

    BlockType grass;
    grass.setAllFaceTextureNums(3);
    grass.setFaceTextureNum(Face::TOP, 2);

    Chunk chunk = Chunk::genRandom({Block(stone), Block(grass)}, .2);
    Mesh chunk_mesh = chunk.tesselate();

    PerspectiveProjection projection;
    projection.aspect = window.getAspectRatio();

    Renderer renderer;
    renderer.setWindow(window);
    renderer.setProjection(projection);
    renderer.setProgram(prgm);
    renderer.setTexture(0, tex, sampler);

    RPYCameraManipulator camera_manipulator{.002, 2};
    RPYCamera camera;

    while (!window.isClosed()) {
        if (camera_manipulator.update(camera, window, 1/50.0)) {
            renderer.setCamera(camera);
        }

        if (window.isKeyPressed('p')) {
            std::pair<glm::vec3, glm::vec3> ray =
                renderer.unproject(window.getNDCPos(window.getMousePos()));

            auto pick_result = chunk.pick(ray.first, ray.second, 30);
            if (pick_result) {
                chunk(pick_result->first) = Block::air();
                chunk_mesh = chunk.tesselate();
            }
        }

        window.clear();
        renderer.render(chunk_mesh);

        window.swapBuffers();
        usleep(20000);
    }

    glfwTerminate();
    return 0;
}
