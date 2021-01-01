/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2007 - 2021 Teunis van Beelen
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




#ifndef SHOWACTUALMONTAGEFORM1_H
#define SHOWACTUALMONTAGEFORM1_H



#include <QtGlobal>
#include <QApplication>
#include <QObject>
#include <QDialog>
#include <QPushButton>
#include <QTreeView>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QPixmap>
#include <QIcon>
#include <QColor>
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



class UI_ShowActualMontagewindow : public QObject
{
  Q_OBJECT

public:
  UI_ShowActualMontagewindow(QWidget *parent);

  UI_Mainwindow *mainwindow;

private:

  QDialog      *ShowMontageDialog;

  QPushButton  *CloseButton;

  QBoxLayout   *box;

  QTreeView    *tree;

  QStandardItemModel *t_model;

};



#endif // SHOWACTUALMONTAGEFORM1_H


