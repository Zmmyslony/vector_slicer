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
    InitialFillingMethod, CollisionRadius, StepLength, PrintRadius, Repulsion, MinimalStepLength,
    IsInitialFillingRandom
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

    void setInitialFillingMethod(FillingMethod method);

    void setCollisionRadius(int radius);

    void setRepulsion(double repulsionCoefficient);

    void setStepLength(int step);

    void setMinimalStepLength(int step);

    void setPrintRadius(int radius);

    void setInitialFillingMethodToRandomPerimeter();

    void setInitialFillingMethodToConsecutivePerimeter();

    void setInitialFillingMethodToRandomRadial();

    void setInitialFillingMethodToConsecutiveRadial();

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
};


#endif //VECTOR_SLICER_FILLINGCONFIG_H
