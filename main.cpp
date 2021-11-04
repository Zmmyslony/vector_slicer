#include <iostream>
#include "./libs/high_level/AutomaticPathGeneration.h"

double VERSION = 0.1;
// Best seed out of 200: 193

std::string mainDirectory = R"(C:\Work\Cambridge\printer\Vector Slicer Patterns)";


int main() {
    printf("\n\tVector slicer version %.1f.", VERSION);

    std::string radial = mainDirectory + R"(\radial, r = 1 cm)";
    std::string azimuthal = mainDirectory + R"(\azimuthal, r = 1 cm)";
    std::string diagonal = mainDirectory + R"(\diagonal, 2x1 cm)";
    std::string linear = mainDirectory + R"(\linear, 2x1 cm)";
    std::string spiral = mainDirectory + R"(\spiral, r = 1 cm)";

    std::vector<std::string> allPatterns = {radial, azimuthal, diagonal, linear, spiral};
    for (auto& patternType : allPatterns) {
        generatePrintPattern(patternType, 1, 100);
    }
//    generatePrintPattern(diagonal, 1, 5);
//    generatePrintPattern(azimuthal, 1, 100);
//    generatePrintPatternsMultithread(linear, 1, 5);
    return 0;
}
