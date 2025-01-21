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

#include "simulation.h"
#include "vector_slicer_config.h"
#include <iostream>
#include "interactive_input.h"

Simulation::Simulation(const fs::path &pattern_directory, bool is_default_used) :
        pattern_directory(pattern_directory),
        BayesianOptimisationConfig(pattern_directory / "bayesian_optimisation.cfg", BAYESIAN_CONFIG),
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
                saveBayesianOptimisationConfig(pattern_directory / "bayesian_optimisation.cfg", BAYESIAN_CONFIG);
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
    std::cout << "Editing configuration complete. Beginning optimisation." << std::endl;
}
