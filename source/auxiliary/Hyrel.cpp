//
// Created by Michał Zmyślony on 04/08/2022.
//

#include <iostream>
#include "Hyrel.h"
#include "ValarrayOperations.h"
#include "Exporting.h"


int toolNumberVariable(unsigned int toolNumber) {
    const unsigned int numberOfTools = 4;
    if (toolNumber > numberOfTools) {
        std::cout << "Gcode writing -> tool number: max value: "
                  << numberOfTools - 1 << ", used value: " << toolNumber << "\n";
        return -1;
    } else {
        return toolNumber;
    }
}


std::string toolNumberString(unsigned int toolNumber) {
    const unsigned int numberOfTools = 4;
    if (toolNumber > numberOfTools) {
        std::cout << "Gcode writing -> tool number: max value: "
                  << numberOfTools - 1 << ", used value: " << toolNumber << "\n";
        return "";
    } else {
        return "T" + std::to_string(toolNumberVariable(toolNumber));
    }
}

void Hyrel::selectTool(unsigned int toolNumber) {
    const unsigned int numberOfTools = 4;
    if (toolNumber < numberOfTools) {
        addComment("Selecting tool");
        generalCommand('T', (int) toolNumber);
    }
}

void Hyrel::defineHeightOffset(unsigned int registerNumber, double height) {
// TODO Obtain max tool numbers from a new class of Printer -> Hyrel and automate retrieving maximal values
    const double maxHeight = 120;
    if (0 > height || height > maxHeight) {
        std::cout << "Gcode writing -> defineHeightOffset -> tool height: max value "
                  << maxHeight << ", used value " << height << "\n";
    } else {
        bodyStream << "M660 H" << registerNumber << " Z" << height << "\n";
    }
}

void Hyrel::defineToolOffset(unsigned int toolNumber, const std::vector<double> &xyz, unsigned int offsetRegister) {
    const double X_MAX = 200;
    const double Y_MAX = 200;
    const double Z_MAX = 120;
    if (0 > xyz[0] || xyz[0] > X_MAX) {
        std::cout << "Gcode writing -> defineToolOffset -> tool x coordinate: max value "
                  << X_MAX << ", used value " << xyz[0] << "\n";
    } else if (0 > xyz[1] || xyz[1] > Y_MAX) {
        std::cout << "Gcode writing -> defineToolOffset -> tool y coordinate: max value "
                  << Y_MAX << ", used value " << xyz[1] << "\n";
    } else if (0 > xyz[2] || xyz[2] > Z_MAX) {
        std::cout << "Gcode writing -> defineToolOffset -> tool z coordinate: max value "
                  << Z_MAX << ", used value " << xyz[2] << "\n";
    } else {
        addComment("Defining tool offset");
        generalCommand(
                {'M', 'T', 'O', 'X', 'Y', 'Z'},
                {true, true, true, false, false, false},
                {6, (double) toolNumberVariable(toolNumber), (double) offsetRegister, xyz[0], xyz[1], xyz[2]});
    }
}

void Hyrel::extrude(const std::valarray<double> &xy) {
    positions[0] = xy[0];
    positions[1] = xy[1];
//    bodyStream << "G1 X" << xy[0] << " Y" << xy[1] << " F" << printSpeed << " E1" << "\n";
    generalCommand({'G', 'X', 'Y', 'F', 'E'}, {true, false, false, true, true},
                   {0, xy[0], xy[1], (double) moveSpeed, 1});
}

void Hyrel::configureFlow(double nozzleWidth, double layerHeight, double flowMultiplier, int pulses, int tool) {
    addComment("Configuring flow");
    generalCommand({'G', 'W', 'Z', 'S', 'P', 'T'}, {true, false, false, false, true, true},
                   {221, nozzleWidth, layerHeight, flowMultiplier, (double) pulses, (double) tool});
}

void Hyrel::setUnitsToMillimetres() {
    addComment("Setting units to millimetres");
    generalCommand('G', 21);
}

void Hyrel::turnMotorsOff() {
    addComment("Turning motors off");
    generalCommand('M', 18);
}

void Hyrel::signalFinishedPrint() {
    addComment("Signaling finished print");
    generalCommand('M', 30);
}

void Hyrel::clearOffsets() {
    addComment("Clearing offsets");
    generalCommand('G', 53);
}

void Hyrel::clean(double cleanLength, int numberOfLines, double nozzleWidth) {
    for (int i = 0; i < numberOfLines; i++) {
        if (i % 2 == 0) {
            movePlanar({0, i * nozzleWidth});
            extrude({cleanLength, i * nozzleWidth});

        } else {
            movePlanar({cleanLength, i * nozzleWidth});
            extrude({0, i * nozzleWidth});
        }
    }
}

void Hyrel::init(int hotendTemperature, int bedTemperature, double cleanLength, double nozzleWidth,
                 double layerHeight, int toolNumber, double zOffset, double xOffset, double yOffset) {
    const int POSITION_OFFSET_REGISTER = 0;
    const int KRA2_PULSES_PER_MICROLITRE = 1297;
    const int CLEANING_LINES = 4;

    setUnitsToMillimetres();
    clearOffsets();
    addBreak();

    autoHome();
    addBreak();
    defineToolOffset(toolNumber, {xOffset, yOffset, zOffset}, POSITION_OFFSET_REGISTER);
    addBreak();

    selectTool(toolNumber);
    setTemperatureHotend(hotendTemperature);
    setTemperatureBed(bedTemperature);
    addBreak();

    configureFlow(nozzleWidth, layerHeight, extrusionCoefficient, KRA2_PULSES_PER_MICROLITRE, toolNumber);
    addBreak();
    clean(cleanLength, CLEANING_LINES, nozzleWidth);
    addBreak();
}

void Hyrel::shutDown() {
    setTemperatureBed(0);
    setTemperatureHotend(0);
    addBreak();

    autoHome();
    addBreak();

    turnMotorsOff();
    signalFinishedPrint();
}

Hyrel::Hyrel(int moveSpeed, int printSpeed, double extrusionCoefficient) : GCodeFile(moveSpeed, printSpeed,
                                                                                     extrusionCoefficient) {

}

void Hyrel::printPath(const std::vector<std::valarray<int>> &path, const std::valarray<double> &positionOffset,
                      double gridDistance) {
    addComment("Moving up.");
    moveVerticalRelative(1);
    addComment("Moving to new starting point.");

    movePlanar(itodArray(path[0]) * gridDistance + positionOffset);
    addComment("Moving down.");
    moveVerticalRelative(-1);
    addComment("Starting new path.");
    for (auto &position: path) {
        extrude(itodArray(position) * gridDistance + positionOffset);
    }
}

void Hyrel::printPattern(const std::vector<std::vector<std::valarray<int>>> &sortedSequenceOfPaths,
                         const std::valarray<double> &positionOffset, double gridDistance) {
    for (auto &path: sortedSequenceOfPaths) {
        printPath(path, positionOffset, gridDistance);
    }
}


void testHeaderAndFooter() {
    Hyrel hyrel(600, 100, 1);
    hyrel.init(20, 0, 30, 0.335, 0.16, 1, 50, 30, 30);
    hyrel.shutDown();
    std::cout << hyrel.getText();
}


void generateGCodeHyrel(const std::string &baseDirectory, int temperature, double cleaningDistance,
                        const std::valarray<double> &positionOffset, double gridDistance, int moveSpeed, int printSpeed,
                        double extrusionCoefficient, double nozzleWidth, double layerHeight, int toolNumber,
                        double xOffset, double yOffset, double zOffset) {
    std::cout << std::endl;
    std::string directoryPath = baseDirectory + R"(\results)";
    std::vector<std::vector<std::valarray<int>>> sortedPaths = read3DVectorFromFile(directoryPath, "best_paths");
    Hyrel hyrel(moveSpeed, printSpeed, extrusionCoefficient);
    hyrel.init(temperature, 0, cleaningDistance, nozzleWidth,
               layerHeight, toolNumber, zOffset, xOffset, yOffset);
    hyrel.printPattern(sortedPaths, positionOffset, gridDistance);
    hyrel.shutDown();

    hyrel.exportToFile(baseDirectory);
}