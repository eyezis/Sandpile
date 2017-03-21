#pragma once

#include "ofMain.h"
#include <vector>
#include <stdio.h>

// note: PALETTE_COUNT is zero-based
#define PALETTE_COUNT 255
typedef unsigned int surface_t;

template <class T> class bivector {
private:
    T defaultValue;
    std::vector<T> left; // i < 0
    std::vector<T> right; // i >= 0

public:
    bivector() {
    }

    bivector(T dv) : defaultValue(dv) {
    }

    void init(T dv){
        defaultValue = dv;
    }

    T& operator[](const intptr_t i) {
        if (i < 0) {
            if (static_cast<size_t>(-i) > left.size())
                left.resize(static_cast<size_t>(-i), defaultValue);
            return left[static_cast<size_t>(-i - 1)];
        }

        if (static_cast<size_t>(i + 1) > right.size())
            right.resize(static_cast<size_t>(i + 1), defaultValue);
        return right[static_cast<size_t>(i)];
    }
};

class SandpileSurface {
private:
    bivector<bivector<surface_t>> surface;

public:
    intptr_t leftBounds, rightBounds;
    intptr_t topBounds, bottomBounds;

    SandpileSurface() : leftBounds(0), rightBounds(0), topBounds(0), bottomBounds(0) {
        surface.init(bivector<surface_t>(0));
    }

    void set(const intptr_t x, const intptr_t y, const surface_t v) {
        if (v > 0) {
            if (x > rightBounds) rightBounds = x;
            else if (x < leftBounds) leftBounds = x;

            if (y > bottomBounds) bottomBounds = y;
            else if (y < topBounds) topBounds = y;
        }

        surface[x][y] = v;
    }
    surface_t operator()(const intptr_t x, const intptr_t y) {
        return surface[x][y];
    }

    size_t getWidth() {
        return static_cast<size_t>(rightBounds - leftBounds + 1);
    }
    size_t getHeight() {
        return static_cast<size_t>(bottomBounds - topBounds + 1);
    }
};

struct rgb
{
    unsigned char r, g, b;
};

class ofApp : public ofBaseApp{
    public:
        rgb palette[PALETTE_COUNT+1];
        ofColor background;
        ofImage buffer;

        SandpileSurface surface1;
        SandpileSurface surface2;
        bool surface1IsSource;

        void processFalls();
        void renderSurface();

        void setup();
        void update();
        void draw();
};
