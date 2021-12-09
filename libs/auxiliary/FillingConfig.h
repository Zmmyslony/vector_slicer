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
    InitialFillingMethod, CollisionRadius, StepLength, PrintRadius, Repulsion, StartingPointSeparation, Seed
};


class FillingConfig {
    FillingMethod fillingMethod;
    int collisionRadius;
    double repulsion;
    int stepLength;
    int startingPointSeparation;
    int printRadius;
    unsigned int seed;

    void readLineOfConfig(std::vector<std::string> line);

public:
    void printConfig();

    void setConfigOption(const ConfigOptions &option, const std::string &value);

    FillingMethod getInitialFillingMethod() const;

    int getCollisionRadius() const;

    double getRepulsion() const;

    int getStepLength() const;

    int getStartingPointSeparation() const;

    int getPrintRadius() const;

    unsigned int getSeed() const;


    explicit FillingConfig(std::string &configPath);

    FillingConfig(FillingMethod newPerimeterFillingMethod, int newCollisionRadius,
                  int newStartPointSeparation, double newRepulsion, int newStepLength,
                  int newPrintRadius, unsigned int seed);


    std::string getConfigOption(ConfigOptions option);
};


#endif //VECTOR_SLICER_FILLINGCONFIG_H
