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
// Created by Michał Zmyślony on 19/06/2023.
//

#include <cmath>
#include <iostream>

#include "repulsion.h"
#include "perimeter.h"
#include "valarray_operations.h"
#include "geometry.h"
#include "simple_math_operations.h"


std::vector<coord> generateLineDisplacements(const coord_d &tangent, double radius) {
    coord_d normal = normalized(perpendicular(tangent)) * radius;

    int distance_i = (int) std::max(fabs(normal.x), fabs(normal.y));
    std::vector<coord> displacement_list;
    for (int i = -distance_i; i <= distance_i; i++) {
        coord_d displacement_d = normal * (double) i / (double) distance_i;
        displacement_list.emplace_back(displacement_d);
    }
    return displacement_list;
}


coord_d
getRepulsionFromDisplacement(const coord_d &coordinates, const std::vector<coord> &current_displacements,
                             const veci &sizes,
                             const std::vector<std::vector<uint8_t>> &shape_matrix,
                             const std::vector<std::vector<uint8_t>> &filled_table) {
    int number_of_repulsing_coordinates = 0;
    coord empty_spot_attraction = {0, 0};
    auto i_coords = coord(coordinates);
    for (auto &displacement: current_displacements) {
        coord neighbour = i_coords + displacement;
        if (isInRange(neighbour, sizes) &&
            !isEmpty(neighbour, shape_matrix) &&
            isEmpty(neighbour, filled_table)) {

            empty_spot_attraction = empty_spot_attraction + displacement;
            number_of_repulsing_coordinates++;
        }
    }
    if (number_of_repulsing_coordinates != 0) {
        return to_coord_d(empty_spot_attraction) / number_of_repulsing_coordinates;
    }
    return {0, 0};
}


coord_d getLineBasedRepulsion(const std::vector<std::vector<uint8_t>> &shape_matrix,
                              const std::vector<std::vector<uint8_t>> &filled_table, const coord_d &tangent,
                              const coord_d &coordinates, const veci &sizes, double radius,
                              double repulsion_coefficient, double minimum_projection) {
    std::vector<coord> normal_displacements = generateLineDisplacements(tangent, radius - 1);
    coord_d maximal_repulsion_vector =
            getRepulsionFromDisplacement(coordinates, normal_displacements, sizes,
                                         shape_matrix, filled_table) * repulsion_coefficient;

    double maximal_repulsion_length = norm(maximal_repulsion_vector);
    if (maximal_repulsion_length < 1) {
        double repulsion_projection = dot(tangent.normalized(), (tangent + maximal_repulsion_vector).normalized());
        bool is_maximal_angle_exceeded = (repulsion_projection <= minimum_projection);
        if (is_maximal_angle_exceeded) {
            return {0, 0};
        } else {
            return maximal_repulsion_vector;
        }
    }
    int maximal_repulsion_length_i = (int) std::max(fabs(maximal_repulsion_vector.x),
                                                    fabs(maximal_repulsion_vector.y));

    coord_d previous_displacement = {0, 0};
    for (int i = 1; i <= maximal_repulsion_length_i; i++) {
        coord_d local_displacement = maximal_repulsion_vector * (double) i / (double) maximal_repulsion_length_i;
        coord_d local_repulsion =
                getRepulsionFromDisplacement(coordinates + local_displacement, normal_displacements,
                                             sizes, shape_matrix, filled_table) *
                repulsion_coefficient;

        double repulsion_cos = dot(tangent.normalized(), (tangent + local_repulsion).normalized());
        bool is_maximal_angle_exceeded = repulsion_cos <= minimum_projection;
        // Test to see if the repulsion has changed its sign, resulting in over repulsing
        bool is_repulsion_inverted = dot(local_repulsion, maximal_repulsion_vector) < 0;
        if (is_maximal_angle_exceeded || is_repulsion_inverted) {
            return previous_displacement;
        }
        previous_displacement = local_displacement;
    }
    return previous_displacement;
}