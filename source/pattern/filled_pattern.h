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

using vecd = std::vector<double>;
using veci = std::vector<int>;

enum rootPointSearchStage {
    SplayFilling, PerimeterFilling, RemainingFilling
};

/// Object used to fill the DesiredPattern based on selected FillingConfig
class FilledPattern : public FillingConfig {
    rootPointSearchStage search_stage = PerimeterFilling;
    std::mt19937 random_engine;

    std::vector<Path> sequence_of_paths;
    std::vector<veci> print_circle;
    std::vector<veci> repulsion_circle;
    std::vector<veci> list_of_points;
    std::vector<veci> collision_list;
    /// Shuffled seed points from the current seed line.
    std::vector<SeedPoint> seed_points;
    /// List of seed lines obtained from the DesiredPattern
    std::vector<std::vector<veci>> seed_lines;
    /// Keeps track of which seed line are we using currently.
    int current_seed_line_index = 0;
    /// All seed points from all suitable splay lines, line order is shuffled and point order within a line is also shuffled.
    std::vector<std::vector<SeedPoint>> zero_splay_seeds;
    /// All seed points from all suitable perimeters, line order is shuffled and point order within a line is also shuffled.
    std::vector<std::vector<SeedPoint>> perimeter_seeds;
    /// All possible points where a path can start, binned based on their splay.
    std::vector<std::vector<coord>> binned_root_points;
    /// Parameter allowing for controlling whether dual seeding based reseeding occurs. Default: true
    bool is_reseeding_enabled = true;
    /// Switches whether seeds from seed line are taken at random or consecutively. Default: true
    bool is_random_filling_enabled = true;

    void fillPoint(const veci &point, const vecd &normalized_direction, int value);

    void fillPointsFromList(const std::vector<veci> &points_to_fill, const veci &direction, int value);

    void fillPointsFromDisplacement(const veci &starting_position, const std::vector<veci> &list_of_displacements,
                                    const veci &previous_step, int value);

    void fillPointsFromDisplacement(const veci &starting_position, const std::vector<veci> &list_of_displacements,
                                    const veci &previous_step);

    vecd getNewStep(vecd &real_coordinates, int &length, vecd &previous_move) const;

    bool propagatePath(Path &current_path, vecd &positions, vecd &previous_step, int length);


    [[nodiscard]] matrix_d getDualTensor(const veci &coordinates) const;

    double distance(const veci &first_point, const veci &second_point);

    void updateDualLineInDirection(coord_set &line_elements, coord_vector &line, vecd previous_dual_director);

    veci findRemainingRootPoint();

    veci getFillablePoint();

/// Removes a selected path and removes the points which were filled while creating the path
    void removeLine(Path path);

    [[nodiscard]] bool isFilled(const veci &coordinates) const;

    [[nodiscard]] bool isFillable(const veci &point) const;

    [[nodiscard]] bool isFillablePointLeft() const;

    std::vector<veci> findDualLine(const veci &start);

    void tryAddingPointToSpacedLine(const veci &current_position, veci &previous_position,
                                    bool &is_filled_coordinate_encountered, double separation,
                                    std::vector<SeedPoint> &separated_starting_points, int line_index,
                                    int point_index);

    std::vector<SeedPoint> getSpacedLine(const std::vector<veci> &line, int line_index, int starting_index);

    /// Creates a path starting in starting_coordinates, where the first step is in the direction starting_step
    Path generateNewPathForDirection(const SeedPoint &seed_point, const vecd &starting_step);

    void updateSeedPoints();

    void setupRootPoints();

    std::vector<std::vector<SeedPoint>> separateLines(std::vector<std::vector<veci>> list_of_lines, int line_index);

    vecd calculateNextPosition(vecd &positions, vecd &previous_step, int length);

    [[nodiscard]] bool isDirectorContinuous(const veci &previous_coordinates, const veci &new_coordinates) const;

    [[nodiscard]] bool isInRange(const veci &index) const;

    [[nodiscard]] bool isInRange(const vecd &index) const;

    double getOverlap(const std::vector<veci> &points_to_check);

    void updatePathOverlap(Path &path);

    vecd getDirector(const vecd &coordinates) const;

    vecd getDirector(const veci &coordinates) const;

    bool isFree(const veci &coordinate) const;

    bool isTerminable(const veci &coordinate, const vecd &direction);

    std::vector<unsigned int> findOverlappingSeedLines();

    std::vector<SeedPoint> getSeedsFromRandomSeedLine();

    std::vector<SeedPoint> getSeedsFromOverlappingSeedLine(const std::vector<unsigned int> &overlapping_indexes);

    std::vector<SeedPoint> getSpacedLineRandom(const std::vector<veci> &line, int line_index);

    std::vector<SeedPoint> getSpacedLineOverlapping(const std::vector<veci> &line, int line_index);

    void extendSeedLines();
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
    void fillPointsInCircle(const veci &coordinates);

    /// Fills points in half circle at the end of the path
    void fillPointsInHalfCircle(const veci &last_coordinate, const veci &previous_coordinate, int value);

    /// Looks for a suitable point where a new path can be started from. If
    SeedPoint findSeedPoint();

    /// Creates a path starting from starting_coordinates, going in both directions of the vector field
    Path generateNewPath(const SeedPoint &seed_point);

    void addNewPath(Path &new_path);

    [[nodiscard]] std::vector<Path> getSequenceOfPaths() const;

    void exportFilledMatrix(const fs::path &path) const;

    /// Updates overlaps in Paths for postprocessing.
    void updatePathsOverlap();

    std::vector<coord> getSeedCoordinates();

    void fillPointsInHalfCircle(const Path &path, int value, bool is_front);
};


#endif //VECTOR_SLICER_FILLED_PATTERN_H
