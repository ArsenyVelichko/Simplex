//
// Created by awesyr on 22.02.2021.
//

// You may need to build the project (run Qt uic code generator) to get "ui_LPInput.h" resolved

#include <QHBoxLayout>

#include "Common.h"
#include "SimplexMethod.h"
#include "LPInput.h"
#include "ui_LPInput.h"

LPInput::LPInput(QWidget* parent) :
        QWidget(parent), ui(new Ui::LPInput) {
    ui->setupUi(this);

    mVarNumber = 0;
    mConstrNumber = 0;

    mValues = QVector<QVector<double>>(1, QVector<double>(1));
    mSetValueFunctor = new SetValueFunctor(this);

    onVariablesNChanged(2);
    onConstraintsNChanged(2);

    connect(ui->variablesNumber, &QSlider::valueChanged, this, &LPInput::onVariablesNChanged);
    connect(ui->constraintsNumber, &QSlider::valueChanged, this, &LPInput::onConstraintsNChanged);
    connect(ui->calcButton, &QPushButton::clicked, this, &LPInput::onCalculate);
}

LPInput::~LPInput() {
    delete ui;
}

void LPInput::onConstraintsNChanged(int n) {
    for (int i = mConstrNumber; i < n; i++) {
        addConstr();
    }

    for (int i = n; i < mConstrNumber; i++) {
        removeConstr();
    }

    mConstrNumber = n;
}

void LPInput::onVariablesNChanged(int n) {
    for (int i = mVarNumber; i < n; i++) {
        addVar(i + 1);
    }

    for (int i = n; i < mVarNumber ; i++) {
        removeVar();
    }

    mVarNumber = n;
}

void LPInput::addVar(int n) {
    QString newVarName = "x" + QString::number(n);

    for (int i = 0; i <= mConstrNumber; i++) {
        auto* plusLabel = new QLabel("+");
        auto* xLabel = new QLabel(newVarName);
        auto* numberLineEdit = new QLineEdit();

        numberLineEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        numberLineEdit->setAlignment(Qt::AlignCenter);

        mEditHash[numberLineEdit] = QPoint(n, i);
        mValues[i].append(0.0);
        connect(numberLineEdit, &QLineEdit::editingFinished,
                mSetValueFunctor, &SetValueFunctor::onEditingFinished);

        int row = 2 + 2 * i;
        auto* horLayout = (QHBoxLayout*)ui->varLayout->itemAt(row)->layout();

        int bias = i == 0 ? 1 : 2;
        horLayout->insertWidget(horLayout->count() - bias, plusLabel);
        horLayout->insertWidget(horLayout->count() - bias, numberLineEdit);
        horLayout->insertWidget(horLayout->count() - bias, xLabel);
    }

    auto* checkBox = new QCheckBox(newVarName);
    checkBox->setCheckState(Qt::Checked);
    connect(checkBox, &QCheckBox::stateChanged, mSetValueFunctor,
            &SetValueFunctor::onStateChanged);

    ui->positiveConstraint->insertWidget(n - 1, checkBox);
    mPositiveConstraints.append(true);
    mCheckBoxHash[checkBox] = n - 1;
}

void LPInput::removeVar() {
    for (int i = 0; i <= mConstrNumber; i++) {
        int row = 2 + 2 * i;
        auto* horLayout = (QHBoxLayout*)ui->varLayout->itemAt(row)->layout();

        int bias = i == 0 ? 2 : 3;
        for (int j = 0; j < 3; j++) {
            auto* item = horLayout->takeAt(horLayout->count() - bias);

            auto* lineEdit = qobject_cast<QLineEdit*>(item->widget());
            if (lineEdit) {
                mEditHash.remove(lineEdit);
            }

            delete item->widget();
            delete item;
        }

        mValues[i].removeLast();
    }

    int lastVar = mValues[0].size() - 1;
    auto* checkBoxItem = ui->positiveConstraint->takeAt(lastVar);
    auto* checkBox = (QCheckBox*)checkBoxItem->widget();
    mCheckBoxHash.remove(checkBox);

    delete checkBox;
    delete checkBoxItem;
}

void LPInput::addConstr() {
    auto* newLayout = new QHBoxLayout;

    QSizePolicy numberSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    for (int i = 0; i < mVarNumber; i++) {
        auto* numberLineEdit = new QLineEdit();
        auto* xLabel = new QLabel("x" + QString::number(i + 1));

        numberLineEdit->setSizePolicy(numberSizePolicy);
        numberLineEdit->setAlignment(Qt::AlignCenter);

        mEditHash[numberLineEdit] = QPoint(i + 1, mValues.size());
        connect(numberLineEdit, &QLineEdit::editingFinished,
                mSetValueFunctor, &SetValueFunctor::onEditingFinished);

        newLayout->addWidget(numberLineEdit);
        newLayout->addWidget(xLabel);

        if (i < mVarNumber - 1) {
            auto* plusLabel = new QLabel("+");
            newLayout->addWidget(plusLabel);
        }
    }

    auto* eqComboBox = new QComboBox;
    eqComboBox->addItem("<=");
    eqComboBox->addItem(">=");
    eqComboBox->addItem("=");
    newLayout->addWidget(eqComboBox);

    mComboBoxHash[eqComboBox] = mSigns.size();
    mSigns.append(InequalitySigns::Less_Or_Equal);
    connect(eqComboBox, &QComboBox::currentTextChanged,
            mSetValueFunctor, &SetValueFunctor::onCurrentTextChanged);

    auto* numberLineEdit = new QLineEdit();
    numberLineEdit->setSizePolicy(numberSizePolicy);
    numberLineEdit->setAlignment(Qt::AlignCenter);
    newLayout->addWidget(numberLineEdit);

    mEditHash[numberLineEdit] = QPoint(0, mValues.size());;
    connect(numberLineEdit, &QLineEdit::editingFinished,
            mSetValueFunctor, &SetValueFunctor::onEditingFinished);

    auto* spacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

    int lastRow = ui->varLayout->count() - 1;

    ui->varLayout->insertLayout(lastRow - 1, newLayout, 1);
    ui->varLayout->insertItem(lastRow, spacer);

    mValues.append(QVector<double>(mValues[0].size()));
}

void LPInput::removeConstr() {
    int lastExpr = ui->varLayout->count() - 4;
    auto* layoutItem = ui->varLayout->takeAt(lastExpr);
    auto* horLayout = layoutItem->layout();

    while (auto* item = horLayout->takeAt(0)) {

        auto* lineEdit = qobject_cast<QLineEdit*>(item->widget());
        if (lineEdit) {
            mEditHash.remove(lineEdit);
        }

        auto* comboBox = qobject_cast<QComboBox*>(item->widget());
        if (comboBox) {
            mComboBoxHash.remove(comboBox);
        }

        delete item->widget();
        delete item;
    }
    delete horLayout;

    auto* lowerSpacer = ui->varLayout->takeAt(lastExpr - 1);
    delete lowerSpacer;

    mValues.removeLast();
}

void LPInput::onCalculate() {
    ui->textEdit->clear();

    LP linearProgram(mValues);
    linearProgram.setSigns(mSigns);
    linearProgram.setPositConstr(mPositiveConstraints);

    if (ui->minimizationBox->currentText() == "min") {
        linearProgram.toMinimization();
    }

    LP dualProgram = linearProgram.dual();

    SimplexMethod simplexMethod;

    QVector<double> solution = simplexMethod.solve(linearProgram);

    QString varCoeffs = Common::vectorToString(solution);
    ui->textEdit->append("Primal program coefficients: " + varCoeffs);

    QVector<double> targetValues = solution.mid(0, linearProgram.nonbasicDim());
    double optimumValue = linearProgram.at(targetValues);
    ui->textEdit->append("Primal optimum value:" + QString::number(optimumValue));

    solution = simplexMethod.solve(dualProgram);

    varCoeffs = Common::vectorToString(solution);
    ui->textEdit->append("Dual program coefficients: " + varCoeffs);

    targetValues = solution.mid(0, dualProgram.nonbasicDim());
    dualProgram.toMinimization();
    optimumValue = dualProgram.at(targetValues);
    ui->textEdit->append("Dual optimum value:" + QString::number(optimumValue));
}

SetValueFunctor::SetValueFunctor(LPInput* lpInput)
    : mLPInput(lpInput), QObject(lpInput) {}

void SetValueFunctor::onEditingFinished() {
    auto* lineEdit = qobject_cast<QLineEdit*>(sender());
    if (lineEdit == nullptr) { return; }

    if (!mLPInput->mEditHash.contains(lineEdit)) { return; }
    QPoint pos = mLPInput->mEditHash[lineEdit];
    mLPInput->mValues[pos.y()][pos.x()] = lineEdit->text().toDouble();
}

void SetValueFunctor::onCurrentTextChanged(const QString& text) {
    auto* comboBox = qobject_cast<QComboBox*>(sender());
    if (comboBox == nullptr) { return; }

    if (!mLPInput->mComboBoxHash.contains(comboBox)) { return; }
    int pos = mLPInput->mComboBoxHash[comboBox];

    InequalitySigns sign;
    if (text == "<=") {
        sign = InequalitySigns::Less_Or_Equal;
    } else if (text == ">=") {
        sign = InequalitySigns::Greater_Or_Equal;
    } else {
        sign = InequalitySigns::Equal;
    }

    mLPInput->mSigns[pos] = sign;
}

void SetValueFunctor::onStateChanged(int state) {
    auto* checkBox = qobject_cast<QCheckBox*>(sender());
    if (checkBox == nullptr) { return; }

    if (!mLPInput->mCheckBoxHash.contains(checkBox)) { return; }
    int pos = mLPInput->mCheckBoxHash[checkBox];

    bool isChecked = state == Qt::Checked;
    mLPInput->mPositiveConstraints[pos] = isChecked;
}
