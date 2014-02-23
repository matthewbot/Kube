#include "gfx/Image.h"

#include <cstdio>
#include <png.h>
#include <algorithm>

std::ostream &operator<<(std::ostream &out, Pixel p) {
    out << "(" << static_cast<int>(p.r) << ", "
        << static_cast<int>(p.g) << ", "
        << static_cast<int>(p.b) << ", "
        << static_cast<int>(p.a) << ")";
    return out;
}

Image::Image(unsigned int width, unsigned int height, double gamma, Pixel p) :
    width(width),
    height(height),
    gamma(gamma),
    pixels(width*height, p)
{ }

void Image::resize(unsigned int width, unsigned int height) {
    this->width = width;
    this->height = height;
    pixels.resize(width*height);
}

Image Image::flipped() const {
    Image ret;
    ret.width = width;
    ret.height = height;
    ret.gamma = gamma;
    ret.pixels.resize(pixels.size());

    for (unsigned int y = 0; y < height; y++) {
        auto dest = ret.pixels.begin() + (height-1-y)*width;
        auto begin = pixels.begin() + y*width;
        auto end = begin + width;
        std::copy(begin, end, dest);
    }

    return ret;
}

Image Image::flippedArray(unsigned int n) const {
    Image ret;
    ret.width = width;
    ret.height = height;
    ret.gamma = gamma;
    ret.pixels.resize(pixels.size());

    auto ah = height / n;
    for (unsigned int y = 0; y < height; y++) {
        auto dest = ret.pixels.begin() + ((y/ah)*ah + ah-1-(y%ah))*width;
        auto begin = pixels.begin() + y*width;
        auto end = begin + width;
        std::copy(begin, end, dest);
    }

    return ret;
}


void Image::blit(const Image &src, unsigned int x, unsigned int y) {
    for (unsigned int sy = 0; sy < src.getHeight(); sy++) {
        for (unsigned int sx = 0; sx < src.getWidth(); sx++) {
            (*this)(sx + x, sy + y) = src(sx, sy);
        }
    }
}

Image Image::loadPNG(std::istream &in) {
    png_structp png_ptr = png_create_read_struct(
        PNG_LIBPNG_VER_STRING,
        nullptr, nullptr, nullptr);
    if (!png_ptr) {
        throw PNGException("Failed to create read struct");
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        png_destroy_read_struct(&png_ptr, nullptr, nullptr);
        throw PNGException("Failed to create info struct");
    }
    
    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
        throw PNGException("Error reading PNG");
    }

    auto read_data_fn = [](png_structp png_ptr, png_bytep out, png_size_t count) {
        std::istream &in = *reinterpret_cast<std::istream *>(png_get_io_ptr(png_ptr));
        in.read(reinterpret_cast<char *>(out), count);
        if (!in) {
            png_error(png_ptr, "Failed to read from istream");
        }
    };
    png_set_read_fn(png_ptr, &in, read_data_fn);
    
    png_read_info(png_ptr, info_ptr);

    png_uint_32 width, height;
    int bit_depth;
    int color_type;
    int interlace_type;
    png_get_IHDR(png_ptr, info_ptr, &width, &height,
                 &bit_depth, &color_type, &interlace_type,
                 nullptr, nullptr);

    if (interlace_type != PNG_INTERLACE_NONE) {
        png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
        throw PNGException("Can't handle interlaced PNG");
    }

    if (bit_depth == 16)
        png_set_strip_16(png_ptr);
    else if (bit_depth < 8)
        png_set_packing(png_ptr);

    if (color_type == PNG_COLOR_TYPE_PALETTE) {
        png_set_palette_to_rgb(png_ptr);
        png_set_filler(png_ptr, 0xFF, PNG_FILLER_AFTER);
    }

    if (color_type == PNG_COLOR_TYPE_RGB)
        png_set_filler(png_ptr, 0xFF, PNG_FILLER_AFTER);

    if (color_type == PNG_COLOR_TYPE_GRAY) {
        png_set_gray_to_rgb(png_ptr);
        png_set_filler(png_ptr, 0xFF, PNG_FILLER_AFTER);
    }

    if (color_type == PNG_COLOR_TYPE_GRAY_ALPHA) {
        png_set_gray_to_rgb(png_ptr);
    }

    double gamma;
    if (!png_get_gAMA(png_ptr, info_ptr, &gamma)) {
        gamma = 0.45455;
    }

    Image image{static_cast<unsigned int>(width),
                static_cast<unsigned int>(height),
                gamma};

    for (unsigned int row = 0; row < height; row++) {
        uint8_t *row_ptr = reinterpret_cast<uint8_t *>(&image(0, row));
        png_read_row(png_ptr, row_ptr, nullptr);
    }

    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);

    return image;
}
