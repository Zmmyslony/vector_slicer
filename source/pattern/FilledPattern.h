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
// Created by Michał Zmyślony on 21/09/2021.
//
#ifndef VECTOR_SLICER_FILLEDPATTERN_H
#define VECTOR_SLICER_FILLEDPATTERN_H


#include "DesiredPattern.h"
#include "Path.h"
#include "../auxiliary/FillingConfig.h"
#include <string>
#include <random>

enum pointSearchStage {PerimeterSearch, TotallyRandomPointSelection, EmptySpotRandomSelection};

class FilledPattern {
//    bool isPerimeterSearchOn = true;

    std::vector<Path> sequence_of_paths;
    std::vector<std::valarray<int>> points_in_circle;

    void
    fillPointsFromList(const std::vector<std::valarray<int>> &list_of_points, const std::valarray<int> &direction);

    void fillPointsFromDisplacement(const std::valarray<int> &starting_position,
                                    const std::vector<std::valarray<int>> &list_of_displacements,
                                    const std::valarray<int> &previous_step);

    std::vector<std::valarray<int>> findAllFillablePoints();

    std::valarray<double>
    getNewStep(std::valarray<double> &real_coordinates, int &length, std::valarray<double> &previous_move);

    bool
    tryGeneratingPathWithLength(Path &current_path, std::valarray<double> &positions, std::valarray<double> &previous_step,
                                int length);

    void fillPoint(const std::valarray<int> &point, const std::valarray<double> &normalized_direction);

    std::vector<std::valarray<int>> findRemainingFillablePointsInList(
            std::vector<std::valarray<int>> &list_of_points) const;

    std::vector<std::valarray<int>> findInitialStartingPoints(fillingMethod method);

    std::mt19937 random_engine;
    std::uniform_int_distribution<unsigned int> distribution;
    std::uniform_int_distribution<int> x_distribution;
    std::uniform_int_distribution<int> y_distribution;

    std::valarray<double> getDirector(const std::valarray<int> &positions);

    std::vector<std::valarray<int>>
    getSpacedLine(const double &distance, const std::vector<std::valarray<int>> &line);

    std::vector<std::valarray<int>> findDualLine(const std::valarray<int> &start);

    std::valarray<double> getDirector(const std::valarray<double> &positions);

    std::vector<std::valarray<int>>
    findDualLineOneDirection(std::valarray<double> coordinates, std::valarray<double> previous_dual_director);

public:
    FillingConfig config;
    bool is_filling_method_random = true;
    std::vector<std::vector<double>> x_field_filled;
    std::vector<std::vector<double>> y_field_filled;
    const DesiredPattern &desired_pattern;

    std::vector<std::valarray<int>> points_to_fill;
    std::vector<std::valarray<int>> collision_list;
    std::vector<std::vector<int>> number_of_times_filled;
    pointSearchStage search_stage = PerimeterSearch;

    FilledPattern(const DesiredPattern &desired_pattern, int print_radius, int collision_radius, int step_length,
                  unsigned int seed);

    FilledPattern(const DesiredPattern &desired_pattern, int print_radius, int collision_radius, int step_length);

    FilledPattern(const DesiredPattern &new_desired_pattern, FillingConfig new_config);

    void addNewPath(Path &new_path);

    void updateSearchStageAndFillablePoints();

    void fillPointsInCircle(const std::valarray<int> &starting_coordinates);

    Path generateNewPathForDirection(std::valarray<int> &starting_coordinates, const std::valarray<int> &starting_step);

    std::vector<Path> getSequenceOfPaths();

    unsigned int getNewElement();

    void exportToDirectory(const fs::path &directory) const;

    void fillPointsInHalfCircle(const std::valarray<int> &last_point, const std::valarray<int> &previous_point);

    std::valarray<int> findPointInShape();
};


#endif //VECTOR_SLICER_FILLEDPATTERN_H
