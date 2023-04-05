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
// Created by Michał Zmyślony on 21/11/2022.
//

#include "disagreement_weights.h"
#include "auxiliary/configuration_reading.h"

DisagreementWeights::DisagreementWeights(double empty_spot_weight, double empty_spot_exponent, double overlap_weight,
                                         double overlap_exponent, double director_weight, double director_exponent,
                                         double path_weight, double path_exponent) : empty_spot_weight(
        empty_spot_weight), empty_spot_exponent(empty_spot_exponent), overlap_weight(overlap_weight),
                                                                                     overlap_exponent(overlap_exponent),
                                                                                     director_weight(director_weight),
                                                                                     director_exponent(
                                                                                             director_exponent),
                                                                                     path_weight(path_weight),
                                                                                     path_exponent(path_exponent) {}

DisagreementWeights::DisagreementWeights(const fs::path &path) :
        DisagreementWeights(readKeyDouble(path, "empty_spot_weight"),
                            readKeyDouble(path, "empty_spot_power"),
                            readKeyDouble(path, "overlap_weight"),
                            readKeyDouble(path, "overlap_power"),
                            readKeyDouble(path, "director_weight"),
                            readKeyDouble(path, "director_power"),
                            0, 1) {}


