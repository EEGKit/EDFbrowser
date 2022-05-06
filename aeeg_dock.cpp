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



#include "aeeg_dock.h"


UI_aeeg_dock::UI_aeeg_dock(QWidget *w_parent, struct aeeg_dock_param_struct par)
{
  char str[1024]={""};

  QFrame *frame;

  QGridLayout *grid_layout;

  mainwindow = (UI_Mainwindow *)w_parent;

  w_scaling = mainwindow->w_scaling;
  h_scaling = mainwindow->h_scaling;

  param = par;

  is_deleted = 0;

  sigcomp_uid = param.signalcomp->uid;

  snprintf(str, 1024, " aEEG   %s", param.signalcomp->signallabel);

  frame = new QFrame;
  frame->setFrameStyle(QFrame::NoFrame);
  frame->setLineWidth(0);
  frame->setMidLineWidth(0);
  frame->setContentsMargins(0, 0, 0, 0);

  curve1 = new SignalCurve;
  curve1->setSignalColor(Qt::darkBlue);
  curve1->setBackgroundColor(Qt::white);
  curve1->setRasterColor(Qt::gray);
  curve1->setTraceWidth(0);
  curve1->setMinimumHeight(120 * h_scaling);
  curve1->setMaximumHeight(120 * h_scaling);
  curve1->setMinimumWidth(100 * w_scaling);
//  curve1->setV_label(param.signalcomp->physdimension);
  curve1->setH_label("Sec.");
  curve1->setH_RulerValues(0, param.signalcomp->edfhdr->recording_len_sec);
  curve1->setH_rulerEnabled(false);
  curve1->setHor_BorderHeight(10 * h_scaling);
  curve1->setVert_BorderWidth(50 * w_scaling);
  curve1->setDashBoardEnabled(false);
  curve1->setMarker1Color(Qt::red);
  curve1->setMarker1Enabled(true);
  curve1->setCursorEnabled(false);
  curve1->setPrintEnabled(false);
  curve1->setV_LogarithmicEnabled(true);
  curve1->setH_Resolution(((6.0 / mainwindow->x_pixelsizefactor) / (3600.0 / param.segment_len)) / 2);  /* set horziontal scale to 6 cm per hour -> approx. 1 pixel per segment */
  curve1->setFixedH_resolutionEnabled(true, 1);
  curve1->drawCurve(param.min_max_val, param.segments_in_recording * 2, param.scale_max_amp, 0);
  curve1->setSignalColor(Qt::green, 1);
  curve1->setTraceWidth(0, 1);
  curve1->drawCurve(param.max_median_val, param.medians_in_recording, param.scale_max_amp, 0, 1);
  curve1->setSignalColor(Qt::green, 2);
  curve1->setTraceWidth(0, 2);
  curve1->drawCurve(param.min_median_val, param.medians_in_recording, param.scale_max_amp, 0, 2);
  curve1->setContentsMargins(0, 0, 0, 0);

//   aeeg_dock = new QDockWidget(str, mainwindow);
//   aeeg_dock->setFeatures(QDockWidget::AllDockWidgetFeatures);
//   aeeg_dock->setAllowedAreas(Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea);
//   aeeg_dock->setAttribute(Qt::WA_DeleteOnClose);
//   aeeg_dock->setContextMenuPolicy(Qt::CustomContextMenu);
//   aeeg_dock->setWidget(curve1);
//   aeeg_dock->setMinimumHeight(150 * h_scaling);
//   aeeg_dock->setMinimumWidth(300 * w_scaling);
//
//   mainwindow->addDockWidget(Qt::BottomDockWidgetArea, aeeg_dock, Qt::Horizontal);

  grid_layout = new QGridLayout(frame);
  grid_layout->addWidget(curve1, 0, 0);
  grid_layout->setColumnStretch(0, 100);

  aeeg_dock = new QToolBar(str, mainwindow);
  aeeg_dock->setOrientation(Qt::Horizontal);
  aeeg_dock->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);
  aeeg_dock->setAttribute(Qt::WA_DeleteOnClose);
  aeeg_dock->setContextMenuPolicy(Qt::CustomContextMenu);
  aeeg_dock->addWidget(frame);

  QObject::connect(aeeg_dock,  SIGNAL(destroyed(QObject *)),               this, SLOT(aeeg_dock_destroyed(QObject *)));
  QObject::connect(aeeg_dock,  SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextmenu_requested(QPoint)));
  QObject::connect(mainwindow, SIGNAL(file_position_changed(long long)),   this, SLOT(file_pos_changed(long long)));

  file_pos_changed(0);
}


UI_aeeg_dock::~UI_aeeg_dock()
{
  if(!is_deleted)
  {
    is_deleted = 1;

    aeeg_dock->close();

    param.signalcomp->aeeg_idx[param.instance_num] = 0;

    mainwindow->aeeg_dock[param.instance_num] = NULL;
  }

  free(param.min_max_val);
  free(param.max_median_val);
  free(param.min_median_val);
}


void UI_aeeg_dock::aeeg_dock_destroyed(QObject *)
{
  if(!is_deleted)
  {
    is_deleted = 1;

    param.signalcomp->aeeg_idx[param.instance_num] = 0;

    mainwindow->aeeg_dock[param.instance_num] = NULL;
  }

  delete this;
}


void UI_aeeg_dock::file_pos_changed(long long)
{
  int i;

  long long f_pos=0;

  for(i=0; i<mainwindow->signalcomps; i++)
  {
    if(mainwindow->signalcomp[i]->uid == sigcomp_uid)
    {
      if(param.signalcomp == mainwindow->signalcomp[i])
      {
        f_pos = mainwindow->signalcomp[i]->edfhdr->viewtime;

        break;
      }
    }
  }

  if(i == mainwindow->signalcomps)  return;

  if(f_pos < 0)
  {
    f_pos = 0;
  }

  if(f_pos > param.signalcomp->file_duration)
  {
    f_pos = param.signalcomp->file_duration;
  }

  curve1->setMarker1Position((double)f_pos / (double)(param.signalcomp->file_duration));
}


void UI_aeeg_dock::contextmenu_requested(QPoint)
{
  char str[4096]={"aEEG "};

  strlcat(str, param.signalcomp->signallabel, 4096);

  QDialog *myobjectDialog = new QDialog;
  myobjectDialog->setMinimumSize(300, 215);
  myobjectDialog->setWindowTitle(str);
  myobjectDialog->setModal(true);
  myobjectDialog->setAttribute(Qt::WA_DeleteOnClose, true);

  QLabel *label = new QLabel;

  QPushButton *pushButton1 = new QPushButton;
  pushButton1->setText("Close");

  snprintf(str, 4096,
         " \n"
         "Segment length: %i sec\n"
         " \n"
         "Bandpass filter: %.1f - %.1f Hz\n"
         " \n"
         "Envelope Lowpass filter: %.2f Hz\n"
         " \n",
         param.segment_len,
         param.bp_min_hz,
         param.bp_max_hz,
         param.lp_hz);

  label->setText(str);

  QHBoxLayout *hlayout1 = new QHBoxLayout;
  hlayout1->addStretch(1000);
  hlayout1->addWidget(pushButton1);

  QVBoxLayout *vlayout1 = new QVBoxLayout;
  vlayout1->addWidget(label);
  vlayout1->addStretch(1000);
  vlayout1->addSpacing(20);
  vlayout1->addLayout(hlayout1);

  myobjectDialog->setLayout(vlayout1);

  QObject::connect(pushButton1, SIGNAL(clicked()), myobjectDialog, SLOT(close()));

  myobjectDialog->exec();
}





























