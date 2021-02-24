//
// Created by awesyr on 22.02.2021.
//

#ifndef SIMPLEX_LPINPUT_H
#define SIMPLEX_LPINPUT_H

#include <QWidget>
#include <QLineEdit>
#include <QCheckBox>

#include <functional>

#include "LP.h"

QT_BEGIN_NAMESPACE
namespace Ui { class LPInput; }
QT_END_NAMESPACE

class SetValueFunctor;

class LPInput : public QWidget {
Q_OBJECT

public:
    explicit LPInput(QWidget* parent = nullptr);

    ~LPInput() override;

private slots:
    void onConstraintsNChanged(int n);
    void onVariablesNChanged(int n);

    void onCalculate();

private:
    void addVar(int n);
    void removeVar();

    void addConstr();
    void removeConstr();

    Ui::LPInput* ui;

    QHash<QLineEdit*, QPoint> mEditHash;
    SetValueFunctor* mSetValueFunctor;

    int mVarNumber;
    int mConstrNumber;
    QVector<QVector<double>> mValues;

    QHash<QComboBox*, int> mComboBoxHash;
    QVector<InequalitySigns> mSigns;

    QHash<QCheckBox*, int> mCheckBoxHash;
    QVector<bool> mPositiveConstraints;

    friend class SetValueFunctor;
};

class SetValueFunctor : public QObject {
    Q_OBJECT
public:
    SetValueFunctor(LPInput* lpInput);

public slots:
    void onEditingFinished();
    void onCurrentTextChanged(const QString& text);
    void onStateChanged(int state);

private:
    LPInput* mLPInput;
};

#endif //CMAKE_AND_CUDA_LPINPUT_H
