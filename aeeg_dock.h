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


class UI_Mainwindow;
class a_simple_tracking_indicator;
class a_simple_ruler_indicator;



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
};


class UI_aeeg_dock : public QObject
{
  Q_OBJECT

public:
  UI_aeeg_dock(QWidget *, struct aeeg_dock_param_struct);
  ~UI_aeeg_dock();

  UI_Mainwindow  *mainwindow;

  QDockWidget *aeeg_dock;

  double w_scaling,
         h_scaling;

  struct aeeg_dock_param_struct param;

private:

  a_simple_tracking_indicator *trck_indic;
  a_simple_ruler_indicator     *srl_indic;

  unsigned long long sigcomp_uid;

  int is_deleted;

private slots:

  void aeeg_dock_destroyed(QObject *);
  void file_pos_changed(long long);
  void contextmenu_requested(QPoint);

};


class a_simple_tracking_indicator: public QWidget
{
  Q_OBJECT

public:
  double w_scaling,
         h_scaling;

  a_simple_tracking_indicator(QWidget *parent=0);

  QSize sizeHint() const {return minimumSizeHint(); }
  QSize minimumSizeHint() const {return QSize(5, 5); }

  void set_position(long long);
  void set_maximum(long long);
  void set_scaling(double, double);

public slots:

protected:
  void paintEvent(QPaintEvent *);

private:

  long long pos, max;

  void draw_small_arrow(QPainter *, int, int, int, QColor);
};


class a_simple_ruler_indicator: public QWidget
{
  Q_OBJECT

public:
  a_simple_ruler_indicator(QWidget *parent=0);

  double w_scaling,
         h_scaling;

  QSize sizeHint() const {return minimumSizeHint(); }
  QSize minimumSizeHint() const {return QSize(5, 5); }

  void set_minimum(int);
  void set_maximum(int);
  void set_unit(const char *);
  void set_scaling(double, double);

public slots:

protected:
  void paintEvent(QPaintEvent *);
  void contextmenu_requested(QPoint);

private:

  char unit[32];

  int min, max;
};


#endif










