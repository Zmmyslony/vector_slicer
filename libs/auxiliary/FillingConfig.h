//
// Created by zmmys on 05/11/2021.
//

#ifndef VECTOR_SLICER_FILLINGCONFIG_H
#define VECTOR_SLICER_FILLINGCONFIG_H
#include <string>
#include <vector>

class FillingConfig {
    bool isPerimeterFilledRandomly;
    int collisionRadius;
    double repulsion;
    int stepLength;
    int minimalStepLength;
    int printRadius;

public:
    void printConfig();
    void setPerimeterFillingToRandom();
    void setPerimeterFillingToConsecutive();
    void setCollisionRadius(int radius);
    void setRepulsion (double repulsionCoefficient);\
    void setStepLength (int step);
    void setPrintRadius(int radius);

    bool getPerimeterFillingMode() const;
    int getCollisionRadius() const;
    double getRepulsion() const;
    int getStepLength() const;
    int getPrintRadius() const;

    void readLineOfConfig(std::vector<std::string> line);
    explicit FillingConfig(std::string &configPath);
};


#endif //VECTOR_SLICER_FILLINGCONFIG_H
