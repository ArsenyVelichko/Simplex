//
// Created by awesyr on 21.02.2021.
//

#ifndef SIMPLEX_COMMON_H
#define SIMPLEX_COMMON_H

#include <QVector>

#include <limits>
#include <QTextStream>

namespace Common {
    constexpr double DBL_INFINITY = std::numeric_limits<double>::infinity();

    template<typename T>
    QVector<QVector<T>> transposeMatrix(const QVector<QVector<T>>& matrix) {
        QVector<QVector<T>> transposed(matrix[0].size(), QVector<T>(matrix.size()));

        for (int i = 0; i < transposed.size(); i++) {
            for (int j = 0; j < transposed[0].size(); j++) {
                transposed[i][j] = matrix[j][i];
            }
        }
        return transposed;
    }
    template<typename T>
    QString matrixToString(const QVector<QVector<T>>& matrix) {
        QString str;
        QTextStream textStream(&str);

        for (auto& row : matrix) {
            for (const T& elem : row) {
                textStream << elem << " ";
            }
            textStream << "\n";
        }
        return str;
    }

    template<typename T>
    QString vectorToString(const QVector<T>& vector) {
        QString str;
        QTextStream textStream(&str);

        for (const T& elem : vector) {
            textStream << elem << " ";
        }

        return str;
    }
}

#endif //SIMPLEX_COMMON_H
