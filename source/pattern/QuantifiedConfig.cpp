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

#include "QuantifiedConfig.h"
#include "FillingPatterns.h"
#include "../auxiliary/vector_operations.h"

#include <utility>
#include <cmath>
#include <omp.h>

QuantifiedConfig::QuantifiedConfig(FilledPattern pattern, DisagreementWeights disagreement_weights) :
        FilledPattern(std::move(pattern)),
        DisagreementWeights(disagreement_weights) {

}

QuantifiedConfig::QuantifiedConfig(const DesiredPattern &desired_pattern, FillingConfig &filling_config,
                                   DisagreementWeights disagreement_weights) :
        FilledPattern(desired_pattern, filling_config),
        DisagreementWeights(disagreement_weights) {

}

QuantifiedConfig::QuantifiedConfig(QuantifiedConfig &template_config, vectord parameters) :
        QuantifiedConfig(template_config) {
    if (parameters.size() != 3) {
        throw std::runtime_error("Config options can only be set with 3D vectors.");
    }
    setConfigOption(Repulsion, std::to_string(parameters[0]));
    setConfigOption(CollisionRadius, std::to_string(parameters[1]));
    setConfigOption(StartingPointSeparation, std::to_string(parameters[2]));
}

QuantifiedConfig::QuantifiedConfig(QuantifiedConfig &template_config, int seed) :
        QuantifiedConfig(template_config) {
    setConfigOption(Seed, std::to_string(seed));
}


double QuantifiedConfig::calculateEmptySpots() {
    int number_of_empty_spots = 0;
    int x_size = desired_pattern.get().getDimensions()[0];
    int y_size = desired_pattern.get().getDimensions()[1];
    int number_of_elements = 0;

    for (int i = 0; i < x_size; i++) {
        for (int j = 0; j < y_size; j++) {
            if (desired_pattern.get().getShapeMatrix()[i][j] == 1) {
                number_of_elements++;
                if (number_of_times_filled[i][j] == 0) {
                    number_of_empty_spots++;
                }
            }
        }
    }
    return (double) number_of_empty_spots / (double) number_of_elements;
}


double QuantifiedConfig::calculateAverageOverlap() {
    int number_of_filled_times = 0;
    int x_size = desired_pattern.get().getDimensions()[0];
    int y_size = desired_pattern.get().getDimensions()[1];
    int number_of_elements = 0;

    for (int i = 0; i < x_size; i++) {
        for (int j = 0; j < y_size; j++) {
            number_of_filled_times += number_of_times_filled[i][j];
            number_of_elements += desired_pattern.get().getShapeMatrix()[i][j];
        }
    }
    return (double) number_of_filled_times / (double) number_of_elements - 1 + empty_spots;
}

double QuantifiedConfig::calculateDirectorDisagreement() {
    double director_agreement = 0;
    int x_size = desired_pattern.get().getDimensions()[0];
    int y_size = desired_pattern.get().getDimensions()[1];
    int number_of_filled_elements = 0;

    for (int i = 0; i < x_size; i++) {
        for (int j = 0; j < y_size; j++) {
            if (number_of_times_filled[i][j] > 0) {
                double filled_director_norm = sqrt(
                        pow(x_field_filled[i][j], 2) + pow(y_field_filled[i][j], 2));
                double desired_director_norm = sqrt(pow(desired_pattern.get().getXFieldPreferred()[i][j], 2) +
                                                    pow(desired_pattern.get().getYFieldPreferred()[i][j], 2));
                double x_direction_agreement =
                        x_field_filled[i][j] * desired_pattern.get().getXFieldPreferred()[i][j];
                double y_direction_agreement =
                        y_field_filled[i][j] * desired_pattern.get().getYFieldPreferred()[i][j];
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

double QuantifiedConfig::calculateNumberOfPaths() {
    unsigned int paths = getSequenceOfPaths().size();
    auto perimeter_length = (unsigned int) fmax(desired_pattern.get().getDimensions()[0],
                                                desired_pattern.get().getDimensions()[1]);
    return (double) paths / (double) perimeter_length;
}

void QuantifiedConfig::evaluate() {
    fillWithPaths(*this);
    empty_spots = calculateEmptySpots();
    average_overlap = calculateAverageOverlap();
    director_disagreement = calculateDirectorDisagreement();
    number_of_paths = calculateNumberOfPaths();

    disagreement = empty_spot_weight * pow(empty_spots, empty_spot_exponent) +
                   overlap_weight * pow(average_overlap, overlap_exponent) +
                   director_weight * pow(director_disagreement, director_exponent) +
                   path_weight * pow(number_of_paths, path_exponent);
}

FillingConfig QuantifiedConfig::getConfig() const {
    return *this;
}

double QuantifiedConfig::getDisagreement() const {
    if (disagreement == DBL_MAX) {
        throw std::runtime_error("Disagreement of a pattern is equal to DBL_MAX. Most likely the pattern has"
                                 "not been evaluated before retrieving disagreement.");
    }
    return disagreement;
}

DesiredPattern QuantifiedConfig::getDesiredPattern() {
    return FilledPattern::desired_pattern;
}

double QuantifiedConfig::getDisagreement(int seeds, int threads) {
    std::vector<QuantifiedConfig> configs_with_various_seeds;
    std::vector<double> disagreements(seeds);
    for (int i = 0; i < seeds; i++) {
        configs_with_various_seeds.emplace_back(*this, i);
    }
    omp_set_num_threads(threads);
#pragma omp parallel for
    for (int i = 0; i < seeds; i++) {
        configs_with_various_seeds[i].evaluate();
        disagreements[i] = configs_with_various_seeds[i].getDisagreement();
    }

    return mean(disagreements);
}

FilledPattern QuantifiedConfig::getFilledPattern() {
    return FilledPattern((FilledPattern)*this);
}


