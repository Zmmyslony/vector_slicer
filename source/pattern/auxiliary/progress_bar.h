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

#ifndef VECTOR_SLICER_PROGRESS_BAR_H
#define VECTOR_SLICER_PROGRESS_BAR_H

#include <chrono>
#include "../quantified_config.h"

void showProgress(double progress);

void showProgress(int current_step, int max_step);

void
showProgressBase(int current_step, int max_step, int steps_from_improvement, int steps_threshold,
                 std::chrono::steady_clock::time_point begin, std::chrono::steady_clock::time_point now,
                 const std::string &suffix);

#endif //VECTOR_SLICER_PROGRESS_BAR_H
