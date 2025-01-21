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

#include "interactive_input.h"
#include <iostream>
#include <sstream>
#include <fstream>

void editInt(int &variable, const std::string &variable_name) {
    std::string input;
    std::cout << "\r" << variable_name << " (int, current: " << variable << "),  leave empty to keep unchanged." << std::endl;
    std::getline(std::cin, input);
    if (!input.empty() || input == "\n") {
        variable = std::stoi(input);
    } else {
        std::cout << "Value unchanged." << std::endl;
    }
}

void editDouble(double &variable, const std::string &variable_name) {
    std::string input;
    std::cout << "\r" <<variable_name << " (double, current: " << variable << "),  leave empty to keep unchanged." << std::endl;
    std::getline(std::cin, input);
    if (!input.empty() || input == "\n") {
        variable = std::stod(input);
    } else {
        std::cout << "Value unchanged." << std::endl;
    }
}

void editBool(bool &variable, const std::string &variable_name) {
    std::string input;
    std::cout << "\r" <<variable_name << " (bool, current: " << variable << "), leave empty to keep unchanged." << std::endl;
    std::getline(std::cin, input);
    if (!input.empty() || input == "\n") {
        std::istringstream(input) >> variable;
    } else {
        std::cout << "Value unchanged." << std::endl;
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
    std::cout << "Do you confirm (0/1 default: 0)" << std::endl;
    return readBool(false);
}
