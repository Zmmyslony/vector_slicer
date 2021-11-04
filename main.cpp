#include <iostream>
#include "./libs/high_level/AutomaticPathGeneration.h"

double VERSION = 0.2;
// Best seed out of 200: 193

int main() {
    printf("\n\tVector slicer version %.1f.", VERSION);

    std::string radial = R"(C:\Work\Cambridge\printer\Vector Slicer Patterns\radial, r = 1 cm)";
    std::string azimuthal = R"(C:\Work\Cambridge\printer\Vector Slicer Patterns\azimuthal, r = 1 cm)";
    std::string diagonal = R"(C:\Work\Cambridge\printer\Vector Slicer Patterns\diagonal, 2x1 cm)";
    std::string linear = R"(C:\Work\Cambridge\printer\Vector Slicer Patterns\linear, 2x1 cm)";
    std::string spiral = R"(C:\Work\Cambridge\printer\Vector Slicer Patterns\spiral, r = 1 cm)";

    std::vector<std::string> allPatterns = {radial, azimuthal, diagonal, linear, spiral};
    for (auto& patternType : allPatterns) {
        generatePrintPattern(patternType, 1, 100);
    }
//    generatePrintPattern(radial, 1, 50);
//    generatePrintPattern(azimuthal, 1, 100);
//    generatePrintPatternsMultithread(linear, 1, 5);
    return 0;
}
