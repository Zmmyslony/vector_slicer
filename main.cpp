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
#include "./source/high_level/ReadingFromOutside.h"
#include "./source/auxiliary/TableReading.h"

namespace fs = boost::filesystem;

const double VERSION = 1.0;

std::vector<std::string> getPatterns(const std::string &listOfPatternsPath) {
    std::vector<std::string> patterns;
    std::string line;
    std::fstream file(listOfPatternsPath);

    while (std::getline(file, line)) {
        if (line.find("ignore") != std::string::npos) {}
        else {
            patterns.push_back(line);
        }
    }
    return patterns;
}

std::vector<int> readConfig(const std::string &filename) {
    std::vector<std::vector<int>> table = readFileToTableInt(filename);
    std::vector<int> configRow = table[0];
    return configRow;
}


int main() {
    printf("\n\tVector slicer version %.1f.\n", VERSION);
    fs::path cwd = boost::dll::program_location().parent_path();
    fs::path results = cwd.parent_path() / "results";
    fs::path config_path = results / "config.txt";
    fs::path optimizer_path = results / "optimizationSequence.txt";
    fs::path patterns_path = results / "filesToTest.txt";

    std::vector<int> config = readConfig(config_path.string());
    std::vector<std::string> patterns = getPatterns(patterns_path.string());

    printf("\nTested patterns: \n");
    for (auto &patternType: patterns) {
        std::cout << "\t" << patternType << std::endl;
    }
    printf("Testing seeds from %d to %d using %d threads.\n", config[0], config[1], config[2]);

    for (auto &patternType: patterns) {
        generalFinderString(patternType, config[0], config[1], config[2], optimizer_path.string());
//        recalculateBestConfig(patternType);
//        generateGCode(patternType, 30, 10, std::valarray<double>({0, 10}), 0.020);
    }


    return 0;
}