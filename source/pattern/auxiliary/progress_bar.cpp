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
// Created by Michał Zmyślony on 04/11/2021.
//

#include "progress_bar.h"
#include <iostream>
#include <string>
#include <iomanip>

void showProgress(double progress) {
    int bar_width = 20;
    int pos = (int) (bar_width * progress);
    std::cout << "\r[";
    for (int i = 0; i < bar_width; ++i) {
        if (i <= pos) {
            std::cout << "=";
        } else {
            std::cout << " ";
        }
    }
    std::cout << "] " << int(progress * 100.0) << "%";
    std::cout.flush();
}

void showProgress(int current_step, int max_step) {
    showProgress((double) current_step / (double) max_step);
}

void
showProgress(double progress, std::chrono::steady_clock::time_point begin, std::chrono::steady_clock::time_point now,
             const std::string &suffix) {
    int bar_width = 20;
    int pos = (int) (bar_width * progress);
    std::cout << "\r " << std::chrono::duration_cast<std::chrono::seconds>(now - begin).count() << "/" <<
              std::chrono::duration_cast<std::chrono::seconds>((now - begin) / progress).count() << " s: \t[";
    for (int i = 0; i < bar_width; ++i) {
        if (i <= pos) {
            std::cout << "=";
        } else {
            std::cout << " ";
        }
    }
    std::cout << "] " << int(progress * 100.0) << "% " << suffix;
    std::cout.flush();
}

void
showProgress(int current_step, int max_step, std::chrono::steady_clock::time_point begin, double min_value,
             const vectord &best_configuration, int steps_from_improvement, int steps_threshold) {
    double repulsion = best_configuration[0];
    double collision_radius = best_configuration[1];
    double starting_point_separation = best_configuration[2];
    std::stringstream suffix_stream;
    suffix_stream << std::setprecision(6) << "Minimal disagreement:" << min_value;
    suffix_stream << std::setprecision(2) << ", at Rep " << repulsion << ", ColRad " << collision_radius
                  << ", StaSep " << starting_point_separation << ". Steps since improvement: " << steps_from_improvement
                  << "/" << steps_threshold;
    std::string suffix = suffix_stream.str();
    showProgress((double) current_step / (double) max_step, begin, std::chrono::steady_clock::now(), suffix);
}