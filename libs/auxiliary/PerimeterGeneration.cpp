//
// Created by Michał Zmyślony on 21/09/2021.
//

#include "PerimeterGeneration.h"

std::vector<std::valarray<int>> generatePerimeterList(int radius) {
    std::vector<std::valarray<int>> perimeterList;
    for (int i = -radius; i <= radius; i++) {
        for (int j = -radius; j <= radius; j++) {
            if (ceil(sqrt(i * i + j * j)) == radius) {
                perimeterList.push_back({i, j});
            }
        }
    }

    return perimeterList;
}

//std::vector<std::valarray<int>> generateAreaList(int radius) {
//    std::vector<std::valarray<int>> areaList;
//    for (int i = -radius; i <= radius; i++) {
//        for (int j = -radius; j <= radius; j++) {
//            if (ceil(i * i + j * j) <= radius * radius) {
//                std::valarray<int> newElement = {i, j};
//                areaList.push_back(newElement);
//            }
//        }
//    }
//    return areaList;
//}