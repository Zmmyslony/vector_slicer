//
// Created by Michał Zmyślony on 21/10/2021.
//

#ifndef VECTOR_SLICER_AUTOMATICPATHGENERATION_H
#define VECTOR_SLICER_AUTOMATICPATHGENERATION_H

#include "../pattern/FilledPattern.h"

FilledPattern generateAPrintPattern(std::string directorPath, DesiredPattern desiredPattern, int seed);

void exportPatternToDirectory(const FilledPattern& pattern, const std::string& directorPath, const int& seed);

void generateAndExportPrintPattern(const std::string& directorPath, const DesiredPattern& desiredPattern, int seed);

void generatePrintPattern(std::string& directorPath, int minSeed, int maxSeed);


#endif //VECTOR_SLICER_AUTOMATICPATHGENERATION_H
