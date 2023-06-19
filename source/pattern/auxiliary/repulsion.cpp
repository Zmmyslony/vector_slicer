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

#include "repulsion.h"

#include "perimeter.h"
#include "valarray_operations.h"
#include "geometry.h"


std::vector<vali> generateLineDisplacements(const vald &tangent, double radius) {
    vald normal = normalize(perpendicular(tangent)) * radius;
    vald normal_abs = std::abs(normal);
    int distance_i = (int)std::max(normal_abs[0], normal_abs[1]);
    std::vector<vali> displacement_list;
    for (int i = -distance_i; i <= distance_i; i++) {
        vald displacement_d = normal * (double) i / (double) distance_i;
        displacement_list.push_back(dtoi(displacement_d));
    }
    return displacement_list;
}


vald
getRepulsionFromDisplacement(const vald &coordinates, const std::vector<vali> &current_displacements, const vali &sizes,
                             const std::vector<std::vector<int>> &shape_matrix,
                             const std::vector<std::vector<int>> &filled_table) {
    int number_of_repulsing_coordinates = 0;
    vald empty_spot_attraction = {0, 0};

    for (auto &displacement: current_displacements) {
        vali neighbour = dtoi(coordinates) + displacement;
        if (isInRange(neighbour, sizes) &&
            !isEmpty(neighbour, shape_matrix) &&
            isEmpty(neighbour, filled_table)) {

            empty_spot_attraction += itod(displacement) ;
            number_of_repulsing_coordinates++;
        }
    }
    if (number_of_repulsing_coordinates != 0) {
        empty_spot_attraction /= number_of_repulsing_coordinates;
    }
    return empty_spot_attraction;
}


vald getLineBasedRepulsion(const std::vector<std::vector<int>> &shape_matrix,
                           const std::vector<std::vector<int>> &filled_table, const vald &tangent, double radius,
                           const vald &coordinates, const vali &sizes, double repulsion_coefficient,
                           double maximal_repulsion_angle) {
    std::vector<vali> normal_displacements = generateLineDisplacements(tangent, radius - 1);
    vald maximal_repulsion_vector = repulsion_coefficient *
                                    getRepulsionFromDisplacement(coordinates, normal_displacements, sizes,
                                                                 shape_matrix, filled_table);

    double maximal_repulsion_length = norm(maximal_repulsion_vector);
    if (maximal_repulsion_length < 1) {
        return maximal_repulsion_vector;
    }
    vali maximal_repulsion_vector_i = dtoi(maximal_repulsion_vector);
    int maximal_repulsion_length_i = std::max(std::abs(maximal_repulsion_vector_i[0]),
                                              std::abs(maximal_repulsion_vector_i[1]));

    vald previous_displacement = {0, 0};
    for (int i = 1; i <= maximal_repulsion_length_i; i++) {
        vald local_displacement = maximal_repulsion_vector * (double) i / (double) maximal_repulsion_length_i;
        vald local_repulsion = repulsion_coefficient *
                               getRepulsionFromDisplacement(coordinates + local_displacement, normal_displacements,
                                                            sizes, shape_matrix, filled_table);

        double local_angle = angle(tangent, tangent + local_repulsion);
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