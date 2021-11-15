//
// Created by zmmys on 05/11/2021.
//

#ifndef VECTOR_SLICER_FILLINGCONFIG_H
#define VECTOR_SLICER_FILLINGCONFIG_H

#include <string>
#include <vector>

enum FillingMethod {
    ConsecutivePerimeter, RandomPerimeter, ConsecutiveRadial, RandomRadial
};
enum ConfigOptions {
    InitialFillingMethod, CollisionRadius, StepLength, PrintRadius, Repulsion, MinimalStepLength
};


class FillingConfig {
    FillingMethod fillingMethod;
    int collisionRadius;
    double repulsion;
    int stepLength;
    int minimalStepLength;
    int printRadius;

    void readLineOfConfig(std::vector<std::string> line);

public:
    void printConfig();
    void setConfigOption(ConfigOptions option, std::string value);

    FillingMethod getInitialFillingMethod() const;

    int getCollisionRadius() const;

    double getRepulsion() const;

    int getStepLength() const;

    int getMinimalStepLength() const;

    int getPrintRadius() const;


    explicit FillingConfig(std::string &configPath);

    FillingConfig(FillingMethod newPerimeterFillingMethod, int newCollisionRadius,
                  int newMinimalStepLength, double newRepulsion, int newStepLength,
                  int newPrintRadius);


//    auto getConfigOption(ConfigOptions option);
};


#endif //VECTOR_SLICER_FILLINGCONFIG_H
