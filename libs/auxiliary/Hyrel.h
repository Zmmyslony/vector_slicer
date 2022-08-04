//
// Created by Michał Zmyślony on 04/08/2022.
//

#ifndef VECTOR_SLICER_HYREL_H
#define VECTOR_SLICER_HYREL_H

#include "GCodeFile.h"

class Hyrel : public GCodeFile {
    void defineHeightOffset(unsigned int toolNumber, double height);

    void defineToolOffset(unsigned int toolNumber, const std::vector<double> &xy, unsigned int offsetRegister);

    void
    initHyrel(int hotendTemperature, int bedTemperature, double cleanLength, double nozzleWidth, double layerHeight,
              int toolNumber, double zOffset, double xOffset, double yOffset);

    void extrudeHyrel(const std::valarray<double> &xy);

    void selectTool(unsigned int toolNumber);

};


#endif //VECTOR_SLICER_HYREL_H
