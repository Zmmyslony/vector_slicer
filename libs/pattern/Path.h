//
// Created by Michał Zmyślony on 21/09/2021.
//

#ifndef VECTOR_SLICER_PATH_H
#define VECTOR_SLICER_PATH_H

#include <vector>
#include <valarray>

class Path {
public:
    std::vector<std::valarray<int>> sequenceOfPositions;

    explicit Path(std::valarray<int>& startingPosition);
    Path(Path forwardPath, Path backwardPath);

    void addPoint(std::valarray<int>& positions);
    unsigned int getLength() const;
};


#endif //VECTOR_SLICER_PATH_H
