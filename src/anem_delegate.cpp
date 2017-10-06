/***************************************************************************
  anem_delegate.cpp
  -------------------
  Copyright (C) 2007-2011, Eco2s team, Antonio Forgione
  Copyright (C) 2011-2017, LI-COR Biosciences
  Author: Antonio Forgione

  This file is part of EddyPro (R).

  EddyPro (R) is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  EddyPro (R) is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with EddyPro (R). If not, see <http://www.gnu.org/licenses/>.
****************************************************************************/

#include "anem_delegate.h"

#include <QAbstractItemView>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>

#include "anem_desc.h"
#include "anem_model.h"
#include "defs.h"
#include "dbghelper.h"

AnemDelegate::AnemDelegate(QObject *parent) : QStyledItemDelegate(parent)
{
    installEventFilter(this);
}

AnemDelegate::~AnemDelegate()
{
}

QWidget *AnemDelegate::createEditor(QWidget* parent,
                                     const QStyleOptionViewItem& option,
                                     const QModelIndex& index) const
{
    Q_UNUSED(option)

    QLabel *label;
    QComboBox *combo;
    QDoubleSpinBox *dspin;
    QLineEdit *ledit;
    QString currentManufacturer = index.model()->data(index.model()->index(AnemModel::MANUFACTURER, index.column())).toString();
    QString currentModel = index.model()->data(index.model()->index(AnemModel::MODEL, index.column())).toString();

    // can only edit name on blank column
    if (index.column() >= index.model()->columnCount()) return 0;

    // different kind of editor for each row
    switch (index.row())
    {
      case AnemModel::MANUFACTURER:
          combo = new QComboBox(parent);
          combo->setEditable(false);
          combo->addItems(AnemDesc::manufacturerStringList());
          combo->view()->setTextElideMode(Qt::ElideNone);
          connect(combo, SIGNAL(activated(int)),
                  this, SLOT(commitAndCloseEditor()));
          return combo;
      case AnemModel::MODEL:
          combo = new QComboBox(parent);
          combo->setEditable(false);
          if (currentManufacturer.isEmpty())
          {
              combo->addItems(AnemDesc::allModelStringList());
          }
          else if (currentManufacturer == AnemDesc::getANEM_MANUFACTURER_STRING_0())
          {
              combo->addItems(AnemDesc::campbellModelStringList());
          }
          else if (currentManufacturer == AnemDesc::getANEM_MANUFACTURER_STRING_1())
          {
              combo->addItems(AnemDesc::gillModelStringList());
          }
          else if (currentManufacturer == AnemDesc::getANEM_MANUFACTURER_STRING_2())
          {
              combo->addItems(AnemDesc::metekModelStringList());
          }
          else if (currentManufacturer == AnemDesc::getANEM_MANUFACTURER_STRING_3())
          {
              combo->addItems(AnemDesc::youngModelStringList());
          }
          else if (currentManufacturer == AnemDesc::getANEM_MANUFACTURER_STRING_4())
          {
              combo->addItems(AnemDesc::otherModelStringList());
          }
          combo->view()->setTextElideMode(Qt::ElideNone);
          combo->setMaxVisibleItems(15);
          connect(combo, SIGNAL(activated(int)),
                  this, SLOT(commitAndCloseEditor()));
          return combo;
      case AnemModel::SWVERSION:
          ledit = new QLineEdit(parent);
          ledit->setPlaceholderText(QStringLiteral("2329-660-01"));
          connect(ledit, SIGNAL(editingFinished()),
                  this, SLOT(commitAndCloseEditor()));
          return ledit;
      case AnemModel::ID:
          ledit = new QLineEdit(parent);
          ledit->setPlaceholderText(QStringLiteral("Alphanumeric ID"));
          connect(ledit, SIGNAL(editingFinished()),
                  this, SLOT(commitAndCloseEditor()));
          return ledit;
      case AnemModel::HEIGHT:
          dspin = new QDoubleSpinBox(parent);
          dspin->setDecimals(2);
          dspin->setRange(0.1, 500.0);
          dspin->setSingleStep(1.0);
          dspin->setAccelerated(true);
          dspin->setSuffix(QStringLiteral(" [m]"));
          connect(dspin, SIGNAL(editingFinished()),
                  this, SLOT(commitAndCloseEditor()));
          return dspin;
      case AnemModel::WINDFORMAT:
          combo = new QComboBox(parent);
          combo->setEditable(false);
          if (currentManufacturer.isEmpty()
              || currentManufacturer == AnemDesc::getANEM_MANUFACTURER_STRING_1())
          {
              combo->addItems(AnemDesc::allWindFormatStringList());
          }
          else if (currentManufacturer == AnemDesc::getANEM_MANUFACTURER_STRING_0())
          {
              combo->addItems(AnemDesc::simplestWindFormatStringList());
          }
          else
          {
              combo->addItems(AnemDesc::commonWindFormatStringList());
          }
          combo->view()->setTextElideMode(Qt::ElideNone);
          connect(combo, SIGNAL(activated(int)),
                  this, SLOT(commitAndCloseEditor()));
          return combo;
      case AnemModel::NORTHALIGNMENT:
          combo = new QComboBox(parent);
          combo->setEditable(false);
          if (currentManufacturer.isEmpty())
          {
              combo->addItems(AnemDesc::allNorthAlignmentStringList());
          }
          // gill
          else if (currentManufacturer == AnemDesc::getANEM_MANUFACTURER_STRING_1())
          {
              combo->addItems(AnemDesc::gillNorthAlignmentStringList());
          }
          else
          {
              combo->addItems(AnemDesc::naNorthAlignmentStringList());
          }
          combo->view()->setTextElideMode(Qt::ElideNone);
          connect(combo, SIGNAL(activated(int)),
                  this, SLOT(commitAndCloseEditor()));
          return combo;
      case AnemModel::NORTHOFFSET:
          dspin = new QDoubleSpinBox(parent);
          dspin->setDecimals(1);
          dspin->setRange(0.0, 360.0);
          dspin->setSingleStep(1.0);
          dspin->setAccelerated(true);
          dspin->setSuffix(tr("  [%1]", "Degrees").arg(Defs::DEGREE));
          connect(dspin, SIGNAL(editingFinished()),
                  this, SLOT(commitAndCloseEditor()));
          return dspin;
      case AnemModel::NSEPARATION:
      case AnemModel::ESEPARATION:
      case AnemModel::VSEPARATION:
          if (index.column() == 0)
          {
              label = new QLabel(parent);
              return label;
          }
          else
          {
              dspin = new QDoubleSpinBox(parent);
              dspin->setDecimals(2);
              dspin->setRange(-1000.0, 1000.0);
              dspin->setSingleStep(1.0);
              dspin->setAccelerated(true);
              dspin->setSuffix(QStringLiteral(" [cm]"));
              connect(dspin, SIGNAL(editingFinished()),
                      this, SLOT(commitAndCloseEditor()));
              return dspin;
          }
      case AnemModel::VPATHLENGTH:
      case AnemModel::HPATHLENGTH:
        if (currentModel != AnemDesc::getANEM_MODEL_STRING_12())
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
      case AnemModel::TAU:
        if (currentModel != AnemDesc::getANEM_MODEL_STRING_12())
        {
            label = new QLabel(parent);
            return label;
        }
        else
        {
          dspin = new QDoubleSpinBox(parent);
          dspin->setDecimals(4);
          dspin->setRange(0.001, 1000.0);
          dspin->setSingleStep(1.0);
          dspin->setAccelerated(true);
          dspin->setSuffix(QStringLiteral(" [s]"));
          connect(dspin, SIGNAL(editingFinished()),
                  this, SLOT(commitAndCloseEditor()));
          return dspin;
        }
        default:
          return 0;
    }
}

void AnemDelegate::setEditorData(QWidget* editor,
                                  const QModelIndex& index) const
{
    QComboBox *combo;
    QDoubleSpinBox *dspin;
    QLineEdit *ledit;
    QLabel *label;
    QString currentModel = index.model()->data(index.model()->index(AnemModel::MODEL, index.column())).toString();
    QVariant value = index.model()->data(index, Qt::EditRole);

    // different kind of editor for each row
    switch (index.row())
    {
        case AnemModel::MANUFACTURER:
        case AnemModel::MODEL:
        case AnemModel::WINDFORMAT:
        case AnemModel::NORTHALIGNMENT:
            combo = static_cast<QComboBox*>(editor);
            if (!combo) { return; }
            combo->setCurrentIndex(combo->findText(value.toString()));
            break;
        case AnemModel::SWVERSION:
              ledit = static_cast<QLineEdit*>(editor);
              if (!ledit) { return; }
              ledit->setText(value.toString());
              break;
        case AnemModel::ID:
            ledit = static_cast<QLineEdit*>(editor);
            if (!ledit) { return; }
            ledit->setText(value.toString());
            break;
        case AnemModel::HEIGHT:
        case AnemModel::NORTHOFFSET:
            dspin = static_cast<QDoubleSpinBox*>(editor);
            if (!dspin) { return; }
            dspin->setValue(value.toReal());
            break;
        case AnemModel::VPATHLENGTH:
        case AnemModel::HPATHLENGTH:
        case AnemModel::TAU:
            if (currentModel != AnemDesc::getANEM_MODEL_STRING_12())
            {
                label = static_cast<QLabel*>(editor);
                if (!label) { return; }
            }
            else
            {
                dspin = static_cast<QDoubleSpinBox*>(editor);
                if (!dspin) { return; }
                dspin->setValue(value.toReal());
            }
            break;
        case AnemModel::ESEPARATION:
        case AnemModel::VSEPARATION:
        case AnemModel::NSEPARATION:
            if (index.column() == 0)
            {
                label = static_cast<QLabel*>(editor);
                if (!label) { return; }
            }
            else
            {
                dspin = static_cast<QDoubleSpinBox*>(editor);
                if (!dspin) { return; }
                dspin->setValue(value.toReal());
            }
            break;
        default:
            QStyledItemDelegate::setEditorData(editor, index);
            break;
    }
}

void AnemDelegate::setModelData(QWidget* editor, QAbstractItemModel* model,
                                const QModelIndex& index) const
{
    QComboBox *combo;
    QDoubleSpinBox *dspin;
    QLineEdit *ledit;
    QVariant value;

    // different kind of editor for each row
    switch (index.row())
    {
        case AnemModel::MANUFACTURER:
        case AnemModel::MODEL:
        case AnemModel::WINDFORMAT:
        case AnemModel::NORTHALIGNMENT:
            combo = static_cast<QComboBox*>(editor);
            if (!combo) { return; }
            value = combo->currentText();
            model->setData(index, value);
            break;
        case AnemModel::SWVERSION:
            ledit = static_cast<QLineEdit*>(editor);
            if (!ledit) { return; }
            value = ledit->text();
            model->setData(index, value);
            break;
        case AnemModel::ID:
            ledit = static_cast<QLineEdit*>(editor);
            if (!ledit) { return; }
            value = ledit->text();
            model->setData(index, value);
            break;
        case AnemModel::HEIGHT:
        case AnemModel::NORTHOFFSET:
            dspin = static_cast<QDoubleSpinBox*>(editor);
            if (!dspin) { return; }
            value = dspin->value();
            model->setData(index, value);
            break;
        case AnemModel::VPATHLENGTH:
        case AnemModel::HPATHLENGTH:
        case AnemModel::TAU:
            dspin = static_cast<QDoubleSpinBox*>(editor);
            if (!dspin) { return; }
            value = dspin->value();
            model->setData(index, value);
            break;
        case AnemModel::NSEPARATION:
        case AnemModel::ESEPARATION:
        case AnemModel::VSEPARATION:
            dspin = static_cast<QDoubleSpinBox*>(editor);
            if (!dspin) { return; }
            value = dspin->value();
            model->setData(index, value);
            break;
        default:
            QStyledItemDelegate::setModelData(editor, model, index);
            break;
    }
}

void AnemDelegate::updateEditorGeometry(QWidget* editor,
                                        const QStyleOptionViewItem& option,
                                        const QModelIndex& index) const
{
    Q_UNUSED(index)
    if (editor) editor->setGeometry(option.rect);
}

void AnemDelegate::commitAndCloseEditor()
{
    QWidget* senderWidget = qobject_cast<QWidget *>(sender());

    emit commitData(senderWidget);
    emit closeEditor(senderWidget, QAbstractItemDelegate::NoHint);
}

void AnemDelegate::commitAndCloseEditor(QObject* editor)
{
    emit commitData(qobject_cast<QWidget *>(editor));
    emit closeEditor(qobject_cast<QWidget *>(editor), QAbstractItemDelegate::NoHint);
}

bool AnemDelegate::eventFilter(QObject* editor, QEvent* event)
{
    QComboBox* combo = qobject_cast<QComboBox *>(editor);
    QEvent::Type eventType = event->type();
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
    else if ((eventType == QEvent::ShortcutOverride && eventKey == Qt::Key_Escape)
             || eventType == QEvent::CloseSoftwareInputPanel)
    {
        commitAndCloseEditor(editor);
        return true;
    }
    else
    {
        return QObject::eventFilter(editor, event);
    }
}
