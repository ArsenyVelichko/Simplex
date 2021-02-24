//
// Created by awesyr on 21.02.2021.
//

#ifndef SIMPLEX_SIMPLEXMETHOD_H
#define SIMPLEX_SIMPLEXMETHOD_H

#include "LP.h"

class SimplexMethod {
public:
    QVector<double> solve(const LP& linearProgram);

private:
    bool init(const LP& linearProgram);
    void findSimplex();
    void pivot(int e, int l);

    QVector<QVector<double>> mExtendedMatrix;
    QVector<int> mPivotVec;
};


#endif //CMAKE_AND_CUDA_SIMPLEXMETHOD_H
