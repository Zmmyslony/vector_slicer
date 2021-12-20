//
// Created by Michał Zmyślony on 21/09/2021.
//

#include "PerimeterGeneration.h"

std::vector<std::valarray<int>> generatePerimeterList(double radius) {
    std::vector<std::valarray<int>> perimeterList;
    int range = (int)radius + 1;
    for (int i = -range; i <= range; i++) {
        for (int j = -range; j <= range; j++) {
            if (floor(sqrt(i * i + j * j) - radius) == 0) {
                perimeterList.push_back({i, j});
            }
        }
    }

    return perimeterList;
}