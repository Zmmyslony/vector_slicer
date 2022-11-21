// 2022, Michał Zmyślony, mlz22@cam.ac.uk.
//
// Please cite Michał Zmyślony and Dr John Biggins if you use any part of this code in work you publish or distribute.
//
// This file is part of Vector Slicer.
//
// Vector Slicer is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
//
// Vector Slicer is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with Foobar. If not, see <https://www.gnu.org/licenses/>.

//
// Created by Michał Zmyślony on 21/11/2022.
//

#include <boost/filesystem/path.hpp>
#include "bayesian_optimisation.h"

#include "FillingOptimization.h"
#include "../importing_and_exporting/OpenFiles.h"

namespace fs = boost::filesystem;

//FillingConfig
//findBestFillingForSeeds(const DesiredPattern &desired_pattern, const std::vector<FillingConfig> &config_list,
//                        int min_seed, int max_seed, int threads, const std::string &type, configOptions optimized_option,
//                        const std::string &initial_value)
//
//void bayesianOptimisation(const fs::path &pattern_path, int min_seed, int max_seed, int threads) {
//    time_t start_time = clock();
//    std::cout << "\n\nCurrent directory: " << pattern_path << std::endl;
//    DesiredPattern desired_pattern = openPatternFromDirectory(pattern_path);
//    fs::path config_path = pattern_path / "config.txt";
//    FillingConfig initial_config(config_path);
//
//    bayesopt::Parameters parameters;
//    parameters = initialize_parameters_to_default();
//    parameters.n_iterations = 190;
//    parameters.random_seed = 0;
//    parameters.verbose_level = 1;
//    parameters.noise = 1e-2;
//
//    BayesianOptimisation bayesian_optimisation(parameters);
//    vectord result(3);
//    bayesian_optimisation.optimize(result);
//    std::cout << "Result: " << result << "->"
//              << bayesian_optimisation.evaluateSample(result) << std::endl;
//
//    printf("Execution time %.2f", (double) (clock() - start_time) / CLOCKS_PER_SEC);
//}