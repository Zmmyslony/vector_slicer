//
// Created by Michał Zmyślony on 17/09/2021.
//

#ifndef VECTOR_SLICER_PRINTER_H
#define VECTOR_SLICER_PRINTER_H


class Printer {
public:
    double movePrecision;
    double extrusionRadius;
    double minimalLineLength;
    double layerHeight;

    Printer(double movePrecision, double extrusionRadius, double minimalLineLength, double layerHeight);
};


#endif //VECTOR_SLICER_PRINTER_H
