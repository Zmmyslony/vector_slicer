// Copyright (c) 2024, Michał Zmyślony, mlz22@cam.ac.uk.
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
// Created by Michał Zmyślony on 12/03/2024.
//

#include "seed_line.h"


int countLines(const std::vector<Path> &paths) {
    int lines_count = 0;
    for (auto &path: paths) {
        int current_line_index = path.getSeedPoint().getSeedLine();
        if (current_line_index > lines_count) {
            lines_count = current_line_index;
        }
    }
    return lines_count + 1;
}


std::vector<std::vector<Path>> groupBySeedLine(const std::vector<Path> &paths) {
    int lines_count = countLines(paths);
    auto lines = std::vector<std::vector<Path >>(lines_count);
    for (auto &path: paths) {
        int i = path.getSeedPoint().getSeedLine();
        lines[i].emplace_back(path);
    }
    return lines;
}


void removeEmptyLines(std::vector<std::vector<Path>> &seed_line) {
    auto i = seed_line.begin();
    while (i != seed_line.end()) {
        if (i->empty()) {
            i = seed_line.erase(i);
        } else {
            i++;
        }
    }
}


std::vector<SeedLine> groupIntoSeedLines(const FilledPattern &pattern) {
    std::vector<Path> unsorted_paths = pattern.getSequenceOfPaths();
    std::vector<std::vector<Path>> raw_seed_lines = groupBySeedLine(unsorted_paths);
    removeEmptyLines(raw_seed_lines);

    double distance_threshold = pattern.getSeedSpacing() * 2;
    std::vector<SeedLine> seed_lines;
    seed_lines.reserve(raw_seed_lines.size());
    for (auto &raw_seed_line: raw_seed_lines) {
        seed_lines.emplace_back(raw_seed_line, distance_threshold);
    }
    return seed_lines;
}


void sort_by_seed_index(std::vector<Path> &seed_line) {
    std::sort(seed_line.begin(), seed_line.end(),
              [](Path &path_one, Path &path_two) {
                  return path_one.getSeedPoint().getIndex() < path_two.getSeedPoint().getIndex();
              });
}


bool is_line_closed(const std::vector<Path> &seed_line, double spacing) {
    SeedPoint first_seed = seed_line.front().getSeedPoint();
    SeedPoint last_seed = seed_line.back().getSeedPoint();
    double seed_distance = norm(first_seed.getCoordinates() - last_seed.getCoordinates());
    return seed_distance <= spacing;
}


SeedLine::SeedLine(const std::vector<Path> &unsorted_seed_pairs, double distance_threshold) {
    paths = unsorted_seed_pairs;
    sort_by_seed_index(paths);
    is_closed = is_line_closed(paths, distance_threshold);
}


double SeedLine::loopDistance(const vali &point, bool is_vector_sorted) {
    double closest_distance = paths.front().distance(point, is_vector_sorted);

    for (int i = 1; i < paths.size(); i++) {
        double distance = paths[i].distance(point, is_vector_sorted);
        if (distance < closest_distance) {
            closest_distance = distance;
            i_closest = i;
        }
    }
    return closest_distance;
}


double SeedLine::distance(const vali &point, bool is_vector_sorted) {
    if (is_closed) {
        return loopDistance(point, is_vector_sorted);
    }

    double front_distance = paths.front().distance(point, is_vector_sorted);
    double back_distance = paths.back().distance(point, is_vector_sorted);

    if (back_distance < front_distance) {
        is_reversed = true;
        return back_distance;
    } else {
        is_reversed = false;
        return front_distance;
    }
}


std::vector<Path> SeedLine::getOrderedPaths(bool is_vector_filled) {
    std::vector<Path> sorted_paths;
    sorted_paths.reserve(paths.size());
    if (is_closed && i_closest > 0) {
        sorted_paths.emplace_back(paths[i_closest]);
        for (int i = 1; i < paths.size(); i++) {
            int i_current = (i_closest + i) % paths.size();
            int i_previous = (i_closest + i - 1) % paths.size();
            vali end_point = paths[i_previous].endPoint();
            paths[i_current].distance(end_point, is_vector_filled);
            sorted_paths.emplace_back(paths[i_current]);
        }
        return sorted_paths;
    }

    if (is_reversed) {
        sorted_paths.emplace_back(paths.back());
        for (int i = paths.size() - 2; i >= 0; i--) {
            vali end_point = paths[i + 1].endPoint();
            paths[i].distance(end_point, is_vector_filled);
            sorted_paths.emplace_back(paths[i]);
        }
    } else {
        sorted_paths.emplace_back(paths.front());
        for (int i = 1; i < paths.size(); i++) {
            vali end_point = paths[i - 1].endPoint();
            paths[i].distance(end_point, is_vector_filled);
            sorted_paths.emplace_back(paths[i]);
        }
    }
    return sorted_paths;
}

vali SeedLine::endPoint() const {
    if (paths.size() == 1) {
        return paths.front().endPoint();
    }

    if (is_closed) {
        if (i_closest == 0) {
            return paths.back().endPoint();
        }
        return paths[i_closest - 1].endPoint();
    }

    if (is_reversed) {
        return paths.front().endPoint();
    } else {
        return paths.back().endPoint();
    }
}

SeedLine getClosestLine(std::vector<SeedLine> &seed_lines, vali &starting_point, bool is_vector_sorted) {
    SeedLine closest_line = seed_lines.front();
    int i_closest = 0;
    for (int i = 1; i < seed_lines.size(); i++) {
        if (seed_lines[i].distance(starting_point, is_vector_sorted) <
            closest_line.distance(starting_point, is_vector_sorted)) {
            closest_line = seed_lines[i];
            i_closest = i;
        }
    }
    seed_lines.erase(seed_lines.begin() + i_closest);
    return closest_line;
}


std::vector<Path> seedLineSort(const FilledPattern &pattern, vali &starting_point) {
    bool is_vector_sorted = pattern.desired_pattern.get().isVectorSorted();
    std::vector<SeedLine> seed_lines = groupIntoSeedLines(pattern);
    vali end_point = starting_point;
    std::vector<Path> sorted_paths;
    while (!seed_lines.empty()) {
        SeedLine closest_line = getClosestLine(seed_lines, end_point, is_vector_sorted);
        std::vector<Path> ordered_paths = closest_line.getOrderedPaths(is_vector_sorted);
        sorted_paths.insert(sorted_paths.end(), ordered_paths.begin(), ordered_paths.end());
        end_point = closest_line.endPoint();
    }
    return sorted_paths;
}

