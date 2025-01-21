// Copyright (c) 2023-2025, Michał Zmyślony, mlz22@cam.ac.uk.
//
// Please cite following publication if you use any part of this code in work you publish or distribute:
// [1] Michał Zmyślony M., Klaudia Dradrach, John S. Biggins,
//    Slicing vector fields into tool paths for additive manufacturing of nematic elastomers,
//    Additive Manufacturing, Volume 97, 2025, 104604, ISSN 2214-8604, https://doi.org/10.1016/j.addma.2024.104604.
//
// This file is part of Vector Slicer.
//
// Vector Slicer is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any
// later version.
//
// Vector Slicer is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the
// implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
// Public License for more details.
//
// You should have received a copy of the GNU General Public License along with Vector Slicer.
// If not, see <https://www.gnu.org/licenses/>.

//
// Created by Michał Zmyślony on 27/10/2023.
//

#include "disagreement_config.h"
#include "configuration_reading.h"
#include "interactive_input.h"

#include <iostream>
#include <sstream>
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
    file << textDisagreementConfig();
    file.close();
}

void DisagreementConfig::saveDisagreementConfig(const fs::path &local_path, const fs::path &default_path) {
    bool is_saving_complete = false;
    while(!is_saving_complete) {
        std::cout << "Save the config? 0 - don't save, 1 - save locally, 2 - save as default" << std::endl;
        int saving_destination = readInt(0);
        switch(saving_destination) {
            case 0:
                std::cout << "Config is going to be not saved, do you confirm?" << std::endl;
                is_saving_complete = readBool(true);
                break;
            case 1:
                if(confirmation()) {
                    is_saving_complete = true;
                    saveDisagreementConfig(local_path);
                }
                break;
            case 2:
                if(confirmation()) {
                    is_saving_complete = true;
                    saveDisagreementConfig(default_path);
                }
                break;
            default:
                std::cout << "Invalid value! Select values from range 0-2" << std::endl;
                break;
        }
    }
}

std::string DisagreementConfig::textDisagreementConfig() const {
    std::ostringstream textForm;
    textForm
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
            << "\nis_disagreement_details_printed = " << is_disagreement_details_printed;
    return textForm.str();
}

void DisagreementConfig::printDisagreementConfig() const {
    std::cout << textDisagreementConfig() << std::endl;
}

void DisagreementConfig::editDisagreementConfig() {
    bool is_editing_finished = false;
    while (!is_editing_finished) {
        std::cout << "Editing disagreement config." << std::endl;
        editInt(threads, "threads");
        editInt(optimisation_seeds, "optimisation_seeds");
        editInt(final_seeds, "final_seeds");
        editDouble(agreement_percentile, "agreement_percentile");
        editInt(number_of_layers, "number_of_layers");
        editBool(is_disagreement_details_printed, "is_disagreement_details_printed");

        std::cout << std::endl << "Current configuration:" << std::endl;
        printDisagreementConfig();

        std::cout << std::endl << std::endl << "Finish editing? (default: true)";
        is_editing_finished = readBool(true);
    }
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

