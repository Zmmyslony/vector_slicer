// Copyright (c) 2023, Michał Zmyślony, mlz22@cam.ac.uk.
//
// Please cite Michał Zmyślony and Dr John Biggins if you use any part of this code in work you publish or distribute.
//
// This file is part of Vector Slicer.
//
// Vector Slicer is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any
// later version.
//
// Vector Slicer is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the
// implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
// Public License for more details.
//
// You should have received a copy of the GNU General Public License along with Vector Slicer.
// If not, see <https://www.gnu.org/licenses/>.

// 2023, Michał Zmyślony, mlz22@cam.ac.uk.
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
// Created by Michał Zmyślony on 23/06/2023.
//

#ifndef VECTOR_SLICER_LINE_THINNING_H
#define VECTOR_SLICER_LINE_THINNING_H

#include <vector>
#include <set>
#include <unordered_set>
#include <boost/functional/hash.hpp>

using veci = std::vector<int>;

using coord = std::pair<u_int16_t , u_int16_t>;

struct pairhash {
public:
    u_int32_t operator()(const std::pair<u_int16_t, u_int16_t> &x) const
    {
//        u_int32_t seed = 0;
        return x.first * 65536 + x.second;
//        auto first_hash = std::hash<u_int32_t>()(x.first);
//        auto second_hash = std::hash<u_int32_t>()(x.second);
//        boost::hash_combine(seed, first_hash);
//        boost::hash_combine(seed, second_hash);
//        return seed;
//        return std::hash<int>()(x.first) ^ std::hash<int>()(x.second);
    }
};

using coord_set = std::unordered_set<coord, pairhash>;
using coord_sequence = std::set<coord>;
using coord_vector = std::vector<coord>;

/// Zhang-Suen line thinning algorithm
coord_set skeletonize(coord_set shape, int grow_size, int threads);

coord_set grow_pattern(const coord_set &shape, double radius);


#endif //VECTOR_SLICER_LINE_THINNING_H
