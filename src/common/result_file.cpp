#include "result_file.h"

#include <limits>

ResultFile::ResultFile(const std::string &path, bool rescale/* = false*/) : d_current_frame(0) {
    d_stream.open(path, std::ios::in | std::ios::binary);

    // Get size of file
    d_stream.ignore(std::numeric_limits<std::streamsize>::max());
    std::streamsize size = d_stream.gcount();
    d_stream.clear();
    d_stream.seekg(0, std::ios_base::beg);

    // Read and calculate counts
    d_stream.read((char*) &d_num_bodies, sizeof(size_t));
    d_count_frames = (size - sizeof(size_t) - d_num_bodies * sizeof(float)) / (3 * d_num_bodies * sizeof(float));

    // Read in frame 0 of data
    d_masses.resize(d_num_bodies);
    d_positions.resize(3 * d_num_bodies);
    d_stream.read((char*) d_masses.data(), d_num_bodies * sizeof(float));
    d_stream.read((char*) d_positions.data(), 3 * d_num_bodies * sizeof(float));

    // Normalize positions
    d_scale = 1.0;
    if (rescale) {
        for (int i = 0; i < d_positions.size(); i++) {
            d_scale = std::max(d_scale, std::abs(d_positions[i]));
        }
        d_scale = 1 / d_scale;
        for (int i = 0; i < d_positions.size(); i++) {
            d_positions[i] = d_positions[i] * d_scale;
        }
    }

    // Normalize masses
    float maxMass = std::numeric_limits<float>::min();
    float minMass = std::numeric_limits<float>::max();
    for (float mass : d_masses) {
        maxMass = std::max(mass, maxMass);
        minMass = std::min(mass, minMass);
    }
    for (int i = 0; i < d_masses.size(); i++) {
        d_masses[i] = (d_masses[i] - minMass) / (maxMass - minMass);
    }
}

ResultFile::~ResultFile() {
    if (d_stream.is_open())
        d_stream.close();
}

void ResultFile::seek(size_t frame) {
    if (frame >= 0 && frame < d_count_frames && frame != d_current_frame) {
        d_stream.seekg(sizeof(size_t) + d_num_bodies * sizeof(float) + frame * 3 * d_num_bodies * sizeof(float),
           std::ios::beg);
        d_stream.read((char *) d_positions.data(), 3 * d_num_bodies * sizeof(float));
        for (int i = 0; i < d_positions.size(); i++) {
            d_positions[i] = d_positions[i] * d_scale;
        }
        d_current_frame = frame;
    }
}
