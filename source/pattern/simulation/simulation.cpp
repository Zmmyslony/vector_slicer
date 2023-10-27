//
// Created by Michał Zmyślony on 27/10/2023.
//

#include "simulation.h"
#include "vector_slicer_config.h"

Simulation::Simulation(const fs::path &pattern_directory, bool is_default_used) :
        BayesianOptimisationConfig(pattern_directory / "config.cfg", BAYESIAN_CONFIG),
        DisagreementConfig(pattern_directory / "disagreement.cfg", DISAGREEMENT_CONFIG),
        DisagreementFunctionConfig(pattern_directory / "disagreement_function.cfg", DISAGREEMENT_FUNCTION_CONFIG)
        {

}
