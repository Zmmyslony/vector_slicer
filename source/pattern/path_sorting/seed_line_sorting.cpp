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
// Created by Michał Zmyślony on 11/03/2024.
//

#include "seed_line_sorting.h"

int countLines(const std::vector<Path> &paths) {
    int lines_count = 0;
    for (auto &path: paths) {
        int current_line_index = path.getSeedPoint().getSeedLine();
        if (current_line_index > lines_count) {
            lines_count = current_line_index;
        }
    }
    return lines_count;
}


std::vector<SeedLine> groupPathsIntoLines(const std::vector<Path> &paths) {
    int lines_count = countLines(paths);
    auto lines = std::vector<SeedLine>(lines_count);
    for (int i = 0; i < paths.size(); i++) {
        SeedPoint seed_point = paths[i].getSeedPoint();
        int current_line_index = seed_point.getSeedLine();
        SeedPair current_pair = {seed_point, i};
        lines[current_line_index].emplace_back(current_pair);
    }
    return lines;
}

void removeEmptyLines(std::vector<SeedLine> &seed_line) {
    auto i = seed_line.begin();
    while(i != seed_line.end()) {
        if (i -> empty()) {
            i = seed_line.erase(i);
        } else {
            i++;
        }
    }
}

void sort_seed_line(SeedLine &seed_line) {
    std::sort(seed_line.begin(), seed_line.end(),
              [](SeedPair &seed_pair_one, SeedPair &seed_pair_two) {
                  return seed_pair_one.first.getIndex() < seed_pair_two.first.getIndex();
              });
}

bool is_closed(const SeedLine &seed_line, double spacing) {
    SeedPoint first_seed = seed_line.front().first;
    SeedPoint last_seed = seed_line.back().first;
    double seed_distance = norm(first_seed.getCoordinates() - last_seed.getCoordinates());
    return seed_distance <= spacing;
}


SeedLineSorting::SeedLineSorting(const FilledPattern &pattern) {
    unsorted_paths = pattern.getSequenceOfPaths();
    seed_lines = groupPathsIntoLines(unsorted_paths);
    removeEmptyLines(seed_lines);
    for (auto &line : seed_lines) {
        sort_seed_line(line);
    }

    double seed_spacing = pattern.getSeedSpacing();
    is_seed_line_closed = std::vector<bool>(seed_lines.size());
    for (int i = 0; i < is_seed_line_closed.size(); i++) {
        is_seed_line_closed[i] = is_closed(seed_lines[i], seed_spacing * 2);
    }
}

std::vector<vali> SeedLineSorting::getSortedPaths() const {
    std::vector<vali> sorted_paths = std::vector<vali>(unsorted_paths.size());


    return sorted_paths;
}


