//
// Created by zmmys on 05/11/2021.
//

#ifndef VECTOR_SLICER_OPTIMIZEPATHGENERATION_H
#define VECTOR_SLICER_OPTIMIZEPATHGENERATION_H

#include "../auxiliary/FillingConfig.h"
#include <string>

FillingConfig findBestConfig(const std::string &path, int threads, int iterationDepth, int seed);

FillingConfig findBestConfig(const std::string &path, int threads, int iterationDepth);

#endif //VECTOR_SLICER_OPTIMIZEPATHGENERATION_H
