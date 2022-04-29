/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2022 Teunis van Beelen
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


#ifndef UI_AEEGFORM_H
#define UI_AEEGFORM_H


#include "qt_headers.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "global.h"
#include "mainwindow.h"
#include "utils.h"
#include "filteredblockread.h"
#include "aeeg_dock.h"
#include "third_party/fidlib/fidlib.h"


class UI_Mainwindow;



class UI_aeeg_window : public QObject
{
  Q_OBJECT

public:
  UI_aeeg_window(QWidget *, struct signalcompblock *, int, struct aeeg_dock_param_struct *p_par=NULL);

  UI_Mainwindow  *mainwindow;

private:

  int sf, aeeg_instance_nr;

  struct signalcompblock *signalcomp;

  struct aeeg_dock_param_struct *no_dialog_params;

  QDialog       *myobjectDialog;

  QFormLayout   *flayout;

  QSpinBox      *segmentlen_spinbox;

  QDoubleSpinBox *min_hz_spinbox,
                 *max_hz_spinbox;

  QPushButton   *close_button,
                *start_button,
                *default_button;

private slots:

  void start_button_clicked();
  void default_button_clicked();
  void segmentlen_spinbox_changed(int);
  void min_hz_spinbox_changed(double);
  void max_hz_spinbox_changed(double);

};

#endif










