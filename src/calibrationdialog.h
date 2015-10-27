#ifndef CALIBRATIONDIALOG_H
#define CALIBRATIONDIALOG_H

#include <QDialog>

class QDateTimeEdit;
class QDoubleSpinBox;

class QScienceSpinBox;

class EcProject;
struct ConfigState;

class CalibrationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CalibrationDialog(QWidget* parent, EcProject *ecProject, ConfigState* config);

signals:
    void saveRequest();

public slots:
    void close();
    void reset();
    void refresh();

private:
    EcProject *ecProject_;
    ConfigState* configState_;

    QDateTimeEdit* calibDate;
    QDateTimeEdit* zeroDate;

    QScienceSpinBox* co2_A_spinbox;
    QScienceSpinBox* co2_B_spinbox;
    QScienceSpinBox* co2_C_spinbox;
    QScienceSpinBox* co2_D_spinbox;
    QScienceSpinBox* co2_E_spinbox;
    QScienceSpinBox* co2_XS_spinbox;
    QScienceSpinBox* co2_Z_spinbox;

    QScienceSpinBox* h2o_A_spinbox;
    QScienceSpinBox* h2o_B_spinbox;
    QScienceSpinBox* h2o_C_spinbox;
    QScienceSpinBox* h2o_XS_spinbox;
    QScienceSpinBox* h2o_Z_spinbox;

    QScienceSpinBox* co2_Zero_spinbox;
    QScienceSpinBox* co2_Span_spinbox;
    QDoubleSpinBox* co2_SpanTarget_spinbox;
    QScienceSpinBox* co2_Span2_spinbox;
    QDoubleSpinBox* co2_Span2Target_spinbox;
    QScienceSpinBox* co2_CX_spinbox;

    QScienceSpinBox* h2o_Zero_spinbox;
    QScienceSpinBox* h2o_Span_spinbox;
    QDoubleSpinBox* h2o_SpanTarget_spinbox;
    QScienceSpinBox* h2o_Span2_spinbox;
    QDoubleSpinBox* h2o_Span2Target_spinbox;
    QScienceSpinBox* h2o_WX_spinbox;
};

#endif // CALIBRATIONDIALOG_H
