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

#include <string>
#include <random>

#include "desired_pattern.h"
#include "path.h"
#include "filling_config.h"
#include "auxiliary/valarray_operations.h"
#include "seed_point.h"

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
    /// Shuffled seed points from the current seed line.
    std::vector<SeedPoint> seed_points;
    /// How many seed lines are within the pattern.
    int seed_lines = 0;
    /// All seed points from all suitable splay lines, line order is shuffled and point order within a line is also shuffled.
    std::vector<std::vector<SeedPoint>> zero_splay_seeds;
    /// All seed points from all suitable perimeters, line order is shuffled and point order within a line is also shuffled.
    std::vector<std::vector<SeedPoint>> perimeter_seeds;
    /// All possible points where a path can start, binned based on their splay.
    std::vector<std::vector<vali>> binned_root_points;
    /// Parameter allowing for controlling whether dual seeding based reseeding occurs.
    bool is_reseeding_enabled = false;
    /// Switches whether seeds from seed line are taken at random or consecutively.
    bool is_random_filling_enabled = false;

    void fillPoint(const vali &point, const vald &normalized_direction, int value);

    void fillPointsFromList(const std::vector<vali> &points_to_fill, const vali &direction, int value);

    void fillPointsFromDisplacement(const vali &starting_position, const std::vector<vali> &list_of_displacements,
                                    const vali &previous_step, int value);

    void fillPointsFromDisplacement(const vali &starting_position, const std::vector<vali> &list_of_displacements,
                                    const vali &previous_step);

    vald getNewStep(vald &real_coordinates, int &length, vald &previous_move) const;

    bool propagatePath(Path &current_path, vald &positions, vald &previous_step, int length);


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
                                    std::vector<SeedPoint> &separated_starting_points, int line_index,
                                    int point_index);

    std::vector<SeedPoint> getSpacedLine(const double &separation, const std::vector<vali> &line, int line_index);

    /// Creates a path starting in starting_coordinates, where the first step is in the direction starting_step
    Path generateNewPathForDirection(const SeedPoint &seed_point, const vald &starting_step);

    void updateSeedPoints();

    std::vector<vali>
    findLineGeneral(const vali &start, std::vector<vali> (FilledPattern::*line_propagation)(vald, vald));

    void setupRootPoints();

    std::vector<std::vector<SeedPoint>> separateLines(std::vector<std::vector<vali>> list_of_lines, int line_index);

    vald calculateNextPosition(vald &positions, vald &previous_step, int length);

    [[nodiscard]] bool isDirectorContinuous(const vali &previous_coordinates, const vali &new_coordinates) const;

    [[nodiscard]] bool isInRange(const vali &index) const;

    [[nodiscard]] bool isInRange(const vald &index) const;

    double getOverlap(const std::vector<vali> &points_to_check);

    void updatePathOverlap(Path &path);

    vald getDirector(const vald &coordinates) const;

    vald getDirector(const vali &coordinates) const;

    bool isFree(const vali &coordinate) const;

    bool isTerminable(const vali &coordinate, const vald &direction);

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
    void fillPointsInHalfCircle(const vali &last_coordinate, const vali &previous_coordinate, int value);

    /// Looks for a suitable point where a new path can be started from. If
    SeedPoint findSeedPoint();

    /// Creates a path starting from starting_coordinates, going in both directions of the vector field
    Path generateNewPath(const SeedPoint &seed_point);

    void addNewPath(Path &new_path);

    [[nodiscard]] std::vector<Path> getSequenceOfPaths() const;

    void exportFilledMatrix(const fs::path &path) const;

    /// Updates overlaps in Paths for postprocessing.
    void updatePathsOverlap();
};


#endif //VECTOR_SLICER_FILLED_PATTERN_H
