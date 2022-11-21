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
// Created by Michał Zmyślony on 21/11/2022.
//

#ifndef VECTOR_SLICER_BAYESIAN_OPTIMISATION_H
#define VECTOR_SLICER_BAYESIAN_OPTIMISATION_H

#include <bayesopt/bayesopt.hpp>

//class BayesianOptimisation : public bayesopt::ContinuousModel {
//
//public:
//    BayesianOptimisation(bayesopt::Parameters parameters) :
//            ContinuousModel(3, parameters) {}
//
//    double evaluateSample(const vectord &x_in) {
//        if (x_in.size() != 3) {
//            std::cout << "WARNING: This only works for 3D inputs." << std::endl
//                      << "WARNING: Using only first three components." << std::endl;
//        }
//
//    }
//
//    bool checkReachability(const vectord &query) { return true; };
//
//};


#endif //VECTOR_SLICER_BAYESIAN_OPTIMISATION_H
