////
//// Created by Michał Zmyślony on 21/10/2021.
////
//
//#include <algorithm>
//#include <iostream>
//#include <omp.h>
//#include <thread>
//#include <Windows.h>
//
//#include "AutomaticPathGeneration.h"
//#include "../high_level/OpenFiles.h"
//#include "../high_level/ProgressBar.h"
//
//#include "../pattern/FillingPatterns.h"
//#include "../pattern/QuantifyPattern.h"
//#include "../pattern/PathFillingOrder.h"
//
//
////#include "mpi.h"
//
//FilledPattern generateAPrintPattern(const std::string &directorPath, const DesiredPattern &desiredPattern, int seed) {
//    FilledPattern pattern = openFilledPatternFromDirectoryAndPattern(directorPath, desiredPattern, seed);
//    fillWithPaths(pattern);
//
//    return pattern;
//}
//
//
//void exportPatternToDirectory(const FilledPattern &pattern, const std::string &directorPath, const int &seed) {
//    std::string resultsDirectory = directorPath + R"(\results\)";
//    std::string patternDirectory = directorPath + R"(\results)";
//    CreateDirectory(resultsDirectory.c_str(), nullptr);
//    CreateDirectory(patternDirectory.c_str(), nullptr);
//    pattern.exportToDirectory(patternDirectory);
//}
//
//
//void findBestPath(FilledPattern &pattern) {
//    PathFillingOrder gcodeBase(pattern);
//    gcodeBase.findBestStartingPoints();
//}
//
//
//void generateAndExportPrintPattern(const std::string &directorPath, const DesiredPattern &desiredPattern, int seed) {
//    FilledPattern pattern = generateAPrintPattern(directorPath, desiredPattern, seed);
////    findBestPath(pattern);
//    exportPatternToDirectory(pattern, directorPath, seed);
//}
//
//
//void checkSeed(const DesiredPattern &desiredPattern, const std::string &directorPath, int seed, int &bestSeed,
//               double &bestDisagreement) {
//    FilledPattern testPattern = generateAPrintPattern(directorPath, desiredPattern, seed);
//    QuantifyPattern patternAgreement(testPattern);
//
//    double currentDisagreement = patternAgreement.calculateCorrectness(5, 0.2, 1, 1);
//    if (currentDisagreement < bestDisagreement) {
//        bestSeed = seed;
//        bestDisagreement = currentDisagreement;
//    }
//}
//
//void checkSeed(DesiredPattern &desiredPattern, std::string &directorPath, int seed, double &disagreementValue) {
//    FilledPattern testPattern = generateAPrintPattern(directorPath, desiredPattern, seed);
//    QuantifyPattern patternAgreement(testPattern);
//
//    disagreementValue = patternAgreement.calculateCorrectness(5, 0.2, 1, 1);
//}
//
//void generatePrintPattern(std::string &directorPath, int minSeed, int maxSeed) {
//    std::cout << "\nCurrent directory: " << directorPath << std::endl;
//    DesiredPattern desiredPattern = openPatternFromDirectory(directorPath);
//
//    int bestSeed = 0;
//    double bestDisagreement = DBL_MAX;
//
//    time_t startTime = clock();
//    for (int currentSeed = minSeed; currentSeed <= maxSeed; currentSeed++) {
//        showProgress(currentSeed - minSeed, maxSeed - minSeed);
//        checkSeed(desiredPattern, directorPath, currentSeed, bestSeed, bestDisagreement);
//    }
//    printf("\nSingle-thread execution time %.2f", (double) (clock() - startTime) / CLOCKS_PER_SEC);
//    printf("\n%i seeds from %i to %i were tested. Seed %i had the lowest disagreement of %.2f\n",
//           maxSeed - minSeed + 1, minSeed, maxSeed, bestSeed, bestDisagreement);
//    generateAndExportPrintPattern(directorPath, desiredPattern, bestSeed);
//}
//
//
//unsigned int indexOfSmallestElement(std::vector<double> agreementArray) {
//    auto it = std::min_element(std::begin(agreementArray), std::end(agreementArray));
//    return std::distance(std::begin(agreementArray), it);
//}
//
//
//void generatePrintPatternMultithreading(std::string &directorPath, int minSeed, int maxSeed, int threads) {
//    std::cout << "\nCurrent directory: " << directorPath << std::endl;
//    DesiredPattern desiredPattern = openPatternFromDirectory(directorPath);
//
//
//    std::vector<double> disagreements(maxSeed - minSeed + 1);
//    time_t startTime = clock();
//
//    omp_set_num_threads(threads);
//#pragma omp parallel for
//    for (int currentSeed = minSeed; currentSeed <= maxSeed; currentSeed++) {
//        checkSeed(desiredPattern, directorPath, currentSeed, disagreements[currentSeed - minSeed]);
//    }
//
//    printf("Multi-thread execution time %.2f", (double) (clock() - startTime) / CLOCKS_PER_SEC);
//    unsigned int bestSeed = indexOfSmallestElement(disagreements);
//    double bestDisagreement = disagreements[bestSeed];
//
//    printf("\n%i seeds from %i to %i were tested. Seed %i had the lowest disagreement of %.2f\n",
//           maxSeed - minSeed + 1, minSeed, maxSeed, bestSeed + minSeed, bestDisagreement);
//    generateAndExportPrintPattern(directorPath, desiredPattern, bestSeed + minSeed);
//}