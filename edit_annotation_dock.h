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




#ifndef ANNOTATION_EDIT_FORM1_H
#define ANNOTATION_EDIT_FORM1_H



#include <QtGlobal>
#include <QApplication>
#include <QObject>
#include <QDockWidget>
#include <QByteArray>
#include <QDialog>
#include <QLineEdit>
#include <QTime>
#include <QTimeEdit>
#include <QPushButton>
#include <QLabel>
#include <QString>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QSpinBox>
#include <QToolBar>
#include <QFrame>
#include <QToolTip>
#include <QCompleter>
#include <QStringList>
#include <QStringListModel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFormLayout>
#include <QMessageBox>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "mainwindow.h"
#include "annotations_dock.h"
#include "edf_annot_list.h"
#include "utils.h"



class UI_Mainwindow;




class UI_AnnotationEditwindow : public QObject
{
  Q_OBJECT

public:
  UI_AnnotationEditwindow(struct edfhdrblock *e_hdr, QWidget *parent);
  ~UI_AnnotationEditwindow();

  UI_Mainwindow *mainwindow;

  QToolBar  *dockedit;

  void annotEditSetOnset(long long);

  long long annotEditGetOnset(void);

  void annotEditSetDuration(long long);

  void set_selected_annotation(int);

  void set_edf_header(struct edfhdrblock *);

  void set_selected_annotation(struct annotationblock *);

private:

  int annot_num,
      is_deleted;

  struct edfhdrblock *edf_hdr;

  QFrame *annot_edit_frame;

  QLabel *onsetLabel,
         *durationLabel,
         *descriptionLabel;

  QLineEdit *annot_descript_lineEdit;

  QCompleter *completer;

  QTimeEdit *onset_timeEdit;

  QSpinBox  *onset_daySpinbox;

  QDoubleSpinBox *duration_spinbox;

  QPushButton *modifybutton,
              *deletebutton,
              *createbutton;

  QComboBox *posNegTimebox;

  void update_description_completer(void);

private slots:

  void modifyButtonClicked();
  void deleteButtonClicked();
  void createButtonClicked();

  void dockedit_destroyed(QObject *);
};



#endif




