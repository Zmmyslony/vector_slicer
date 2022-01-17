//
// Created by Michał Zmyślony on 17/09/2021.
//
#ifndef VECTOR_SLICER_DESIREDPATTERN_H
#define VECTOR_SLICER_DESIREDPATTERN_H

#include <string>
#include <vector>
#include <valarray>

class DesiredPattern {
    std::valarray<int> dimensions;
    std::vector<std::valarray<int>> perimeterList;
    std::vector<std::vector<int>> shapeMatrix;
    std::vector<std::vector<double>> xFieldPreferred;
    std::vector<std::vector<double>> yFieldPreferred;
public:

    const std::valarray<int> &getDimensions() const;

    const std::vector<std::valarray<int>> &getPerimeterList() const;

    const std::vector<std::vector<int>> &getShapeMatrix() const;

    const std::vector<std::vector<double>> &getXFieldPreferred() const;

    const std::vector<std::vector<double>> &getYFieldPreferred() const;

    DesiredPattern(std::string &shapeFilename, std::string &xVectorFieldFilename, std::string &yVectorFieldFilename);

    std::valarray<int> preferredDirection(const std::valarray<int> &position, int distance) const;

    std::valarray<double> preferredDirection(const std::valarray<double> &position, int distance) const;

    bool isInShape(const std::valarray<int> &position) const;

    bool isInShape(const std::valarray<double> &position) const;
};


#endif //VECTOR_SLICER_DESIREDPATTERN_H
