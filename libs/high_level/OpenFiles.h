//
// Created by Michał Zmyślony on 24/09/2021.
//

#ifndef VECTOR_SLICER_OPENFILES_H
#define VECTOR_SLICER_OPENFILES_H

#include "../pattern/FilledPattern.h"
#include <string>
#include <vector>

DesiredPattern openPatternFromDirectory(const std::string& directoryPath);

FilledPattern openFilledPatternFromDirectory(const std::string& directoryPath);
FilledPattern openFilledPatternFromDirectory(const std::string& directoryPath, unsigned int seed);
std::vector<int> readConfigTable(const std::string& configPath);
FilledPattern openFilledPatternFromDirectoryAndPattern(const std::string& directoryPath, DesiredPattern& pattern, unsigned int seed);
FilledPattern openFilledPatternFromDirectoryAndPattern(const std::string& directoryPath, DesiredPattern& pattern);

#endif //VECTOR_SLICER_OPENFILES_H
