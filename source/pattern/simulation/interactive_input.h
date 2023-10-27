//
// Created by Michał Zmyślony on 27/10/2023.
//

#ifndef VECTOR_SLICER_INTERACTIVE_INPUT_H
#define VECTOR_SLICER_INTERACTIVE_INPUT_H

#include <sstream>

class InteractiveInput {
    std::stringstream new_file_content;

    std::string clean(const std::string &input, const std::string &type);
};


#endif //VECTOR_SLICER_INTERACTIVE_INPUT_H
