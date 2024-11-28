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
#include "auxiliary/simple_math_operations.h"
#include "vector_slicer_config.h"

#include <utility>
#include <cmath>
#include <omp.h>
#include <iomanip>
#include <algorithm>
#include <vector>
#include <random>
#include <iostream>

QuantifiedConfig::QuantifiedConfig(const FilledPattern &pattern,
                                   const Simulation &simulation) :
        FilledPattern(pattern),
        Simulation(simulation) {
}

QuantifiedConfig::QuantifiedConfig(const DesiredPattern &desired_pattern, FillingConfig &filling_config,
                                   const Simulation &simulation) :
        FilledPattern(desired_pattern, filling_config),
        Simulation(simulation) {
}

QuantifiedConfig::QuantifiedConfig(QuantifiedConfig &template_config, vectord parameters) :
        QuantifiedConfig(template_config) {
    vecd vector_parameters(parameters.begin(), parameters.end());
    if (isCollisionRadiusOptimised()) {
        setConfigOption(TerminationRadius, std::to_string(vector_parameters.back()));
        vector_parameters.pop_back();
    }
    if (isStartingPointSeparationOptimised()) {
        setConfigOption(SeedSpacing, std::to_string(vector_parameters.back()));
        vector_parameters.pop_back();
    }
    if (isRepulsionMagnitudeOptimised()) {
        setConfigOption(Repulsion, std::to_string(vector_parameters.back()));
        vector_parameters.pop_back();
    }
    if (isRepulsionAngleOptimised()) {
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
    if (number_of_elements > 0) {
        return (double) number_of_empty_spots / (double) number_of_elements;
    } else {
        return 1;
    }
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
    if (total_filled_elements > 0) {
        return (double) total_overlap / (double) total_filled_elements;
    } else {
        return 1;
    }
}


double QuantifiedConfig::localDirectorAgreement(int i, int j) {

    coord_d filled_director = {x_field_filled[i][j], y_field_filled[i][j]};
    coord_d desired_director = {desired_pattern.get().getXFieldPreferred()[i][j],
                             desired_pattern.get().getYFieldPreferred()[i][j]};
    double filled_director_norm = norm(filled_director);
    double desired_director_norm = norm(desired_director);
    double current_director_agreement = dot(filled_director, desired_director);

    double local_director_agreement = std::abs(current_director_agreement) /
                                      (filled_director_norm * desired_director_norm);
    if (isnan(local_director_agreement)) {
        return 0;
    }
    insertIntoBucket(local_director_agreement);
    return local_director_agreement;
}

void QuantifiedConfig::insertIntoBucket(double local_director_agreement) {
    double disagreement_angle = acos(local_director_agreement);
    if (local_director_agreement >= 1) {
        disagreement_angle = 0;
    }
    unsigned int bucket = int(disagreement_angle / bucket_size);
    director_disagreement_distribution[bucket]++;
    total_angular_director_disagreement += disagreement_angle;
}


double QuantifiedConfig::calculateDirectorDisagreement() {
    double director_agreement = 0;
    int x_size = desired_pattern.get().getDimensions()[0];
    int y_size = desired_pattern.get().getDimensions()[1];
    int number_of_filled_elements = 0;

    for (int i = 0; i < x_size; i++) {
        for (int j = 0; j < y_size; j++) {
            if (number_of_times_filled[i][j] > 0 &&
                desired_pattern.get().isInShape(veci{i, j})) {
                director_agreement += localDirectorAgreement(i, j);
                number_of_filled_elements++;
            }
        }
    }

    if (number_of_filled_elements > 0) {
        average_angular_director_disagreement =
                total_angular_director_disagreement / (double) number_of_filled_elements;
        return 1 - (double) director_agreement / (double) number_of_filled_elements;
    } else {
        return 1;
    }
}

void QuantifiedConfig::evaluate() {
#ifdef TIMING
    auto t1 = std::chrono::high_resolution_clock::now();
#endif
    setup();
    fillWithPaths(*this);
    empty_spots = calculateEmptySpots();
    average_overlap = calculateAverageOverlap();
    average_director_disagreement = calculateDirectorDisagreement();

    paths_number = (double) getSequenceOfPaths().size();
    path_multiplier = fmax(pow(paths_number, getPathsPower()), 1);

    disagreement_norm = getEmptySpotWeight() + getOverlapWeight() + getDirectorWeight();
    if (disagreement_norm <= 0) {
        throw std::runtime_error("Sum of disagreement weights must be positive.");
    }

    empty_spot_disagreement = getEmptySpotWeight() * pow(empty_spots, getEmptySpotPower()) / disagreement_norm;
    overlap_disagreement = getOverlapWeight() * pow(average_overlap, getOverlapPower()) / disagreement_norm;
    director_disagreement =
            getDirectorWeight() * pow(average_director_disagreement, getDirectorPower()) / disagreement_norm;

    disagreement = empty_spot_disagreement + overlap_disagreement + director_disagreement;

    total_disagreement = disagreement * path_multiplier;
#ifdef TIMING
    time_t end_time;
    time(&end_time);
    auto t2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> ms = t2 - t1;
    double us_per_pix =
            1000 * ms.count() / (desired_pattern.get().getDimensions()[0] * desired_pattern.get().getDimensions()[1]);
    std::cout << "Iteration duration: " << ms.count() << " ms (" << us_per_pix << " us/pix)" << std::endl;
    throw std::runtime_error("Timing finished.");
#endif
}

void QuantifiedConfig::printDisagreement() const {
    std::stringstream stream;

    double empty_spot_ratio = empty_spot_disagreement / disagreement * 100;
    double overlap_ratio = overlap_disagreement / disagreement * 100;
    double director_ratio = director_disagreement / disagreement * 100;

    stream << std::setprecision(3);

    stream << std::endl;
    stream << "Disagreement " << disagreement * path_multiplier << std::endl;
    stream << "\tType \t\tValue \tDisagreement \tPercentage" << std::endl;
    stream << "\tCoverage\t" << (1 - empty_spots) * 100 << "%\t" << empty_spot_disagreement << "\t"
           << empty_spot_ratio << std::endl;
    stream << "\tOverlap\t\t" << average_overlap * 100 << "%\t" << overlap_disagreement << "\t"
           << overlap_ratio << std::endl;
    stream << "\tDirector\t" << average_director_disagreement << "\t" << director_disagreement
           << "\t" << director_ratio << std::endl;
    stream << "\tAngle disagreement\t" << average_angular_director_disagreement * 180 / M_PI << "°" << std::endl;
    stream << "\n\tPaths\t" << paths_number << std::endl;
    stream << "\tPaths multiplier\t" << path_multiplier << std::endl;

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

double QuantifiedConfig::averagedFillDensity(const veci &position, int averaging_radius) const {
    int x_size = desired_pattern.get().getDimensions()[0];
    int y_size = desired_pattern.get().getDimensions()[1];

    int target_count = 0;
    int filling_count = 0;
    int tracked_pixels = 0;
    for (int x = position[0] - averaging_radius; x <= position[0] + averaging_radius; x++) {
        if (x < 0 || x >= x_size) {
            continue;
        }
        for (int y = position[1] - averaging_radius; y <= position[1] + averaging_radius; y++) {
            if (y < 0 || y >= y_size) {
                continue;
            }
            target_count += desired_pattern.get().getShapeMatrix()[x][y];
            filling_count += number_of_times_filled[x][y];
            tracked_pixels++;
        }
    }
    return (double) (filling_count - target_count) / (double) tracked_pixels;
}

std::vector<double> QuantifiedConfig::sampleFillDensities(uint16_t sample_count, int averaging_radius) const {
    int x_size = desired_pattern.get().getDimensions()[0];
    int y_size = desired_pattern.get().getDimensions()[1];

    std::mt19937 random_engine(getSeed());
    std::uniform_int_distribution x_distribution(0, x_size - 1);
    std::uniform_int_distribution y_distribution(0, y_size - 1);

    std::vector<double> sampled_fill_densities;
    sampled_fill_densities.reserve(sample_count);
    for (int i = 0; i < sample_count; i++) {
        veci position = {x_distribution(random_engine), y_distribution(random_engine)};
//        if (!desired_pattern.get().isInShape(position)) {
//            i--;
//            continue;
//        }
        double fill_density = averagedFillDensity(position, averaging_radius);
        sampled_fill_densities.emplace_back(fill_density);
    }
    return sampled_fill_densities;
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
    int number_of_layers = getNumberOfLayers();

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
                        getEmptySpotWeight() * getEmptySpotPower() * pow(empty_spots, getEmptySpotPower() - 1);
            }
            if (number_of_times_filled[i][j] > 1) {
                int local_overlap = number_of_times_filled[i][j] - 1;
                local_disagreement +=
                        getOverlapWeight() * getOverlapPower() * pow(average_overlap, getOverlapPower() - 1) *
                        local_overlap;
            }
            if (number_of_times_filled[i][j] > 0) {
                double local_director_disagreement = 1 - localDirectorAgreement(i, j);
                local_disagreement +=
                        getDirectorWeight() * getDirectorPower() *
                        pow(average_director_disagreement, getDirectorPower() - 1) *
                        local_director_disagreement;
            }

            local_disagreement *= path_multiplier;
            disagreement_row.emplace_back(local_disagreement);
        }
        disagreement_grid.emplace_back(disagreement_row);
    }
    return disagreement_grid;
}

const std::vector<unsigned int> &QuantifiedConfig::getDirectorDisagreementDistribution() const {
    return director_disagreement_distribution;
}


