#pragma once

class Texture {

public:
    Texture(const char* start, const char* end, int textureOffset);
    ~Texture();

    void bind() const;

private:
    int width;
    int height;
    int nrChannels;

    unsigned int texture;

};
