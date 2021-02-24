#include <iostream>

#include <QApplication>

#include "LP.h"
#include "SimplexMethod.h"
#include "LPInput.h"

int main(int argc, char** argv) {
    QApplication app(argc, argv);

    QVector<double> targetCoeffs = { 2, -1 };
    QVector<double> bias = { 2, -4 };
    QVector<QVector<double>> matrixCoeffs = { { 2, -1 },
                                              { 1, -5 }};


    LPInput lpInput;
    lpInput.show();

    LP linearProgram(targetCoeffs, bias, matrixCoeffs);
    SimplexMethod sm;

    std::cout << std::endl;
    return app.exec();
}
