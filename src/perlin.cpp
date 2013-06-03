#include <random>
#include <glm/glm.hpp>
#include <cstdint>

static glm::vec3 getGVec(const glm::ivec3 &pos, uint32_t seed) {
    uint32_t pos_seed = ((pos.x & 0xFF) << 16) + ((pos.y & 0xFF) << 8) + (pos.z & 0xFF);

    std::minstd_rand rand(pos_seed ^ seed);
    rand.discard(10);

    std::uniform_real_distribution<float> reals(-1, 1);
    return glm::vec3{reals(rand), reals(rand), reals(rand)};
}

static float curve(float t) {
    return t*t*(-t*2 + 3);
}

float perlin3(const glm::vec3 &P, uint32_t seed) {
    glm::ivec3 iP{floor(P.x), floor(P.y), floor(P.z)};
    glm::vec3 fP = P - glm::vec3{iP};

    float zvals[2];
    for (int z : {0, 1}) {
        float yvals[2];
        for (int y : {0, 1}) {
            float xvals[2];
            for (int x : {0, 1}) {
                glm::ivec3 Q = iP + glm::ivec3{x, y, z};
                glm::vec3 G = getGVec(Q, seed);
                xvals[x] = glm::dot(G, P - glm::vec3{Q});
            }
            yvals[y] = glm::mix(xvals[0], xvals[1], curve(fP.x));
        }
        zvals[z] = glm::mix(yvals[0], yvals[1], curve(fP.y));
    }
    return glm::mix(zvals[0], zvals[1], curve(fP.z));
}
