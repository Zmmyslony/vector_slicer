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
// Created by Michał Zmyślony on 05/11/2021.
//

#ifndef VECTOR_SLICER_CONFIGDISAGREEMENT_H
#define VECTOR_SLICER_CONFIGDISAGREEMENT_H

#include "../auxiliary/FillingConfig.h"
#include "../pattern/FilledPattern.h"
#include "../pattern/DesiredPattern.h"
#include <string>

void findBestConfig(const std::string &directorPath, int minSeed, int maxSeed, int threads);

void recalculateBestConfig(const std::string &director_path);

class ConfigDisagreement {
    FillingConfig config;
    double disagreement;

public:
    const FillingConfig &getConfig() const;

    FilledPattern getPattern(const DesiredPattern &desiredPattern) const;

    void fillWithPatterns(const DesiredPattern &desiredPattern);

    double getDisagreement() const;

    explicit ConfigDisagreement(FillingConfig config);
};


#endif //VECTOR_SLICER_CONFIGDISAGREEMENT_H
