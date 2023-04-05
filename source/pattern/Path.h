// 2022, Michał Zmyślony, mlz22@cam.ac.uk.
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
// Created by Michał Zmyślony on 21/09/2021.
//

#ifndef VECTOR_SLICER_PATH_H
#define VECTOR_SLICER_PATH_H

#include <vector>
#include <valarray>

class Path {
public:
    std::vector<std::valarray<int>> sequence_of_positions;

    explicit Path(std::valarray<int> &starting_positions);

    Path(const Path& forward_path, const Path& backward_path);

    void addPoint(std::valarray<int> &positions);

    [[nodiscard]] std::valarray<int> first() const ;

    [[nodiscard]] std::valarray<int> last() const ;

    [[nodiscard]] unsigned int size() const;

    [[nodiscard]] std::valarray<int> secondToLast() const;

    [[nodiscard]] std::valarray<int> second() const;

    double getLength();
};


#endif //VECTOR_SLICER_PATH_H
