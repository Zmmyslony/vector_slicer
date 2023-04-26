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
// Created by Michał Zmyślony on 29/09/2021.
//

#include "QuantifiedConfig.h"
#include "FillingPatterns.h"
#include "auxiliary/vector_operations.h"
#include "auxiliary/ValarrayOperations.h"

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

QuantifiedConfig::QuantifiedConfig(QuantifiedConfig &template_config, vectord parameters, int dims) :
        QuantifiedConfig(template_config) {
    if (parameters.size() != dims) {
        throw std::runtime_error("Config options can only be set with " + std::to_string(dims) + "D vectors.");
    }
    setConfigOption(Repulsion, std::to_string(parameters[0]));
    setConfigOption(CollisionRadius, std::to_string(parameters[1]));
    setConfigOption(StartingPointSeparation, std::to_string(parameters[2]));
    if (dims > 3) {
        setConfigOption(RepulsionRadius, std::to_string(parameters[3]));
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

double QuantifiedConfig::calculateDirectorDisagreement() {
    double director_agreement = 0;
    int x_size = desired_pattern.get().getDimensions()[0];
    int y_size = desired_pattern.get().getDimensions()[1];
    int number_of_filled_elements = 0;

    for (int i = 0; i < x_size; i++) {
        for (int j = 0; j < y_size; j++) {
            if (number_of_times_filled[i][j] > 0) {
                vald filled_director = {x_field_filled[i][j], y_field_filled[i][j]};
                vald desired_director = {desired_pattern.get().getXFieldPreferred()[i][j],
                                         desired_pattern.get().getYFieldPreferred()[i][j]};
                double filled_director_norm = norm(filled_director);
                double desired_director_norm = norm(desired_director);
                double current_director_agreement = dot(filled_director, desired_director);
                if (desired_director_norm != 0 && filled_director_norm != 0) {
                    director_agreement +=
                            std::abs(current_director_agreement) /
                            (filled_director_norm * desired_director_norm);
                    number_of_filled_elements++;
                }
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
    unsigned int paths_number = getSequenceOfPaths().size();
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

    disagreement = empty_spot_weight * pow(empty_spots, empty_spot_exponent) +
                   overlap_weight * pow(average_overlap, overlap_exponent) +
                   director_weight * pow(director_disagreement, director_exponent);
}

void QuantifiedConfig::printDisagreement() const {
    double empty_spot_disagreement = empty_spot_weight * pow(empty_spots, empty_spot_exponent);
    double overlap_disagreement = overlap_weight * pow(average_overlap, overlap_exponent);
    double director_disagreement_value = director_weight * pow(director_disagreement, director_exponent);

    std::stringstream stream;
    stream << std::fixed << std::setprecision(3);

    stream << std::endl;
    stream << "Total disagreement " << disagreement << std::endl;
    stream << "\tType \t\tValue \tDisagreement \tPercentage" << std::endl;
    stream << "\tEmpty spot\t" << empty_spots << "\t" << empty_spot_disagreement << "\t"
           << empty_spot_disagreement / disagreement * 100 << std::endl;
    stream << "\tOverlap\t\t" << average_overlap << "\t" << overlap_disagreement << "\t"
           << overlap_disagreement / disagreement * 100 << std::endl;
    stream << "\tDirector\t" << director_disagreement << "\t" << director_disagreement_value << "\t"
           << director_disagreement_value / disagreement * 100 << std::endl;

    std::cout << stream.str() << std::endl;
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
    return {desired_pattern};
}


FilledPattern QuantifiedConfig::getFilledPattern() const {
    return *this;
}

double QuantifiedConfig::getDisagreement(int seeds, int threads, bool is_disagreement_details_printed) {
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
    if (is_disagreement_details_printed) {
        std::cout << "Mean " << mean(disagreements) << ", standard deviation " << standardDeviation(disagreements)
                  << ", noise " << standardDeviation(disagreements) / mean(disagreements) << std::endl;
    }
    return mean(disagreements);
}

std::vector<QuantifiedConfig> QuantifiedConfig::findBestSeeds(int seeds, int threads) {
    std::vector<QuantifiedConfig> configs_with_various_seeds;
    std::vector<std::pair<double, int>> disagreements(seeds);
    for (int i = 0; i < seeds; i++) {
        configs_with_various_seeds.emplace_back(*this, i);
    }
    omp_set_num_threads(threads);
#pragma omp parallel for
    for (int i = 0; i < seeds; i++) {
        configs_with_various_seeds[i].evaluate();
    }

    std::sort(configs_with_various_seeds.begin(), configs_with_various_seeds.end(), [](auto &left, auto&right) {
        return left.getDisagreement() < right.getDisagreement();
    });
    return configs_with_various_seeds;
}


