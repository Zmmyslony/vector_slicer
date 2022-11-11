//
// Created by Michał Zmyślony on 04/08/2022.
//

#ifndef VECTOR_SLICER_HYREL_H
#define VECTOR_SLICER_HYREL_H

#include "GCodeFile.h"

class Hyrel : public GCodeFile {
    void defineHeightOffset(unsigned int toolNumber, double height);

    void defineToolOffset(unsigned int toolNumber, const std::vector<double> &xyz, unsigned int offsetRegister);

    void extrude(const std::valarray<double> &xy);

    void selectTool(unsigned int toolNumber);

    void setUnitsToMillimetres();

    void configureFlow(double nozzleWidth, double layerHeight, double flowMultiplier, int pulses, int tool);

    void signalFinishedPrint();

    void clearOffsets();

    void turnMotorsOff();

    void clean(double cleanLength, int numberOfLines, double nozzleWidth);

public:
    void init(int hotendTemperature, int bedTemperature, double cleanLength, double nozzleWidth, double layerHeight,
              int toolNumber, double zOffset, double xOffset, double yOffset);

    void shutDown();

    Hyrel(int moveSpeed, int printSpeed, double extrusionCoefficient);

    void
    printPath(const std::vector<std::valarray<int>>& path, const std::valarray<double> &positionOffset,
              double gridDistance);

    void printPattern(const std::vector<std::vector<std::valarray<int>>> &sortedSequenceOfPaths,
                      const std::valarray<double> &positionOffset, double gridDistance);

    void exportToFile(const std::string &path);
};

void testHeaderAndFooter();

#endif //VECTOR_SLICER_HYREL_H
