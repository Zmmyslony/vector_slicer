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
// You should have received a copy of the GNU General Public License along with Foobar. If not, see <https://www.gnu.org/licenses/>.

//
// Created by Michał Zmyślony on 04/11/2021.
//

#include "ProgressBar.h"
#include <iostream>

void showProgress(double progress) {
    int bar_width = 20;
    int pos = (int)(bar_width * progress);
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