//
// Created by awesyr on 21.02.2021.
//

#include "LP.h"
#include <QDebug>
#include "Common.h"

LP::LP(const QVector<double>& targetCoeffs,
       const QVector<double>& bias,
       const QVector<QVector<double>>& matrixCoeffs)
       :  mTargetCoeffs(targetCoeffs),
          mBias(bias),
          mMatrixCoeffs(matrixCoeffs) {}

QVector<double> LP::targetCoeffs() const { return mTargetCoeffs; }

QVector<double> LP::bias() const { return mBias; }

QVector<QVector<double>> LP::matrixCoeffs() const { return mMatrixCoeffs; }

int LP::basicDim() const {return mBias.size(); }

int LP::nonbasicDim() const { return mTargetCoeffs.size(); }

int LP::varN() const { return basicDim() + nonbasicDim(); }

LP LP::auxiliary() const {
    QVector<double> auxTarget(nonbasicDim() + 1);
    auxTarget[0] = -1;

    QVector<QVector<double>> auxMatrix = mMatrixCoeffs;
    for (auto& row : auxMatrix) {
        row.prepend(-1);
    }
    return LP(auxTarget, mBias, auxMatrix);
}

LP LP::dual() const {
    const auto& transposedMatrix = Common::transposeMatrix(mMatrixCoeffs);
    LP dual(bias(), targetCoeffs(), transposedMatrix);

    QVector<InequalitySigns> signs(dual.basicDim(), InequalitySigns::Greater_Or_Equal);
    dual.setSigns(signs);
    dual.toMinimization();
    return dual;
}


QVector<QVector<double>> LP::extendedMatrix() const {
    QVector<QVector<double>> extendedMatrix = QVector<QVector<double>>(basicDim() + 1);

    extendedMatrix[0].append(0.0);
    extendedMatrix[0].append(mTargetCoeffs);

    for (int i = 0; i < basicDim(); i++) {
        extendedMatrix[i + 1].append(mBias[i]);
        extendedMatrix[i + 1].append(mMatrixCoeffs[i]);
    }
    return extendedMatrix;
}

double LP::at(const QVector<double>& values) const {
    if (values.size() != nonbasicDim()) {
        qDebug() << "Values array size must be equal to non-basic dimension of linear program";
    }

    double res = 0.0;
    for (int i = 0; i < values.size(); i++) {
        res += values[i] * mTargetCoeffs[i];
    }
    return res;
}

void LP::changeRowSign(int i) {
    for (int j = 0; j < nonbasicDim(); j++) {
        mMatrixCoeffs[i][j] *= -1;
    }
    mBias[i] *= -1;
}


void LP::splitColumn(int i) {
    for (int j = 0; j < basicDim(); j++) {
        double varCoeff = mMatrixCoeffs[j][i];
        mMatrixCoeffs[j].append(-varCoeff);
    }
    mTargetCoeffs.append(-mTargetCoeffs[i]);
}

void LP::splitRow(int i) {
    mMatrixCoeffs.append(mMatrixCoeffs[i]);
    mBias.append(-mBias[i]);
    changeRowSign(mMatrixCoeffs.size() - 1);
}

void LP::setSigns(const QVector<InequalitySigns>& signs) {
    if (signs.size() != basicDim()) {
        qDebug() << "Signs array size doesn't match";
        return;
    }

    for (int i = 0; i < signs.size(); i++) {

        switch (signs[i]) {
            case InequalitySigns::Greater_Or_Equal:
                changeRowSign(i);
                break;

            case InequalitySigns::Equal:
                splitRow(i);
                break;

            default:
                break;
        }
    }
}

void LP::setPositConstr(const QVector<bool>& positiveConstraints) {
    if (positiveConstraints.size() != nonbasicDim()) {
        qDebug() << "Positive constraints array size doesn't match";
        return;
    }

    for (int i = 0; i < positiveConstraints.size(); i++) {
        if (!positiveConstraints[i]) {
            splitColumn(i);
        }
    }
}

void LP::toMinimization() {
    for (int i = 0; i < nonbasicDim(); i++) {
        mTargetCoeffs[i] *= -1;
    }
}

LP::LP(const QVector<QVector<double>>& extendedMatrix) {
    mTargetCoeffs = extendedMatrix[0];
    mTargetCoeffs.removeFirst();

    for (int i = 1; i < extendedMatrix.size(); i++) {
        mBias.append(extendedMatrix[i][0]);
        mMatrixCoeffs.append(extendedMatrix[i]);
        mMatrixCoeffs.last().removeFirst();
    }
}