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
// You should have received a copy of the GNU General Public License along with Foobar. If not, see <https://www.gnu.org/licenses/>.

//
// Created by Michał Zmyślony on 29/09/2021.
//

#include "QuantifiedPattern.h"
#include <utility>
#include <cmath>

QuantifiedPattern::QuantifiedPattern(FilledPattern pattern) :
        FilledPattern(std::move(pattern)) {
    empty_spots = calculateEmptySpots();
    average_overlap = calculateAverageOverlap();
    director_disagreement = calculateDirectorDisagreement();
    number_of_paths = calculateNumberOfPaths();
}

double QuantifiedPattern::calculateEmptySpots() {
    int number_of_empty_spots = 0;
    int x_size = desired_pattern.getDimensions()[0];
    int y_size = desired_pattern.getDimensions()[1];
    int number_of_elements = 0;

    for (int i = 0; i < x_size; i++) {
        for (int j = 0; j < y_size; j++) {
            if (desired_pattern.getShapeMatrix()[i][j] == 1) {
                number_of_elements++;
                if (number_of_times_filled[i][j] == 0) {
                    number_of_empty_spots++;
                }
            }
        }
    }
    return (double) number_of_empty_spots / (double) number_of_elements;
}

double QuantifiedPattern::calculateAverageOverlap() {
    int number_of_filled_times = 0;
    int x_size = desired_pattern.getDimensions()[0];
    int y_size = desired_pattern.getDimensions()[1];
    int number_of_elements = 0;

    for (int i = 0; i < x_size; i++) {
        for (int j = 0; j < y_size; j++) {
            number_of_filled_times += number_of_times_filled[i][j];
            number_of_elements += desired_pattern.getShapeMatrix()[i][j];
        }
    }
    return (double) number_of_filled_times / (double) number_of_elements - 1 + empty_spots;
}


double QuantifiedPattern::calculateDirectorDisagreement() {
    double director_agreement = 0;
    int x_size = desired_pattern.getDimensions()[0];
    int y_size = desired_pattern.getDimensions()[1];
    int number_of_filled_elements = 0;

    for (int i = 0; i < x_size; i++) {
        for (int j = 0; j < y_size; j++) {
            if (number_of_times_filled[i][j] > 0) {
                double filled_director_norm = sqrt(
                        pow(x_field_filled[i][j], 2) + pow(y_field_filled[i][j], 2));
                double desired_director_norm = sqrt(pow(desired_pattern.getXFieldPreferred()[i][j], 2) +
                                                    pow(desired_pattern.getYFieldPreferred()[i][j], 2));
                double x_direction_agreement =
                        x_field_filled[i][j] * desired_pattern.getXFieldPreferred()[i][j];
                double y_direction_agreement =
                        y_field_filled[i][j] * desired_pattern.getYFieldPreferred()[i][j];
                if (desired_director_norm != 0 && filled_director_norm != 0) {
                    director_agreement +=
                            abs(x_direction_agreement + y_direction_agreement) /
                            (filled_director_norm * desired_director_norm);
                    number_of_filled_elements++;
                }
            }
        }
    }
    return 1 - (double) director_agreement / (double) number_of_filled_elements;
}


double QuantifiedPattern::calculateNumberOfPaths() {
    unsigned int paths = getSequenceOfPaths().size();
    auto perimeter_length = (unsigned int) fmax(desired_pattern.getDimensions()[0],
                                                desired_pattern.getDimensions()[1]);
    return (double) paths / (double) perimeter_length;
}

double QuantifiedPattern::disagreement(double empty_spot_weight, double overlap_weight, double director_weight,
                                       double path_weight, double empty_spot_exponent, double over_lap_exponent,
                                       double director_exponent, double path_exponent) const {
    return empty_spot_weight * pow(empty_spots, empty_spot_exponent) +
           overlap_weight * pow(average_overlap, over_lap_exponent) +
           director_weight * pow(director_disagreement, director_exponent) +
           path_weight * pow(number_of_paths, path_exponent);
}

double QuantifiedPattern::disagreement(double empty_spot_weight, double overlap_weight, double director_weight,
                                       double path_weight) const {
    return disagreement(empty_spot_weight, overlap_weight, director_weight, path_weight, 1, 1, 1, 1);
}

