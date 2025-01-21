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
// Created by Michał Zmyślony on 29/09/2021.
//

#ifndef VECTOR_SLICER_QUANTIFIED_CONFIG_H
#define VECTOR_SLICER_QUANTIFIED_CONFIG_H

#include "filled_pattern.h"
#include "simulation/simulation.h"
#include "coord.h"

#include <boost/numeric/ublas/vector.hpp>
#include <cfloat>

#define DISAGREEMENT_BUCKET_COUNT 90

typedef boost::numeric::ublas::vector<double> vectord;

/// Class quantifying how given FillingConfig works for a number of seeds
class QuantifiedConfig : FilledPattern, public Simulation {
    double empty_spots = 0;
    double average_overlap = 0;
    double average_director_disagreement = 0;
    double paths_number = 0;
    double disagreement = DBL_MAX;
    double total_disagreement = DBL_MAX;
    double path_multiplier = 1;
    double disagreement_norm = 0;
    double empty_spot_disagreement = DBL_MAX;
    double overlap_disagreement = DBL_MAX;
    double director_disagreement = DBL_MAX;
    std::vector<unsigned int> director_disagreement_distribution = std::vector<unsigned int>(DISAGREEMENT_BUCKET_COUNT,
                                                                                             0);
    double bucket_size = M_PI_2 / (DISAGREEMENT_BUCKET_COUNT - 1);
    double total_angular_director_disagreement = 0;
    double average_angular_director_disagreement = DBL_MAX;

    double calculateEmptySpots();

    double calculateAverageOverlap();

    double calculateDirectorDisagreement();

    double localDirectorAgreement(int i, int j);

    void insertIntoBucket(double local_director_agreement);

    double averagedFillDensity(const veci &position, int averaging_radius) const;

public:

    QuantifiedConfig(const FilledPattern &pattern, const Simulation &simulation);

    QuantifiedConfig(const DesiredPattern &desired_pattern, FillingConfig &filling_config,
                     const Simulation &simulation);

    /// Function allowing BayesianOptimisation to create new FillingConfig using vectord input
    QuantifiedConfig(QuantifiedConfig &template_config, vectord parameters);

    /// Creating copies of QuantifiedConfig with differing seeds
    QuantifiedConfig(QuantifiedConfig &template_config, int seed);

    [[nodiscard]] FilledPattern getFilledPattern() const;

    DesiredPattern getDesiredPattern();

    [[nodiscard]] FillingConfig getConfig() const;

    [[nodiscard]] double getDisagreement() const;

    const std::vector<unsigned int> &getDirectorDisagreementDistribution() const;

    /// Fills the pattern
    void evaluate();

    /// Returns percentile based disagreement for a number of seeds
    double getDisagreement(int seeds, int threads, bool is_disagreement_details_printed,
                           double disagreement_percentile);

    /// Evaluates number of seeds and sorts them according to their disagreement
    std::vector<QuantifiedConfig> findBestSeeds(int seeds, int threads);

    void printDisagreement() const;

    std::vector<std::vector<double>> localDisagreementGrid();

    std::vector<double> sampleFillDensities(uint16_t sample_count, int averaging_radius) const;
};


#endif //VECTOR_SLICER_QUANTIFIED_CONFIG_H
