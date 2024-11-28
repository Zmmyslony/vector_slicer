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

#include "repulsion.h"
#include "perimeter.h"
#include "valarray_operations.h"
#include "geometry.h"
#include "simple_math_operations.h"


std::vector<veci> generateLineDisplacements(const vecd &tangent, double radius) {
    vecd normal = multiply(normalize(perpendicular(tangent)), radius);
    vecd normal_abs = abs(normal);
    int distance_i = (int) std::max(normal_abs[0], normal_abs[1]);
    std::vector<veci> displacement_list;
    for (int i = -distance_i; i <= distance_i; i++) {
        vecd displacement_d = multiply(normal, (double) i / (double) distance_i);
        displacement_list.push_back(dtoi(displacement_d));
    }
    return displacement_list;
}


vecd
getRepulsionFromDisplacement(const vecd &coordinates, const std::vector<veci> &current_displacements, const veci &sizes,
                             const std::vector<std::vector<int>> &shape_matrix,
                             const std::vector<std::vector<int>> &filled_table) {
    int number_of_repulsing_coordinates = 0;
    vecd empty_spot_attraction = {0, 0};

    for (auto &displacement: current_displacements) {
        veci neighbour = add(dtoi(coordinates), displacement);
        if (isInRange(neighbour, sizes) &&
            !isEmpty(neighbour, shape_matrix) &&
            isEmpty(neighbour, filled_table)) {

            empty_spot_attraction = add(empty_spot_attraction, itod(displacement));
            number_of_repulsing_coordinates++;
        }
    }
    if (number_of_repulsing_coordinates != 0) {
        empty_spot_attraction = divide(empty_spot_attraction, number_of_repulsing_coordinates);
    }
    return empty_spot_attraction;
}


vecd getLineBasedRepulsion(const std::vector<std::vector<int>> &shape_matrix,
                           const std::vector<std::vector<int>> &filled_table, const vecd &tangent, double radius,
                           const vecd &coordinates, const veci &sizes, double repulsion_coefficient,
                           double maximal_repulsion_angle) {
    std::vector<veci> normal_displacements = generateLineDisplacements(tangent, radius - 1);
    vecd maximal_repulsion_vector = multiply(
            getRepulsionFromDisplacement(coordinates, normal_displacements, sizes,
                                         shape_matrix, filled_table),
            repulsion_coefficient);

    double maximal_repulsion_length = norm(maximal_repulsion_vector);
    if (maximal_repulsion_length < 1) {
        return maximal_repulsion_vector;
    }
    veci maximal_repulsion_vector_i = dtoi(maximal_repulsion_vector);
    int maximal_repulsion_length_i = std::max(std::abs(maximal_repulsion_vector_i[0]),
                                              std::abs(maximal_repulsion_vector_i[1]));

    vecd previous_displacement = {0, 0};
    for (int i = 1; i <= maximal_repulsion_length_i; i++) {
        vecd local_displacement = multiply(maximal_repulsion_vector, (double) i / (double) maximal_repulsion_length_i);
        vecd local_repulsion = multiply(
                getRepulsionFromDisplacement(add(coordinates, local_displacement), normal_displacements,
                                             sizes, shape_matrix, filled_table),
                repulsion_coefficient);

        double local_angle = angle(tangent, add(tangent, local_repulsion));
        bool is_maximal_angle_exceeded = local_angle >= maximal_repulsion_angle;
        // Test to see if the repulsion has changed its sign, resulting in over repulsing
        bool is_repulsion_inverted = dot(local_repulsion, maximal_repulsion_vector) < 0;
        if (is_maximal_angle_exceeded || is_repulsion_inverted) {
            return previous_displacement;
        }
        previous_displacement = local_displacement;
    }
    return previous_displacement;
}