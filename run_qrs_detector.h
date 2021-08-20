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


#ifndef UI_QRS_DETECTORFORM_H
#define UI_QRS_DETECTORFORM_H


#include "qt_headers.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "mainwindow.h"
#include "utils.h"
#include "edf_annot_list.h"
#include "edf_helper.h"
#include "pt_qrs.h"
#include "ecg_filter.h"
#include "filteredblockread.h"
#include "hrv_dock.h"



class UI_Mainwindow;



class UI_QRS_detector : public QObject
{
  Q_OBJECT

public:

  UI_QRS_detector(QWidget *parent, struct signalcompblock *signalcomp_);

  UI_Mainwindow *mainwindow;

private:

  double sf;

  struct signalcompblock *signalcomp;

  struct ecg_filter_settings *ecg_filter_bu;

private slots:

};




#endif



