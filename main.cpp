#include <iostream>
#include "./libs/gui/OpenFiles.h"
#include "./libs/pattern/FillingPatterns.h"
#include "./libs/pattern/QuantifyPattern.h"
#include <ctime>
#include <windows.h>
#include <thread>

double VERSION = 0.1;
// Best seed out of 200: 193


FilledPattern generateAPrintPattern(std::string directorPath, DesiredPattern desiredPattern, int seed) {
    FilledPattern pattern = openFilledPatternFromDirectoryAndPattern(directorPath, desiredPattern, seed);
    fillWithPaths(pattern);

    return pattern;
}

void exportPatternToDirectory(const FilledPattern& pattern, const std::string& directorPath, const int& seed) {
    std::string resultsDirectory = directorPath + R"(\results\)";
    std::string patternDirectory = directorPath + R"(\results\seed_)" + std::to_string(seed);

    CreateDirectory(resultsDirectory.c_str(), nullptr);
    CreateDirectory(patternDirectory.c_str(), nullptr);
    pattern.exportToDirectory(patternDirectory);
}


void generateAndExportPrintPattern(const std::string& directorPath, const DesiredPattern& desiredPattern, int seed) {
    FilledPattern pattern = generateAPrintPattern(directorPath, desiredPattern, seed);
    exportPatternToDirectory(pattern, directorPath, seed);
}

void generatePrintPattern(std::string& directorPath, int minSeed, int maxSeed) {
    clock_t tStart = clock();
    printf("\nReading the pattern.\n");
    DesiredPattern desiredPattern = openPatternFromDirectory(directorPath);
    printf("Pattern read in %.2fs.\n", (double)(clock() - tStart) / CLOCKS_PER_SEC);

    int bestSeed = 0;
    double bestDisagreement = 1000;
    for (int currentSeed = minSeed; currentSeed <= maxSeed; currentSeed++) {
        clock_t seedStart = clock();

        FilledPattern testPattern = generateAPrintPattern(directorPath, desiredPattern, currentSeed);
        QuantifyPattern patternAgreement(testPattern);
        printf("\n%i/%i done in %.2fs.\n", currentSeed - minSeed + 1, maxSeed - minSeed + 1,
               (double)(clock() - seedStart) / CLOCKS_PER_SEC);
        patternAgreement.printResults();
        double currentDisagreement = patternAgreement.calculateCorrectness(5, 0.2, 1, 1);
        if (currentDisagreement < bestDisagreement) {
            bestSeed = currentSeed;
            bestDisagreement = currentDisagreement;
        }
    }
    printf("%i seeds from %i to %i were tested. Seed %i had the lowest disagreement of %.2f\n",
           maxSeed - minSeed + 1, minSeed, maxSeed, bestSeed, bestDisagreement);
    generateAndExportPrintPattern(directorPath, desiredPattern, bestSeed);
}


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
//    generatePrintPattern(azimuthal, 1, 100);
//    generatePrintPatternsMultithread(linear, 1, 5);
    return 0;
}
