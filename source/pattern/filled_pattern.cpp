// Copyright (c) 2021-2025, Michał Zmyślony, mlz22@cam.ac.uk.
//
// Please cite following publication if you use any part of this code in work you publish or distribute:
// [1] Michał Zmyślony M., Klaudia Dradrach, John S. Biggins,
//    Slicing vector fields into tool paths for additive manufacturing of nematic elastomers,
//    Additive Manufacturing, Volume 97, 2025, 104604, ISSN 2214-8604, https://doi.org/10.1016/j.addma.2024.104604.
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

#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-msc51-cpp"

#include <iostream>
#include "filled_pattern.h"

#include "importing_and_exporting/exporting.h"
#include "auxiliary/geometry.h"
#include "auxiliary/perimeter.h"
#include "auxiliary/repulsion.h"
#include "auxiliary/valarray_operations.h"
#include "auxiliary/vector_operations.h"
#include "auxiliary/line_operations.h"
#include "auxiliary/simple_math_operations.h"

#define INVALID_POSITION {-1, -1}
/// Minimal value of cosine between current and previous director for it to be assumed as continuous.
#define DIRECTOR_DISCONTINUITY_THRESHOLD 0

bool isValid(const coord &positions) {
    return positions.x >= 0 && positions.y >= 0;
}

bool isValid(const coord_d &positions) {
    return positions.x >= 0 && positions.y >= 0;
}

FilledPattern::FilledPattern(const DesiredPattern &new_desired_pattern, const FillingConfig new_config) :
        desired_pattern(std::cref(new_desired_pattern)),
        FillingConfig(new_config) {
    desired_pattern.get().isPatternUpdated();
    int x_dim = desired_pattern.get().getDimensions()[0];
    int y_dim = desired_pattern.get().getDimensions()[1];

    number_of_times_filled = std::vector<std::vector<uint8_t >>(x_dim, std::vector<uint8_t>(y_dim, 0));
    x_field_filled = std::vector<std::vector<double >>(x_dim, std::vector<double>(y_dim, 0));
    y_field_filled = std::vector<std::vector<double >>(x_dim, std::vector<double>(y_dim, 0));
    setup();
}

/**
 * Takes in a list of seed lines which are sorted by adjacency (ordered), shuffles the order of lines, separates in
 * equidistant seeds and returns shuffled seeds.
 * @param list_of_lines
 * @param line_index starting line index
 * @return 2D array of Seed objects, shuffled so that they can be popped back
 */
std::vector<std::vector<SeedPoint>>
FilledPattern::separateLines(std::vector<std::vector<coord>> list_of_lines, int line_index) {
    std::vector<std::vector<SeedPoint>> separated_lines;
    std::shuffle(list_of_lines.begin(), list_of_lines.end(), random_engine);
    for (auto &line: list_of_lines) {
        std::vector<SeedPoint> spaced_line = getSpacedLine(line, line_index, 0);
        if (is_random_filling_enabled) {
            std::shuffle(spaced_line.begin(), spaced_line.end(), random_engine);
        }
        separated_lines.emplace_back(spaced_line);
        line_index++;
    }
    return separated_lines;
}

/// Extends the seed lines by SeedSeparation in the dual direction, so there is an overlap between different seed lines
/// which will make the inter-seed-line spacing consistent.
void FilledPattern::extendSeedLines() {
    for (std::vector<coord> &seed_line: seed_lines) {
        if (isLooped(seed_line)) { continue; }

        coord_d front_dual = perpendicular(getDirector(seed_line.front()));
        coord_d previous_displacement = to_coord_d(seed_line.front() - seed_line[3]);
        if (dot(front_dual, previous_displacement) < 0) { front_dual = front_dual * (-1); }

        std::vector<coord> front_displacements = pixeliseLine(front_dual * 2 * getSeedSpacing());
        coord front = seed_line.front();
        for (auto &displacement: front_displacements) {
            coord current = front + displacement;
            if (desired_pattern.get().isInRange(current)) {
                seed_line.insert(seed_line.begin(), current);
            }
        }

        coord_d back_dual = perpendicular(getDirector(seed_line.back()));
        previous_displacement = to_coord_d(seed_line.back() - seed_line[seed_line.size() - 4]);
        if (dot(back_dual, previous_displacement) < 0) { back_dual = back_dual * (-1); }

        std::vector<coord> back_displacements = pixeliseLine(back_dual * 2 * getSeedSpacing());
        coord back = seed_line.back();
        for (auto &displacement: back_displacements) {
            coord current = back + displacement;
            if (desired_pattern.get().isInRange(current)) {
                seed_line.emplace_back(current);
            }
        }
    }
}


void FilledPattern::setup() {
    random_engine = std::mt19937(getSeed());
    print_circle = findPointsInDisk(getPrintRadius());
    collision_list = circleDisplacements(getTerminationRadius());

    switch (getInitialSeedingMethod()) {
        case Splay:
            if (desired_pattern.get().isSplayProvided()) {
                search_stage = SplayFilling;
                seed_lines = desired_pattern.get().getLineDensityMinima();
                extendSeedLines();
            } else {
                search_stage = RemainingFilling;
                seed_lines = desired_pattern.get().getPerimeterList();
            }
            break;
        case Perimeter:
            search_stage = PerimeterFilling;
            seed_lines = desired_pattern.get().getPerimeterList();
            break;
        case Dual:
            search_stage = RemainingFilling;
            break;
    }
    setupRootPoints();
    updateSeedPoints();
}


FilledPattern::FilledPattern(const DesiredPattern &desired_pattern, int print_radius, int collision_radius,
                             int step_length, unsigned int seed) :
        FilledPattern::FilledPattern(desired_pattern,
                                     FillingConfig(Perimeter, collision_radius, 2 * print_radius, 1.0, step_length,
                                                   print_radius,
                                                   0, seed)) {
}


FilledPattern::FilledPattern(const DesiredPattern &desired_pattern, int print_radius, int collision_radius,
                             int step_length) :
        FilledPattern::FilledPattern(desired_pattern, print_radius, collision_radius, step_length, 0) {}


/**
 * Contains all points that can be used as seeds, sorted depending on their value of splay, as areas of low splay
 * ought to be more suitable for rooting a seed line.
 */
void FilledPattern::setupRootPoints() {
    std::vector<std::vector<coord>> root_points = desired_pattern.get().getSplaySortedEmptySpots();
    if (root_points.empty()) {
        return;
    }

    for (auto &bin: root_points) {
        if (!bin.empty()) {
            std::shuffle(bin.begin(), bin.end(), random_engine);
        }
    }
    binned_root_points = root_points;
}

/// Finds index of overlapping seed line.
std::vector<unsigned int> FilledPattern::findOverlappingSeedLines() {
    std::vector<unsigned int> overlapping_indices;
    for (int i = 0; i < seed_lines.size(); i++) {
        for (auto &coordinate: seed_lines[i]) {
            if (isFilled(coordinate)) {
                overlapping_indices.emplace_back(i);
                break;
            }
        }
    }
    return overlapping_indices;
}

std::vector<SeedPoint> FilledPattern::getSeedsFromRandomSeedLine() {
//    uint sum_of_sizes = 0;
//    for (auto &seed_line  : seed_lines) {
//        sum_of_sizes += seed_line.size();
//    }
//    std::uniform_int_distribution<> distribution(0, sum_of_sizes - 1);
//    int i_raw = distribution(random_engine);
//    uint previous_line_end = 0;
//    int i = 0;
//    for (int j = 0; i < seed_lines.size(); j++) {
//        if (i_raw >= previous_line_end && i_raw < previous_line_end + seed_lines[j].size()) {
//            i = j;
//            break;
//        }
//        previous_line_end += seed_lines[j].size();
//    }

    std::uniform_int_distribution<> distribution(0, seed_lines.size() - 1);
    unsigned int i = distribution(random_engine);
    std::vector<coord> current_seed_line = seed_lines[i];
    seed_lines.erase(seed_lines.begin() + i);

    return getSpacedLineRandom(current_seed_line, current_seed_line_index);
}

std::vector<SeedPoint> FilledPattern::getSpacedLineRandom(const std::vector<coord> &line, int line_index) {
    std::uniform_int_distribution<> index_distribution(0, line.size() - 1);
    int starting_index = index_distribution(random_engine);
    return getSpacedLine(line, line_index, starting_index);
}

std::vector<SeedPoint>
FilledPattern::getSeedsFromOverlappingSeedLine(const std::vector<unsigned int> &overlapping_indices) {
//    uint sum_of_sizes = 0;
//    for (uint j : overlapping_indices) {
//        sum_of_sizes += seed_lines[j].size();
//    }
//    std::uniform_int_distribution<> distribution(0, sum_of_sizes - 1);
//    int i_raw = distribution(random_engine);
//    uint previous_line_end = 0;
//    int i = 0;
//    for (uint j : overlapping_indices) {
//        if (i_raw >= previous_line_end && i_raw < previous_line_end + seed_lines[j].size()) {
//            i = j;
//            break;
//        }
//        previous_line_end += seed_lines[j].size();
//    }

    std::uniform_int_distribution<> distribution(0, overlapping_indices.size() - 1);
    unsigned int i = overlapping_indices[distribution(random_engine)];
    std::vector<coord> current_seed_line = seed_lines[i];
    seed_lines.erase(seed_lines.begin() + i);

    return getSpacedLineOverlapping(current_seed_line, current_seed_line_index);
}

std::vector<SeedPoint> FilledPattern::getSpacedLineOverlapping(const std::vector<coord> &line, int line_index) {
    std::vector<int> i_overlapping;
    for (int i = 0; i < line.size(); i++) {
        if (isFilled(line[i])) {
            i_overlapping.emplace_back(i);
        }
    }
    if (i_overlapping.empty()) {
        throw std::runtime_error(
                "No overlapping coordinates were found during spacing the path. This cannot happen.");
    }
    std::uniform_int_distribution<> index_distribution(0, i_overlapping.size() - 1);
    int starting_index = i_overlapping[index_distribution(random_engine)];
    return getSpacedLine(line, line_index, starting_index);
}

void FilledPattern::updateSeedPoints() {
    if (!seed_lines.empty()) {
        std::vector<unsigned int> overlapping_seed_lines = findOverlappingSeedLines();
        if (overlapping_seed_lines.empty()) {
            seed_points = getSeedsFromRandomSeedLine();
        } else {
            seed_points = getSeedsFromOverlappingSeedLine(overlapping_seed_lines);
        }
        if (is_random_filling_enabled) {
            std::shuffle(seed_points.begin(), seed_points.end(), random_engine);
        }
        current_seed_line_index++;
        return;
    }

    if (is_reseeding_enabled) {
        coord root_point = findRemainingRootPoint();
        if (root_point.x == -1) {
            seed_points = {INVALID_SEED};
            return;
        }
        std::vector<coord> dual_line = findDualLine(root_point);
        int first_seed_position = (int) getPrintRadius();
        if (dual_line.size() <= (int) (getSeedSpacing())) {
            first_seed_position = dual_line.size() / 2;
        }
        std::vector<SeedPoint> spaced_dual_line = getSpacedLine(dual_line, current_seed_line_index,
                                                                first_seed_position);
        current_seed_line_index++;
        if (is_random_filling_enabled) {
            std::shuffle(spaced_dual_line.begin(), spaced_dual_line.end(), random_engine);
        }
        seed_points = spaced_dual_line;
    } else {
        seed_points = {INVALID_SEED};
        return;
    }
}


coord FilledPattern::findRemainingRootPoint() {
    while (isFillablePointLeft()) {
        coord test_point = getFillablePoint();
        if (!isTerminable(test_point)) {
            return test_point;
        }
    }
    // If there are no remaining root points, return invalid position.
    return INVALID_POSITION;
}

void FilledPattern::fillPoint(const coord &point, const coord_d &normalized_direction, int value) {
    if (isInRange(point) && number_of_times_filled[point.x][point.y] + value >= 0) {
        number_of_times_filled[point.x][point.y] += value;

        if (normalized_direction.x * x_field_filled[point.x][point.y] +
            normalized_direction.y * y_field_filled[point.x][point.y] < 0) {
            value *= -1;
        }
        x_field_filled[point.x][point.y] += normalized_direction.x * value;
        y_field_filled[point.x][point.y] += normalized_direction.y * value;
    }
}

void FilledPattern::fillPointNonAligned(const coord &point, const coord_d &normalized_direction, int value) {
    if (isInRange(point) && number_of_times_filled[point.x][point.y] + value >= 0) {
        number_of_times_filled[point.x][point.y] += value;
        x_field_filled[point.x][point.y] += normalized_direction.x * value;
        y_field_filled[point.x][point.y] += normalized_direction.y * value;
    }
}


void FilledPattern::fillPointsFromList(const std::vector<coord> &points_to_fill, const coord_d &direction, int value) {
    coord_d normalized_direction = normalized(direction);
    for (auto &point: points_to_fill) {
        fillPoint(point, normalized_direction, value);
    }
}


void FilledPattern::fillPointsFromDisplacement(const coord &starting_position,
                                               const std::vector<coord> &list_of_displacements,
                                               const coord &previous_step, int value) {
    coord_d normalized_direction = normalized(previous_step);
    for (auto &displacement: list_of_displacements) {
        coord point = starting_position + displacement;
        if (isInRange(point)) {
            fillPoint(point, normalized_direction, value);
        }
    }
}

void FilledPattern::fillPointsFromDisplacementNonAligned(const coord &starting_position,
                                                         const std::vector<coord> &list_of_displacements,
                                                         const coord &previous_step, int value) {
    coord_d normalized_direction = normalized(previous_step);
    for (auto &displacement: list_of_displacements) {
        coord point = starting_position + displacement;
        if (isInRange(point)) {
            fillPointNonAligned(point, normalized_direction, value);
        }
    }
}


void FilledPattern::fillPointsFromDisplacement(const coord &starting_position,
                                               const std::vector<coord> &list_of_displacements,
                                               const coord &previous_step) {
    fillPointsFromDisplacement(starting_position, list_of_displacements, previous_step, 1);
}

coord_d FilledPattern::getDirector(const coord_d &coordinates) const {
    return desired_pattern.get().getDirector(coordinates);
}

coord_d FilledPattern::getDirector(const coord &coordinates) const {
    return desired_pattern.get().getDirector(coordinates);
}

coord_d FilledPattern::getNewStep(coord_d &real_coordinates, coord_d &previous_move, int &length) const {
    coord_d new_move = getDirector(real_coordinates) * length;

    if (dot(new_move, previous_move) >= 0) {
        return new_move;
    } else {
        return -1 * new_move;
    }
}


coord_d FilledPattern::calculateNextPosition(coord_d &positions, coord_d &previous_step, int length) {
    coord_d new_step = getNewStep(positions, previous_step, length);
    coord_d new_positions = positions + new_step;

    if (getRepulsion() > 0) {
        coord_d repulsion = getLineBasedRepulsion(desired_pattern.get().getShapeMatrix(), number_of_times_filled,
                                                  new_step, new_positions, desired_pattern.get().getDimensions(),
                                                  getPrintRadius(), getRepulsion(), getRepulsionAngleCosine());
        new_positions = new_positions + repulsion;
        new_step = new_step + repulsion;
    }

    // Check if newly generated position is valid
    if (isTerminable(new_positions, new_step)) {
        return INVALID_POSITION;
    } else {
        return new_positions;
    }
}

bool FilledPattern::isInRange(const coord &index) const {
    return desired_pattern.get().isInRange(index);
}

bool FilledPattern::isInRange(const coord_d &index) const {
    return desired_pattern.get().isInRange(coord(index));
}


bool FilledPattern::isDirectorContinuous(const coord_d &previous_coordinates, const coord_d &new_coordinates) const {
    coord_d previous_director = getDirector(previous_coordinates);
    coord_d new_director = getDirector(new_coordinates);
    double product = dot(previous_director, new_director);

    if (!desired_pattern.get().isVectorFilled()) { product = fabs(product); }

    return product >= desired_pattern.get().getDiscontinuityThresholdCos();
}

/// Returns bool depending whether propagation was successful.
bool FilledPattern::propagatePath(Path &current_path, coord_d &positions, coord_d &previous_step, int length) {
    if (!desired_pattern.get().isInShape(positions)) { return false; }

    // Try creating the longest possible step
    coord_d new_positions = INVALID_POSITION;
    /// Keeps last valid position, even if it is discontinuous.
    coord_d last_discontinuous_positions = INVALID_POSITION;

    while (--length > 1 && !isValid(new_positions)) {
        new_positions = calculateNextPosition(positions, previous_step, length);

        if (desired_pattern.get().isInShape(new_positions)) {
            bool is_director_continuous = isDirectorContinuous(positions, new_positions);
            switch (desired_pattern.get().getDiscontinuityBehaviour()) {
                case DISCONTINUITY_IGNORE:
                    break;
                case DISCONTINUITY_STICK:
                    if (is_director_continuous) {
                        if (isValid(last_discontinuous_positions)) {
                            new_positions = last_discontinuous_positions;
                        }
                        break;
                    } else {
                        last_discontinuous_positions = new_positions;
                        new_positions = INVALID_POSITION;
                    }
                case DISCONTINUITY_TERMINATE:
                    if (is_director_continuous) {
                        break;
                    } else {
                        new_positions = INVALID_POSITION;
                    }
            }
        }
    }

    if (!isValid(new_positions)) { return false; }

    previous_step = new_positions - positions;
    if (norm(previous_step) < 1) { return false; }

    coord_d tangent = normalisedResultant(previous_step, getDirector(positions));
    coord_d normal = perpendicular(tangent) * getPrintRadius();

    current_path.addPoint(new_positions, new_positions + normal, new_positions - normal);
    std::vector<coord> current_points_to_fill = current_path.findPointsToFill(isFilled(coord(positions)));
    fillPointsFromList(current_points_to_fill, previous_step, 1);
    // If no points were filled in this step, it indicates that the move ``bounced'' in an unpredicted direction
    // indicating the path is no longer valid as it most likely encountered a singularity.
    if (current_points_to_fill.empty()) { return false; }

    positions = new_positions;
    return true;
}

double FilledPattern::getOverlap(const std::vector<coord> &points_to_check) {
    int points_count = points_to_check.size();
    int overlap = 0;
    for (auto &point: points_to_check) {
        if (isInRange(point)) {
            overlap += number_of_times_filled[point.x][point.y] - 1;
        }
    }
    if (points_count == 0) {
        return 0;
    }
    return (double) overlap / (double) points_count;
}

void FilledPattern::updatePathOverlap(Path &path) {
    std::vector<double> overlap_array;
    overlap_array.reserve(path.size());
    std::vector<coord> first_segment = path.findPointsToFill(1, false);

    double current_edge_overlap = getOverlap(first_segment);
    // Overlap is defined on edges, while paths are defined on nodes, so first and last nodes will have overlaps
    // corresponding to first and last edges, while the middle one will have n[i] = (e[i] + e[i - 1]) / 2
    overlap_array.emplace_back(current_edge_overlap);
    for (int i = 2; i < path.size(); i++) {
        double previous_edge_overlap = current_edge_overlap;
        std::vector<coord> current_segment = path.findPointsToFill(i, false);

        current_edge_overlap = getOverlap(current_segment);
        overlap_array.emplace_back((previous_edge_overlap + current_edge_overlap) / 2);
    }
    overlap_array.emplace_back(current_edge_overlap);
    path.setOverlap(overlap_array);
}

void FilledPattern::updatePathsOverlap() {
    for (auto &path: sequence_of_paths) {
        updatePathOverlap(path);
    }
}

Path FilledPattern::generateNewPathForDirection(const SeedPoint &seed_point, const coord_d &starting_step) {
    Path path(seed_point, getPrintRadius());
    coord_d current_positions = to_coord_d(seed_point.getCoordinates());
    coord_d current_step = starting_step;

    bool is_previous_propagation_successful = true;
    while (is_previous_propagation_successful) {
        is_previous_propagation_successful = propagatePath(path, current_positions, current_step, getStepLength());
    }

    return path;
}


Path FilledPattern::generateNewPath(const SeedPoint &seed_point) {
    const coord_d starting_step = getDirector(seed_point.getCoordinates());
    Path forward_path = generateNewPathForDirection(seed_point, starting_step);
    Path backward_path = generateNewPathForDirection(seed_point, (-1) * starting_step);

    return {forward_path, backward_path};
}


void FilledPattern::fillPointsInCircle(const coord &coordinates) {
    fillPointsFromDisplacementNonAligned(coordinates, print_circle, {1, 0}, 1);
    list_of_points.push_back(coordinates);
}


void FilledPattern::removePoints() {
    for (auto &position: list_of_points) {
        fillPointsFromDisplacementNonAligned(position, print_circle, {1, 0}, -1);
    }
    list_of_points.clear();
}


void FilledPattern::removeLine(Path path) {
    std::vector<coord> current_points_to_fill;
    coord_d current_coordinates = path.position(1);
    coord_d previous_coordinates = path.position(0);
    current_points_to_fill = path.findPointsToFill(1, isFillable(previous_coordinates));

    coord_d new_step = current_coordinates - previous_coordinates;
    fillPointsFromList(current_points_to_fill, new_step, -1);

    for (int i = 2; i < path.size(); i++) {
        current_coordinates = path.position(i);
        previous_coordinates = path.position(i - 1);
        coord_d second_previous_coordinates = path.position(i - 2);
        current_points_to_fill = path.findPointsToFill(i, isFillable(previous_coordinates));

        new_step = current_coordinates - previous_coordinates;
        fillPointsFromList(current_points_to_fill, new_step, -1);
    }
    fillPointsInHalfCircle(path, -1, true);
    fillPointsInHalfCircle(path, -1, false);
}


void FilledPattern::removeShortLines(double length_coefficient) {
    double minimal_length = length_coefficient * getPrintRadius();
    std::vector<Path> new_sequence_of_paths;

    for (auto &path: sequence_of_paths) {
        if (path.getLength() < minimal_length) {
            removeLine(path);
        } else {
            new_sequence_of_paths.push_back(path);
        }
    }
    sequence_of_paths = new_sequence_of_paths;
}


void
FilledPattern::fillPointsInHalfCircle(const Path &path, int value, bool is_front) {
    coord_d last_coordinate = path.last();
    coord_d previous_coordinate = path.secondToLast();
    coord_d corner_first = path.getNegativePathEdge().back();
    coord_d corner_second = path.getPositivePathEdge().back();
    double distance = 0;
    if (is_front) {
        last_coordinate = path.first();
        int i = 1;
        while (distance < 1 && i != path.size()) {
            previous_coordinate = path.getPositionSequence()[i++];
            distance = norm(last_coordinate - previous_coordinate);
        }
        corner_first = path.getNegativePathEdge().front();
        corner_second = path.getPositivePathEdge().front();
    } else {
        unsigned int i = path.size() - 1;
        while (distance < 1 && i != 0) {
            previous_coordinate = path.getPositionSequence()[--i];
            distance = norm(last_coordinate - previous_coordinate);
        }
    }

    coord_d last_move_direction = last_coordinate - previous_coordinate;

    std::vector<coord> half_circle_points = findHalfCircleEdges(last_coordinate, corner_first, corner_second,
                                                                getPrintRadius(),
                                                                isFilled(coord(last_coordinate)),
                                                                last_move_direction);
    fillPointsFromList(half_circle_points, last_coordinate - previous_coordinate, value);
}


void FilledPattern::exportFilledMatrix(const fs::path &path) const {
    fs::path filled_filename = path;
    exportVectorTableToFile(number_of_times_filled, filled_filename);
}


std::vector<Path> FilledPattern::getSequenceOfPaths() const {
    return sequence_of_paths;
}


void FilledPattern::addNewPath(Path &new_path) {
    sequence_of_paths.push_back(new_path);
}

coord_d normalizedDualVector(const coord_d &vector) {
    return normalized(perpendicular(vector));
}


void
FilledPattern::updateDualLineInDirection(coord_set &line_elements, coord_vector &line, coord_d previous_dual_director) {
    coord current_coord = line.back();
    coord_d position = to_coord_d(current_coord);

    while (
            !isTerminable(current_coord) &&
            (line_elements.find(current_coord) == line_elements.end() ||
             (line.back() == current_coord))
            ) {
        line_elements.insert(current_coord);
        // Avoid doubling the entries.
        if (line.empty() || current_coord != line.back()) {
            line.emplace_back(current_coord);
        }
        coord_d dual_director = normalizedDualVector(getDirector(position));
        if (dot(dual_director, previous_dual_director) < 0) {
            dual_director = -1 * dual_director;
        }
        previous_dual_director = dual_director;
        position = position + dual_director;
        current_coord = coord(position);
    }
}

std::vector<coord> FilledPattern::findDualLine(const coord &start) {
    coord_set line_elements = {start};
    coord_vector line_sequence = {start};
    coord_d starting_dual_director = normalizedDualVector(getDirector(start));

    updateDualLineInDirection(line_elements, line_sequence, starting_dual_director);
    std::reverse(line_sequence.begin(), line_sequence.end());
    updateDualLineInDirection(line_elements, line_sequence, -1 * starting_dual_director);

    std::vector<coord> line_vector;
    line_vector.reserve(line_sequence.size());
    for (auto &coordinate: line_sequence) {
        line_vector.emplace_back(coordinate);
    }

    return line_vector;
}


matrix_d FilledPattern::getDualTensor(const coord &coordinates) const {
    coord_d dual_director = normalizedDualVector(getDirector(coordinates));
    return tensor(dual_director, dual_director);
}

double FilledPattern::distance(const coord &first_point, const coord &second_point) {
    coord_d connecting_vector = to_coord_d(second_point - first_point);
    matrix_d first_dual_tensor = getDualTensor(first_point);
    matrix_d second_dual_tensor = getDualTensor(second_point);
    double first_distance = sqrt(dot(connecting_vector, matrix_multiply(first_dual_tensor, connecting_vector)));
    double second_distance = sqrt(dot(connecting_vector, matrix_multiply(second_dual_tensor, connecting_vector)));

    double dist = 2 * first_distance * second_distance / (first_distance + second_distance);
    return std::max({first_distance, second_distance, dist});
}


void FilledPattern::tryAddingPointToSpacedLine(const coord &current_position, coord &previous_position,
                                               bool &is_filled_coordinate_encountered, double separation,
                                               std::vector<SeedPoint> &separated_starting_points, int line_index,
                                               int point_index) {
    double current_distance = distance(current_position, previous_position);
    if (!isFillable(current_position)) {
        is_filled_coordinate_encountered = true;
        previous_position = current_position;
    } else if (!is_filled_coordinate_encountered && current_distance >= separation ||
               is_filled_coordinate_encountered && current_distance >= separation / 2) {
        separated_starting_points.emplace_back(current_position, getDirector(current_position), line_index,
                                               point_index);
        previous_position = current_position;
        is_filled_coordinate_encountered = false;
    }
}


std::vector<SeedPoint>
FilledPattern::getSpacedLine(const std::vector<coord> &line, int line_index, int starting_index) {
    double separation = getSeedSpacing();
    std::vector<SeedPoint> separated_starting_points = {{line[starting_index], getDirector(line[starting_index]),
                                                         line_index, starting_index}};
    bool is_looped = isLooped(line);
    bool is_filled_coordinate_encountered = false;

    coord previous_position = line[starting_index];
    for (int i = starting_index + 1; i < line.size(); i++) {
        tryAddingPointToSpacedLine(line[i], previous_position, is_filled_coordinate_encountered, separation,
                                   separated_starting_points, line_index, i);
    }

    if (is_looped) {
        for (int i = 0; i < starting_index; i++) {
            tryAddingPointToSpacedLine(line[i], previous_position, is_filled_coordinate_encountered, separation,
                                       separated_starting_points, line_index, i);
        }
    } else {
        std::reverse(separated_starting_points.begin(), separated_starting_points.end());

        is_filled_coordinate_encountered = false;
        previous_position = line[starting_index];
        for (int i = starting_index - 1; i >= 0; i--) {
            tryAddingPointToSpacedLine(line[i], previous_position, is_filled_coordinate_encountered, separation,
                                       separated_starting_points, line_index, i);
        }
    }
    return separated_starting_points;
}

/// Tests if the coordinate is within the pattern and unfilled.
bool FilledPattern::isFillable(const coord &coordinate) const {
    return desired_pattern.get().isInShape(coordinate) && number_of_times_filled[coordinate.x][coordinate.y] == 0;
}

/// Tests if the coordinate is within the pattern and unfilled.
bool FilledPattern::isFillable(const coord_d &coordinate) const {
    return isFillable(coord(coordinate));
}

bool FilledPattern::isFilled(const coord &coordinate) const {
    return isInRange(coordinate) && number_of_times_filled[coordinate.x][coordinate.y];
}


bool FilledPattern::isTerminable(const coord &point) const {
    if (!isFillable(point)) { return true; }

    for (const coord &displacement: collision_list) {
        if (isFilled(point + displacement)) {
            return true;
        }
    }
    return false;
}

bool FilledPattern::isTerminable(const coord_d &coordinate, const coord_d &direction) {
    if (!isFillable(coordinate)) { return true; }
    if (getTerminationRadius() <= 0) { return !isFillable(coordinate); }

    coord_d tangent = normalized(direction) * getTerminationRadius();
    coord_d normal = perpendicular(tangent);
    auto point = coord(coordinate);
    for (auto &displacement: collision_list) {
        if (isFilled(point + displacement) && isLeftOfEdge(displacement, normal, -1 * normal, true)) {
            return true;
        }
    }
    return false;
}


coord FilledPattern::getFillablePoint() {
    while (!binned_root_points.empty() && binned_root_points.back().empty()) {
        binned_root_points.pop_back();
    }
    if (binned_root_points.empty()) {
        return INVALID_POSITION;
    }
    coord last_coord = binned_root_points.back().back();
    binned_root_points.back().pop_back();
    return last_coord;
}


bool FilledPattern::isFillablePointLeft() const {
    return !binned_root_points.empty();
}


SeedPoint FilledPattern::findSeedPoint() {
    if (seed_points.empty()) {
        updateSeedPoints();
    }
    SeedPoint last_element = seed_points.back();
    seed_points.pop_back();

    return last_element;
}

std::vector<coord> FilledPattern::getSeedCoordinates() {
    std::vector<coord> seed_coordinates;
    seed_coordinates.reserve(sequence_of_paths.size());
    for (auto &path: sequence_of_paths) {
        coord coordinates = path.getSeedPoint().getCoordinates();
        seed_coordinates.emplace_back(coordinates);
    }
    return seed_coordinates;
}

void FilledPattern::setIsReseedingEnabled(bool is_reseeding_enabled) {
    FilledPattern::is_reseeding_enabled = is_reseeding_enabled;
}

#pragma clang diagnostic pop