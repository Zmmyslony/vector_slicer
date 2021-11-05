#include <iostream>
#include "./libs/high_level/AutomaticPathGeneration.h"

double VERSION = 0.2;
// Best seed out of 200: 193

std::string mainDirectory = R"(C:\Work\Cambridge\printer\Vector Slicer Patterns)";


int main() {
    printf("\n\tVector slicer version %.1f.", VERSION);

//    std::string radial = mainDirectory + R"(\radial, r = 1 cm)";
//    std::string azimuthal = mainDirectory + R"(\azimuthal, r = 1 cm)";
//    std::string diagonal = mainDirectory + R"(\diagonal, 2x1 cm)";
//    std::string linear = mainDirectory + R"(\linear, 2x1 cm)";
//    std::string spiral = mainDirectory + R"(\spiral, r = 1 cm)";

    std::string radial = mainDirectory + R"(\radial, r = 0.5 cm)";
    std::string azimuthal = mainDirectory + R"(\azimuthal, r = 0.5 cm)";
    std::string diagonal = mainDirectory + R"(\diagonal, 1x0.5 cm)";
    std::string linear = mainDirectory + R"(\linear, 1x0.5 cm)";
    std::string spiral = mainDirectory + R"(\spiral, r = 0.5 cm)";

    std::vector<std::string> allPatterns = {radial, azimuthal, diagonal, linear, spiral};
    for (auto &patternType : allPatterns) {
        generatePrintPatternMultithreading(patternType, 1, 1000, 12);
    }
    return 0;
}
