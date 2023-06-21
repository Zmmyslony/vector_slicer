// Copyright (c) 2021-2023, Michał Zmyślony, mlz22@cam.ac.uk.
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

//
// Created by Michał Zmyślony on 29/09/2021.
//

#include "quantified_config.h"
#include "filling_patterns.h"
#include "auxiliary/vector_operations.h"
#include "auxiliary/valarray_operations.h"
#include "vector_slicer_config.h"
#include "auxiliary/configuration_reading.h"

#include <utility>
#include <cmath>
#include <omp.h>
#include <iomanip>
#include <algorithm>
#include <vector>


QuantifiedConfig::QuantifiedConfig(const FilledPattern &pattern, const DisagreementWeights &disagreement_weights) :
        FilledPattern(pattern),
        DisagreementWeights(disagreement_weights) {

}

QuantifiedConfig::QuantifiedConfig(const DesiredPattern &desired_pattern, FillingConfig &filling_config,
                                   DisagreementWeights disagreement_weights) :
        FilledPattern(desired_pattern, filling_config),
        DisagreementWeights(disagreement_weights) {
}

QuantifiedConfig::QuantifiedConfig(QuantifiedConfig &template_config, vectord parameters) :
        QuantifiedConfig(template_config) {
    vecd vector_parameters( parameters.begin(), parameters.end());
    if (readKeyBool(BAYESIAN_CONFIG, "is_collision_radius_optimised")) {
        setConfigOption(CollisionRadius, std::to_string(vector_parameters.back()));
        vector_parameters.pop_back();
    }
    if (readKeyBool(BAYESIAN_CONFIG, "is_starting_point_separation_optimised")) {
        setConfigOption(StartingPointSeparation, std::to_string(vector_parameters.back()));
        vector_parameters.pop_back();
    }
    if (readKeyBool(BAYESIAN_CONFIG, "is_repulsion_magnitude_optimised")) {
        setConfigOption(Repulsion, std::to_string(vector_parameters.back()));
        vector_parameters.pop_back();
    }
    if (readKeyBool(BAYESIAN_CONFIG, "is_repulsion_angle_optimised")) {
        setConfigOption(RepulsionAngle, std::to_string(vector_parameters.back()));
        vector_parameters.pop_back();
    }
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
    int total_overlap = 0;
    int x_size = desired_pattern.get().getDimensions()[0];
    int y_size = desired_pattern.get().getDimensions()[1];
    int total_filled_elements = 0;

    for (int i = 0; i < x_size; i++) {
        for (int j = 0; j < y_size; j++) {
            if (number_of_times_filled[i][j] > 0) {
                total_overlap += number_of_times_filled[i][j] - 1;
                total_filled_elements++;
            }
        }
    }
    return (double) total_overlap / (double) total_filled_elements;
}


double QuantifiedConfig::localDirectorAgreement(int i, int j) {
    double local_director_agreement = 0;
    vald filled_director = {x_field_filled[i][j], y_field_filled[i][j]};
    vald desired_director = {desired_pattern.get().getXFieldPreferred()[i][j],
                             desired_pattern.get().getYFieldPreferred()[i][j]};
    double filled_director_norm = norm(filled_director);
    double desired_director_norm = norm(desired_director);
    double current_director_agreement = dot(filled_director, desired_director);
    if (desired_director_norm != 0 && filled_director_norm != 0) {
        local_director_agreement += std::abs(current_director_agreement) /
                                    (filled_director_norm * desired_director_norm);
    }
    return local_director_agreement;
}


double QuantifiedConfig::calculateDirectorDisagreement() {
    double director_agreement = 0;
    int x_size = desired_pattern.get().getDimensions()[0];
    int y_size = desired_pattern.get().getDimensions()[1];
    int number_of_filled_elements = 0;

    for (int i = 0; i < x_size; i++) {
        for (int j = 0; j < y_size; j++) {
            if (number_of_times_filled[i][j] > 0) {
                director_agreement += localDirectorAgreement(i, j);
                number_of_filled_elements++;
            }
        }
    }
    return 1 - (double) director_agreement / (double) number_of_filled_elements;
}

double QuantifiedConfig::calculatePathLengthDeviation(int order) {
    double length_scale = fmax(desired_pattern.get().getDimensions()[0], desired_pattern.get().getDimensions()[1]);
    double sum_of_lengths = 0;
    for (auto &path: getSequenceOfPaths()) {
        sum_of_lengths += path.getLength();
    }
    double average_length = sum_of_lengths / paths_number;

    double deviations_sum = 0;
    for (auto &path: getSequenceOfPaths()) {
        deviations_sum += pow(std::abs((path.getLength() - average_length) / length_scale), order);
    }
    deviations_sum /= paths_number;
    return pow(deviations_sum, 1. / order);
}

void QuantifiedConfig::evaluate() {
    setup();
    fillWithPaths(*this);
    empty_spots = calculateEmptySpots();
    average_overlap = calculateAverageOverlap();
    director_disagreement = calculateDirectorDisagreement();

    paths_number = (double) getSequenceOfPaths().size();
    multiplier = fmax(pow(paths_number, path_exponent), 1);

    disagreement = empty_spot_weight * pow(empty_spots, empty_spot_exponent) +
                   overlap_weight * pow(average_overlap, overlap_exponent) +
                   director_weight * pow(director_disagreement, director_exponent);

    total_disagreement = disagreement * multiplier;
}

void QuantifiedConfig::printDisagreement() const {
    double empty_spot_disagreement = empty_spot_weight * pow(empty_spots, empty_spot_exponent);
    double overlap_disagreement = overlap_weight * pow(average_overlap, overlap_exponent);
    double director_disagreement_value = director_weight * pow(director_disagreement, director_exponent);

    std::stringstream stream;
    stream << std::setprecision(2);

    stream << std::endl;
    stream << "Disagreement " << disagreement * multiplier << std::endl;
    stream << "\tType \t\tValue \tDisagreement \tPercentage" << std::endl;
    stream << "\tEmpty spot\t" << empty_spots * multiplier << "\t" << empty_spot_disagreement << "\t"
           << empty_spot_disagreement / disagreement * 100 << std::endl;
    stream << "\tOverlap\t\t" << average_overlap * multiplier << "\t" << overlap_disagreement << "\t"
           << overlap_disagreement / disagreement * 100 << std::endl;
    stream << "\tDirector\t" << director_disagreement * multiplier << "\t" << director_disagreement_value << "\t"
           << director_disagreement_value / disagreement * 100 << std::endl;
    stream << "\n\tPaths\t" << paths_number << std::endl;
    stream << "\tPaths multiplier\t" << multiplier << std::endl;

    std::cout << stream.str() << std::endl;
}

FillingConfig QuantifiedConfig::getConfig() const {
    return *this;
}

double QuantifiedConfig::getDisagreement() const {
    if (total_disagreement == DBL_MAX) {
        throw std::runtime_error("Disagreement of a pattern is equal to DBL_MAX. Most likely the pattern has"
                                 "not been evaluated before retrieving disagreement.");
    }
    return total_disagreement;
}

DesiredPattern QuantifiedConfig::getDesiredPattern() {
    return {desired_pattern};
}


FilledPattern QuantifiedConfig::getFilledPattern() const {
    return *this;
}

double QuantifiedConfig::getDisagreement(int seeds, int threads, bool is_disagreement_details_printed,
                                         double disagreement_percentile) {
    std::vector<double> disagreements(seeds);
    omp_set_num_threads(threads);
#pragma omp parallel for
    for (int i = 0; i < seeds; i++) {
        QuantifiedConfig current_config(*this, i);
        current_config.evaluate();
        disagreements[i] = current_config.getDisagreement();
    }

    if (is_disagreement_details_printed) {
        std::cout << "Mean " << mean(disagreements) << ", standard deviation " << standardDeviation(disagreements)
                  << ", noise " << standardDeviation(disagreements) / mean(disagreements) << std::endl;
    }
    std::sort(disagreements.begin(), disagreements.end());
    int return_index = disagreements.size() * (1 - disagreement_percentile);
    return disagreements[return_index];
}

std::vector<QuantifiedConfig> QuantifiedConfig::findBestSeeds(int seeds, int threads) {
    std::vector<QuantifiedConfig> configs_with_various_seeds;
    std::vector<std::pair<double, unsigned int>> disagreements(seeds);

    omp_set_num_threads(threads);
#pragma omp parallel for
    for (int i = 0; i < seeds; i++) {
        QuantifiedConfig current_config(*this, i);
        current_config.evaluate();
        disagreements[i] = {current_config.getDisagreement(), i};
    }

    std::sort(disagreements.begin(), disagreements.end(), [](auto &left, auto &right) {
        return (left.first < right.first);
    });
    int number_of_layers = readKeyInt(DISAGREEMENT_CONFIG, "number_of_layers");

    std::vector<QuantifiedConfig> configs_to_export;
    for (int i = 0; i < number_of_layers; i++) {
        configs_to_export.emplace_back(*this, disagreements[i].second);
    }
#pragma omp parallel for
    for (int i = 0; i < number_of_layers; i++) {
        configs_to_export[i].evaluate();
    }
    return configs_to_export;
}

std::vector<std::vector<double>> QuantifiedConfig::localDisagreementGrid() {
    int x_size = desired_pattern.get().getDimensions()[0];
    int y_size = desired_pattern.get().getDimensions()[1];

    std::vector<std::vector<double>> disagreement_grid;
    for (int i = 0; i < x_size; i++) {
        std::vector<double> disagreement_row;
        for (int j = 0; j < y_size; j++) {
            double local_disagreement = 0;
            if (desired_pattern.get().getShapeMatrix()[i][j] == 1 && number_of_times_filled[i][j] == 0) {
                local_disagreement +=
                        empty_spot_weight * empty_spot_exponent * pow(empty_spots, empty_spot_exponent - 1);
            }
            if (number_of_times_filled[i][j] > 1) {
                int local_overlap = number_of_times_filled[i][j] - 1;
                local_disagreement +=
                        overlap_weight * overlap_exponent * pow(average_overlap, overlap_exponent - 1) * local_overlap;
            }
            if (number_of_times_filled[i][j] > 0) {
                double local_director_disagreement = 1 - localDirectorAgreement(i, j);
                local_disagreement +=
                        director_weight * director_exponent * pow(director_disagreement, director_exponent - 1) * local_director_disagreement;
            }

            local_disagreement *= multiplier;
            disagreement_row.emplace_back(local_disagreement);
        }
        disagreement_grid.emplace_back(disagreement_row);
    }
    return disagreement_grid;
}


