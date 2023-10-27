//
// Created by Michał Zmyślony on 27/10/2023.
//

#ifndef VECTOR_SLICER_SIMULATION_H
#define VECTOR_SLICER_SIMULATION_H

#include <boost/filesystem.hpp>
#include "bayesian_optimisation_config.h"
#include "disagreement_config.h"
#include "disagreement_function_config.h"
#include "filling_method_config.h"

namespace fs = boost::filesystem;

class Simulation {
    fs::path pattern_directory;
    BayesianOptimisationConfig bayesian_optimisation_config;
    DisagreementConfig disagreement_config;
    DisagreementFunctionConfig disagreement_function_config;
    FillingMethodConfig filling_method_config;

public:
    Simulation(const fs::path &pattern_directory, bool is_default_used);
};


#endif //VECTOR_SLICER_SIMULATION_H
