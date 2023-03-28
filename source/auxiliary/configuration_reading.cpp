// 2023, Michał Zmyślony, mlz22@cam.ac.uk.
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

//
// Created by Michał Zmyślony on 28/03/2023.
//

#include "configuration_reading.h"
#include <sstream>
#include <iostream>


std::string cleanString(std::string str) {
    str.erase(remove_if(str.begin(), str.end(), isspace), str.end());
    std::string string_without_hash = str.substr(0, str.find('#'));
    return string_without_hash;
}

std::string findSuffix(const std::string &str, char suffix) {
    return str.substr(str.find(suffix) + 1);
}


std::string readKey(const fs::path &file_path, const std::string &key) {
    std::string line;
    std::ifstream file(file_path.string());
    while (std::getline(file, line)) {
        std::string clean_line = cleanString(line);

        if (clean_line.find(key) != std::string::npos) {
            return findSuffix(clean_line, '=');
        }
    }
    throw std::runtime_error("Key " + key + " was not found in the config file " + file_path.string());
}


int readKeyInt(const fs::path &file_path, const std::string &key) {
    return std::stoi(readKey(file_path, key));
}


double readKeyDouble(const fs::path &file_path, const std::string &key) {
    return std::stod(readKey(file_path, key));
}


bool readKeyBool(const fs::path &file_path, const std::string &key) {
    bool b;
    std::istringstream(readKey(file_path, key)) >> std::boolalpha >> b;
    return b;
}