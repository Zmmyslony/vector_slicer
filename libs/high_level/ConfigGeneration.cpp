//
// Created by Michał Zmyślony on 10/01/2022.
//

#include "ConfigGeneration.h"

//std::vector<FillingConfig> iterateOverFillingMethod(const DesiredPattern &desiredPattern, FillingConfig initialConfig) {
//    std::vector<FillingConfig> listOfConfigs;
//    std::vector<std::string> allFillingMethods = {"RandomRadial", "ConsecutiveRadial", "RandomPerimeter",
//                                                  "ConsecutivePerimeter"};
//    initialConfig.setConfigOption(Repulsion, std::to_string(0));
//    for (auto &fillingMethod: allFillingMethods) {
//        initialConfig.setConfigOption(InitialFillingMethod, fillingMethod);
//        listOfConfigs.push_back(initialConfig);
//    }
//    return listOfConfigs;
//}


std::vector<FillingConfig> iterateOverOption(const DesiredPattern &desiredPattern, FillingConfig initialConfig,
                                             double delta, int numberOfConfigs, ConfigOptions option) {
    std::vector<FillingConfig> listOfConfigs;
    double increment = delta / numberOfConfigs;
    double initialValue = stod(initialConfig.getConfigOption(option));
    for (int i = -numberOfConfigs; i <= numberOfConfigs; i++) {
        double newValue = initialValue + i * increment;
        initialConfig.setConfigOption(option, std::to_string(newValue));
        listOfConfigs.push_back(initialConfig);
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