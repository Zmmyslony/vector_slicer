//
// Created by Michał Zmyślony on 27/10/2023.
//

#include "disagreement_config.h"
#include "configuration_reading.h"
#include <iostream>
#include <fstream>


DisagreementConfig::DisagreementConfig(const fs::path &config_path) :
        threads(readKeyInt(config_path, "threads")),
        optimisation_seeds(readKeyInt(config_path, "seeds")),
        final_seeds(readKeyInt(config_path, "final_seeds")),
        agreement_percentile(readKeyDouble(config_path, "agreement_percentile")),
        number_of_layers(readKeyInt(config_path, "number_of_layers")),
        is_disagreement_details_printed(readKeyBool(config_path, "is_disagreement_details_printed")) {

}


DisagreementConfig::DisagreementConfig(const fs::path &local_path, const fs::path &global_path) {
    if (fs::exists(local_path)) {
        std::cout << "Local disagreement configuration file found." << std::endl;
        *this = DisagreementConfig(local_path);
    } else {
        *this = DisagreementConfig(global_path);
    }
}

void DisagreementConfig::saveDisagreementConfig(const fs::path &config_path) {
    std::ofstream file;
    file.open(config_path.string());
    file
            << "# Maximum number of concurrent configurations that are calculated during calculation of the disagreement of given"
            << "\n# generating parameters. Should not exceed the number of physical cores of CPU."
            << "\nthreads = " << threads
            << "\n\n# Number of seeds calculated for each set of generating parameters in Bayesian optimisation. The more there are, the"
            << "\n# lower is the standard deviation, but also higher is the execution time. It is best to use values that are multiples"
            << "\n# of the number of threads"
            << "\nseeds = " << optimisation_seeds
            << "\n\n# Number of seeds to scan over after the Bayesian optimisation is complete and the best seeds is chosen as the result."
            << "\nfinal_seeds = " << final_seeds
            << "\n\n# Percentile of the disagreement distribution, taken to be the disagreement of given set of generating parameters."
            << "\n# Ought to be, so that this percentile of final seeds is more than the desired number of layers."
            << "\nagreement_percentile = " << agreement_percentile
            << "\n\n# Number of patterns to be exported in layer-like fashion"
            << "\nnumber_of_layers = " << number_of_layers
            << "\n\n# Switch to print mean disagreement, standard deviation and noise for generating parameters. Used to identify whether"
            << "\n# the noise parameter in Bayesian optimisation is correct, mostly for debugging."
            << "\nis_disagreement_details_printed" << is_disagreement_details_printed;
    file.close();
}


int DisagreementConfig::getThreads() const {
    return threads;
}

int DisagreementConfig::getOptimisationSeeds() const {
    return optimisation_seeds;
}

int DisagreementConfig::getFinalSeeds() const {
    return final_seeds;
}

double DisagreementConfig::getAgreementPercentile() const {
    return agreement_percentile;
}

int DisagreementConfig::getNumberOfLayers() const {
    return number_of_layers;
}

bool DisagreementConfig::isDisagreementDetailsPrinted() const {
    return is_disagreement_details_printed;
}
