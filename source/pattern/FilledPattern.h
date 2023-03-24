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
// Created by Michał Zmyślony on 21/09/2021.
//
#ifndef VECTOR_SLICER_FILLEDPATTERN_H
#define VECTOR_SLICER_FILLEDPATTERN_H


#include "DesiredPattern.h"
#include "Path.h"
#include "FillingConfig.h"
#include <string>
#include <random>

using vald = std::valarray<double>;
using vali = std::valarray<int>;

enum pointSearchStage {
    PerimeterSearch, FullyRandomPointSelection, EmptySpotRandomSelection
};

class FilledPattern : public FillingConfig {
//    bool isPerimeterSearchOn = true;

    std::vector<Path> sequence_of_paths;
    std::vector<vali> print_circle;
    std::vector<vali> repulsion_circle;
    std::mt19937 random_engine;
    std::uniform_int_distribution<unsigned int> distribution;
    std::uniform_int_distribution<int> x_distribution;
    std::uniform_int_distribution<int> y_distribution;
    std::vector<vali> list_of_points;

    void fillPoint(const vali &point, const vald &normalized_direction, int value);

    void
    fillPointsFromList(const std::vector<vali> &list_of_points, const vali &direction, int value);

    void fillPointsFromDisplacement(const vali &starting_position, const std::vector<vali> &list_of_displacements,
                                    const vali &previous_step, int value);

    void fillPointsFromDisplacement(const vali &starting_position, const std::vector<vali> &list_of_displacements,
                                    const vali &previous_step);

    std::vector<vali> findAllFillablePoints() const;

    std::vector<vali> findRemainingFillablePointsInList(std::vector<vali> &list_of_points) const;

    std::vector<vali> findInitialStartingPoints(fillingMethod method);

    vald
    getNewStep(vald &real_coordinates, int &length, vald &previous_move) const;

    bool
    tryGeneratingPathWithLength(Path &current_path, vald &positions, vald &previous_step, int length);

    vald getDirector(const vali &positions) const;

    std::vector<vali>
    getSpacedLine(const double &distance, const std::vector<vali> &line);

    std::vector<vali> findDualLine(const vali &start);

    vald getDirector(const vald &positions);

    std::vector<vali>
    findDualLineOneDirection(vald coordinates, vald previous_dual_director);

public:
    bool is_filling_method_random = true;
    std::reference_wrapper<const DesiredPattern> desired_pattern;
    std::vector<std::vector<double>> x_field_filled;
    std::vector<std::vector<double>> y_field_filled;

    std::vector<vali> points_to_fill;
    std::vector<vali> collision_list;
    std::vector<std::vector<int>> number_of_times_filled;
    pointSearchStage search_stage = PerimeterSearch;

    FilledPattern(const DesiredPattern &desired_pattern, int print_radius, int collision_radius, int step_length,
                  unsigned int seed);

    FilledPattern(const DesiredPattern &desired_pattern, int print_radius, int collision_radius, int step_length);

    FilledPattern(const DesiredPattern &new_desired_pattern, FillingConfig new_config);

    void setup();

    void addNewPath(Path &new_path);

    void updateSearchStageAndFillablePoints();

    void fillPointsInCircle(const vali &starting_coordinates);

    void removePoints();

    Path generateNewPathForDirection(vali &starting_coordinates, const vali &starting_step);

    std::vector<Path> getSequenceOfPaths();

    unsigned int getNewElement();

    void exportFilledMatrix(const fs::path &directory) const;

    void fillPointsInHalfCircle(const vali &last_point, const vali &previous_point, int value);

    vali findPointInShape();

    bool isFilled(const vali &coordinates);

    void removeShortLines();

    void removeLine(Path path);

    bool isPointPerimeterFree(const vali &point);

    bool isPointInShape(const vali &point);
};


#endif //VECTOR_SLICER_FILLEDPATTERN_H
