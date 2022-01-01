/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2007 - 2022 Teunis van Beelen
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




#include "pagetime_dialog.h"



UI_Userdefined_timepage_Dialog::UI_Userdefined_timepage_Dialog(QWidget *w_parent)
{
  mainwindow = (UI_Mainwindow *)w_parent;

  set_diplaytime_dialog = new QDialog(w_parent);
  set_diplaytime_dialog->setMinimumSize(250 * mainwindow->w_scaling, 150 * mainwindow->h_scaling);
  set_diplaytime_dialog->setWindowTitle("Set timescale");
  set_diplaytime_dialog->setModal(true);
  set_diplaytime_dialog->setAttribute(Qt::WA_DeleteOnClose, true);

  QLabel *label1 = new QLabel;
  label1->setText("hh:mm:ss:mmm");

  timeEdit1 = new QTimeEdit;
  timeEdit1->setDisplayFormat("hh:mm:ss.zzz");
  timeEdit1->setMinimumTime(QTime(0, 0, 0, 0));

  OKButton = new QPushButton;
  OKButton->setText("OK");

  CloseButton = new QPushButton;
  CloseButton->setText("Cancel");

  QHBoxLayout *hlayout1 = new QHBoxLayout;
  hlayout1->addWidget(OKButton);
  hlayout1->addStretch(1000);
  hlayout1->addWidget(CloseButton);

  QVBoxLayout *vlayout1 = new QVBoxLayout;
  vlayout1->addWidget(label1);
  vlayout1->addWidget(timeEdit1);
  vlayout1->addStretch(1000);
  vlayout1->addSpacing(20);
  vlayout1->addLayout(hlayout1);

  set_diplaytime_dialog->setLayout(vlayout1);

  QObject::connect(CloseButton, SIGNAL(clicked()), set_diplaytime_dialog, SLOT(close()));
  QObject::connect(OKButton,    SIGNAL(clicked()), this,                  SLOT(okbutton_pressed()));

  set_diplaytime_dialog->exec();
}


void UI_Userdefined_timepage_Dialog::okbutton_pressed()
{
  long long milliseconds;

  if(!mainwindow->files_open)
  {
    set_diplaytime_dialog->close();

    return;
  }

  milliseconds = (long long)(timeEdit1->time().hour()) * 3600000LL;
  milliseconds += (long long)(timeEdit1->time().minute()) * 60000LL;
  milliseconds += (long long)(timeEdit1->time().second()) * 1000LL;
  milliseconds += (long long)(timeEdit1->time().msec());

  if(milliseconds < 1)
  {
    return;
  }

  mainwindow->pagetime = milliseconds * (TIME_DIMENSION / 1000);

  mainwindow->setup_viewbuf();

  set_diplaytime_dialog->close();
}





