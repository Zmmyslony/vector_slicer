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
showProgressBase(int current_step, int max_step, int steps_from_improvement, int steps_threshold,
                 std::chrono::steady_clock::time_point begin, std::chrono::steady_clock::time_point now,
                 const std::string &suffix) {
    double progress;
    long current_time = std::chrono::duration_cast<std::chrono::seconds>(now - begin).count();
    double time_per_step = (double) current_time / (double) current_step;
    long estimated_completion_time;
    if (max_step > 0) {
        progress =  (double) current_step / (double) max_step;
        estimated_completion_time = (long) (time_per_step * (double) max_step);
    } else {
        progress = (double) current_step / (double)(current_step + steps_threshold - steps_from_improvement);
        estimated_completion_time =
                current_time + (long) (time_per_step * (double) (steps_threshold - steps_from_improvement));
    }

    int bar_width = 20;
    int pos = (int) (bar_width * progress);
    std::cout << "\r " << current_time << "/" << estimated_completion_time << " s: \t[";
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