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
// You should have received a copy of the GNU General Public License along with Vector Slicer. If not, see <https://www.gnu.org/licenses/>.

#include <boost/filesystem.hpp>
#include <boost/dll.hpp>
#include <string>
#include <iostream>
#include "source/bayesian_optimisation.h"
#include "source/importing_patterns.h"
#include "vector_slicer_config.h"

namespace fs = boost::filesystem;


int main() {
    printf("\n\tVector slicer version %s\n", SLICER_VER);
    fs::path cwd = boost::dll::program_location().parent_path();
    fs::path results = cwd.parent_path() / "execution_setup";
    fs::path patterns_path = results / "filesToTest.txt";

    std::vector<fs::path> patterns = getPatterns(patterns_path);

    std::cout << "\nTested patterns:" << std::endl;
    for (auto &pattern_type: patterns) {
        std::cout << "\t" << pattern_type.string() << std::endl;
    }
    std::cout << "End of tested patterns." << std::endl;

    for (auto &pattern_type: patterns) {
        try {
//            optimisePattern(pattern_type, 24, 8);
            recalculateBestConfig(pattern_type);
        }
        catch (std::runtime_error &error) {
            std::cout << error.what() << std::endl;
        }
    }

    return 0;
}