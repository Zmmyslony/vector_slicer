//
// Created by Michał Zmyślony on 10/01/2022.
//

#ifndef VECTOR_SLICER_CONFIGGENERATION_H
#define VECTOR_SLICER_CONFIGGENERATION_H

#include "../auxiliary/FillingConfig.h"
#include "../pattern/DesiredPattern.h"


std::vector<FillingConfig>
iterateOverSeeds(const DesiredPattern &desiredPattern, const std::vector<FillingConfig> &configList, int minSeed,
                 int maxSeed);

std::vector<FillingConfig>
iterateOverOption(const DesiredPattern &desiredPattern, FillingConfig initialConfig, double delta, int numberOfConfigs,
                  ConfigOptions option);

std::vector<FillingConfig>
iterateOverSeeds(const DesiredPattern &desiredPattern, FillingConfig initialConfig, int minSeed, int maxSeed);

#endif //VECTOR_SLICER_CONFIGGENERATION_H
