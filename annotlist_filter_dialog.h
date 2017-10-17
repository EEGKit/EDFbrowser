/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2017 Teunis van Beelen
*
* Email: teuniz@gmail.com
*
***************************************************************************
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
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




#ifndef ANNOTFILTER_FORM1_H
#define ANNOTFILTER_FORM1_H



#include <QtGlobal>
#include <QApplication>
#include <QObject>
#include <QListWidget>
#include <QListWidgetItem>
#include <QDialog>
#include <QPushButton>
#include <QLabel>
#include <QProgressDialog>
#include <QComboBox>
#include <QList>
#include <QTime>
#include <QTimeEdit>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QToolTip>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "mainwindow.h"
#include "edf_annot_list.h"
#include "utils.h"
#include "annotations_dock.h"



class UI_Mainwindow;

class UI_Annotationswindow;



class UI_AnnotFilterWindow : public QObject
{
  Q_OBJECT

public:
  UI_AnnotFilterWindow(QWidget *, struct annotationblock *, struct annot_filter_struct *);

  UI_Mainwindow *mainwindow;


private:

QDialog     *annot_filter_dialog;

QLabel      *annotLabel,
            *annotNameLabel,
            *t1Label,
            *t2Label;

QPushButton *CloseButton,
            *ApplyButton;

QListWidget *list;

// QComboBox   *ratioBox;
//
// QTimeEdit    *timeEdit1,
//              *timeEdit2;
//
// QTime        time1,
//              time2;

QDoubleSpinBox *t1_dspinbox,
               *t2_dspinbox;

QCheckBox *hide_in_list_checkbox,
          *hide_other_checkbox,
          *invert_checkbox;

char sel_annot_str[MAX_ANNOTATION_LEN + 1];

struct annotation_list *annot_list;

struct annotationblock *sel_annot_ptr;

struct annot_filter_struct *filter_params;

UI_Annotationswindow *annots_dock;

private slots:

void apply_filter();
void t1_dspinbox_changed(double);
void t2_dspinbox_changed(double);
void hide_other_checkbox_changed(int);
void hide_in_list_checkbox_changed(int);
void invert_checkbox_changed(int);

};



#endif // ANNOTFILTER_FORM1_H










