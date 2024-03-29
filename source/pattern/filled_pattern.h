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
// Created by Michał Zmyślony on 21/09/2021.
//
#ifndef VECTOR_SLICER_FILLED_PATTERN_H
#define VECTOR_SLICER_FILLED_PATTERN_H


#include "desired_pattern.h"
#include "path.h"
#include "filling_config.h"
#include "auxiliary/valarray_operations.h"
#include <string>
#include <random>

using vald = std::valarray<double>;
using vali = std::valarray<int>;

enum rootPointSearchStage {
    SplayFilling, PerimeterFilling, RemainingFilling
};

/// Object used to fill the DesiredPattern based on selected FillingConfig
class FilledPattern : public FillingConfig {
    rootPointSearchStage search_stage = PerimeterFilling;
    std::mt19937 random_engine;

    std::vector<Path> sequence_of_paths;
    std::vector<vali> print_circle;
    std::vector<vali> repulsion_circle;
    std::vector<vali> list_of_points;
    std::vector<vali> collision_list;
    std::vector<vali> seed_points = {};
    std::vector<std::vector<vali>> zero_splay_seeds;
    std::vector<std::vector<vali>> perimeter_seeds;

    std::vector<std::vector<vali>> binned_root_points;

    void fillPoint(const vali &point, const vald &normalized_direction, int value);

    void fillPointsFromList(const std::vector<vali> &points_to_fill, const vali &direction, int value);

    void fillPointsFromDisplacement(const vali &starting_position, const std::vector<vali> &list_of_displacements,
                                    const vali &previous_step, int value);

    void fillPointsFromDisplacement(const vali &starting_position, const std::vector<vali> &list_of_displacements,
                                    const vali &previous_step);

    vald getNewStep(vald &real_coordinates, int &length, vald &previous_move) const;

    bool tryGeneratingPathWithLength(Path &current_path, vald &positions, vald &previous_step, int length);


    [[nodiscard]] matrix_d getDualTensor(const vali &coordinates) const;

    double distance(const vali &first_point, const vali &second_point);

    std::vector<vali> findDualLineOneDirection(vald coordinates, vald previous_dual_director);

    vali findRemainingRootPoint();

    vali getFillablePoint();

/// Removes a selected path and removes the points which were filled while creating the path
    void removeLine(Path path);

    [[nodiscard]] bool isFilled(const vali &coordinates) const;

    [[nodiscard]] bool isFillable(const vali &point) const;

    [[nodiscard]] bool isFillablePointLeft() const;

    std::vector<vali> findDualLine(const vali &start);

    void tryAddingPointToSpacedLine(const vali &current_position, vali &previous_position,
                                    bool &is_filled_coordinate_encountered, double separation,
                                    std::vector<vali> &separated_starting_points);

    std::vector<vali> getSpacedLine(const double &separation, const std::vector<vali> &line);
    /// Creates a path starting in starting_coordinates, where the first step is in the direction starting_step
    Path generateNewPathForDirection(vali &starting_coordinates, const vali &starting_step);

    void updateSeedPoints();

    std::vector<vali>
    findLineGeneral(const vali &start, std::vector<vali> (FilledPattern::*line_propagation)(vald, vald));

    void setupRootPoints();

    std::vector<std::vector<vali>> separateLines(std::vector<std::vector<vali>> list_of_lines);

    vald calculateNextPosition(vald &positions, vald &previous_step, int length);

    [[nodiscard]] bool isDirectorContinuous(const vali &previous_coordinates, const vali &new_coordinates) const;

    [[nodiscard]] bool isInRange(const vali &index) const;

    [[nodiscard]] bool isInRange(const vald &index) const;

public:

    std::vector<std::vector<double>> x_field_filled;

    std::vector<std::vector<double>> y_field_filled;

    std::reference_wrapper<const DesiredPattern> desired_pattern;

    std::vector<std::vector<int>> number_of_times_filled;

    FilledPattern(const DesiredPattern &desired_pattern, int print_radius, int collision_radius, int step_length,
                  unsigned int seed);

    FilledPattern(const DesiredPattern &desired_pattern, int print_radius, int collision_radius, int step_length);

    FilledPattern(const DesiredPattern &new_desired_pattern, FillingConfig new_config);

    /// Sets up the objects deriving from the FillingConfig
    void setup();

    /// Postprocessing: removes paths shorter than the set threshold
    void removeShortLines(double length_coefficient);

    /// Postprocessing: removes paths consisting of only single points
    void removePoints();

    /// Fills points around the coordinates in a radius of print_radius
    void fillPointsInCircle(const vali &coordinates);

    /// Fills points in half circle at the end of the path
    void fillPointsInHalfCircle(const vali &last_point, const vali &previous_point, int value);

    /// Looks for a suitable point where a new path can be started from. If
    vali findSeedPoint();

    /// Creates a path starting from starting_coordinates, going in both directions of the vector field
    Path generateNewPath(vali &starting_coordinates);

    void addNewPath(Path &new_path);

    std::vector<Path> getSequenceOfPaths();

    void exportFilledMatrix(const fs::path &path) const;
};


#endif //VECTOR_SLICER_FILLED_PATTERN_H
