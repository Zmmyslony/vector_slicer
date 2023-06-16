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

#ifndef VECTOR_SLICER_PROGRESS_BAR_H
#define VECTOR_SLICER_PROGRESS_BAR_H

#include <chrono>
#include "../quantified_config.h"

void showProgress(double progress);

void showProgress(int current_step, int max_step);

void showProgress(int current_step, int max_step, std::chrono::steady_clock::time_point begin, double min_value,
                  const vectord &best_configuration, int steps_from_improvement, int steps_threshold, int dims);

#endif //VECTOR_SLICER_PROGRESS_BAR_H
