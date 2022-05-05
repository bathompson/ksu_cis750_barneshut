#include "common/result_file.h"
#include <memory>

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Requires arguments of two input files\n");
    }

    // Load to input files
    auto expected = std::make_shared<ResultFile>(argv[1]);
    auto actual = std::make_shared<ResultFile>(argv[2]);

    // For each frame
    for (int i = 0; i < expected->count_frames(); i++) {
        expected->seek(i);
        actual->seek(i);
        float totalError = 0;
        // For each position component
        for (int j = 0; j < expected->num_bodies() * 3; j++) {
            // Calculate percent error for that component
            float expectedVal = *(expected->positions() + j);
            float actualVal = *(actual->positions() + j);
            float compPercentError = abs((actualVal - expectedVal) / expectedVal);
            totalError += compPercentError;
        }
        float avgPercentError = totalError / (expected->num_bodies() * 3);
        // Output is CSV to allow some nice looking graphs
        printf("%d, %.3f\n", i, avgPercentError);
    }
}
