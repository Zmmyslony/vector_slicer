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
