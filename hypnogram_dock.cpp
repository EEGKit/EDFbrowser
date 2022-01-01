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



#include "hypnogram_dock.h"


UI_hypnogram_dock::UI_hypnogram_dock(QWidget *w_parent, struct hypnogram_dock_param_struct par)
{
  QLabel *ruler_label;

  QFrame *frame;

  QGridLayout *grid_layout;

  mainwindow = (UI_Mainwindow *)w_parent;

  w_scaling = mainwindow->w_scaling;
  h_scaling = mainwindow->h_scaling;

  param = par;

  is_deleted = 0;

  frame = new QFrame;
  frame->setFrameStyle(QFrame::NoFrame);
  frame->setLineWidth(0);
  frame->setMidLineWidth(0);
  frame->setContentsMargins(0, 0, 0, 0);

  hypnogram_curve = new hypnogram_curve_widget;
  hypnogram_curve->setMinimumHeight(100 * h_scaling);
  hypnogram_curve->setMinimumWidth(100 * w_scaling);
  hypnogram_curve->setContentsMargins(0, 0, 0, 0);
  hypnogram_curve->set_params(&param);

  trck_indic = new simple_tracking_indicator2;
  trck_indic->set_scaling(w_scaling, h_scaling);
  trck_indic->set_maximum(param.edfhdr->recording_len_sec);
  trck_indic->setContentsMargins(0, 0, 0, 0);

  srl_indic = new simple_ruler_indicator2;
  srl_indic->set_scaling(w_scaling, h_scaling);
  srl_indic->set_params(&param);
  srl_indic->setContentsMargins(0, 0, 0, 0);
  srl_indic->setMinimumWidth(80 * w_scaling);

  ruler_label = new QLabel;
  ruler_label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
  ruler_label->setText("Stage / Time");
  ruler_label->setContentsMargins(0, 0, 0, 0);

  grid_layout = new QGridLayout(frame);
  grid_layout->addWidget(srl_indic,  0, 0);
  grid_layout->addWidget(hypnogram_curve, 0, 1);
  grid_layout->addWidget(ruler_label, 1, 0);
  grid_layout->addWidget(trck_indic, 1, 1);
  grid_layout->setColumnStretch(1, 100);

  hypnogram_dock = new QToolBar("Hypnogram", mainwindow);
  hypnogram_dock->setOrientation(Qt::Horizontal);
  hypnogram_dock->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);
  hypnogram_dock->addWidget(frame);
  hypnogram_dock->setAttribute(Qt::WA_DeleteOnClose, true);

  QObject::connect(mainwindow,     SIGNAL(file_position_changed(long long)), this, SLOT(file_pos_changed(long long)));
  QObject::connect(mainwindow,     SIGNAL(file_position_changed(long long)), this, SLOT(file_pos_changed(long long)));
  QObject::connect(hypnogram_dock, SIGNAL(destroyed(QObject *)),             this, SLOT(hypnogram_dock_destroyed(QObject *)));

  file_pos_changed(0);
}


UI_hypnogram_dock::~UI_hypnogram_dock()
{
  if(!is_deleted)
  {
    is_deleted = 1;

    mainwindow->removeToolBar(hypnogram_dock);

    param.edfhdr->hypnogram_dock[param.instance_num] = 0;

    mainwindow->hypnogram_dock[param.instance_num] = NULL;
  }
}


void UI_hypnogram_dock::update_curve(void)
{
  hypnogram_curve->update();
}


void UI_hypnogram_dock::hypnogram_dock_destroyed(QObject *)
{
  if(!is_deleted)
  {
    is_deleted = 1;

    param.edfhdr->hypnogram_dock[param.instance_num] = 0;

    mainwindow->hypnogram_dock[param.instance_num] = NULL;
  }

  delete this;
}


void UI_hypnogram_dock::file_pos_changed(long long)
{
  trck_indic->set_position((int)((param.edfhdr->viewtime + (mainwindow->pagetime / 2)) / 10000000LL));
}


simple_tracking_indicator2::simple_tracking_indicator2(QWidget *w_parent) : QWidget(w_parent)
{
  setAttribute(Qt::WA_OpaquePaintEvent);

  setFixedHeight(16);

  pos = 0;
  max = 100;
}


void simple_tracking_indicator2::set_scaling(double w, double h)
{
  w_scaling = w;
  h_scaling = h;

  setFixedHeight(16 * h_scaling);
}


void simple_tracking_indicator2::set_position(long long pos_)
{
  pos = pos_;

  update();
}


void simple_tracking_indicator2::set_maximum(long long max_)
{
  max = max_;
}


void simple_tracking_indicator2::paintEvent(QPaintEvent *)
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


void simple_tracking_indicator2::draw_small_arrow(QPainter *painter, int xpos, int ypos, int rot, QColor color)
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


simple_ruler_indicator2::simple_ruler_indicator2(QWidget *w_parent) : QWidget(w_parent)
{
  setAttribute(Qt::WA_OpaquePaintEvent);

  setFixedWidth(60);
}


void simple_ruler_indicator2::set_scaling(double w, double h)
{
  w_scaling = w;
  h_scaling = h;

  setFixedWidth(60 * w_scaling);
}


void simple_ruler_indicator2::set_params(struct hypnogram_dock_param_struct *parms)
{
  param = *parms;
}


void simple_ruler_indicator2::paintEvent(QPaintEvent *)
{
  int i, w, h;

  double pixel_per_unit,
         offset;

  w = width();
  h = height();

  QPainter painter(this);

  painter.fillRect(0, 0, w, h, Qt::lightGray);

  painter.setPen(Qt::black);

  pixel_per_unit = (double)h / 6.0;

  offset = (double)h / 12.0;

  painter.drawLine(w - 4, offset, w - 4, h - offset);

  for(i=0; i<6; i++)
  {
    painter.drawLine(w - 4, (int)((pixel_per_unit * i) + 0.5 + offset), w - 13, (int)((pixel_per_unit * i) + 0.5 + offset));

    painter.drawText(QRectF(2, (int)((pixel_per_unit * i) + 0.5 + offset) - (9 * h_scaling), 60 * w_scaling, 25 * h_scaling), Qt::AlignRight | Qt::AlignHCenter, param.stage_name[i]);
  }
}


hypnogram_curve_widget::hypnogram_curve_widget(QWidget *w_parent) : QWidget(w_parent)
{
  mainwindow = NULL;

  setAttribute(Qt::WA_OpaquePaintEvent);
}


void hypnogram_curve_widget::set_params(struct hypnogram_dock_param_struct *parms)
{
  param = *parms;

  mainwindow = param.mainwindow;
}


void hypnogram_curve_widget::paintEvent(QPaintEvent *)
{
  int w, h, i, j, n, pos_x1=0, pos_x2=0, stage=0, tmp1, tmp2;

  double pixel_per_unit,
         pixel_per_sec,
         offset;

  long long annot_duration=-1LL,
            annot_end=0LL,
            prev_annot_end=0x7fffffffffffffffLL,
            tdiff=0LL;

  char str[64];

  struct annotation_list *annot_list;

  struct annotationblock *annot;

  w = width();
  h = height();

  QPainter painter(this);

  pixel_per_unit = ((double)h / 6.0);

  offset = ((double)h / 12.0);

  tmp1 = pixel_per_unit + 0.5;
  painter.fillRect(0,    0, w, tmp1, QColor(255, 255, 217));
  tmp2 = pixel_per_unit * 2.0 + 0.5;
  painter.fillRect(0, tmp1, w, tmp2 - tmp1, QColor(217, 255, 217));
  tmp1 = pixel_per_unit * 3.0 + 0.5;
  painter.fillRect(0, tmp2, w, tmp1 - tmp2, QColor(231, 248, 255));
  tmp2 = pixel_per_unit * 4.0 + 0.5;
  painter.fillRect(0, tmp1, w, tmp2 - tmp1, QColor(213, 237, 255));
  tmp1 = pixel_per_unit * 5.0 + 0.5;
  painter.fillRect(0, tmp2, w, tmp1 - tmp2, QColor(186, 225, 255));
  tmp2 = pixel_per_unit * 6.0 + 0.5;
  painter.fillRect(0, tmp1, w, tmp2 - tmp1, QColor(160, 210, 255));

  painter.setPen(Qt::blue);

  if(mainwindow == NULL)  return;

  pixel_per_sec = (double)w / (double)(param.edfhdr->recording_len_sec);

  annot_list = &param.edfhdr->annot_list;

  n = edfplus_annotation_size(annot_list);

  for(i=0; i<n; i++)
  {
    annot = edfplus_annotation_get_item(annot_list, i);

    strlcpy(str, annot->description, 48);
    trim_spaces(str);

    for(j=0; j<6; j++)
    {
      if(!strcmp(str, param.annot_name[j]))
      {
        pos_x2 = ((double)(annot->onset) * pixel_per_sec) / TIME_DIMENSION;

        annot_duration = annot->long_duration;

        if(mainwindow->hypnogram_use_epoch_len)
        {
          annot_end = annot->onset + annot->long_duration;

          pos_x1 = ((double)(annot_end) * pixel_per_sec) / TIME_DIMENSION;

          painter.drawLine(pos_x1, offset + (j * pixel_per_unit) + 0.5,
                           pos_x2, offset + (j * pixel_per_unit) + 0.5);
        }
        else if(i && (!mainwindow->hypnogram_use_epoch_len))
          {
            painter.drawLine(pos_x1, offset + (stage * pixel_per_unit) + 0.5,
                             pos_x2, offset + (stage * pixel_per_unit) + 0.5);

            pos_x1 = pos_x2;
          }

        if(i)
        {
          painter.drawLine(pos_x2, offset + (stage * pixel_per_unit) + 0.5,
                           pos_x2, offset + (j * pixel_per_unit) + 0.5);
        }

        if(mainwindow->hypnogram_use_epoch_len)
        {
          if(prev_annot_end != 0x7fffffffffffffff)
          {
            tdiff = annot->onset - prev_annot_end;

            if(tdiff > mainwindow->hypnogram_epoch_len_threshold)
            {
              painter.fillRect((int)(((double)(prev_annot_end) * pixel_per_sec) / TIME_DIMENSION),
                               0,
                               ((int)(((double)(tdiff) * pixel_per_sec) / TIME_DIMENSION)) + 2,
                               h,
                               QColor(128, 0, 0, 48));
            }
            else if(tdiff < -mainwindow->hypnogram_epoch_len_threshold)
              {
                painter.fillRect((int)(((double)(prev_annot_end) * pixel_per_sec) / TIME_DIMENSION),
                                 0,
                                 ((int)(((double)(tdiff) * pixel_per_sec) / TIME_DIMENSION)) - 2,
                                 h,
                                 QColor(128, 0, 0, 48));
              }
          }

          prev_annot_end = annot_end;
        }

        stage = j;

        break;
      }
    }
  }

  if(!mainwindow->hypnogram_use_epoch_len)
  {
    if(pos_x1)
    {
      if(annot_duration > 0)
      {
        pos_x2 = (((double)annot_duration * pixel_per_sec) / TIME_DIMENSION) + pos_x1;

        painter.drawLine(pos_x1, offset + (stage * pixel_per_unit) + 0.5,
                         pos_x2, offset + (stage * pixel_per_unit) + 0.5);
      }
      else
      {
        painter.drawLine(pos_x1, offset + (stage * pixel_per_unit) + 0.5,
                         w, offset + (stage * pixel_per_unit) + 0.5);
      }
    }
  }
}





























