/***************************************************************************
  variable_delegate.cpp
  ---------------------
  Copyright © 2007-2011, Eco2s team, Antonio Forgione
  Copyright © 2011-2019, LI-COR Biosciences, Inc. All Rights Reserved.
  Author: Antonio Forgione

  This file is part of EddyPro®.

  NON-COMMERCIAL RESEARCH PURPOSES ONLY - EDDYPRO® is licensed for
  non-commercial academic and government research purposes only,
  as provided in the EDDYPRO® End User License Agreement.
  EDDYPRO® may only be used as provided in the End User License Agreement
  and may not be used or accessed for any commercial purposes.
  You may view a copy of the End User License Agreement in the file
  EULA_NON_COMMERCIAL.rtf.

  Commercial companies that are LI-COR flux system customers are
  encouraged to contact LI-COR directly for our commercial EDDYPRO®
  End User License Agreement.

  EDDYPRO® contains Open Source Components (as defined in the
  End User License Agreement). The licenses and/or notices for the
  Open Source Components can be found in the file LIBRARIES.txt.

  EddyPro® is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
****************************************************************************/

#include "variable_delegate.h"

#include <QAbstractItemView>
#include <QComboBox>
#include <QDebug>
#include <QDoubleSpinBox>
#include <QKeyEvent>
#include <QLabel>

#include "customcombobox.h"
#include "globalsettings.h"
#include "nonzerodoublespinbox.h"
#include "variable_model.h"
#include "widget_utils.h"

VariableDelegate::VariableDelegate(QObject *parent) :
    QStyledItemDelegate(parent),
    varsBuffer_(new QStringList())
{
    installEventFilter(this);
    GlobalSettings::getCustomVariableList(varsBuffer_.data());
}

QWidget *VariableDelegate::createEditor(QWidget* parent,
                                     const QStyleOptionViewItem& option,
                                     const QModelIndex& index) const
{
    QLabel *label;
    QComboBox *combo;
    CustomComboBox *custom_combo;
    QDoubleSpinBox *dspin;

    int column = index.column();
    QString currentVar = index.model()->data(index.model()->index(VariableModel::VARIABLE, column)).toString();
    QString currentInputUnit = index.model()->data(index.model()->index(VariableModel::INPUTUNIT, column)).toString();

    // can only edit name on blank column
    if (column >= index.model()->columnCount()) return nullptr;

    // different kind of editor for each row
    switch (index.row())
    {
        case VariableModel::IGNORE:
            combo = new QComboBox(parent);
            combo->addItems(VariableDesc::yesNoStringList());
            combo->setMinimumWidth(130);
            connect(combo, SIGNAL(activated(int)),
                    this, SLOT(commitAndCloseEditor()));
            return combo;
        case VariableModel::NUMERIC:
            combo = new QComboBox(parent);
            combo->addItems(VariableDesc::yesNoStringList());
            combo->setMinimumWidth(130);
            connect(combo, SIGNAL(activated(int)),
                    this, SLOT(commitAndCloseEditor()));
            return combo;
        case VariableModel::VARIABLE:
            custom_combo = new CustomComboBox(parent);
            custom_combo->setEditable(true);
            custom_combo->setMaxVisibleItems(20);
            custom_combo->addParentItem(QStringLiteral("Standard Variables"));
            custom_combo->addChildItems(VariableDesc::variableStringList());
            if (!varsBuffer_->isEmpty())
            {
                custom_combo->addSeparator();
                custom_combo->addParentItem(QStringLiteral("Custom Variables"));
                custom_combo->addChildItems(*varsBuffer_);
            }
            custom_combo->setMinimumWidth(130);
            custom_combo->view()->setAlternatingRowColors(true);
            connect(custom_combo, SIGNAL(activated(int)),
                    this, SLOT(commitAndCloseEditor()));
            return custom_combo;
        case VariableModel::INSTRUMENT:
            combo = new QComboBox(parent);
            combo->setEditable(false);
            combo->addItems((dynamic_cast<const VariableModel *>(index.model()))->instrModels());
            combo->setMinimumWidth(130);
            connect(combo, SIGNAL(activated(int)),
                    this, SLOT(commitAndCloseEditor()));
            return combo;
        case VariableModel::MEASURETYPE:
            if (!VariableDesc::isGasVariable(currentVar)
                && !VariableDesc::isCustomVariable(currentVar))
            {
                label = new QLabel(parent);
                return label;
            }
            else
            {
                combo = new QComboBox(parent);
                combo->setEditable(false);
                combo->addItems(VariableDesc::measureTypeStringList());
                combo->setMinimumWidth(130);
                connect(combo, SIGNAL(activated(int)),
                        this, SLOT(commitAndCloseEditor()));
                return combo;
            }
        case VariableModel::INPUTUNIT:
            combo = new QComboBox(parent);
            combo->setEditable(false);

            if (VariableDesc::isVelocityVar(currentVar))
            {
                combo->addItems(VariableDesc::velocityInputUnitStringList());
            }
            else if (VariableDesc::isAngleVar(currentVar))
            {
                combo->addItems(VariableDesc::angleInputUnitStringList());
            }
            else if (VariableDesc::isTemperatureVar(currentVar))
            {
                combo->addItems(VariableDesc::temperatureInputUnitStringList());
            }
            else if (VariableDesc::isPressureVar(currentVar))
            {
                combo->addItems(VariableDesc::pressureInputUnitStringList());
            }
            else if (VariableDesc::isGasVariable(currentVar))
            {
                combo->addItems(VariableDesc::gasInputUnitStringList());
            }
            else if (VariableDesc::isFlowRateVar(currentVar))
            {
                combo->addItems(VariableDesc::flowRateInputUnitStringList());
            }
            else if (VariableDesc::isDiagnosticVar(currentVar))
            {
                combo->addItem(VariableDesc::getVARIABLE_MEASURE_UNIT_STRING_17());
            }
            else if (VariableDesc::isCustomVariable(currentVar))
            {
                combo->addItems(VariableDesc::inputUnitStringList());
            }
            // empty
            else
            {
                combo->addItems(VariableDesc::inputUnitStringList());
            }

            combo->setMaxVisibleItems(20);
            combo->setMinimumWidth(130);
            connect(combo, SIGNAL(activated(int)),
                    this, SLOT(commitAndCloseEditor()));

            return combo;
        case VariableModel::CONVERSIONTYPE:
            if (!VariableDesc::isScalableVariable(currentInputUnit)
                || (VariableDesc::isScalableVariable(currentInputUnit)
                    && VariableDesc::isDiagnosticVar(currentVar)))
            {
                label = new QLabel(parent);
                return label;
            }
            else
            {
                combo = new QComboBox(parent);
                combo->setEditable(false);
                combo->setMaxVisibleItems(20);
                combo->addItems(VariableDesc::conversionTypeStringList());
                combo->setMinimumWidth(130);
                combo->view()->setAlternatingRowColors(true);
                connect(combo, SIGNAL(activated(int)),
                        this, SLOT(commitAndCloseEditor()));
                return combo;
            }
        case VariableModel::OUTPUTUNIT:
            if (!VariableDesc::isScalableVariable(currentInputUnit)
                || (VariableDesc::isScalableVariable(currentInputUnit)
                    && VariableDesc::isDiagnosticVar(currentVar)))
            {
                label = new QLabel(parent);
                return label;
            }
            else
            {
                combo = new QComboBox(parent);
                combo->setEditable(false);

                if (VariableDesc::isVelocityVar(currentVar))
                {
                    combo->addItems(VariableDesc::velocityOutputUnitStringList());
                }
                else if (VariableDesc::isAngleVar(currentVar))
                {
                    combo->addItems(VariableDesc::angleOutputUnitStringList());
                }
                else if (VariableDesc::isTemperatureVar(currentVar))
                {
                    combo->addItems(VariableDesc::temperatureOutputUnitStringList());
                }
                else if (VariableDesc::isPressureVar(currentVar))
                {
                    combo->addItems(VariableDesc::pressureOutputUnitStringList());
                }
                else if (VariableDesc::isGasVariable(currentVar))
                {
                    combo->addItems(VariableDesc::gasOutputUnitStringList());
                }
                else if (VariableDesc::isFlowRateVar(currentVar))
                {
                    combo->addItems(VariableDesc::flowRateOutputUnitStringList());
                }
                else if (VariableDesc::isDiagnosticVar(currentVar))
                {
                    combo->addItem(VariableDesc::getVARIABLE_MEASURE_UNIT_STRING_17());
                }
                else if (VariableDesc::isCustomVariable(currentVar))
                {
                    combo->addItems(VariableDesc::outputUnitStringList());
                }
                // currentVar is empty
                else
                {
                    combo->addItems(VariableDesc::outputUnitStringList());
                }

                combo->setMaxVisibleItems(20);
                combo->setMinimumWidth(130);
                connect(combo, SIGNAL(activated(int)),
                        this, SLOT(commitAndCloseEditor()));
                return combo;
            }
        case VariableModel::AVALUE:
        case VariableModel::BVALUE:
            if (!VariableDesc::isScalableVariable(currentInputUnit)
                || (VariableDesc::isScalableVariable(currentInputUnit)
                    && VariableDesc::isDiagnosticVar(currentVar)))
            {
                label = new QLabel(parent);
                return label;
            }
            else
            {
                auto nzdspin = new NonZeroDoubleSpinBox(parent);
                nzdspin->setDecimals(6);
                nzdspin->setRange(-999999.0, 999999.0);
                nzdspin->setSingleStep(1.0);
                nzdspin->setAccelerated(true);
                connect(nzdspin, SIGNAL(editingFinished()),
                        this, SLOT(commitAndCloseEditor()));
                return nzdspin;
            }
        case VariableModel::NOMTIMELAG:
        case VariableModel::MINTIMELAG:
        case VariableModel::MAXTIMELAG:
            dspin = new QDoubleSpinBox(parent);
            dspin->setDecimals(2);
            dspin->setRange(-100.0, 100.0);
            dspin->setSingleStep(1.0);
            dspin->setAccelerated(true);
            dspin->setSuffix(QStringLiteral(" [s]"));
            connect(dspin, SIGNAL(editingFinished()),
                    this, SLOT(commitAndCloseEditor()));
            return dspin;
        default:
            return QStyledItemDelegate::createEditor(parent, option, index);
    }
}

void VariableDelegate::setEditorData(QWidget* editor,
                                  const QModelIndex& index) const
{
    QLabel *label;
    QComboBox *combo;
    CustomComboBox *custom_combo;
    QDoubleSpinBox *dspin;

    int column = index.column();
    QString currentVar = index.model()->data(index.model()->index(VariableModel::VARIABLE, column)).toString();
    QString currentInputUnit = index.model()->data(index.model()->index(VariableModel::INPUTUNIT, column)).toString();

    QVariant value = index.model()->data(index, Qt::EditRole);
    QString stringValue = value.toString();

    // different kind of editor for each row
    switch (index.row())
    {
        case VariableModel::IGNORE:
        case VariableModel::NUMERIC:
        case VariableModel::INSTRUMENT:
        case VariableModel::INPUTUNIT:
            combo = dynamic_cast<QComboBox*>(editor);
            if (!combo) { return; }

            // prevent empty variables
            if (stringValue.isEmpty()) { break; }

            combo->setCurrentIndex(combo->findText(stringValue));
            break;
        case VariableModel::VARIABLE:
            custom_combo = dynamic_cast<CustomComboBox*>(editor);
            if (!custom_combo) { return; }

            // prevent empty variables
            if (stringValue.isEmpty()) break;

            // skip parent items
            if (stringValue == QLatin1String("Standard Variables")
                or stringValue == QLatin1String("Custom Variables"))
            {
                custom_combo->setCurrentIndex(1);
                break;
            }

            // add the editable item which was selected but not present
            // in the original combo
            if (custom_combo->findText(stringValue) < 0
                and stringValue != QStringLiteral("Standard Variables")
                and stringValue != QStringLiteral("Custom Variables"))
            {
                varsBuffer_->append(stringValue);
                GlobalSettings::setCustomVariableList(*varsBuffer_);
                custom_combo->addChildItem(stringValue);
            }

            custom_combo->setCurrentIndex(custom_combo->findText(stringValue));
            break;
        case VariableModel::MEASURETYPE:
            if (!VariableDesc::isGasVariable(currentVar)
                && !VariableDesc::isCustomVariable(currentVar))
            {
                label = dynamic_cast<QLabel*>(editor);
                if (!label) { return; }
            }
            else
            {
                combo = dynamic_cast<QComboBox*>(editor);
                if (!combo) { return; }

                combo->setCurrentIndex(combo->findText(stringValue));
            }
            break;
        case VariableModel::CONVERSIONTYPE:
        case VariableModel::OUTPUTUNIT:
            if (!VariableDesc::isScalableVariable(currentInputUnit)
                || (VariableDesc::isScalableVariable(currentInputUnit)
                    && VariableDesc::isDiagnosticVar(currentVar)))
            {
                label = dynamic_cast<QLabel*>(editor);
                if (!label) { return; }
            }
            else
            {
                combo = dynamic_cast<QComboBox*>(editor);
                if (!combo) { return; }

                combo->setCurrentIndex(combo->findText(stringValue));
            }
            break;
        case VariableModel::AVALUE:
        case VariableModel::BVALUE:
            if (!VariableDesc::isScalableVariable(currentInputUnit)
                || (VariableDesc::isScalableVariable(currentInputUnit)
                    && VariableDesc::isDiagnosticVar(currentVar)))
            {
                label = dynamic_cast<QLabel*>(editor);
                if (!label) { return; }
            }
            else
            {
                auto nzdspin = dynamic_cast<NonZeroDoubleSpinBox*>(editor);
                if (!nzdspin) { return; }
                nzdspin->setValue(value.toReal());
            }
            break;
        case VariableModel::NOMTIMELAG:
        case VariableModel::MINTIMELAG:
        case VariableModel::MAXTIMELAG:
            dspin = dynamic_cast<QDoubleSpinBox*>(editor);
            if (!dspin) { return; }
            dspin->setValue(value.toReal());
            break;
        default:
            QStyledItemDelegate::setEditorData(editor, index);
            break;
    }
}

void VariableDelegate::setModelData(QWidget* editor, QAbstractItemModel* model,
                                const QModelIndex& index) const
{
    QComboBox *combo;
    QDoubleSpinBox *dspin;
    QVariant value;
    QLabel *label;

    int row = index.row();
    int column = index.column();
    QString currentVar = index.model()->data(index.model()->index(VariableModel::VARIABLE, column)).toString();
    QString currentInputUnit = index.model()->data(index.model()->index(VariableModel::INPUTUNIT, column)).toString();

    // different kind of editor for each row
    switch (row)
    {
        case VariableModel::IGNORE:
        case VariableModel::NUMERIC:
        case VariableModel::INSTRUMENT:
        case VariableModel::INPUTUNIT:
            combo = dynamic_cast<QComboBox*>(editor);
            if (!combo) { return; }
            value = combo->currentText();
            model->setData(index, value);
            break;
        case VariableModel::VARIABLE:
            combo = dynamic_cast<CustomComboBox*>(editor);
            if (!combo) { return; }
            value = combo->currentText();

            // skip parent items
            if (value == QLatin1String("Standard Variables")
                or value == QLatin1String("Custom Variables"))
            {
                value = QStringLiteral("u");
            }

            model->setData(index, value);
        break;
        case VariableModel::CONVERSIONTYPE:
        case VariableModel::OUTPUTUNIT:
            if (!VariableDesc::isScalableVariable(currentInputUnit)
                || (VariableDesc::isScalableVariable(currentInputUnit)
                    && VariableDesc::isDiagnosticVar(currentVar)))
            {
                label = dynamic_cast<QLabel*>(editor);
                if (!label) { return; }
            }
            else
            {
                combo = dynamic_cast<QComboBox*>(editor);
                if (!combo) { return; }
                value = combo->currentText();
                model->setData(index, value);
            }
            break;
        case VariableModel::MEASURETYPE:
            if (!VariableDesc::isGasVariable(currentVar)
                && !VariableDesc::isCustomVariable(currentVar))
            {
                label = dynamic_cast<QLabel*>(editor);
                if (!label) { return; }
            }
            else
            {
                combo = dynamic_cast<QComboBox*>(editor);
                if (!combo) { return; }
                value = combo->currentText();
                model->setData(index, value);
            }
            break;
        case VariableModel::AVALUE:
        case VariableModel::BVALUE:
            if (!VariableDesc::isScalableVariable(currentInputUnit)
                || (VariableDesc::isScalableVariable(currentInputUnit)
                    && VariableDesc::isDiagnosticVar(currentVar)))
            {
                label = dynamic_cast<QLabel*>(editor);
                if (!label) { return; }
            }
            else
            {
                auto nzdspin = dynamic_cast<NonZeroDoubleSpinBox*>(editor);
                if (!nzdspin) { return; }
                value = nzdspin->value();
                model->setData(index, value);
            }
            break;
        case VariableModel::NOMTIMELAG:
        case VariableModel::MINTIMELAG:
        case VariableModel::MAXTIMELAG:
            dspin = dynamic_cast<QDoubleSpinBox*>(editor);
            if (!dspin) { return; }
            value = dspin->value();
            model->setData(index, value);
            break;
        default:
            QStyledItemDelegate::setModelData(editor, model, index);
            break;
    }
}

void VariableDelegate::updateEditorGeometry(QWidget* editor,
                                        const QStyleOptionViewItem& option,
                                        const QModelIndex& index) const
{
    Q_UNUSED(index)

    if (editor) editor->setGeometry(option.rect);
}

void VariableDelegate::clearCustomVariableBuffer()
{
    varsBuffer_.data()->clear();
    GlobalSettings::setCustomVariableList(*varsBuffer_);
}

void VariableDelegate::commitAndCloseEditor()
{
    QWidget* senderWidget = qobject_cast<QWidget *>(sender());

    emit commitData(senderWidget);
    emit closeEditor(senderWidget, QAbstractItemDelegate::NoHint);
}

void VariableDelegate::commitAndCloseEditor(QObject* editor)
{
    emit commitData(qobject_cast<QWidget *>(editor));
    emit closeEditor(qobject_cast<QWidget *>(editor), QAbstractItemDelegate::NoHint);
}

bool VariableDelegate::eventFilter(QObject* editor, QEvent* event)
{
    auto combo = qobject_cast<QComboBox *>(editor);
    QEvent::Type eventType = event->type();

    // NOTE: do not use dynamic_cast<> here, otherwise the app will crash
    int eventKey = static_cast<const QKeyEvent*>(event)->key();

    if (combo
        && (eventType == QEvent::MouseButtonRelease
            || (eventType == QEvent::KeyPress && (eventKey == Qt::Key_Space
                                               || eventKey == Qt::Key_Enter
                                                || eventKey == Qt::Key_Return))))
    {
        if (combo)
        {
            combo->showPopup();
        }
        return true;
    }
    if ((eventType == QEvent::ShortcutOverride && eventKey == Qt::Key_Escape)
             || eventType == QEvent::CloseSoftwareInputPanel)
    {
        commitAndCloseEditor(editor);
        return true;
    }
    return QObject::eventFilter(editor, event);
}
