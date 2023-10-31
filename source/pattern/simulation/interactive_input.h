//
// Created by Michał Zmyślony on 27/10/2023.
//

#ifndef VECTOR_SLICER_INTERACTIVE_INPUT_H
#define VECTOR_SLICER_INTERACTIVE_INPUT_H

#include <sstream>

void editInt(int &variable, const std::string &variable_name);

void editDouble(double &variable, const std::string &variable_name);

void editBool(bool &variable, const std::string &variable_name);

int readInt(int default_value);

bool readBool(bool default_value);

bool confirmation();

#endif //VECTOR_SLICER_INTERACTIVE_INPUT_H
