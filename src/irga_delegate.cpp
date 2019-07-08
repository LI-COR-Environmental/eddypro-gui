/***************************************************************************
  irga_delegate.cpp
  -----------------
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

#include "irga_delegate.h"

#include <QAbstractItemView>
#include <QComboBox>
#include <QDebug>
#include <QDoubleSpinBox>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>

#include "defs.h"
#include "irga_desc.h"
#include "irga_model.h"

IrgaDelegate::IrgaDelegate(QObject *parent) : QStyledItemDelegate(parent)
{
    installEventFilter(this);
}

QWidget *IrgaDelegate::createEditor(QWidget* parent,
                                     const QStyleOptionViewItem& option,
                                     const QModelIndex& index) const
{
    Q_UNUSED(option)

    QLabel *label;
    QComboBox *combo;
    QDoubleSpinBox *dspin;
    QLineEdit *ledit;
    QString currentManufacturer = index.model()->data(index.model()->index(IrgaModel::MANUFACTURER, index.column())).toString();
    QString currentModel = index.model()->data(index.model()->index(IrgaModel::MODEL, index.column())).toString();

    // can only edit name on blank column
    if (index.column() >= index.model()->columnCount()) return nullptr;

    // different kind of editor for each row
    switch (index.row())
    {
      case IrgaModel::MANUFACTURER:
          combo = new QComboBox(parent);
          combo->setEditable(false);
          combo->addItems(IrgaDesc::manufacturerStringList());
          combo->view()->setTextElideMode(Qt::ElideNone);
          connect(combo, SIGNAL(activated(int)),
                  this, SLOT(commitAndCloseEditor()));
          return combo;
      case IrgaModel::MODEL:
          combo = new QComboBox(parent);
          combo->setEditable(false);
          if (currentManufacturer.isEmpty())
          {
              combo->addItems(IrgaDesc::allModelStringList());
          }
          else if (currentManufacturer == IrgaDesc::getIRGA_MANUFACTURER_STRING_0())
          {
              combo->addItems(IrgaDesc::licorModelStringList());
          }
          else if (currentManufacturer == IrgaDesc::getIRGA_MANUFACTURER_STRING_1())
          {
              combo->addItems(IrgaDesc::otherModelStringList());
          }
          combo->view()->setTextElideMode(Qt::ElideNone);
          combo->setMaxVisibleItems(15);
          connect(combo, SIGNAL(activated(int)),
                  this, SLOT(commitAndCloseEditor()));
          return combo;
      case IrgaModel::SWVERSION:
            ledit = new QLineEdit(parent);
            ledit->setPlaceholderText(QStringLiteral("8.0.0"));
            connect(ledit, SIGNAL(editingFinished()),
                    this, SLOT(commitAndCloseEditor()));
            return ledit;
      case IrgaModel::ID:
            ledit = new QLineEdit(parent);
            ledit->setPlaceholderText(QStringLiteral("Alphanumeric ID"));
            connect(ledit, SIGNAL(editingFinished()),
                  this, SLOT(commitAndCloseEditor()));
            return ledit;
      case IrgaModel::TUBELENGTH:
            if (IrgaDesc::isOpenPathModel(currentModel))
            {
                label = new QLabel(parent);
                return label;
            }
            else
            {
                dspin = new QDoubleSpinBox(parent);
                dspin->setDecimals(1);
                dspin->setRange(0.0, 9999.0);
                dspin->setSingleStep(1.0);
                dspin->setAccelerated(true);
                dspin->setSuffix(QStringLiteral(" [cm]"));
                connect(dspin, SIGNAL(editingFinished()),
                      this, SLOT(commitAndCloseEditor()));
                return dspin;
            }
      case IrgaModel::TUBEDIAMETER:
            if (IrgaDesc::isOpenPathModel(currentModel))
            {
                label = new QLabel(parent);
                return label;
            }
            else
            {
              dspin = new QDoubleSpinBox(parent);
              dspin->setDecimals(1);
              dspin->setRange(0.0, 99.0);
              dspin->setSingleStep(1.0);
              dspin->setAccelerated(true);
              dspin->setSuffix(QStringLiteral(" [mm]"));
              connect(dspin, SIGNAL(editingFinished()),
                      this, SLOT(commitAndCloseEditor()));
              return dspin;
            }
      case IrgaModel::TUBEFLOWRATE:
            if (IrgaDesc::isOpenPathModel(currentModel))
            {
                label = new QLabel(parent);
                return label;
            }
            else
            {
              dspin = new QDoubleSpinBox(parent);
              dspin->setDecimals(1);
              dspin->setRange(0.0, 100.0);
              dspin->setSingleStep(1.0);
              dspin->setAccelerated(true);
              dspin->setSuffix(QStringLiteral(" [l/m]"));
              connect(dspin, SIGNAL(editingFinished()),
                      this, SLOT(commitAndCloseEditor()));
              return dspin;
            }
      case IrgaModel::TUBENSEPARATION:
      case IrgaModel::TUBEESEPARATION:
      case IrgaModel::TUBEVSEPARATION:
          dspin = new QDoubleSpinBox(parent);
          dspin->setDecimals(2);
          dspin->setRange(-1000.0, 1000.0);
          dspin->setSingleStep(1.0);
          dspin->setAccelerated(true);
          dspin->setSuffix(QStringLiteral(" [cm]"));
          connect(dspin, SIGNAL(editingFinished()),
                  this, SLOT(commitAndCloseEditor()));
          return dspin;
        case IrgaModel::VPATHLENGTH:
        case IrgaModel::HPATHLENGTH:
            if (currentModel != IrgaDesc::getIRGA_MODEL_STRING_6()
                && currentModel != IrgaDesc::getIRGA_MODEL_STRING_7()
                && currentModel != IrgaDesc::getIRGA_MODEL_STRING_8()
                && currentModel != IrgaDesc::getIRGA_MODEL_STRING_9()
                && currentModel != IrgaDesc::getIRGA_MODEL_STRING_10()
                && currentModel != IrgaDesc::getIRGA_MODEL_STRING_11())
            {
                label = new QLabel(parent);
                return label;
            }
            else
            {
                dspin = new QDoubleSpinBox(parent);
                dspin->setDecimals(4);
                dspin->setRange(0.0001, 1000.0);
                dspin->setSingleStep(1.0);
                dspin->setAccelerated(true);
                dspin->setSuffix(QStringLiteral(" [cm]"));
                connect(dspin, SIGNAL(editingFinished()),
                        this, SLOT(commitAndCloseEditor()));
                return dspin;
            }
        case IrgaModel::TAU:
            if (currentModel != IrgaDesc::getIRGA_MODEL_STRING_6()
                && currentModel != IrgaDesc::getIRGA_MODEL_STRING_7()
                && currentModel != IrgaDesc::getIRGA_MODEL_STRING_8()
                && currentModel != IrgaDesc::getIRGA_MODEL_STRING_9()
                && currentModel != IrgaDesc::getIRGA_MODEL_STRING_10()
                && currentModel != IrgaDesc::getIRGA_MODEL_STRING_11())
            {
                label = new QLabel(parent);
                return label;
            }
            else
            {
                dspin = new QDoubleSpinBox(parent);
                dspin->setDecimals(4);
                dspin->setRange(0.0001, 500.0);
                dspin->setSingleStep(1.0);
                dspin->setAccelerated(true);
                dspin->setSuffix(QStringLiteral(" [s]"));
                connect(dspin, SIGNAL(editingFinished()),
                        this, SLOT(commitAndCloseEditor()));
                return dspin;
            }
        case IrgaModel::KWATER:
        case IrgaModel::KOXYGEN:
            if (currentModel != IrgaDesc::getIRGA_MODEL_STRING_8()
                && currentModel != IrgaDesc::getIRGA_MODEL_STRING_9()
                && currentModel != IrgaDesc::getIRGA_MODEL_STRING_10()
                && currentModel != IrgaDesc::getIRGA_MODEL_STRING_11())
            {
                label = new QLabel(parent);
                return label;
            }
            else
            {
                dspin = new QDoubleSpinBox(parent);
                dspin->setDecimals(6);
                dspin->setRange(0.000001, 1.0);
                dspin->setSingleStep(1.0);
                dspin->setAccelerated(true);
                dspin->setSuffix(QStringLiteral(" [") + Defs::M3_G_CM_STRING + QStringLiteral("]"));
                connect(dspin, SIGNAL(editingFinished()),
                        this, SLOT(commitAndCloseEditor()));
                return dspin;
            }
          default:
              return QStyledItemDelegate::createEditor(parent, option, index);
    }
}

void IrgaDelegate::setEditorData(QWidget* editor,
                                  const QModelIndex& index) const
{
    QComboBox *combo;
    QDoubleSpinBox *dspin;
    QLineEdit *ledit;
    QLabel *label;

    QString currentModel = index.model()->data(index.model()->index(IrgaModel::MODEL, index.column())).toString();
    QVariant value = index.model()->data(index, Qt::EditRole);

    // different kind of editor for each row
    switch (index.row())
    {
        case IrgaModel::MANUFACTURER:
        case IrgaModel::MODEL:
              combo = dynamic_cast<QComboBox*>(editor);
              if (!combo) { return; }
              combo->setCurrentIndex(combo->findText(value.toString()));
              break;
        case IrgaModel::SWVERSION:
              ledit = dynamic_cast<QLineEdit*>(editor);
              if (!ledit) { return; }
              ledit->setText(value.toString());
              break;
        case IrgaModel::ID:
            ledit = dynamic_cast<QLineEdit*>(editor);
            if (!ledit) { return; }
            ledit->setText(value.toString());
            break;
        case IrgaModel::VPATHLENGTH:
        case IrgaModel::HPATHLENGTH:
        case IrgaModel::TAU:
            if (currentModel != IrgaDesc::getIRGA_MODEL_STRING_6()
                && currentModel != IrgaDesc::getIRGA_MODEL_STRING_7()
                && currentModel != IrgaDesc::getIRGA_MODEL_STRING_8()
                && currentModel != IrgaDesc::getIRGA_MODEL_STRING_9()
                && currentModel != IrgaDesc::getIRGA_MODEL_STRING_10()
                && currentModel != IrgaDesc::getIRGA_MODEL_STRING_11())
            {
                label = dynamic_cast<QLabel*>(editor);
                if (!label) { return; }
            }
            else
            {
                dspin = dynamic_cast<QDoubleSpinBox*>(editor);
                if (!dspin) { return; }
                dspin->setValue(value.toReal());
            }
            break;
        case IrgaModel::TUBELENGTH:
        case IrgaModel::TUBEDIAMETER:
        case IrgaModel::TUBEFLOWRATE:
            if (IrgaDesc::isOpenPathModel(currentModel))
            {
                label = dynamic_cast<QLabel*>(editor);
                if (!label) { return; }
            }
            else
            {
                dspin = dynamic_cast<QDoubleSpinBox*>(editor);
                if (!dspin) { return; }
                dspin->setValue(value.toReal());
            }
            break;
        case IrgaModel::TUBENSEPARATION:
        case IrgaModel::TUBEESEPARATION:
        case IrgaModel::TUBEVSEPARATION:
            dspin = dynamic_cast<QDoubleSpinBox*>(editor);
            if (!dspin) { return; }
            dspin->setValue(value.toReal());
            break;
        case IrgaModel::KWATER:
        case IrgaModel::KOXYGEN:
            if (currentModel != IrgaDesc::getIRGA_MODEL_STRING_8()
                && currentModel != IrgaDesc::getIRGA_MODEL_STRING_9()
                && currentModel != IrgaDesc::getIRGA_MODEL_STRING_10()
                && currentModel != IrgaDesc::getIRGA_MODEL_STRING_11())
            {
                label = dynamic_cast<QLabel*>(editor);
                if (!label) { return; }
            }
            else
            {
                dspin = dynamic_cast<QDoubleSpinBox*>(editor);
                if (!dspin) { return; }
                dspin->setValue(value.toReal());
            }
            break;
        default:
            QStyledItemDelegate::setEditorData(editor, index);
            break;
    }
}

void IrgaDelegate::setModelData(QWidget* editor, QAbstractItemModel* model,
                                const QModelIndex& index) const
{
    QComboBox *combo;
    QDoubleSpinBox *dspin;
    QLineEdit *ledit;
    QVariant value;
    QLabel *label;

    QString currentModel = index.model()->data(index.model()->index(IrgaModel::MODEL, index.column())).toString();

    // different kind of editor for each row
    switch (index.row())
    {
        case IrgaModel::MANUFACTURER:
        case IrgaModel::MODEL:
            combo = dynamic_cast<QComboBox*>(editor);
            if (!combo) { return; }
            value = combo->currentText();
            model->setData(index, value);
            break;
        case IrgaModel::SWVERSION:
            ledit = dynamic_cast<QLineEdit*>(editor);
            if (!ledit) { return; }
            value = ledit->text();
            model->setData(index, value);
            break;
        case IrgaModel::ID:
            ledit = dynamic_cast<QLineEdit*>(editor);
            if (!ledit) { return; }
            value = ledit->text();
            model->setData(index, value);
            break;
        case IrgaModel::VPATHLENGTH:
        case IrgaModel::HPATHLENGTH:
        case IrgaModel::TAU:
            if (currentModel != IrgaDesc::getIRGA_MODEL_STRING_6()
                && currentModel != IrgaDesc::getIRGA_MODEL_STRING_7()
                && currentModel != IrgaDesc::getIRGA_MODEL_STRING_8()
                && currentModel != IrgaDesc::getIRGA_MODEL_STRING_9()
                && currentModel != IrgaDesc::getIRGA_MODEL_STRING_10()
                && currentModel != IrgaDesc::getIRGA_MODEL_STRING_11())
            {
                label = dynamic_cast<QLabel*>(editor);
                if (!label) { return; }
            }
            else
            {
                dspin = dynamic_cast<QDoubleSpinBox*>(editor);
                if (!dspin) { return; }
                value = dspin->value();
                model->setData(index, value);
            }
            break;
        case IrgaModel::TUBELENGTH:
        case IrgaModel::TUBEDIAMETER:
        case IrgaModel::TUBEFLOWRATE:
            if (IrgaDesc::isOpenPathModel(currentModel))
            {
                label = dynamic_cast<QLabel*>(editor);
                if (!label) { return; }
            }
            else
            {
                dspin = dynamic_cast<QDoubleSpinBox*>(editor);
                if (!dspin) { return; }
                value = dspin->value();
                model->setData(index, value);
            }
            break;
        case IrgaModel::TUBENSEPARATION:
        case IrgaModel::TUBEESEPARATION:
        case IrgaModel::TUBEVSEPARATION:
            dspin = dynamic_cast<QDoubleSpinBox*>(editor);
            if (!dspin) { return; }
            value = dspin->value();
            model->setData(index, value);
            break;
        case IrgaModel::KWATER:
        case IrgaModel::KOXYGEN:
            if (currentModel != IrgaDesc::getIRGA_MODEL_STRING_8()
                && currentModel != IrgaDesc::getIRGA_MODEL_STRING_9()
                && currentModel != IrgaDesc::getIRGA_MODEL_STRING_10()
                && currentModel != IrgaDesc::getIRGA_MODEL_STRING_11())
            {
                label = dynamic_cast<QLabel*>(editor);
                if (!label) { return; }
            }
            else
            {
                dspin = dynamic_cast<QDoubleSpinBox*>(editor);
                if (!dspin) { return; }
                value = dspin->value();
                model->setData(index, value);
            }
            break;
        default:
            QStyledItemDelegate::setModelData(editor, model, index);
            break;
    }
}

void IrgaDelegate::updateEditorGeometry(QWidget* editor,
                                        const QStyleOptionViewItem& option,
                                        const QModelIndex& index) const
{
    Q_UNUSED(index)

    if (editor) editor->setGeometry(option.rect);
}

void IrgaDelegate::commitAndCloseEditor()
{
    QWidget* senderWidget = qobject_cast<QWidget *>(sender());

    emit commitData(senderWidget);
    emit closeEditor(senderWidget, QAbstractItemDelegate::NoHint);
}

void IrgaDelegate::commitAndCloseEditor(QObject* editor)
{
    emit commitData(qobject_cast<QWidget *>(editor));
    emit closeEditor(qobject_cast<QWidget *>(editor), QAbstractItemDelegate::NoHint);
}

bool IrgaDelegate::eventFilter(QObject* editor, QEvent* event)
{
    auto combo = qobject_cast<QComboBox *>(editor);
    QEvent::Type eventType = event->type();
    // NOTE: do not use dynamic_cast<> here, otherwise the app will crash
    int eventKey = static_cast<QKeyEvent*>(event)->key();
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
