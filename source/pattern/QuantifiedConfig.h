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
// Created by Michał Zmyślony on 29/09/2021.
//

#ifndef VECTOR_SLICER_QUANTIFIEDCONFIG_H
#define VECTOR_SLICER_QUANTIFIEDCONFIG_H

#include "FilledPattern.h"
#include "disagreement_weights.h"

#include <boost/numeric/ublas/vector.hpp>
#include <cfloat>

typedef boost::numeric::ublas::vector<double> vectord;

class QuantifiedConfig : FilledPattern, DisagreementWeights {
    double empty_spots = 0;
    double average_overlap = 0;
    double director_disagreement = 0;
    double paths_number = 0;
    double disagreement = DBL_MAX;
    double total_disagreement = DBL_MAX;

    double calculateEmptySpots();

    double calculateAverageOverlap();

    double calculateDirectorDisagreement();

    double calculatePathLengthDeviation(int order);

public:
    QuantifiedConfig(const FilledPattern &pattern, const DisagreementWeights &disagreement_weights);

    QuantifiedConfig(const DesiredPattern &desired_pattern, FillingConfig &filling_config,
                     DisagreementWeights disagreement_weights);

    QuantifiedConfig(QuantifiedConfig &template_config, vectord parameters, int dims);

    QuantifiedConfig(QuantifiedConfig &template_config, int seed);

    void evaluate();

    [[nodiscard]] double getDisagreement() const;

    double getDisagreement(int seeds, int threads, bool is_disagreement_details_printed);

    FilledPattern getFilledPattern() const;

    DesiredPattern getDesiredPattern();

    [[nodiscard]] FillingConfig getConfig() const;

    std::vector<QuantifiedConfig> findBestSeeds(int seeds, int threads);

    void printDisagreement() const;
};


#endif //VECTOR_SLICER_QUANTIFIEDCONFIG_H
