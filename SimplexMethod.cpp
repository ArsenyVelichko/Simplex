//
// Created by awesyr on 21.02.2021.
//

#include "SimplexMethod.h"
#include "Common.h"

#include <QDebug>

QVector<double> SimplexMethod::solve(const LP& linearProgram) {
    if (!init(linearProgram)) {
        qDebug() << "LP has no solution";
        return QVector<double>();
    }

    findSimplex();

    QVector<double> solution(linearProgram.varN());
    for (int i = linearProgram.nonbasicDim(); i < linearProgram.varN(); i++) {
        int basicI = i - linearProgram.nonbasicDim() + 1;
        solution[mPivotVec[i]] = mExtendedMatrix[basicI][0];
    }
    return solution;
}

bool SimplexMethod::init(const LP& linearProgram) {
    const auto& bias = linearProgram.bias();
    double minBias = Common::DBL_INFINITY;
    int minBiasI = -1;

    for (int i = 0; i < bias.size(); i++) {
        if (bias[i] < minBias) {
            minBias = bias[i];
            minBiasI = i;
        }
    }

    mPivotVec = QVector<int>(linearProgram.varN());
    for (int i = 0; i < mPivotVec.size(); i++) {
        mPivotVec[i] = i;
    }

    if (minBias >= 0.0) {
        mExtendedMatrix = linearProgram.extendedMatrix();
        return true;
    }

    mPivotVec.append(linearProgram.varN());

    const LP& auxiliary = linearProgram.auxiliary();
    mExtendedMatrix = auxiliary.extendedMatrix();

    pivot(1, minBiasI + 1);

    findSimplex();

    if (!qFuzzyIsNull(mExtendedMatrix[0][0])) { return false; }

    int auxVarI = mPivotVec.indexOf(0, auxiliary.nonbasicDim());
    if (auxVarI != -1) {
        auxVarI -= auxiliary.nonbasicDim() - 1;

        for (int j = 1; j < mExtendedMatrix[0].size(); j++) {
            if (mExtendedMatrix[auxVarI][j] > 0) {
                pivot(auxVarI, j);
                break;
            }
        }
    }

    auxVarI = mPivotVec.indexOf(0, 0);
    for (auto& row : mExtendedMatrix) {
        row.removeAt(auxVarI + 1);
    }

    mPivotVec.removeAt(auxVarI);
    for (int& index : mPivotVec) {
        index--;
    }

    const auto& oldTarget = linearProgram.targetCoeffs();
    mExtendedMatrix[0].fill(0.0);
    for (int i = 0; i < linearProgram.nonbasicDim(); i++) {
        if (mPivotVec[i] < oldTarget.size()) {
            mExtendedMatrix[0][i + 1] = oldTarget[mPivotVec[i]];
        }
    }

    for (int i = linearProgram.nonbasicDim(); i < mPivotVec.size(); i++) {

        if (mPivotVec[i] < oldTarget.size()) {
            for (int j = 0; j < mExtendedMatrix[0].size(); j++) {
                mExtendedMatrix[0][j] -= oldTarget[mPivotVec[i]] * mExtendedMatrix[mPivotVec[i] + 1][j];
            }
        }
    }

    return true;
}


void SimplexMethod::findSimplex() {
    while (true) {
        int positiveI = -1;

        for (int i = 1; i < mExtendedMatrix[0].size(); i++) {
            if (mExtendedMatrix[0][i] > 0.0) {
                positiveI = i;
                break;
            }
        }

        if (positiveI == -1) { break; }

        double minRatio = Common::DBL_INFINITY;
        int minRatioI = -1;
        for (int i = 1; i < mExtendedMatrix.size(); i++) {
            if (mExtendedMatrix[i][positiveI] > 0.0) {
                double ratio = mExtendedMatrix[i][0] / mExtendedMatrix[i][positiveI];
                if (ratio < minRatio) {
                    minRatio = ratio;
                    minRatioI = i;
                }
            }
        }

        if (minRatioI == -1) {
            qDebug() << "Linear program is unbounded";
            return;
        }
        pivot(positiveI, minRatioI);
    }
}

void SimplexMethod::pivot(int e, int l) {
    int rowN = mExtendedMatrix.size();
    int colN = mExtendedMatrix[0].size();

    double divider = mExtendedMatrix[l][e];
    mExtendedMatrix[l][e] = 1;

    for (int i = 0; i < colN; i++) {
        mExtendedMatrix[l][i] /= divider;
    }

    for (int i = 0; i < rowN; i++) {
        if (i == l) { continue; }

        for (int j = 0; j < colN; j++) {
            if (j == e) { continue; }
            mExtendedMatrix[i][j] -= mExtendedMatrix[i][e] * mExtendedMatrix[l][j];
        }
        mExtendedMatrix[i][e] *= -mExtendedMatrix[l][e];
    }

    mPivotVec.swapItemsAt(e - 1, colN + l - 2);
}
