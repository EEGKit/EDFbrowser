/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2020 - 2021 Teunis van Beelen
*
* Email: teuniz@protonmail.com
*
***************************************************************************
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, version 3 of the License.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
***************************************************************************
*/


#ifndef UI_CDSAFORM_H
#define UI_CDSAFORM_H


#include <QtGlobal>
#include <QApplication>
#include <QObject>
#include <QWidget>
#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QDateTimeEdit>
#include <QPushButton>
#include <QObject>
#include <QFileDialog>
#include <QCheckBox>
#include <QCursor>
#include <QDoubleSpinBox>
#include <QProgressDialog>
#include <QMessageBox>
#include <QString>
#include <QPixmap>
#include <QDesktopServices>
#include <QUrl>
#include <QTextEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QPainter>
#include <QPen>
#include <QColor>
#include <QDockWidget>
#include <QToolTip>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFormLayout>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "global.h"
#include "mainwindow.h"
#include "utils.h"
#include "filteredblockread.h"
#include "fft_wrap.h"
#include "cdsa_dock.h"



class UI_Mainwindow;


class UI_cdsa_window : public QObject
{
  Q_OBJECT

public:
  UI_cdsa_window(QWidget *, struct signalcompblock *, int);

  UI_Mainwindow  *mainwindow;

private:

  int sf, cdsa_instance_nr;

  struct signalcompblock *signalcomp;

  QDialog       *myobjectDialog;

  QFormLayout   *flayout;

  QSpinBox      *segmentlen_spinbox,
                *blocklen_spinbox,
                *min_hz_spinbox,
                *max_hz_spinbox,
                *max_pwr_spinbox,
                *min_pwr_spinbox;

  QDoubleSpinBox *max_voltage_spinbox;

  QComboBox     *overlap_combobox,
                *windowfunc_combobox;

  QCheckBox     *log_checkbox,
                *pwr_voltage_checkbox;

  QPushButton   *close_button,
                *start_button,
                *default_button;

private slots:

  void start_button_clicked();
  void default_button_clicked();
  void segmentlen_spinbox_changed(int);
  void blocklen_spinbox_changed(int);
  void min_hz_spinbox_changed(int);
  void max_hz_spinbox_changed(int);
  void min_pwr_spinbox_changed(int);
  void max_pwr_spinbox_changed(int);
  void log_checkbox_changed(int);

};

#endif










