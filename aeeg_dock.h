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


#ifndef UI_AEEGDOCKFORM_H
#define UI_AEEGDOCKFORM_H


#include "qt_headers.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "global.h"
#include "mainwindow.h"
#include "utils.h"
#include "signalcurve.h"


class UI_Mainwindow;



struct aeeg_dock_param_struct
{
  struct signalcompblock *signalcomp;
  int sf;
  double min_hz;
  double max_hz;
  int segment_len;
  int segments_in_recording;
  int instance_num;
  char unit[32];
  int no_dialog;
  double *min_max_val;
};


class UI_aeeg_dock : public QObject
{
  Q_OBJECT

public:
  UI_aeeg_dock(QWidget *, struct aeeg_dock_param_struct);
  ~UI_aeeg_dock();

  UI_Mainwindow  *mainwindow;

  QDockWidget *aeeg_dock;

  SignalCurve *curve1;

  double w_scaling,
         h_scaling;

  struct aeeg_dock_param_struct param;

private:

  unsigned long long sigcomp_uid;

  int is_deleted;

private slots:

  void aeeg_dock_destroyed(QObject *);
  void file_pos_changed(long long);
  void contextmenu_requested(QPoint);

};


#endif










