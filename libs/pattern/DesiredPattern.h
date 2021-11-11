//
// Created by Michał Zmyślony on 17/09/2021.
//
#ifndef VECTOR_SLICER_DESIREDPATTERN_H
#define VECTOR_SLICER_DESIREDPATTERN_H

#include <string>
#include <vector>
#include <valarray>

class DesiredPattern {
public:
    std::valarray<int> dimensions;
    std::vector<std::valarray<int>> perimeterList;
    std::vector<std::vector<int>> shapeMatrix;
    std::vector<std::vector<double>> xFieldPreferred;
    std::vector<std::vector<double>> yFieldPreferred;

    DesiredPattern(std::string &shapeFilename, std::string &xVectorFieldFilename, std::string &yVectorFieldFilename);

    std::valarray<int> preferredDirection(std::valarray<int> position, int distance);

    std::valarray<double> preferredDirection(std::valarray<double> position, int distance);
};


#endif //VECTOR_SLICER_DESIREDPATTERN_H
