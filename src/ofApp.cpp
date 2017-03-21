#include "ofApp.h"

void ofApp::setup() {
    ofSetWindowTitle("Sandpile");
    ofSetVerticalSync(false);
    background.set(0);

    ofColor c;
    const float limit = c.limit();
    for (size_t i = 0; i <= PALETTE_COUNT; i++)
    {
        const float p = static_cast<float>(i) / PALETTE_COUNT;

        float hue = (1 - p) - 1 / 6.0f;
        if (hue <= 0)
            hue = 0;
        else if (hue >= 2 / 3.0f)
            hue = 2 / 3.0f;

        float saturation = (1 - p) * 4;
        saturation = saturation < 1 ? saturation : 1;

        float brightness = p * 4;
        brightness = brightness < 1 ? brightness : 1;

        c.setHsb(hue * limit, saturation * limit, brightness * limit);
        const int colour = c.getHex();
        palette[i].r = (colour >> 16) & 0xFF;
        palette[i].g = (colour >> 8) & 0xFF;
        palette[i].b = colour & 0xFF;
    }

    buffer = ofImage();
    // Disable AA on the image
    buffer.getTexture().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);

    //td:image loading
    surface1IsSource = true;
    surface1.set(0, 0, 65536);
}

void ofApp::processFalls()
{
    auto &source = surface1IsSource ? surface1 : surface2;
    auto &dest = surface1IsSource ? surface2 : surface1;

    for (auto x = source.leftBounds-1; x <= source.rightBounds+1; x++)
    {
        for (auto y = source.topBounds-1; y <= source.bottomBounds+1; y++)
        {
            dest.set(x, y, source(x, y) % 4
                     + source(x-1, y) / 4
                     + source(x+1, y) / 4
                     + source(x, y-1) / 4
                     + source(x, y+1) / 4
                    );
        }
    }

    surface1IsSource = !surface1IsSource;
}

// Draws every sandpile on the destination surface to the buffer
void ofApp::renderSurface()
{
    auto &surface = surface1IsSource ? surface1 : surface2;

    const size_t currentWidth = surface.getWidth();
    const size_t currentHeight = surface.getHeight();
    if (currentWidth != static_cast<size_t>(buffer.getWidth()) || currentHeight != static_cast<size_t>(buffer.getHeight()))
        buffer.allocate(static_cast<int>(currentWidth), static_cast<int>(currentHeight), OF_IMAGE_COLOR);

    buffer.setColor(background);

    ofPixels &pixels = buffer.getPixels();
    for (auto x = surface.leftBounds; x <= surface.rightBounds; x++)
    {
        for (auto y = surface.topBounds; y <= surface.bottomBounds; y++)
        {
            //if (surface(x, y) == 0) continue;

            const rgb &color = palette[std::min(
                static_cast<size_t>(surface(x, y) / 32.0f * PALETTE_COUNT),
                static_cast<size_t>(PALETTE_COUNT))];

            const size_t i = (static_cast<size_t>(y - surface.topBounds) * currentWidth + static_cast<size_t>(x - surface.leftBounds))*3;
            pixels[i] = color.r;
            pixels[i+1] = color.g;
            pixels[i+2] = color.b;
        }
    }
    buffer.update();
}

void ofApp::update(){
    if (ofGetFrameNum() > 0) processFalls(); // skip first frame to show the initial state of the sandpile
    renderSurface();

    static int lastWidth = 0, lastHeight = 0;

    const int widthChange = std::abs(lastWidth - ofGetWidth());
    const int heightChange = std::abs(lastHeight - ofGetHeight());
    if (widthChange > heightChange) {
        //adjust height to match width
        ofSetWindowShape(ofGetWidth(), static_cast<int>(buffer.getHeight() / buffer.getWidth() * ofGetWidth()));
    } else if (heightChange > widthChange) {
        //adjust width to match height
        ofSetWindowShape(static_cast<int>(buffer.getWidth() / buffer.getHeight() * ofGetHeight()), ofGetHeight());
    }

    lastWidth = ofGetWidth(); lastHeight = ofGetHeight();
}

void ofApp::draw(){
    buffer.draw(0, 0, ofGetWidth(), ofGetHeight());
}
