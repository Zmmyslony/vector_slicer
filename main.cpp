#include "./libs/auxiliary/GCodeFile.h"
#include <boost/filesystem.hpp>
#include <boost/dll.hpp>
#include <string>
#include <iostream>
#include <fstream>
#include "./libs/high_level/ReadingFromOutside.h"
#include "./libs/auxiliary/TableReading.h"

namespace fs = boost::filesystem;

const double VERSION = 1.0;

std::vector<std::string> getPatterns(const std::string &listOfPatternsPath) {
    std::vector<std::string> patterns;
    std::string line;
    std::fstream file(listOfPatternsPath);

    while (std::getline(file, line)) {
        if (line.find("ignore") != std::string::npos) {}
        else {
            patterns.push_back(line);
        }
    }
    return patterns;
}

std::vector<int> readConfig(const std::string &filename) {
    std::vector<std::vector<int>> table = readFileToTableInt(filename);
    return table[0];
}


int main() {

    printf("\n\tVector slicer version %.1f.\n", VERSION);
//    std::string exePath = getExePath();
    fs::path exePath = boost::dll::program_location().parent_path();
    fs::path configPath = exePath / "config.txt";
    fs::path optimizerPath = exePath / "optimizationSequence.txt";
    fs::path patternsPath = exePath / "filesToTest.txt";

//    std::string configPath = exePath + R"(\config.txt)";
//    std::string optimizerPath = exePath + R"(\optimizationSequence.txt)";
//    std::string patternsPath = exePath + R"(\filesToTest.txt)";
//
    std::vector<int> config = readConfig(configPath.string());
    std::vector<std::string> patterns = getPatterns(patternsPath.string());

    printf("\nTested patterns: \n");
    for (auto &patternType: patterns) {
        std::cout << "\t" << patternType << std::endl;
    }
    printf("Testing seeds from %d to %d using %d threads.\n", config[0], config[1], config[2]);

    for (auto &patternType: patterns) {
        generalFinderString(patternType, config[0], config[1], config[2], optimizerPath.string());
        generateGCode(patternType, 30, 10, std::valarray<double>({0, 10}), 0.020);
    }

//  const std::string mainDirectory = R"(C:\Work\Cambridge\printer\Vector Slicer Patterns)";
////    std::string radial = mainDirectory + R"(\radial, r = 1 cm)";
////    std::string azimuthal = mainDirectory + R"(\azimuthal, r = 1 cm)";
////    std::string diagonal = mainDirectory + R"(\diagonal, 2x1 cm)";
////    std::string linear = mainDirectory + R"(\linear, 2x1 cm)";
////    std::string spiral = mainDirectory + R"(\spiral, r = 1 cm)";
//
//    std::string radial = mainDirectory + R"(\radial, r = 0.5 cm)";
//    std::string azimuthal = mainDirectory + R"(\azimuthal, r = 0.5 cm)";
//    std::string diagonal = mainDirectory + R"(\diagonal, 1x0.5 cm)";
//    std::string linear = mainDirectory + R"(\linear, 1x0.5 cm)";
//    std::string spiral = mainDirectory + R"(\spiral, r = 0.5 cm)";
//    std::string newSpiral = mainDirectory + R"(\new spiral, r = 0.5 cm)";
//
//
//    std::vector<std::string> allPatterns = {radial, azimuthal, diagonal, linear};
//
//    std::string symmetricPositive = mainDirectory + R"(\symmetricPositive, 2x0.6 cm)";
//    std::string symmetricPositiveLarger = mainDirectory + R"(\symmetricPositive, 4x1.2 cm)";
//    std::string diagonalTeethed = mainDirectory + R"(\diagonal teethed, 1x0.5 cm)";
//    std::string test = mainDirectory + R"(\test)";
//    allPatterns = {linear};
//
//    for (auto &patternType: allPatterns) {
//        findBestConfig(patternType, 1, 1, 12);
////        findBestSeed(patternType, 1, 200, 12);
////        recalculateBestConfig(patternType);
//
//        generateGCode(patternType, 30, 10, std::valarray<double>({0, 10}), 0.020);
//    }
    return 0;
}