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

class Simulation
        : public BayesianOptimisationConfig,
          public DisagreementConfig,
          public DisagreementFunctionConfig,
          public FillingMethodConfig {
    fs::path pattern_directory;

public:
    Simulation(const fs::path &pattern_directory, bool is_default_used);

    void editSimulationConfiguration();
};


#endif //VECTOR_SLICER_SIMULATION_H
