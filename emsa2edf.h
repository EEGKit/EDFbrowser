/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2009 - 2021 Teunis van Beelen
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


#ifndef UI_EMSA2EDFFORM_H
#define UI_EMSA2EDFFORM_H


#include <QtGlobal>
#include <QApplication>
#include <QDialog>
#include <QPushButton>
#include <QObject>
#include <QTextEdit>
#include <QFileDialog>
#include <QCursor>
#include <QProgressDialog>
#include <QString>
#include <QByteArray>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFormLayout>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "mainwindow.h"
#include "utils.h"


class UI_Mainwindow;


class UI_EMSA2EDFwindow : public QObject
{
  Q_OBJECT

public:
  UI_EMSA2EDFwindow(QWidget *parent, char *recent_dir=NULL, char *save_dir=NULL);

  UI_Mainwindow *mainwindow;

private:


QPushButton  *pushButton1,
             *pushButton2;

QTextEdit    *textEdit1;

QDialog      *myobjectDialog;

char  *recent_opendir,
      *recent_savedir;



struct event_struct{
         int onset;
         int duration;
         char description[80];
       } *logbuf;


int get_string(char *, FILE *, int, int);

private slots:

void SelectFileButton();

};




#endif


