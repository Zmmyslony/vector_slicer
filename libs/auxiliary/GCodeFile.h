//
// Created by Michał Zmyślony on 04/01/2022.
//

#ifndef VECTOR_SLICER_GCODEFILE_H
#define VECTOR_SLICER_GCODEFILE_H

#include <sstream>
#include <iomanip>
#include <string>
#include <valarray>
#include "../pattern/Path.h"

class GCodeFile {
    std::ostringstream bodyStream;
    const int moveSpeed;
    const int printSpeed;
    const double extrusionCoefficient;

    std::valarray<double> positions = {0, 0, 0};
    double extrusionValue = 0;

    void generalCommand(int number, const std::string &suffix);

    void setRelativePositioning();

    void setAbsolutePositioning();

    void autoHome();

    void levelBed();

    void setTemperatureHotend(int temperature);

    void setTemperatureHotendGradual(int temperature);

    void setTemperatureBed(int temperature);

    void turnMotorsOff();

    void resetPositionOfFilament();

    void generalCommand(const std::vector<char> &commands, const std::vector<bool> &isInt,
                        const std::vector<double> &values);

    void generalCommand(const char &command, double value);

    void generalCommand(const char &command, int value);

public:
    GCodeFile(int moveSpeed, int printSpeed, double extrusionCoefficient);

    explicit GCodeFile();

    void init(int hotendTemperature, int bedTemperature, double cleanLength);

    void init(int hotendTemperature);

    void shutDown();

    void movePlanar(const std::valarray<double> &xy);

    void moveVertical(double z);

    void moveVerticalRelative(double deltaZ);

    void move(double x, double y, double z);

    void extrude(const std::valarray<double> &xy);



    void printPath(std::vector<std::valarray<int>> path, const std::valarray<double> &positionOffset, double gridDistance);

    std::string getText();

    void printPattern(const std::vector<std::vector<std::valarray<int>>> &sortedSequenceOfPaths, const std::valarray<double> &positionOffset,
                      double gridDistance);

    void exportToFile(const std::string &path);

    void addComment(const std::string &comment);

    void setCurrentCoordinatesToZero();

    friend class Hyrel;
};

void generateGCode(const std::string &baseDirectory, int temperature, double cleaningDistance,
                   const std::valarray<double> &positionOffset, double gridDistance);


#endif //VECTOR_SLICER_GCODEFILE_H
