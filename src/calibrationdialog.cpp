#include "calibrationdialog.h"

#include <limits>

#include <QDateTimeEdit>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QPushButton>
#include <QGridLayout>

#include "QScienceSpinBox.h"

#include "configstate.h"
#include "dbghelper.h"
#include "ecprojectstate.h"
#include "widget_utils.h"

CalibrationDialog::CalibrationDialog(QWidget* parent, EcProject *ecProject, ConfigState* config) :
    QDialog(parent),
    ecProject_(ecProject),
    configState_(config)
{
    setWindowModality(Qt::WindowModal);
    setWindowTitle(tr("Calibration Values"));
    WidgetUtils::removeContextHelpButton(this);

    auto calibDateTitle = WidgetUtils::createBlueLabel(this, tr("Calibration date"));

    calibDate = new QDateTimeEdit;
    calibDate->setDisplayFormat(QStringLiteral("yyyy-MM-dd HH:mm:ss"));
    calibDate->setDate(QDate::currentDate());
    calibDate->setTime(QTime::currentTime());

    auto co2ValuesTitle = WidgetUtils::createBlueLabel(this,
                            tr("%1 calibration values").arg(Defs::CO2_STRING));

    auto co2_A_title = new QLabel;
    co2_A_title->setText(QStringLiteral("A :"));
    auto co2_B_title = new QLabel;
    co2_B_title->setText(QStringLiteral("B :"));
    auto co2_C_title = new QLabel;
    co2_C_title->setText(QStringLiteral("C :"));
    auto co2_D_title = new QLabel;
    co2_D_title->setText(QStringLiteral("D :"));
    auto co2_E_title = new QLabel;
    co2_E_title->setText(QStringLiteral("E :"));
    auto co2_XS_title = new QLabel;
    co2_XS_title->setText(QStringLiteral("XS :"));
    auto co2_Z_title = new QLabel;
    co2_Z_title->setText(QStringLiteral("Z :"));

    co2_A_spinbox = WidgetUtils::createCalibrationSpinbox(this);
    co2_B_spinbox = WidgetUtils::createCalibrationSpinbox(this);
    co2_C_spinbox = WidgetUtils::createCalibrationSpinbox(this);
    co2_D_spinbox = WidgetUtils::createCalibrationSpinbox(this);
    co2_E_spinbox = WidgetUtils::createCalibrationSpinbox(this);
    co2_XS_spinbox = WidgetUtils::createCalibrationSpinbox(this);
    co2_Z_spinbox = WidgetUtils::createCalibrationSpinbox(this);

    auto h2oValuesTitle = WidgetUtils::createBlueLabel(this,
                            tr("%1 calibration values").arg(Defs::H2O_STRING));

    auto h2o_A_title = new QLabel;
    h2o_A_title->setText(QStringLiteral("A :"));
    auto h2o_B_title = new QLabel;
    h2o_B_title->setText(QStringLiteral("B :"));
    auto h2o_C_title = new QLabel;
    h2o_C_title->setText(QStringLiteral("C :"));
    auto h2o_XS_title = new QLabel;
    h2o_XS_title->setText(QStringLiteral("XS :"));
    auto h2o_Z_title = new QLabel;
    h2o_Z_title->setText(QStringLiteral("Z :"));

    h2o_A_spinbox = WidgetUtils::createCalibrationSpinbox(this);
    h2o_B_spinbox = WidgetUtils::createCalibrationSpinbox(this);
    h2o_C_spinbox = WidgetUtils::createCalibrationSpinbox(this);
    h2o_XS_spinbox = WidgetUtils::createCalibrationSpinbox(this);
    h2o_Z_spinbox = WidgetUtils::createCalibrationSpinbox(this);

    auto zeroSpanDateTitle = WidgetUtils::createBlueLabel(this, tr("Zero/Span date"));

    zeroDate = new QDateTimeEdit;
    zeroDate->setDisplayFormat(QStringLiteral("yyyy-MM-dd HH:mm:ss"));
    zeroDate->setDate(QDate::currentDate());
    zeroDate->setTime(QTime::currentTime());

    auto zeroSpanValuesTitle = WidgetUtils::createBlueLabel(this, tr("Zero/Span values"));

    auto co2_Zero_title = new QLabel;
    co2_Zero_title->setText(QStringLiteral("%1 Zero :").arg(Defs::CO2_STRING));
    auto co2_Span_title = new QLabel;
    co2_Span_title->setText(QStringLiteral("%1 Span :").arg(Defs::CO2_STRING));
    auto co2_SpanTarget_title = new QLabel;
    co2_SpanTarget_title->setText(QStringLiteral("at :"));
    auto co2_Span2_title = new QLabel;
    co2_Span2_title->setText(QStringLiteral("%1 Span 2 :").arg(Defs::CO2_STRING));
    auto co2_Span2Target_title = new QLabel;
    co2_Span2Target_title->setText(QStringLiteral("at :"));
    auto co2_CX_title = new QLabel;
    co2_CX_title->setText(QStringLiteral("CX :"));

    co2_Zero_spinbox = WidgetUtils::createCalibrationSpinbox(this);
    co2_Span_spinbox = WidgetUtils::createCalibrationSpinbox(this);
    co2_SpanTarget_spinbox = new QDoubleSpinBox;
    co2_SpanTarget_spinbox->setSuffix(QStringLiteral(" [ppm]"));

    co2_Span2_spinbox = WidgetUtils::createCalibrationSpinbox(this);
    co2_Span2Target_spinbox = new QDoubleSpinBox;
    co2_Span2Target_spinbox->setSuffix(QStringLiteral(" [ppm]"));
    co2_CX_spinbox = WidgetUtils::createCalibrationSpinbox(this);

    auto h2o_Zero_title = new QLabel;
    h2o_Zero_title->setText(QStringLiteral("%1 Zero :").arg(Defs::H2O_STRING));
    auto h2o_Span_title = new QLabel;
    h2o_Span_title->setText(QStringLiteral("%1 Span :").arg(Defs::H2O_STRING));
    auto h2o_SpanTarget_title = new QLabel;
    h2o_SpanTarget_title->setText(QStringLiteral("at :"));
    auto h2o_Span2_title = new QLabel;
    h2o_Span2_title->setText(QStringLiteral("%1 Span 2 :").arg(Defs::H2O_STRING));
    auto h2o_Span2Target_title = new QLabel;
    h2o_Span2Target_title->setText(QStringLiteral("at :"));
    auto h2o_WX_title = new QLabel;
    h2o_WX_title->setText(QStringLiteral("WX :"));

    h2o_Zero_spinbox = WidgetUtils::createCalibrationSpinbox(this);
    h2o_Span_spinbox = WidgetUtils::createCalibrationSpinbox(this);
    h2o_SpanTarget_spinbox = new QDoubleSpinBox;
    h2o_SpanTarget_spinbox->setSuffix(QStringLiteral(" [%1]").arg(Defs::DEGREE_C));
    h2o_Span2_spinbox = WidgetUtils::createCalibrationSpinbox(this);
    h2o_Span2Target_spinbox = new QDoubleSpinBox;
    h2o_Span2Target_spinbox->setSuffix(QStringLiteral(" [%1]").arg(Defs::DEGREE_C));
    h2o_WX_spinbox = WidgetUtils::createCalibrationSpinbox(this);

    auto okButton = WidgetUtils::createCommonButton(this, tr("Ok"));

    auto mainLayout = new QGridLayout(this);
    mainLayout->addWidget(calibDateTitle, 0, 0);
    mainLayout->addWidget(calibDate, 0, 1, 1, 2);

    mainLayout->addWidget(co2ValuesTitle, 1, 0, 1, 2);
    mainLayout->addWidget(co2_A_title, 2, 0, 1, 1, Qt::AlignRight);
    mainLayout->addWidget(co2_A_spinbox, 2, 1);
    mainLayout->addWidget(co2_B_title, 3, 0, 1, 1, Qt::AlignRight);
    mainLayout->addWidget(co2_B_spinbox, 3, 1);
    mainLayout->addWidget(co2_C_title, 4, 0, 1, 1, Qt::AlignRight);
    mainLayout->addWidget(co2_C_spinbox, 4, 1);
    mainLayout->addWidget(co2_D_title, 5, 0, 1, 1, Qt::AlignRight);
    mainLayout->addWidget(co2_D_spinbox, 5, 1);
    mainLayout->addWidget(co2_E_title, 6, 0, 1, 1, Qt::AlignRight);
    mainLayout->addWidget(co2_E_spinbox, 6, 1);
    mainLayout->addWidget(co2_XS_title, 7, 0, 1, 1, Qt::AlignRight);
    mainLayout->addWidget(co2_XS_spinbox, 7, 1);
    mainLayout->addWidget(co2_Z_title, 8, 0, 1, 1, Qt::AlignRight);
    mainLayout->addWidget(co2_Z_spinbox, 8, 1);

    mainLayout->addWidget(h2oValuesTitle, 1, 4, 1, 2);
    mainLayout->addWidget(h2o_A_title, 2, 4, 1, 1, Qt::AlignRight);
    mainLayout->addWidget(h2o_A_spinbox, 2, 5);
    mainLayout->addWidget(h2o_B_title, 3, 4, 1, 1, Qt::AlignRight);
    mainLayout->addWidget(h2o_B_spinbox, 3, 5);
    mainLayout->addWidget(h2o_C_title, 4, 4, 1, 1, Qt::AlignRight);
    mainLayout->addWidget(h2o_C_spinbox, 4, 5);
    mainLayout->addWidget(h2o_XS_title, 5, 4, 1, 1, Qt::AlignRight);
    mainLayout->addWidget(h2o_XS_spinbox, 5, 5);
    mainLayout->addWidget(h2o_Z_title, 6, 4, 1, 1, Qt::AlignRight);
    mainLayout->addWidget(h2o_Z_spinbox, 6, 5);

    mainLayout->addWidget(zeroSpanDateTitle, 10, 0);
    mainLayout->addWidget(zeroDate, 10, 1, 1, 2);

    mainLayout->addWidget(zeroSpanValuesTitle, 12, 0);
    mainLayout->addWidget(co2_Zero_title, 13, 0, 1, 1, Qt::AlignRight);
    mainLayout->addWidget(co2_Zero_spinbox, 13, 1);
    mainLayout->addWidget(co2_Span_title, 14, 0, 1, 1, Qt::AlignRight);
    mainLayout->addWidget(co2_Span_spinbox, 14, 1);
    mainLayout->addWidget(co2_SpanTarget_title, 14, 2, 1, 1, Qt::AlignRight);
    mainLayout->addWidget(co2_SpanTarget_spinbox, 14, 3);
    mainLayout->addWidget(co2_Span2_title, 15, 0, 1, 1, Qt::AlignRight);
    mainLayout->addWidget(co2_Span2_spinbox, 15, 1);
    mainLayout->addWidget(co2_Span2Target_title, 15, 2, 1, 1, Qt::AlignRight);
    mainLayout->addWidget(co2_Span2Target_spinbox, 15, 3);
    mainLayout->addWidget(co2_CX_title, 16, 0, 1, 1, Qt::AlignRight);
    mainLayout->addWidget(co2_CX_spinbox, 16, 1);

    mainLayout->addWidget(h2o_Zero_title, 13, 4, 1, 1, Qt::AlignRight);
    mainLayout->addWidget(h2o_Zero_spinbox, 13, 5);
    mainLayout->addWidget(h2o_Span_title, 14, 4, 1, 1, Qt::AlignRight);
    mainLayout->addWidget(h2o_Span_spinbox, 14, 5);
    mainLayout->addWidget(h2o_SpanTarget_title, 14, 6, 1, 1, Qt::AlignRight);
    mainLayout->addWidget(h2o_SpanTarget_spinbox, 14, 7);
    mainLayout->addWidget(h2o_Span2_title, 15, 4, 1, 1, Qt::AlignRight);
    mainLayout->addWidget(h2o_Span2_spinbox, 15, 5);
    mainLayout->addWidget(h2o_Span2Target_title, 15, 6, 1, 1, Qt::AlignRight);
    mainLayout->addWidget(h2o_Span2Target_spinbox, 15, 7);
    mainLayout->addWidget(h2o_WX_title, 16, 4, 1, 1, Qt::AlignRight);
    mainLayout->addWidget(h2o_WX_spinbox, 16, 5);

    mainLayout->addWidget(okButton, 17, 0, 1, -1, Qt::AlignCenter);
    mainLayout->setVerticalSpacing(3);
    mainLayout->setContentsMargins(30, 30, 30, 30);
    mainLayout->setRowMinimumHeight(9, 30);
    mainLayout->setRowMinimumHeight(17, 30);
    setLayout(mainLayout);

    connect(okButton, &QPushButton::clicked,
            this, &CalibrationDialog::close);
}

void CalibrationDialog::close()
{
    DEBUG_FUNC_NAME
    if (isVisible())
    {
        hide();
    }
    emit saveRequest();

}

void CalibrationDialog::reset()
{

}

void CalibrationDialog::refresh()
{

}
