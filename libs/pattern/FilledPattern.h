//
// Created by Michał Zmyślony on 21/09/2021.
//
#ifndef VECTOR_SLICER_FILLEDPATTERN_H
#define VECTOR_SLICER_FILLEDPATTERN_H


#include "DesiredPattern.h"
#include "Path.h"
#include <string>


class FilledPattern {
    int printRadius;
    bool isPerimeterSearchOn = true;
    std::vector<Path> sequenceOfPaths;
    std::vector<std::valarray<int>> pointsInCircle;

    void fillPointsFromList(const std::vector<std::valarray<int>>& listOfPoints, const std::valarray<int>& previousStep);
    void fillPointsFromDisplacement(const std::valarray<int>& startingPosition,
                                    const std::vector<std::valarray<int>>& listOfDisplacements,
                                    const std::valarray<int>& previousStep);
    std::vector<std::valarray<int>> findAllFillablePoints();
//    std::vector<std::valarray<int>> searchForRemainingFillablePoints();
    std::valarray<double> getNewStep(std::valarray<double>& realCoordinates, int& length, std::valarray<double>& previousStep);
    bool
    tryGeneratingPathWithLength(Path& currentPath, std::valarray<double>& positions, std::valarray<double>& newStep,
                                int length);
    void fillPoint(const std::valarray<int> &point, const std::valarray<double>& previousStep);
    std::vector<std::valarray<int>> findRemainingFillablePointsInList(std::vector<std::valarray<int>> listOfPoints);

    std::valarray<int> findFirstPointOnPerimeter();
    std::valarray<int> findNextPointOnPerimeter(std::valarray<int> &currentPoint, std::valarray<int> &previousDirection,
                                                std::vector<std::valarray<int>> &perimeterList);
    std::vector<std::valarray<int>> findPerimeterOfTheShape();

public:
    int stepLength;
    std::vector<std::vector<double>> xFieldFilled;
    std::vector<std::vector<double>> yFieldFilled;
    DesiredPattern desiredPattern;

    std::vector<std::valarray<int>> pointsToFill;
    std::vector<std::valarray<int>> collisionList;
    std::vector<std::vector<int>> numberOfTimesFilled;


    FilledPattern(DesiredPattern& desiredPattern, int printRadius, int collisionRadius, int stepLength, unsigned int seed);
    FilledPattern(DesiredPattern& desiredPattern, int printRadius, int collisionRadius, int stepLength);

    void addNewPath(Path& newPath);
    void findRemainingFillablePoints();
    void fillPointsInCircle(std::valarray<int> &startingCoordinates);
    Path generateNewPathForDirection(std::valarray<int>& startingCoordinates, const std::valarray<int>& startingStep);

    std::vector<Path> getSequenceOfPaths();

    void exportToDirectory(std::string& directory) const;
};


#endif //VECTOR_SLICER_FILLEDPATTERN_H
