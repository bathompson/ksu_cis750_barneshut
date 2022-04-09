#pragma once

#include <string>
#include <fstream>
#include <vector>

class ResultFile {

public:
    explicit ResultFile(const std::string& path);
    ~ResultFile();

    size_t num_bodies() const { return d_num_bodies; };
    size_t count_frames() const { return d_count_frames; };
    size_t current_frame() const { return d_current_frame; };

    const float* positions() const { return d_positions.data(); }
    const float* masses() const { return d_masses.data(); }

    void seek(size_t frame);

private:
    std::fstream d_stream;

    size_t d_num_bodies;
    size_t d_count_frames;
    size_t d_current_frame;

    std::vector<float> d_positions;
    std::vector<float> d_masses;
};
