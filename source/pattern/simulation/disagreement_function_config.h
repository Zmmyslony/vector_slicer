//
// Created by Michał Zmyślony on 27/10/2023.
//

#ifndef VECTOR_SLICER_DISAGREEMENT_FUNCTION_CONFIG_H
#define VECTOR_SLICER_DISAGREEMENT_FUNCTION_CONFIG_H

#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

class DisagreementFunctionConfig {
private:
    double empty_spot_weight;
    double empty_spot_power;
    double overlap_weight;
    double overlap_power;
    double director_weight;
    double director_power;
    double paths_power;

    std::string textDisagreementFunctionConfig() const;
public:
    explicit DisagreementFunctionConfig(const fs::path &config_path);

    DisagreementFunctionConfig(const fs::path &local_path, const fs::path &default_path);

    void saveDisagreementFunctionConfig(const fs::path &config_path);

    void saveDisagreementFunctionConfig(const fs::path &local_path, const fs::path &default_path);

    void printDisagreementFunctionConfig() const;

    void editDisagreementFunctionConfig();

    double getEmptySpotWeight() const;

    double getEmptySpotPower() const;

    double getOverlapWeight() const;

    double getOverlapPower() const;

    double getDirectorWeight() const;

    double getDirectorPower() const;

    double getPathsPower() const;
};


#endif //VECTOR_SLICER_DISAGREEMENT_FUNCTION_CONFIG_H
