#include "mainwindow.hpp"

#include <QColor>
#include <QColorDialog>
#include <QFileDialog>
#include <QShortcut>

#include "./ui_mainwindow.h"

enum ControlSteps {
  WIDTH = 200,
  LOCATION = 1000,
  ROTATION = 1800,
  SCALE = 1000,
};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
  /* Paint the color picker buttons */
  paintButton(ui->backgroundColorPicker, ui->viewport->getBackgroundColor());
  paintButton(ui->lineColorPicker, ui->viewport->getLineColor());
  paintButton(ui->pointColorPicker, ui->viewport->getPointColor());
  /* Set primitives' display status */
  on_displayLinesCheckBox_toggled(true);
  on_displayPointsCheckBox_toggled(false);
  /* Set up primitives' size controls */
  setupWidthControls(ui->lineWidthSlider, ui->lineWidthSpinBox);
  setupWidthControls(ui->pointSizeSlider, ui->pointSizeSpinBox);
  /* Set up location controls */
  setupLocationControls(ui->xLocationSlider, ui->xLocationSpinbox);
  setupLocationControls(ui->yLocationSlider, ui->yLocationSpinbox);
  setupLocationControls(ui->zLocationSlider, ui->zLocationSpinbox);
  /* Set up rotation controls */
  setupRotationControls(ui->xRotationSlider, ui->xRotationSpinbox);
  setupRotationControls(ui->yRotationSlider, ui->yRotationSpinbox);
  setupRotationControls(ui->zRotationSlider, ui->zRotationSpinbox);
  /* Set up scale controls */
  setupScaleControls(ui->xScaleSlider, ui->xScaleSpinbox);
  setupScaleControls(ui->yScaleSlider, ui->yScaleSpinbox);
  setupScaleControls(ui->zScaleSlider, ui->zScaleSpinbox);
  setupScaleControls(ui->uScaleSlider, ui->uScaleSpinbox);
  /* Set up shortcuts */
  new QShortcut(QKeySequence(tr("Ctrl+O")), this, SLOT(openFile()));
  new QShortcut(QKeySequence(tr("e")), this, SLOT(close()));
  //  new QShortcut(QKeySequence(tr("L")), ui->xLocationSpinbox,
  //  SLOT(setFocus()));
  resetSettings();
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::resetSettings() {
  on_lineWidthSlider_doubleValueChanged(1.5);
  ui->lineWidthSpinBox->setValue(1.5);
  on_pointSizeSlider_doubleValueChanged(1.0);
  ui->pointSizeSpinBox->setValue(1.0);
}

/* GUI behaviour related helper functions */

void MainWindow::paintButton(QPushButton *b, const QColor &c) {
  QString qss = QString(
                    "background-color: %1; border-width: 2px;"
                    "border-style: solid; border-radius: 5px;")
                    .arg(c.name());
  b->setStyleSheet(qss);
}

QColor convertColorToGreyscale(const QColor &c) {
  // formula from the documentaiton:
  // https://doc.qt.io/archives/qt-4.8/qcolor.html#qGray
  int greyv = (c.red() * 11 + c.green() * 16 + c.blue() * 5) / 32;
  /* Set the bounds */
  static const int lbound = 0.43 * 255;
  static const int ubound = 0.81 * 255;
  if (greyv < lbound) greyv = lbound;
  if (greyv > ubound) greyv = ubound;
  return QColor(greyv, greyv, greyv);
}

void setLayoutWidgetsState(const QLayout *layout, bool state) {
  for (int i = 0; i < layout->count(); ++i) {
    QWidget *widget = layout->itemAt(i)->widget();
    if (widget != NULL)
      widget->setEnabled(state);
    else {
      QLayout *inner = layout->itemAt(i)->layout();
      setLayoutWidgetsState(inner, state);
    }
  }
}

void syncSliderWithSpinbox(DoubleSlider *s, QDoubleSpinBox *sb) {
  QObject::connect(s, &DoubleSlider::doubleValueChanged,  //
                   sb, &QDoubleSpinBox::setValue);
  QObject::connect(sb, &QDoubleSpinBox::valueChanged,  //
                   s, &DoubleSlider::setDoubleValue);
}

/* GUI behaviour related signal functions */

void MainWindow::on_backgroundColorPicker_clicked() {
  QColor prev_color = ui->viewport->getBackgroundColor();
  QColor color = QColorDialog::getColor(prev_color, this);
  if (!color.isValid()) return;
  ui->viewport->setBackgroundColor(color);
  ui->viewport->update();
  paintButton(ui->backgroundColorPicker, color);
}
void MainWindow::on_lineColorPicker_clicked() {
  QColor prev_color = ui->viewport->getLineColor();
  QColor color = QColorDialog::getColor(prev_color, this);
  if (!color.isValid()) return;
  ui->viewport->setLineColor(color);
  ui->viewport->update();
  paintButton(ui->lineColorPicker, color);
}
void MainWindow::on_pointColorPicker_clicked() {
  QColor prev_color = ui->viewport->getPointColor();
  QColor color = QColorDialog::getColor(prev_color, this);
  if (!color.isValid()) return;
  ui->viewport->setPointColor(color);
  ui->viewport->update();
  paintButton(ui->pointColorPicker, color);
}

void MainWindow::on_displayLinesCheckBox_toggled(bool checked) {
  /* Set the state in the viewport */
  ui->viewport->setLineDisplayEnabled(checked);
  ui->viewport->update();
  /* Set the checkbox check state */
  ui->displayLinesCheckBox->setChecked(checked);
  /* Disable the settings' frame */
  setLayoutWidgetsState(ui->lineSettingsLayout, checked);
  ui->lineStyleDashedCheckBox->setEnabled(checked);
  /* Update the button color */
  QColor res_color = ui->viewport->getLineColor();
  if (checked == false) {
    res_color = convertColorToGreyscale(res_color);
  }
  paintButton(ui->lineColorPicker, res_color);
}
void MainWindow::on_displayPointsCheckBox_toggled(bool checked) {
  /* Set the state in the viewport */
  ui->viewport->setPointDisplayEnabled(checked);
  ui->viewport->update();
  /* Set the checkbox check state */
  ui->displayPointsCheckBox->setChecked(checked);
  /* Disable the settings' frame */
  setLayoutWidgetsState(ui->pointSettingsLayout, checked);
  ui->pointStyleSquareCheckBox->setEnabled(checked);
  /* Update the button color */
  QColor res_color = ui->viewport->getPointColor();
  if (checked == false) {
    res_color = convertColorToGreyscale(res_color);
  }
  paintButton(ui->pointColorPicker, res_color);
}

/* Primitives' style related functions */

void MainWindow::on_pointStyleSquareCheckBox_toggled(bool checked) {
  ui->viewport->setPointStyle(checked == true ? PointStyle::SQUARE
                                              : PointStyle::CIRCLE);
  ui->viewport->update();
}
void MainWindow::on_lineStyleDashedCheckBox_toggled(bool checked) {
  ui->viewport->setLineStyle(checked == true ? LineStyle::DASHED
                                             : LineStyle::SOLID);
  ui->viewport->update();
}

/* Primitives' size related functions */

void MainWindow::setupWidthControls(DoubleSlider *s, QDoubleSpinBox *sb) {
  syncSliderWithSpinbox(s, sb);
  /* Set up the spinbox */
  const unsigned int steps_count = ControlSteps::WIDTH;
  const float sb_limit = 20.0f;
  const float sb_step = sb_limit / steps_count;
  sb->setSingleStep(sb_step);
  sb->setDecimals(1);
  sb->setMinimum(1.0);
  sb->setMaximum(+sb_limit);
  /* Set up the slider */
  s->setMinimum(10);
  s->setMaximum(+steps_count);
  // internally the slider is of int type but emits the signal of type double
  s->divisor = steps_count / sb_limit;
}
void MainWindow::on_lineWidthSlider_doubleValueChanged(double value) {
  ui->viewport->setLineWidth(value);
  ui->viewport->update();
}
void MainWindow::on_pointSizeSlider_doubleValueChanged(double value) {
  ui->viewport->setPointSize(value);
  ui->viewport->update();
}

/* Location related functions */

void MainWindow::setupLocationControls(DoubleSlider *s, QDoubleSpinBox *sb) {
  syncSliderWithSpinbox(s, sb);
  /* Set up the spinbox */
  const unsigned int steps_count = ControlSteps::LOCATION;
  const float sb_limit = 1.0f;
  const float sb_step = sb_limit / steps_count;
  sb->setSingleStep(sb_step);
  sb->setDecimals(3);
  sb->setMinimum(-sb_limit);
  sb->setMaximum(+sb_limit);
  /* Set up the slider */
  s->setMinimum(-steps_count);
  s->setMaximum(+steps_count);
  // internally the slider is of int type but emits the signal of type double
  s->divisor = steps_count;
}
void MainWindow::on_xLocationSlider_doubleValueChanged(double value) {
  ui->viewport->setTranslationX(value);
  ui->viewport->update();
}
void MainWindow::on_yLocationSlider_doubleValueChanged(double value) {
  ui->viewport->setTranslationY(value);
  ui->viewport->update();
}
void MainWindow::on_zLocationSlider_doubleValueChanged(double value) {
  ui->viewport->setTranslationZ(value);
  ui->viewport->update();
}
void MainWindow::on_locationResetPushButton_clicked() {
  ui->xLocationSlider->setValue(0);
  ui->yLocationSlider->setValue(0);
  ui->zLocationSlider->setValue(0);
}

/* Rotation related functions */

void MainWindow::setupRotationControls(DoubleSlider *s, QDoubleSpinBox *sb) {
  syncSliderWithSpinbox(s, sb);
  /* Set up the spinbox */
  const unsigned int steps_count = ControlSteps::ROTATION;
  const float sb_limit = 180.0f;
  const float sb_step = sb_limit / steps_count;
  sb->setSingleStep(sb_step);
  sb->setDecimals(1);
  sb->setMinimum(-sb_limit);
  sb->setMaximum(+sb_limit);
  /* Set up the slider */
  s->setMinimum(-steps_count);
  s->setMaximum(+steps_count);
  // internally the slider is of int type but emits the signal of type double
  s->divisor = steps_count / sb_limit;
}
void MainWindow::on_xRotationSlider_doubleValueChanged(double value) {
  ui->viewport->setRotationX(value);
  ui->viewport->update();
}
void MainWindow::on_yRotationSlider_doubleValueChanged(double value) {
  ui->viewport->setRotationY(value);
  ui->viewport->update();
}
void MainWindow::on_zRotationSlider_doubleValueChanged(double value) {
  ui->viewport->setRotationZ(value);
  ui->viewport->update();
}
void MainWindow::on_rotationResetPushButton_clicked() {
  ui->xRotationSlider->setValue(0);
  ui->yRotationSlider->setValue(0);
  ui->zRotationSlider->setValue(0);
}

/* Scale related functions */

void MainWindow::setupScaleControls(DoubleSlider *s, QDoubleSpinBox *sb) {
  syncSliderWithSpinbox(s, sb);
  /* Set up the spinbox */
  const unsigned int steps_count = ControlSteps::SCALE;
  const float sb_limit = 10.0f;
  const float sb_step = sb_limit / steps_count;
  sb->setSingleStep(sb_step);
  sb->setDecimals(2);
  sb->setMinimum(0.01f);
  sb->setMaximum(10.00f);
  /* Set up the slider */
  s->setMinimum(1);
  s->setMaximum(steps_count);
  // internally the slider is of int type but emits the signal of type double
  s->divisor = steps_count / sb_limit;
  s->setDoubleValue(1.0f);
}
void MainWindow::on_xScaleSlider_doubleValueChanged(double value) {
  ui->viewport->setScaleX(value);
  ui->viewport->update();
}
void MainWindow::on_yScaleSlider_doubleValueChanged(double value) {
  ui->viewport->setScaleY(value);
  ui->viewport->update();
}
void MainWindow::on_zScaleSlider_doubleValueChanged(double value) {
  ui->viewport->setScaleZ(value);
  ui->viewport->update();
}
void MainWindow::on_uScaleSlider_doubleValueChanged(double value) {
  ui->viewport->setScaleU(value);
  ui->viewport->update();
}
void MainWindow::on_scaleResetPushButton_clicked() {
  ui->xScaleSpinbox->setValue(1);
  ui->yScaleSpinbox->setValue(1);
  ui->zScaleSpinbox->setValue(1);
  ui->uScaleSpinbox->setValue(1);
}

void MainWindow::openFile() {
  QString dir = QDir::homePath() + "/Downloads";
  QString file_name = QFileDialog::getOpenFileName(
      this, "Open 3d model", dir, "geometry definition file (*.obj)");
  ui->viewport->setFileName(file_name.toStdString());
  ui->viewport->loadModel();
}
