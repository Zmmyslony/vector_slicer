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

enum rootPointSearchStage {
    SplayFilling, PerimeterFilling, RemainingFilling
};

/// Object used to fill the DesiredPattern based on selected FillingConfig
class FilledPattern : public FillingConfig {
    rootPointSearchStage search_stage = PerimeterFilling;
    std::mt19937 random_engine;

    std::vector<Path> sequence_of_paths;
    std::vector<coord> print_circle;
    std::vector<coord> repulsion_circle;
    std::vector<coord> list_of_points;
    std::vector<coord> collision_list;
    /// Shuffled seed points from the current seed line.
    std::vector<SeedPoint> seed_points;
    /// List of seed lines obtained from the DesiredPattern
    std::vector<std::vector<coord>> seed_lines;
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

    void fillPoint(const coord &point, const coord_d &normalized_direction, int value);

    void fillPointsFromList(const std::vector<coord> &points_to_fill, const coord_d &direction, int value);

    void fillPointsFromDisplacement(const coord &starting_position, const std::vector<coord> &list_of_displacements,
                                    const coord &previous_step, int value);

    void fillPointsFromDisplacement(const coord &starting_position, const std::vector<coord> &list_of_displacements,
                                    const coord &previous_step);

    coord_d getNewStep(coord_d &real_coordinates, coord_d &previous_move, int &length) const;

    bool propagatePath(Path &current_path, coord_d &positions, coord_d &previous_step, int length);


    [[nodiscard]] matrix_d getDualTensor(const coord &coordinates) const;

    double distance(const coord &first_point, const coord &second_point);

    void updateDualLineInDirection(coord_set &line_elements, coord_vector &line, coord_d previous_dual_director);

    coord findRemainingRootPoint();

    coord getFillablePoint();

/// Removes a selected path and removes the points which were filled while creating the path
    void removeLine(Path path);

    [[nodiscard]] bool isTerminable(const coord &point) const;

    [[nodiscard]] bool isFillablePointLeft() const;

    std::vector<coord> findDualLine(const coord &start);

    void tryAddingPointToSpacedLine(const coord &current_position, coord &previous_position,
                                    bool &is_filled_coordinate_encountered, double separation,
                                    std::vector<SeedPoint> &separated_starting_points, int line_index,
                                    int point_index);

    std::vector<SeedPoint> getSpacedLine(const std::vector<coord> &line, int line_index, int starting_index);

    /// Creates a path starting in starting_coordinates, where the first step is in the direction starting_step
    Path generateNewPathForDirection(const SeedPoint &seed_point, const coord_d &starting_step);

    void updateSeedPoints();

    void setupRootPoints();

    std::vector<std::vector<SeedPoint>> separateLines(std::vector<std::vector<coord>> list_of_lines, int line_index);

    coord_d calculateNextPosition(coord_d &positions, coord_d &previous_step, int length);

    [[nodiscard]] bool isDirectorContinuous(const coord_d &previous_coordinates, const coord_d &new_coordinates) const;

    [[nodiscard]] bool isInRange(const coord &index) const;

    double getOverlap(const std::vector<coord> &points_to_check);

    void updatePathOverlap(Path &path);

    coord_d getDirector(const coord_d &coordinates) const;

    coord_d getDirector(const coord &coordinates) const;

    bool isFillable(const coord &coordinate) const;

    bool isTerminable(const coord_d &coordinate, const coord_d &direction);

    std::vector<unsigned int> findOverlappingSeedLines();

    std::vector<SeedPoint> getSeedsFromRandomSeedLine();

    std::vector<SeedPoint> getSeedsFromOverlappingSeedLine(const std::vector<unsigned int> &overlapping_indices);

    std::vector<SeedPoint> getSpacedLineRandom(const std::vector<coord> &line, int line_index);

    std::vector<SeedPoint> getSpacedLineOverlapping(const std::vector<coord> &line, int line_index);

    void extendSeedLines();


public:

    std::vector<std::vector<double>> x_field_filled;

    std::vector<std::vector<double>> y_field_filled;

    std::reference_wrapper<const DesiredPattern> desired_pattern;

    std::vector<std::vector<uint8_t>> number_of_times_filled;

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
    void fillPointsInCircle(const coord &coordinates);

    /// Fills points in half circle at the end of the path
    void fillPointsInHalfCircle(const coord &last_coordinate, const coord &previous_coordinate, int value);

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


    bool isFillable(const coord_d &coordinate) const;

    void
    fillPointsFromDisplacementNonAligned(const coord &starting_position,
                                         const std::vector<coord> &list_of_displacements,
                                         const coord &previous_step, int value);

    void fillPointNonAligned(const coord &point, const coord_d &normalized_direction, int value);

    bool isInRange(const coord_d &index) const;

    bool isFilled(const coord &coordinate) const;
};


#endif //VECTOR_SLICER_FILLED_PATTERN_H
