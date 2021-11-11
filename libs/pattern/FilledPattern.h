//
// Created by Michał Zmyślony on 21/09/2021.
//
#ifndef VECTOR_SLICER_FILLEDPATTERN_H
#define VECTOR_SLICER_FILLEDPATTERN_H


#include "DesiredPattern.h"
#include "Path.h"
#include "../auxiliary/FillingConfig.h"
#include <string>
#include <random>


class FilledPattern {
    bool isPerimeterSearchOn = true;

    std::vector<Path> sequenceOfPaths;
    std::vector<std::valarray<int>> pointsInCircle;

    void
    fillPointsFromList(const std::vector<std::valarray<int>> &listOfPoints, const std::valarray<int> &direction);

    void fillPointsFromDisplacement(const std::valarray<int> &startingPosition,
                                    const std::vector<std::valarray<int>> &listOfDisplacements,
                                    const std::valarray<int> &previousStep);

    std::vector<std::valarray<int>> findAllFillablePoints();

    std::valarray<double>
    getNewStep(std::valarray<double> &realCoordinates, int &length, std::valarray<double> &previousStep);

    bool
    tryGeneratingPathWithLength(Path &currentPath, std::valarray<double> &positions, std::valarray<double> &newStep,
                                int length);

    void fillPoint(const std::valarray<int> &point, const std::valarray<double> &previousStep);

    std::vector<std::valarray<int>> findRemainingFillablePointsInList(std::vector<std::valarray<int>> listOfPoints);

    std::valarray<int> findFirstPointOnPerimeter();

    std::valarray<int> findNextPointOnPerimeter(std::valarray<int> &currentPoint, std::valarray<int> &previousDirection,
                                                std::vector<std::valarray<int>> &perimeterList);

    std::valarray<int> getNewPointOnEdge(const std::valarray<int>& currentPoint, const std::valarray<int>& previousDirection);

    std::valarray<int> findNextPointOnPerimeter(std::valarray<int> &currentPoint,
                                                std::valarray<int> &previousDirection,
                                                int distance);

    std::vector<std::valarray<int>> findPerimeterOfTheShape();

    std::vector<std::valarray<int>> findLineThroughShape();

    std::vector<std::valarray<int>> findInitialStartingPoints(FillingMethod method);

    std::mt19937 randomEngine;
    std::uniform_int_distribution<unsigned int> distribution;

    std::valarray<double> getDirector(const std::valarray<int> &positions);

    std::vector<std::valarray<int>> getSpacedPerimeter(const double &distance);
public:
    FillingConfig config;
    bool isFillingMethodRandom = true;
    std::vector<std::vector<double>> xFieldFilled;
    std::vector<std::vector<double>> yFieldFilled;
    DesiredPattern desiredPattern;

    std::vector<std::valarray<int>> pointsToFill;
    std::vector<std::valarray<int>> collisionList;
    std::vector<std::vector<int>> numberOfTimesFilled;

    FilledPattern(DesiredPattern desiredPattern, int printRadius, int collisionRadius, int stepLength,
                  unsigned int seed);

    FilledPattern(DesiredPattern desiredPattern, int printRadius, int collisionRadius, int stepLength);

    FilledPattern(DesiredPattern desiredPattern, FillingConfig config, int seed);

    void addNewPath(Path &newPath);

    void findRemainingFillablePoints();

    void fillPointsInCircle(std::valarray<int> &startingCoordinates);

    Path generateNewPathForDirection(std::valarray<int> &startingCoordinates, const std::valarray<int> &startingStep);

    std::vector<Path> getSequenceOfPaths();

    unsigned int getNewElement();

    void exportToDirectory(std::string &directory) const;
};


#endif //VECTOR_SLICER_FILLEDPATTERN_H
