/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2020 - 2022 Teunis van Beelen
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


#ifndef UI_HYPNOGRAMDOCKFORM_H
#define UI_HYPNOGRAMDOCKFORM_H

#include "qt_headers.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "global.h"
#include "mainwindow.h"
#include "utils.h"
#include "edf_annot_list.h"


class UI_Mainwindow;
class simple_tracking_indicator2;
class simple_ruler_indicator2;
class hypnogram_curve_widget;


struct hypnogram_dock_param_struct
{
  int instance_num;
  char stage_name[6][32];
  char annot_name[6][32];
  struct edfhdrblock *edfhdr;
  UI_Mainwindow  *mainwindow;
};


class UI_hypnogram_dock : public QObject
{
  Q_OBJECT

public:
  UI_hypnogram_dock(QWidget *, struct hypnogram_dock_param_struct);
  ~UI_hypnogram_dock();

  UI_Mainwindow  *mainwindow;

  QToolBar *hypnogram_dock;

  double w_scaling,
         h_scaling;

public slots:
  void update_curve(void);

private:

  struct hypnogram_dock_param_struct param;

  simple_tracking_indicator2 *trck_indic;

  simple_ruler_indicator2 *srl_indic;

  hypnogram_curve_widget *hypnogram_curve;

  int is_deleted;

private slots:

  void file_pos_changed(long long);
  void hypnogram_dock_destroyed(QObject *);

};


class simple_tracking_indicator2: public QWidget
{
  Q_OBJECT

public:
  simple_tracking_indicator2(QWidget *parent=0);

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

  double w_scaling,
         h_scaling;

  void draw_small_arrow(QPainter *, int, int, int, QColor);
};


class simple_ruler_indicator2: public QWidget
{
  Q_OBJECT

public:
  simple_ruler_indicator2(QWidget *parent=0);

  QSize sizeHint() const {return minimumSizeHint(); }
  QSize minimumSizeHint() const {return QSize(5, 5); }

  void set_params(struct hypnogram_dock_param_struct *);
  void set_scaling(double, double);

public slots:

protected:
  void paintEvent(QPaintEvent *);
  void contextmenu_requested(QPoint);

private:

  double w_scaling,
         h_scaling;

  struct hypnogram_dock_param_struct param;
};


class hypnogram_curve_widget: public QWidget
{
  Q_OBJECT

public:
  hypnogram_curve_widget(QWidget *parent=0);

  QSize sizeHint() const {return minimumSizeHint(); }
  QSize minimumSizeHint() const {return QSize(5, 5); }

  UI_Mainwindow  *mainwindow;

  void set_params(struct hypnogram_dock_param_struct *);

public slots:

protected:
  void paintEvent(QPaintEvent *);

private:

  struct hypnogram_dock_param_struct param;
};


#endif










