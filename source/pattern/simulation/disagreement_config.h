//
// Created by Michał Zmyślony on 27/10/2023.
//

#ifndef VECTOR_SLICER_DISAGREEMENT_CONFIG_H
#define VECTOR_SLICER_DISAGREEMENT_CONFIG_H

#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

class DisagreementConfig {
    int threads;
    int optimisation_seeds;
    int final_seeds;
    double agreement_percentile;
    int number_of_layers;
    bool is_disagreement_details_printed;

    std::string textDisagreementConfig() const;
public:
    explicit DisagreementConfig(const fs::path &config_path);

    DisagreementConfig(const fs::path &local_path, const fs::path &global_path);

    void saveDisagreementConfig(const fs::path &config_path);

    void saveDisagreementConfig(const fs::path &local_path, const fs::path &default_path);

    void printDisagreementConfig() const;

    void editDisagreementConfig();

    int getThreads() const;

    int getOptimisationSeeds() const;

    int getFinalSeeds() const;

    double getAgreementPercentile() const;

    int getNumberOfLayers() const;

    bool isDisagreementDetailsPrinted() const;
};


#endif //VECTOR_SLICER_DISAGREEMENT_CONFIG_H
