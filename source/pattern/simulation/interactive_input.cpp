// Copyright (c) 2023, Michał Zmyślony, mlz22@cam.ac.uk.
//
// Please cite Michał Zmyślony and Dr John Biggins if you use any part of this code in work you publish or distribute.
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

#include "interactive_input.h"
#include <iostream>
#include <sstream>
#include <fstream>

void editInt(int &variable, const std::string &variable_name) {
    std::string input;
    std::cout << variable_name << " (int, current: " << variable << "), press enter for default." << std::endl;
    std::getline(std::cin, input);
    if (!input.empty() || input == "\n") {
        variable = std::stoi(input);
    }
}

void editDouble(double &variable, const std::string &variable_name) {
    std::string input;
    std::cout << variable_name << " (double, current: " << variable << "), press enter for default." << std::endl;
    std::getline(std::cin, input);
    if (!input.empty() || input == "\n") {
        variable = std::stod(input);
    }
}

void editBool(bool &variable, const std::string &variable_name) {
    std::string input;
    std::cout << variable_name << " (bool, current: " << variable << "), press enter for default." << std::endl;
    std::getline(std::cin, input);
    if (!input.empty() || input == "\n") {
        std::istringstream(input) >> variable;
    }
}

int readInt(int default_value) {
    int value = default_value;
    std::string input;
    std::getline(std::cin, input);
    if (!input.empty() || input == "\n") {
        std::istringstream(input) >> value;
    }
    return value;
}

bool readBool(bool default_value) {
    bool value = default_value;
    std::string input;
    std::getline(std::cin, input);
    if (!input.empty() || input == "\n") {
        std::istringstream(input) >> value;
    }
    return value;
}

bool confirmation() {
    std::cout << "Do you confirm (default: false)" << std::endl;
    return readBool(false);
}
