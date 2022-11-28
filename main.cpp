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

#include <boost/filesystem.hpp>
#include <boost/dll.hpp>
#include <string>
#include <iostream>
#include <fstream>
//#include "source/importing_and_exporting/ReadingFromOutside.h"
//#include "source/importing_and_exporting/TableReading.h"
#include "source/pattern/bayesian_optimisation.h"

namespace fs = boost::filesystem;

const double VERSION = 1.0;

std::vector<fs::path> getPatterns(const fs::path &list_of_patterns_path) {
    std::vector<fs::path> patterns;
    std::string line;
    std::fstream file(list_of_patterns_path.string());

    while (std::getline(file, line)) {
        if (line.find("ignore") != std::string::npos) {}
        else {
            patterns.emplace_back(line);
        }
    }
    return patterns;
}


int main() {
    printf("\n\tVector slicer version %.1f.\n", VERSION);
    fs::path cwd = boost::dll::program_location().parent_path();
    fs::path results = cwd.parent_path() / "results";
    fs::path patterns_path = results / "filesToTest.txt";

    std::vector<fs::path> patterns = getPatterns(patterns_path);

    printf("\nTested patterns: \n");
    for (auto &pattern_type: patterns) {
        std::cout << "\t" << pattern_type << std::endl;
    }

    for (auto &pattern_type: patterns) {
        try {
            generalFinder(pattern_type, 24, 8);
        }
        catch (std::runtime_error &error) {
            std::cout << error.what() << std::endl;
        }
    }


    return 0;
}