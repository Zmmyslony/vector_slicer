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

enum PointSearchStage {PerimeterSearch, TotallyRandomPointSelection, EmptySpotRandomSelection};

class FilledPattern {
//    bool isPerimeterSearchOn = true;

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

    std::vector<std::valarray<int>> findRemainingFillablePointsInList(
            std::vector<std::valarray<int>> &listOfPoints) const;

    std::vector<std::valarray<int>> findInitialStartingPoints(FillingMethod method);

    std::mt19937 randomEngine;
    std::uniform_int_distribution<unsigned int> distribution;
    std::uniform_int_distribution<int> xDistribution;
    std::uniform_int_distribution<int> yDistribution;

    std::valarray<double> getDirector(const std::valarray<int> &positions);

    std::vector<std::valarray<int>>
    getSpacedLine(const double &distance, const std::vector<std::valarray<int>> &line);

    std::vector<std::valarray<int>> findDualLine(const std::valarray<int> &start);

    std::valarray<double> getDirector(const std::valarray<double> &positions);

    std::vector<std::valarray<int>>
    findDualLineOneDirection(std::valarray<double> coordinates, std::valarray<double> previousDualDirector);

public:
    FillingConfig config;
    bool isFillingMethodRandom = true;
    std::vector<std::vector<double>> xFieldFilled;
    std::vector<std::vector<double>> yFieldFilled;
    const DesiredPattern &desiredPattern;

    std::vector<std::valarray<int>> pointsToFill;
    std::vector<std::valarray<int>> collisionList;
    std::vector<std::vector<int>> numberOfTimesFilled;
    PointSearchStage searchStage = PerimeterSearch;

    FilledPattern(const DesiredPattern &desiredPattern, int printRadius, int collisionRadius, int stepLength,
                  unsigned int seed);

    FilledPattern(const DesiredPattern &desiredPattern, int printRadius, int collisionRadius, int stepLength);

    FilledPattern(const DesiredPattern &newDesiredPattern, FillingConfig newConfig);

    void addNewPath(Path &newPath);

    void updateSearchStageAndFillablePoints();

    void fillPointsInCircle(const std::valarray<int> &startingCoordinates);

    Path generateNewPathForDirection(std::valarray<int> &startingCoordinates, const std::valarray<int> &startingStep);

    std::vector<Path> getSequenceOfPaths();

    unsigned int getNewElement();

    void exportToDirectory(const std::string &directory) const;

    void fillPointsInHalfCircle(const std::valarray<int> &lastPoint, const std::valarray<int> &previousPoint);

    std::valarray<int> findPointInShape();
};


#endif //VECTOR_SLICER_FILLEDPATTERN_H
