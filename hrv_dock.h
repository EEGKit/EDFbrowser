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


#ifndef UI_HRVDOCKFORM_H
#define UI_HRVDOCKFORM_H


#include <QtGlobal>
#include <QApplication>
#include <QObject>
#include <QWidget>
#include <QFrame>
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
#include <QTextEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QPainter>
#include <QPen>
#include <QColor>
#include <QDockWidget>
#include <QGridLayout>
#include <QRectF>
#include <QPoint>
#include <QToolBar>
#include <QAction>
#include <QMenu>
#include <QToolTip>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QDialog>
#include <QEvent>
#include <QColorDialog>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "global.h"
#include "mainwindow.h"
#include "utils.h"
#include "edf_annot_list.h"
#include "special_button.h"


class UI_Mainwindow;
class simple_tracking_indicator3;
class simple_ruler_indicator3;
class hrv_curve_widget;


struct hrv_dock_param_struct
{
  int instance_num;
  char annot_name[MAX_ANNOTATION_LEN];
  struct edfhdrblock *edfhdr;
  UI_Mainwindow *mainwindow;
};


class UI_hrv_dock : public QObject
{
  Q_OBJECT

public:
  UI_hrv_dock(QWidget *, struct hrv_dock_param_struct);
  ~UI_hrv_dock();

  UI_Mainwindow  *mainwindow;

  QToolBar *hrv_dock;

  double w_scaling,
         h_scaling;

public slots:
  void update_curve(void);

private:

  struct hrv_dock_param_struct param;

  simple_tracking_indicator3 *trck_indic;

  simple_ruler_indicator3 *srl_indic;

  hrv_curve_widget *hrv_curve;

  int is_deleted;

  QMenu *context_menu;

  QPushButton *settings_close_button;

  QSpinBox *max_bpm_spinbox,
           *min_bpm_spinbox,
           *height_spinbox;

  QColor trace_color;

  SpecialButton *color_button;

private slots:

  void file_pos_changed(long long);
  void hrv_dock_destroyed(QObject *);
  void show_context_menu(QPoint);
  void show_settings(bool);
  void close_dock(bool);
  void max_bpm_spinbox_changed(int);
  void min_bpm_spinbox_changed(int);
  void height_spinbox_changed(int);
  void color_button_clicked();
};


class simple_tracking_indicator3: public QWidget
{
  Q_OBJECT

public:
  simple_tracking_indicator3(QWidget *parent=0);

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


class simple_ruler_indicator3: public QWidget
{
  Q_OBJECT

public:
  simple_ruler_indicator3(QWidget *parent=0);

  QSize sizeHint() const {return minimumSizeHint(); }
  QSize minimumSizeHint() const {return QSize(5, 5); }

  void set_params(struct hrv_dock_param_struct *);

  void set_range(int, int);
  void set_scaling(double, double);

public slots:

protected:
  void paintEvent(QPaintEvent *);
  void contextmenu_requested(QPoint);

private:

  struct hrv_dock_param_struct param;

  int max_val;
  int min_val;

  double w_scaling,
         h_scaling;
};


class hrv_curve_widget: public QWidget
{
  Q_OBJECT

public:
  hrv_curve_widget(QWidget *parent=0);

  QSize sizeHint() const {return minimumSizeHint(); }
  QSize minimumSizeHint() const {return QSize(5, 5); }

  UI_Mainwindow  *mainwindow;

  void set_params(struct hrv_dock_param_struct *);

  void set_range(int, int);

  void set_trace_color(QColor);

public slots:

protected:
  void paintEvent(QPaintEvent *);

private:

  struct hrv_dock_param_struct param;

  double bpm_min,
         bpm_max,
         bpm_range;

  QColor trace_color;
};


#endif










