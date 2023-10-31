//
// Created by Michał Zmyślony on 27/10/2023.
//

#include "simulation.h"
#include "vector_slicer_config.h"
#include <iostream>
#include "interactive_input.h"

Simulation::Simulation(const fs::path &pattern_directory, bool is_default_used) :
        BayesianOptimisationConfig(pattern_directory / "config.cfg", BAYESIAN_CONFIG),
        DisagreementConfig(pattern_directory / "disagreement.cfg", DISAGREEMENT_CONFIG),
        DisagreementFunctionConfig(pattern_directory / "disagreement_function.cfg", DISAGREEMENT_FUNCTION_CONFIG),
        FillingMethodConfig(pattern_directory / "filling.cfg", FILLING_CONFIG) {
    if (!is_default_used) {
        editSimulationConfiguration();
    }
}

void Simulation::editSimulationConfiguration() {
    bool is_editing_complete = false;
    while (!is_editing_complete) {
        std::cout << std::endl
                  << "Editing the simulation configuration. Select which file to edit: " << std::endl
                  << "0 - finish editing (default)" << std::endl
                  << "1 - Bayesian optimisation configuration" << std::endl
                  << "2 - disagreement configuration" << std::endl
                  << "3 - disagreement function configuration" << std::endl
                  << "4 - filling method configuration" << std::endl;
        switch (readInt(0)) {
            case 0:
                std::cout << "Finish editing? (default: true)" << std::endl;
                is_editing_complete = readBool(true);
                break;
            case 1:
                editBayesianOptimisationConfig();
                saveBayesianOptimisationConfig(pattern_directory / "config.cfg", BAYESIAN_CONFIG);
                break;
            case 2:
                editDisagreementConfig();
                saveDisagreementConfig(pattern_directory / "disagreement.cfg", DISAGREEMENT_CONFIG);
                break;
            case 3:
                editDisagreementFunctionConfig();
                saveDisagreementFunctionConfig(pattern_directory / "disagreement_function.cfg",
                                               DISAGREEMENT_FUNCTION_CONFIG);
                break;
            case 4:
                editFillingMethodConfig();
                saveFillingMethodConfig(pattern_directory / "filling.cfg", FILLING_CONFIG);
            default:
                std::cout << "Invalid value! Select values from range 0-4" << std::endl;
                break;
        }
    }
}
