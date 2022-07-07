//
// Created by Michał Zmyślony on 10/01/2022.
//

#include "ConfigGeneration.h"
#include <iostream>


std::vector<FillingConfig> iterateOverOption(const DesiredPattern &desiredPattern, FillingConfig initialConfig,
                                             double delta, int numberOfConfigs, ConfigOptions option) {
    std::vector<FillingConfig> listOfConfigs;
    double increment = delta / numberOfConfigs;
    double initialValue = stod(initialConfig.getConfigOption(option));
    for (int i = -numberOfConfigs; i <= numberOfConfigs; i++) {
        double newValue = initialValue + i * increment;
        // TODO fix this condition to omit errors. Collision radius has to smaller than the print radius and greater
        // than zero so that the algorithm can work.
        if (option != CollisionRadius || newValue < initialConfig.getPrintRadius() && newValue > 0) {

            initialConfig.setConfigOption(option, std::to_string(newValue));
            listOfConfigs.push_back(initialConfig);
        }
    }
    return listOfConfigs;
}


std::vector<FillingConfig> iterateOverSeeds(const DesiredPattern &desiredPattern, FillingConfig initialConfig,
                                            int minSeed, int maxSeed) {
    std::vector<FillingConfig> listOfConfigs;
    for (int i = minSeed; i <= maxSeed; i++) {
        initialConfig.setConfigOption(Seed, std::to_string(i));
        listOfConfigs.push_back(initialConfig);
    }
    return listOfConfigs;
}


std::vector<FillingConfig>
iterateOverSeeds(const DesiredPattern &desiredPattern, const std::vector<FillingConfig> &configList, int minSeed,
                 int maxSeed) {
    std::vector<FillingConfig> configsToTest;
    for (auto &configMethod: configList) {
        std::vector<FillingConfig> configsWithDifferentSeeds = iterateOverSeeds(desiredPattern, configMethod, minSeed,
                                                                                maxSeed);
        for (auto &configSeed: configsWithDifferentSeeds) {
            configsToTest.push_back(configSeed);

        }
    }
    return configsToTest;
}

