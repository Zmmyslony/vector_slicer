#include "./libs/high_level/FillingOptimization.h"
#include "./libs/auxiliary/GCodeFile.h"

double VERSION = 0.6;


int main() {
    const std::string mainDirectory = R"(C:\Work\Cambridge\printer\Vector Slicer Patterns)";

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
    std::string newSpiral = mainDirectory + R"(\new spiral, r = 0.5 cm)";

    std::vector<std::string> allPatterns = {radial, azimuthal, diagonal, linear};

    std::string symmetricPositive = mainDirectory + R"(\symmetricPositive, 2x0.6 cm)";
    std::string symmetricPositiveLarger = mainDirectory + R"(\symmetricPositive, 4x1.2 cm)";
    std::string diagonalTeethed = mainDirectory + R"(\diagonal teethed, 1x0.5 cm)";
    std::string test = mainDirectory + R"(\test)";
    allPatterns = {test};

    for (auto &patternType: allPatterns) {
        findBestConfig(patternType, 1, 10, 12);
//        findBestSeed(patternType, 1, 200, 12);
//        recalculateBestConfig(patternType);

        generateGCode(patternType, 30, 10, std::valarray<double>({0, 10}), 0.020);
    }
    return 0;
}