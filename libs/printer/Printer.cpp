//
// Created by Michał Zmyślony on 17/09/2021.
//

#include "Printer.h"

Printer::Printer(double movePrecision, double extrusionRadius, double minimalLineLength, double layerHeight):
        movePrecision(movePrecision),
        extrusionRadius(extrusionRadius),
        minimalLineLength(minimalLineLength),
        layerHeight(layerHeight)
{}