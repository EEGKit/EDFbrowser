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

  QLabel *aeeg_label,
         *ruler_label;

  QFrame *frame;

  QGridLayout *grid_layout;

  mainwindow = (UI_Mainwindow *)w_parent;

  w_scaling = mainwindow->w_scaling;
  h_scaling = mainwindow->h_scaling;

  param = par;

  is_deleted = 0;

  sigcomp_uid = param.signalcomp->uid;

  snprintf(str, 1024, "aEEG   %s", param.signalcomp->signallabel);

  frame = new QFrame;
  frame->setFrameStyle(QFrame::NoFrame);
  frame->setLineWidth(0);
  frame->setMidLineWidth(0);
  frame->setContentsMargins(0, 0, 0, 0);

  aeeg_label = new QLabel;
  aeeg_label->setScaledContents(true);
  aeeg_label->setMinimumHeight(100 * h_scaling);
  aeeg_label->setMinimumWidth(100 * w_scaling);
  aeeg_label->setContentsMargins(0, 0, 0, 0);

  trck_indic = new a_simple_tracking_indicator;
  trck_indic->set_scaling(w_scaling, h_scaling);
  trck_indic->set_maximum(param.segments_in_recording * param.segment_len);
  trck_indic->setContentsMargins(0, 0, 0, 0);

  srl_indic = new a_simple_ruler_indicator;
  srl_indic->set_scaling(w_scaling, h_scaling);
  srl_indic->set_maximum(param.max_hz);
  srl_indic->set_minimum(param.min_hz);
  srl_indic->set_unit("Hz");
  srl_indic->setContentsMargins(0, 0, 0, 0);

  ruler_label = new QLabel;
  ruler_label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
  ruler_label->setText("Hz / Time");
  ruler_label->setContentsMargins(0, 0, 0, 0);

  grid_layout = new QGridLayout(frame);
  grid_layout->addWidget(srl_indic,  0, 0);
  grid_layout->addWidget(aeeg_label, 0, 1);
  grid_layout->addWidget(trck_indic, 1, 1);
  grid_layout->addWidget(ruler_label, 1, 0);
//   grid_layout->addWidget(color_indic, 0, 2);
//   grid_layout->addWidget(color_indic_label, 1, 2);

  aeeg_dock = new QDockWidget(str, mainwindow);
  aeeg_dock->setFeatures(QDockWidget::AllDockWidgetFeatures);
  aeeg_dock->setAllowedAreas(Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea);
  aeeg_dock->setAttribute(Qt::WA_DeleteOnClose);
  aeeg_dock->setContextMenuPolicy(Qt::CustomContextMenu);
  aeeg_dock->setWidget(frame);

  mainwindow->addDockWidget(Qt::BottomDockWidgetArea, aeeg_dock, Qt::Horizontal);

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


void UI_aeeg_dock::contextmenu_requested(QPoint)
{
//   int ov_lap[5]={0, 50, 67, 75, 80};
//
//   char str[4096]={"CDSA "},
//        wnd_func[13][32]={"Rectangular",
//                     "Hamming",
//                     "4-term Blackman-Harris",
//                     "7-term Blackman-Harris",
//                     "Nuttall3b",
//                     "Nuttall4c",
//                     "Hann",
//                     "HFT223D",
//                     "HFT95",
//                     "Kaiser2",
//                     "Kaiser3",
//                     "Kaiser4",
//                     "Kaiser5"},
//       yesno[2][32]={"no", "yes"};
//
//   strlcat(str, param.signalcomp->signallabel, 4096);
//
//   QDialog *myobjectDialog = new QDialog;
//   myobjectDialog->setMinimumSize(300, 215);
//   myobjectDialog->setWindowTitle(str);
//   myobjectDialog->setModal(true);
//   myobjectDialog->setAttribute(Qt::WA_DeleteOnClose, true);
//
//   QLabel *label = new QLabel;
//
//   QPushButton *pushButton1 = new QPushButton;
//   pushButton1->setText("Close");
//
//   if(param.log)
//   {
//     snprintf(str, 4096,
//            "Segment length: %i sec.\n"
//            "Block length: %i sec.\n"
//            "Overlap: %i %%\n"
//            "Window function: %s\n"
//            "Max. level: %i %s\n"
//            "Min. level: %i %s\n"
//            "Logarithmic: %s\n"
//            "Power: %s",
//            param.segment_len,
//            param.block_len,
//            ov_lap[param.overlap - 1],
//            wnd_func[param.window_func],
//            param.max_pwr, param.unit,
//            param.min_pwr, param.unit,
//            yesno[param.log],
//            yesno[param.power_voltage]);
//   }
//   else
//   {
//     snprintf(str, 4096,
//            "Segment length: %i sec.\n"
//            "Block length: %i sec.\n"
//            "Overlap: %i %%\n"
//            "Window function: %s\n"
//            "Max. level: %f %s\n"
//            "Logarithmic: %s\n"
//            "Power: %s",
//            param.segment_len,
//            param.block_len,
//            ov_lap[param.overlap - 1],
//            wnd_func[param.window_func],
//            param.max_voltage, param.unit,
//            yesno[param.log],
//            yesno[param.power_voltage]);
//   }
//
//   label->setText(str);
//
//   QHBoxLayout *hlayout1 = new QHBoxLayout;
//   hlayout1->addStretch(1000);
//   hlayout1->addWidget(pushButton1);
//
//   QVBoxLayout *vlayout1 = new QVBoxLayout;
//   vlayout1->addWidget(label);
//   vlayout1->addStretch(1000);
//   vlayout1->addSpacing(20);
//   vlayout1->addLayout(hlayout1);
//
//   myobjectDialog->setLayout(vlayout1);
//
//   QObject::connect(pushButton1, SIGNAL(clicked()), myobjectDialog, SLOT(close()));
//
//   myobjectDialog->exec();
}


void UI_aeeg_dock::file_pos_changed(long long)
{
  int i;

  for(i=0; i<mainwindow->signalcomps; i++)
  {
    if(mainwindow->signalcomp[i]->uid == sigcomp_uid)
    {
      if(param.signalcomp == mainwindow->signalcomp[i])
      {
        break;
      }
    }
  }

  if(i == mainwindow->signalcomps)  return;

  trck_indic->set_position((int)((param.signalcomp->edfhdr->viewtime + (mainwindow->pagetime / 2)) / 10000000LL));
}


a_simple_tracking_indicator::a_simple_tracking_indicator(QWidget *w_parent) : QWidget(w_parent)
{
  setAttribute(Qt::WA_OpaquePaintEvent);

  setFixedHeight(16);

  w_scaling = 1;
  h_scaling = 1;

  pos = 0;
  max = 100;
}


void a_simple_tracking_indicator::set_scaling(double w, double h)
{
  w_scaling = w;
  h_scaling = h;

  setFixedHeight(16 * h_scaling);
}


void a_simple_tracking_indicator::set_position(long long pos_)
{
  pos = pos_;

  update();
}


void a_simple_tracking_indicator::set_maximum(long long max_)
{
  max = max_;
}


void a_simple_tracking_indicator::paintEvent(QPaintEvent *)
{
  int i, w, h, pos_x, step=0;

  char str[128]={""};

  w = width();
  h = height();

  QPainter painter(this);

  painter.fillRect(0, 0, w, h, Qt::lightGray);

  painter.setPen(Qt::black);

  if(((double)w / (double)max) > 0.02)
  {
    step = 1;
  }
  else if(((double)w / (double)max) > 0.01)
    {
      step = 2;
    }
    else if(((double)w / (double)max) > 0.005)
      {
        step = 4;
      }

  if(step)
  {
    for(i=0; i<200; i+=step)
    {
      pos_x = (((double)(i * 3600) / (double)max) * (double)w) + 0.5;

      if(pos_x > w)
      {
        break;
      }

      painter.drawLine(pos_x, 0, pos_x, h);

      snprintf(str, 128, "%ih", i);

      painter.drawText(pos_x + 4, h - 2, str);
    }
  }

  draw_small_arrow(&painter, (int)((((double)pos / (double)max) * w) + 0.5), 0, 0, Qt::black);
}


void a_simple_tracking_indicator::draw_small_arrow(QPainter *painter, int xpos, int ypos, int rot, QColor color)
{
  QPainterPath path;

  if(rot == 0)
  {
    path.moveTo(xpos,      ypos);
    path.lineTo(xpos - (8 * w_scaling), ypos + (15 * h_scaling));
    path.lineTo(xpos + (8 * w_scaling), ypos + (15 * h_scaling));
    path.lineTo(xpos,      ypos);

    painter->fillPath(path, color);
  }
}


a_simple_ruler_indicator::a_simple_ruler_indicator(QWidget *w_parent) : QWidget(w_parent)
{
  setAttribute(Qt::WA_OpaquePaintEvent);

  setFixedWidth(60);

  w_scaling = 1;
  h_scaling = 1;

  min = 0;
  max = 100;
}


void a_simple_ruler_indicator::set_scaling(double w, double h)
{
  w_scaling = w;
  h_scaling = h;

  setFixedWidth(60 * h_scaling);
}


void a_simple_ruler_indicator::set_minimum(int val)
{
  min = val;
}


void a_simple_ruler_indicator::set_maximum(int val)
{
  max = val;
}


void a_simple_ruler_indicator::set_unit(const char *str)
{
  strlcpy(unit, str, 32);
}


void a_simple_ruler_indicator::paintEvent(QPaintEvent *)
{
  int i, w, h, range, skip;

  double pixel_per_unit;

  char str[64]={""};

  w = width();
  h = height();

  QPainter painter(this);

  painter.fillRect(0, 0, w, h, Qt::lightGray);

  painter.setPen(Qt::black);

  painter.drawLine(w - 4, 0, w - 4, h);

  range = max - min;

  pixel_per_unit = (double)h / (double)range;

  for(skip=1; skip<100000; )
  {
    if((skip * pixel_per_unit) > 25)  break;

    switch(skip)
    {
      case   1 : skip = 2;
                 break;
      case   2 : skip = 5;
                 break;
      case   5 : skip = 10;
                 break;
      case  10 : skip = 20;
                 break;
      case  20 : skip = 50;
                 break;
      case  50 : skip = 100;
                 break;
      case 100 : skip = 200;
                 break;
      case 200 : skip = 500;
                 break;
      case 500 : skip = 1000;
                 break;
      case 1000 : skip = 2000;
                 break;
      case 2000 : skip = 5000;
                 break;
      case 5000 : skip = 10000;
                 break;
      case 10000 : skip = 20000;
                 break;
      case 20000 : skip = 50000;
                 break;
      case 50000 : skip = 100000;
                 break;
      case 100000 : skip = 200000;
                 break;
      case 200000 : skip = 500000;
                 break;
    }
  }

  for(i=0; i<=range; i++)
  {
    if(!((i + min) % skip))
    {
      painter.drawLine(w - 4, h - (int)((pixel_per_unit * i) + 0.5), w - 13, h - (int)((pixel_per_unit * i) + 0.5));

      snprintf(str, 64, "%i", min + i);

      painter.drawText(QRectF(2, h - (int)((pixel_per_unit * i) + 0.5) - (9 * h_scaling), 40 * w_scaling, 25 * h_scaling), Qt::AlignRight | Qt::AlignHCenter, str);
    }
  }
}





























