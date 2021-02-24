//
// Created by awesyr on 21.02.2021.
//

#ifndef SIMPLEX_LP_H
#define SIMPLEX_LP_H

#include <QVector>

enum class InequalitySigns {
    Less_Or_Equal,
    Greater_Or_Equal,
    Equal,
};

class LP {
public:
    LP(const QVector<double>& targetCoeffs,
       const QVector<double>& bias,
       const QVector<QVector<double>>& matrixCoeffs);
    LP(const QVector<QVector<double>>& extendedMatrix);

    void setSigns(const QVector<InequalitySigns>& signs);
    void setPositConstr(const QVector<bool>& positiveConstraints);
    void toMinimization();

    double at(const QVector<double>& values) const;

    QVector<double> targetCoeffs() const;
    QVector<double> bias() const;
    QVector<QVector<double>> matrixCoeffs() const;

    int basicDim() const;
    int nonbasicDim() const;
    int varN() const;

    LP auxiliary() const;
    LP dual() const;
    QVector<QVector<double>> extendedMatrix() const;

private:
    void changeRowSign(int i);
    void splitColumn(int i);
    void splitRow(int i);

    QVector<double> mTargetCoeffs;
    QVector<double> mBias;
    QVector<QVector<double>> mMatrixCoeffs;
};

#endif //CMAKE_AND_CUDA_LP_H
