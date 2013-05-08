#ifndef IMAGE_H
#define IMAGE_H

#include <vector>
#include <stdexcept>
#include <ostream>
#include <cstdint>

struct __attribute__((__packed__)) Pixel {
    Pixel() { }
    Pixel(uint8_t r, uint8_t g, uint8_t b, uint8_t a=255) :
        r(r), g(g), b(b), a(a) { }

    union {
        struct {
            uint8_t r;
            uint8_t g;
            uint8_t b;
            uint8_t a;
        };

        uint32_t rgba;
    };
};

std::ostream &operator<<(std::ostream &out, Pixel p);

class Image {
public:
    Image() : width(0), height(0) { }
    Image(unsigned int width, unsigned int height, double gamma=1.0, Pixel p = Pixel{0, 0, 0});

    unsigned int getWidth() const { return width; }
    unsigned int getHeight() const { return height; }
    void resize(unsigned int width, unsigned int height);

    float getGamma() const { return gamma; }

    Pixel &operator()(unsigned int x, unsigned int y) { return pixels[x+y*width]; }
    Pixel operator()(unsigned int x, unsigned int y) const { return pixels[x+y*width]; }
    Pixel *getData() { return &pixels.front(); }
    const Pixel *getData() const { return &pixels.front(); }

    class PNGException : public std::runtime_error {
    public:
        using std::runtime_error::runtime_error;
    };
    static Image loadPNG(const std::string &png);

private:
    unsigned int width;
    unsigned int height;
    double gamma;
    std::vector<Pixel> pixels;
};

#endif
