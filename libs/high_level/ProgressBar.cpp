//
// Created by zmmys on 04/11/2021.
//

#include "ProgressBar.h"
#include <iostream>

void showProgress(double progress) {
    int barWidth = 70;
    int pos = barWidth * progress;
    std::cout << "\r[";
    for (int i = 0; i < barWidth; ++i) {
        if (i <= pos) {
            std::cout << "=";
        } else {
            std::cout << " ";
        }
    }
    std::cout << "] " << int(progress * 100.0) << " %";
    std::cout.flush();
}

void showProgress(int currentStep, int maxStep) {
    showProgress((double) currentStep / (double) maxStep);
}