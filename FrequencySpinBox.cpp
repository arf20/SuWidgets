//
//    filename: description
//    Copyright (C) 2018 Gonzalo José Carracedo Carballal
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU Lesser General Public License as
//    published by the Free Software Foundation, either version 3 of the
//    License, or (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful, but
//    WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU Lesser General Public License for more details.
//
//    You should have received a copy of the GNU Lesser General Public
//    License along with this program.  If not, see
//    <http://www.gnu.org/licenses/>
//
#include <cmath>
#include "FrequencySpinBox.h"
#include "SuWidgetsHelpers.h"
#include "ui_FrequencySpinBox.h"
#include <QLineEdit>

FrequencySpinBox::FrequencySpinBox(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::FrequencySpinBox)
{
  QLineEdit *lineEdit;
  ui->setupUi(this);

  lineEdit = this->ui->frequencySpin->findChild<QLineEdit *>();
  lineEdit->installEventFilter(this);

  this->ui->frequencySpin->setKeyboardTracking(false);

  this->refreshUi();

  this->connectAll();

  int width = SuWidgetsHelpers::getWidgetTextWidth(
        this->ui->decFreqUnitsButton, "<");

  this->ui->incFreqUnitsButton->setMaximumWidth(4 * width);
  this->ui->decFreqUnitsButton->setMaximumWidth(4 * width);
}

FrequencySpinBox::~FrequencySpinBox()
{
  delete ui;
}

QString
FrequencySpinBox::freqSuffix(void) const
{
  switch (this->UnitMultiplier) {
    case MUL_NONE:
      return this->fUnits;

    case MUL_KILO:
      return "k" + this->fUnits;

    case MUL_MEGA:
      return "M" + this->fUnits;

    case MUL_GIGA:
      return "G" + this->fUnits;

    case MUL_TERA:
      return "T" + this->fUnits;
  }

  return "??";
}

double
FrequencySpinBox::freqMultiplier(void) const
{
  switch (this->UnitMultiplier) {
    case MUL_NONE:
      return 1;

    case MUL_KILO:
      return 1e3;

    case MUL_MEGA:
      return 1e6;

    case MUL_GIGA:
      return 1e9;

    case MUL_TERA:
      return 1e12;
  }

  return 0;
}

void
FrequencySpinBox::connectAll(void)
{
  connect(
        this->ui->incFreqUnitsButton,
        SIGNAL(clicked(void)),
        this,
        SLOT(onIncFreqUnitMultiplier(void)));

  connect(
        this->ui->decFreqUnitsButton,
        SIGNAL(clicked(void)),
        this,
        SLOT(onDecFreqUnitMultiplier(void)));

  connect(
        this->ui->frequencySpin,
        SIGNAL(valueChanged(double)),
        this,
        SLOT(onValueChanged(double)));
}

bool
FrequencySpinBox::eventFilter(QObject *object, QEvent *event)
{
  if (object != nullptr) {
    switch (event->type()) {
      case QEvent::Enter:
        this->expectingFirstClick = true;
        break;

      case QEvent::Leave:
        this->expectingFirstClick = false;
        break;

      case QEvent::MouseButtonRelease:
        if (this->expectingFirstClick) {
          this->ui->frequencySpin->selectAll();
          this->expectingFirstClick = false;
        }
        break;

      default:
        ;
    }
  }

  return QWidget::eventFilter(object, event);
}

void
FrequencySpinBox::adjustUnitMultiplier(void)
{
  qreal absValue = std::fabs(this->currValue);

  if (absValue >= 1e12)
    this->setFrequencyUnitMultiplier(MUL_TERA);
  else if (absValue >= 1e9)
    this->setFrequencyUnitMultiplier(MUL_GIGA);
  else if (absValue >= 1e6)
    this->setFrequencyUnitMultiplier(MUL_MEGA);
  else if (absValue >= 1e3)
    this->setFrequencyUnitMultiplier(MUL_KILO);
  else
    this->setFrequencyUnitMultiplier(MUL_NONE);

  this->refreshUi();
}

void
FrequencySpinBox::refreshUi(void)
{
  if (!this->refreshing) {
    double mul = 1 / this->freqMultiplier();
    this->refreshing = true;

    this->ui->incFreqUnitsButton->setEnabled(this->UnitMultiplier != MUL_TERA);
    this->ui->decFreqUnitsButton->setEnabled(this->UnitMultiplier != MUL_NONE);

    this->ui->frequencySpin->setSuffix(" " + this->freqSuffix());
    this->ui->frequencySpin->setDecimals(
          static_cast<int>(this->UnitMultiplier) * 3
          + static_cast<int>(this->uExtraDecimals));

    this->ui->frequencySpin->setMaximum(this->max * mul);
    this->ui->frequencySpin->setMinimum(this->min * mul);

    this->ui->frequencySpin->setValue(this->currValue * mul);

    this->refreshing = false;
  }
}

void
FrequencySpinBox::setValue(double val)
{
  this->currValue = val;

  if (this->autoUnitMultiplier)
    this->adjustUnitMultiplier();

  this->refreshUi();
}

double
FrequencySpinBox::value(void) const
{
  return currValue;
}

void
FrequencySpinBox::setMaximum(double max)
{
  this->max = max;
  this->refreshUi();
}

double
FrequencySpinBox::maximum(void) const
{
  return this->max;
}

void
FrequencySpinBox::setMinimum(double min)
{
  this->min = min;
  this->refreshUi();
}

double
FrequencySpinBox::minimum(void) const
{
  return this->min;
}

void
FrequencySpinBox::setExtraDecimals(unsigned int extra)
{
  this->uExtraDecimals = extra;
  this->refreshUi();
}

unsigned int
FrequencySpinBox::extraDecimals(void) const
{
  return this->uExtraDecimals;
}

void
FrequencySpinBox::setAutoUnitMultiplierEnabled(bool enabled)
{
  this->autoUnitMultiplier = enabled;

  if (enabled)
    this->adjustUnitMultiplier();
}

bool
FrequencySpinBox::autoUnitMultiplierEnabled(void) const
{
  return this->autoUnitMultiplier;
}

void
FrequencySpinBox::setFrequencyUnitMultiplier(FrequencyUnitMultiplier UnitMultiplier)
{
  this->UnitMultiplier = UnitMultiplier;
  this->refreshUi();
}

FrequencySpinBox::FrequencyUnitMultiplier
FrequencySpinBox::frequencyUnitMultiplier(void) const
{
  return this->UnitMultiplier;
}

void
FrequencySpinBox::incFrequencyUnitMultiplier(void)
{
  if (this->UnitMultiplier < MUL_TERA)
    this->setFrequencyUnitMultiplier(
          static_cast<FrequencyUnitMultiplier>(static_cast<int>(this->UnitMultiplier) + 1));

}

void
FrequencySpinBox::decFrequencyUnitMultiplier(void)
{
  if (this->UnitMultiplier > MUL_NONE)
    this->setFrequencyUnitMultiplier(
          static_cast<FrequencyUnitMultiplier>(static_cast<int>(this->UnitMultiplier) - 1));
}

void
FrequencySpinBox::setUnits(QString const units)
{
  this->fUnits = units;
  this->refreshUi();
}

QString
FrequencySpinBox::units(void) const
{
  return this->fUnits;

}

void
FrequencySpinBox::setEditable(bool editable)
{
  this->ui->frequencySpin->setReadOnly(!editable);
}

bool
FrequencySpinBox::editable(void) const
{
  return !this->ui->frequencySpin->isReadOnly();
}


///////////////////////////////// Slots ///////////////////////////////////////
void
FrequencySpinBox::onValueChanged(double freq)
{
  if (!this->refreshing) {
    this->currValue = freq * this->freqMultiplier();
    emit valueChanged(this->currValue);
  }
}

void
FrequencySpinBox::onIncFreqUnitMultiplier(void)
{
  this->incFrequencyUnitMultiplier();
}

void
FrequencySpinBox::onDecFreqUnitMultiplier(void)
{
  this->decFrequencyUnitMultiplier();
}

