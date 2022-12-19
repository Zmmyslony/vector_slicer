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
// Created by Michał Zmyślony on 07/01/2022.
//

#include "IndexedPath.h"
#include "../auxiliary/ValarrayOperations.h"
#include <utility>
#include <cfloat>

IndexedPath::IndexedPath(int index, bool is_path_reversed, std::valarray<int> start_coordinates,
                         std::valarray<int> end_coordinates) : index(
        index), reversed_path(is_path_reversed), start_coordinates(std::move(start_coordinates)),
                                                               end_coordinates(std::move(end_coordinates)) {}

int IndexedPath::getIndex() const {
    return index;
}

bool IndexedPath::isPathReversed() const {
    return reversed_path;
}

const std::valarray<int> &IndexedPath::getStartCoordinates() const {
    return start_coordinates;
}

const std::valarray<int> &IndexedPath::getEndCoordinates() const {
    return end_coordinates;
}

IndexedPath::IndexedPath() : index(0), reversed_path(false), start_coordinates(std::valarray<int>({0, 0})),
                             end_coordinates(std::valarray<int>({0, 0})) {}

IndexedPath::IndexedPath(int index, bool is_path_reversed, const std::vector<std::valarray<int>> &path,
                         const std::valarray<int> &dimensions) :
        index(index),
        reversed_path(is_path_reversed) {
    if (is_path_reversed) {
        start_coordinates = path.back();
        end_coordinates = path.front();
    } else {
        start_coordinates = path.front();
        end_coordinates = path.back();
    }
    double max_distance = DBL_MIN;
    for (auto &point: path) {
        double x_vector = point[0] - (double) dimensions[0] / 2;
        double y_vector = point[1] - (double) dimensions[1] / 2;
        double distance = norm(point - dimensions / 2);
        double current_angle = atan2(y_vector, x_vector);
        if (x_max < point[0]) { x_max = point[0]; }
        if (x_min > point[0]) { x_min = point[0]; }
        if (y_max < point[1]) { y_max = point[1]; }
        if (y_min > point[1]) { y_min = point[1]; }
        if (max_distance < distance) {
            angle = current_angle;
            max_distance = distance;
        }
    }
}

int IndexedPath::getXMin() const {
    return x_min;
}

int IndexedPath::getXMax() const {
    return x_max;
}

int IndexedPath::getYMin() const {
    return y_min;
}

int IndexedPath::getYMax() const {
    return y_max;
}

double IndexedPath::getAngle() const {
    return angle;
}


std::vector<IndexedPath> indexPaths(const std::vector<Path> &sequence_of_paths, const std::valarray<int> &dimensions) {
    std::vector<IndexedPath> indexed_paths;
    for (int i = 0; i < sequence_of_paths.size(); i++) {
        Path current_path = sequence_of_paths[i];
//        indexed_paths.emplace_back(i, false, current_path.first(), current_path.last());
//        indexed_paths.emplace_back(i, true, current_path.last(), current_path.first());
        indexed_paths.emplace_back(i, false, current_path.sequence_of_positions, dimensions);
        indexed_paths.emplace_back(i, true, current_path.sequence_of_positions, dimensions);
    }
    return indexed_paths;
}


void removePathsWithIndex(std::vector<IndexedPath> &indexed_paths, int index) {
    std::vector<int> indices_to_remove;
    int i = 0;
    for (auto &element: indexed_paths) {
        if (element.getIndex() == index) {
            indices_to_remove.push_back(i);
        }
        i++;
    }
    while (!indices_to_remove.empty()) {
        int index_to_remove = indices_to_remove.back();
        indices_to_remove.pop_back();
        indexed_paths.erase(indexed_paths.begin() + index_to_remove);
    }
}


void removePathsWithSameIndex(std::vector<IndexedPath> &indexed_paths, const IndexedPath &path) {
    removePathsWithIndex(indexed_paths, path.getIndex());
}


double distanceBetweenPaths(const IndexedPath &current_path, const IndexedPath &new_path) {
    std::valarray<int> connecting_vector = current_path.getEndCoordinates() - new_path.getStartCoordinates();
    return norm(connecting_vector);
}


IndexedPath findClosestNeighbour(const std::vector<IndexedPath> &indexed_paths, const IndexedPath &current_path) {
    double minimal_distance = DBL_MAX;
    IndexedPath closest_path = current_path;
    for (auto &element: indexed_paths) {
        double current_distance = distanceBetweenPaths(current_path, element);
        if (current_distance < minimal_distance) {
            minimal_distance = current_distance;
            closest_path = element;
        }
    }
    return closest_path;
}

IndexedPath findClosestNeighbourLeft(const std::vector<IndexedPath> &indexed_paths, const IndexedPath &current_path) {
    double minimal_distance = DBL_MAX;
    double left_most_position = DBL_MAX;
    IndexedPath closest_path = current_path;

    for (auto &element: indexed_paths) {
        double current_pos = element.getXMin();
        if (current_pos < left_most_position) {
            left_most_position = current_pos;
            minimal_distance = distanceBetweenPaths(current_path, element);
            closest_path = element;

        } else if (current_pos == left_most_position) {
            double current_distance = distanceBetweenPaths(current_path, element);
            if (current_distance <= minimal_distance) {
                minimal_distance = current_distance;
                closest_path = element;
            }
        }
    }
    return closest_path;
}

IndexedPath findClosestNeighbourRadial(const std::vector<IndexedPath> &indexed_paths, const IndexedPath &current_path) {
    double minimal_distance = DBL_MAX;
    double minimal_angle = DBL_MAX;
    IndexedPath closest_path = current_path;

    for (auto &element: indexed_paths) {
        double current_angle = element.getAngle();
        if (current_angle < minimal_angle) {
            minimal_angle = current_angle;
            minimal_distance = distanceBetweenPaths(current_path, element);
            closest_path = element;

        } else if (current_angle == minimal_angle) {
            double current_distance = distanceBetweenPaths(current_path, element);
            if (current_distance <= minimal_distance) {
                minimal_distance = current_distance;
                closest_path = element;
            }
        }
    }
    return closest_path;
}


std::vector<IndexedPath>
sortIndexedPaths(std::vector<IndexedPath> indexed_paths, const std::valarray<int> &starting_positions) {
    IndexedPath current_path(0, false, starting_positions, starting_positions);
    std::vector<IndexedPath> sorted_paths;
    while (!indexed_paths.empty()) {
//        current_path = findClosestNeighbour(indexed_paths, current_path);
//        current_path = findClosestNeighbourLeft(indexed_paths, current_path);
        current_path = findClosestNeighbourRadial(indexed_paths, current_path);
        removePathsWithSameIndex(indexed_paths, current_path);
        sorted_paths.emplace_back(current_path);
    }
    return sorted_paths;
}


double getMoveDistance(const std::vector<IndexedPath> &sorted_paths) {
    double distance = 0;
    for (int i = 1; i < sorted_paths.size(); i++) {
        distance += distanceBetweenPaths(sorted_paths[i - 1], sorted_paths[i]);
    }
    return distance;
}


std::vector<IndexedPath>
findBestSortingOfPathsFromStartingPoints(const std::vector<IndexedPath> &unsorted_indexed_paths,
                                         const std::vector<std::valarray<int>> &list_of_starting_positions) {
    double minimal_distance = DBL_MAX;
    std::vector<IndexedPath> best_sorting;
    for (auto &starting_position: list_of_starting_positions) {
        std::vector<IndexedPath> current_sorting = sortIndexedPaths(unsorted_indexed_paths, starting_position);
        if (getMoveDistance(current_sorting) < minimal_distance) {
            minimal_distance = getMoveDistance(current_sorting);
            best_sorting = current_sorting;
        }

    }
    return best_sorting;
}


std::vector<std::valarray<int>> generateStartingPoints(const std::valarray<int> &dimensions, int number_of_steps) {
    std::vector<std::valarray<int>> starting_points = {{0, 0}, {dimensions[0], 0}, dimensions, {0, dimensions[1]}};

    double x_step = (double) dimensions[0] / number_of_steps;
    double y_step = (double) dimensions[1] / number_of_steps;
    for (int step = 0; step < number_of_steps; step++) {
        starting_points.push_back({(int) (x_step * step), 0});
        starting_points.push_back({dimensions[0], (int) (y_step * step)});
        starting_points.push_back({dimensions[0] - (int) (x_step * step), dimensions[1]});
        starting_points.push_back({0, dimensions[1] - (int) (y_step * step)});
    }
    return starting_points;
}


std::vector<std::vector<std::valarray<int>>>
sortedSequenceOfPaths(const std::vector<Path> &paths, const std::vector<IndexedPath> &sorted_indexed_paths) {
    std::vector<std::vector<std::valarray<int>>> sorted_sequence;
    for (auto &indexed_path: sorted_indexed_paths) {
        std::vector<std::valarray<int>> new_path = (paths[indexed_path.getIndex()]).sequence_of_positions;

        if (indexed_path.isPathReversed()) {
            std::reverse(new_path.begin(), new_path.end());
        }
        sorted_sequence.emplace_back(new_path);
    }
    return sorted_sequence;
}


std::vector<IndexedPath> indexPaths(FilledPattern filled_pattern, const std::valarray<int> &dimensions) {
    std::vector<Path> sequence_of_paths = filled_pattern.getSequenceOfPaths();
    return indexPaths(sequence_of_paths, dimensions);
}


std::vector<std::vector<std::valarray<int>>> pathToVector(const std::vector<Path> &path) {
    std::vector<std::vector<std::valarray<int>>> resulting_vector;
    resulting_vector.reserve(path.size());
    for (auto &element: path) {
        resulting_vector.push_back(element.sequence_of_positions);
    }
    return resulting_vector;
}


std::vector<std::vector<std::valarray<int>>> getSortedPaths(FilledPattern &filled_pattern, int starting_point_number) {
    std::valarray<int> dimensions = filled_pattern.desired_pattern.get().getDimensions();
    std::vector<IndexedPath> unsorted_indices = indexPaths(filled_pattern, dimensions);

    std::vector<std::valarray<int>> starting_points = generateStartingPoints(dimensions, starting_point_number);
    std::vector<IndexedPath> sorted_indices = findBestSortingOfPathsFromStartingPoints(unsorted_indices,
                                                                                       starting_points);

    std::vector<Path> unsorted_paths = filled_pattern.getSequenceOfPaths();
    std::vector<std::vector<std::valarray<int>>> sorted_paths = sortedSequenceOfPaths(unsorted_paths, sorted_indices);

    return sorted_paths;
}