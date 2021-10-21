//
// Created by Michał Zmyślony on 21/09/2021.
//

#ifndef VECTOR_SLICER_PATH_H
#define VECTOR_SLICER_PATH_H

#include <vector>
#include <valarray>

class Path {
public:
    std::vector<std::valarray<int>> positionsAlongDirector;
    std::vector<std::valarray<int>> positionsOppositeToDirector;

    explicit Path(std::valarray<int>& startingPosition);
    Path(std::vector<std::valarray<int>>& forwardPath, std::vector<std::valarray<int>>& backwardPath);

    void addPointToForwardArray(std::valarray<int>& startingPosition);
    void addPointToReverseArray(std::valarray<int>& startingPosition);
};


#endif //VECTOR_SLICER_PATH_H
