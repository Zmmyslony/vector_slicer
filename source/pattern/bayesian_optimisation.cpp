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
#include <utility>
#include "bayesian_optimisation.h"

#include "../importing_and_exporting/OpenFiles.h"
#include "../importing_and_exporting/Exporting.h"
#include "../../ExecutionConfig.h"
#include "IndexedPath.h"

namespace fs = boost::filesystem;

BayesianOptimisation::BayesianOptimisation(QuantifiedConfig problem, int threads, int seeds,
                                           bayesopt::Parameters parameters) :
        ContinuousModel(3, std::move(parameters)),
        problem(std::move(problem)),
        threads(threads),
        seeds(seeds) {}


double BayesianOptimisation::evaluateSample(const vectord &x_in) {
    if (x_in.size() != 3) {
        std::cout << "WARNING: This only works for 3D inputs." << std::endl
                  << "WARNING: Using only first three components." << std::endl;
    }
    problem = QuantifiedConfig(problem, x_in);
    return problem.getDisagreement(seeds, threads);
}

void exportPatternToDirectory(FilledPattern pattern, const fs::path &pattern_path) {
    fs::path results_directory = pattern_path / "results";
    fs::path pattern_directory = pattern_path / "results";

    if (!fs::exists(results_directory)) {
        fs::create_directory(results_directory);
    }
    if (!fs::exists(pattern_directory)) {
        fs::create_directory(pattern_directory);
    }
    pattern.exportToDirectory(pattern_directory.string());

    std::vector<std::vector<std::valarray<int>>> sorted_paths = getSortedPaths(pattern, starting_point_number);
    export3DVectorToFile(sorted_paths, results_directory, "best_paths");
}

void generalFinder(const fs::path &pattern_path, int seeds, int threads) {
    time_t start_time = clock();
    std::cout << "\n\nCurrent directory: " << pattern_path << std::endl;
    fs::path config_path = pattern_path / "config.txt";

    FillingConfig initial_config(config_path);
    DesiredPattern desired_pattern = openPatternFromDirectory(pattern_path);
    DisagreementWeights default_weights(10, 1, 8, 1, 100, 2, 10, 2);

    QuantifiedConfig pattern(desired_pattern, initial_config, default_weights);

    bayesopt::Parameters parameters;
    parameters.l_type = L_MCMC;
    BayesianOptimisation pattern_optimisation(pattern, threads, seeds, parameters);
    vectord best_point(3);
    vectord lower_bound(3);
    vectord upper_bound(3);
    lower_bound[0] = 0; // Min repulsion
    lower_bound[1] = 0; // Min collision radius
    lower_bound[2] = 0; // Min starting point separation

    upper_bound[0] = 2;
    upper_bound[1] = pattern.getConfig().getPrintRadius() * 1.5;
    upper_bound[2] = pattern.getConfig().getPrintRadius() * 3;

    pattern_optimisation.setBoundingBox(lower_bound, upper_bound);
    pattern_optimisation.optimize(best_point);

    QuantifiedConfig optimised_pattern(pattern, best_point);

    exportPatternToDirectory(optimised_pattern.getFilledPattern(), pattern_path);
    optimised_pattern.getConfig().exportConfig(pattern_path / "results" / "best_config.txt");
    printf("Multi-thread execution time %.2f", (double) (clock() - start_time) / CLOCKS_PER_SEC);
}