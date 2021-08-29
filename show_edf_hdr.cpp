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



#include "show_edf_hdr.h"



UI_EDFhdrwindow::UI_EDFhdrwindow(QWidget *w_parent)
{
  int i;

  mainwindow = (UI_Mainwindow *)w_parent;

  EDFhdrDialog = new QDialog;
  EDFhdrDialog->setMinimumSize(600 * mainwindow->w_scaling, 680 * mainwindow->h_scaling);
  EDFhdrDialog->setWindowTitle("EDF header info");
  EDFhdrDialog->setModal(true);
  EDFhdrDialog->setAttribute(Qt::WA_DeleteOnClose, true);
  EDFhdrDialog->setSizeGripEnabled(true);

  filelist = new QListWidget;
  filelist->setMinimumSize(600, 75);
  for(i=0; i<mainwindow->files_open; i++)
  {
    new QListWidgetItem(QString::fromLocal8Bit(mainwindow->edfheaderlist[i]->filename), filelist);
  }

  signallist = new QTableWidget;
  signallist->setSelectionMode(QAbstractItemView::NoSelection);
  signallist->setEditTriggers(QAbstractItemView::NoEditTriggers);

  pushButton1 = new QPushButton;
  pushButton1->setText("Close");

  line_edit1 = new QLineEdit;
  line_edit1->setReadOnly(true);
  line_edit1a = new QLineEdit;
  line_edit1a->setReadOnly(true);
  line_edit2 = new QLineEdit;
  line_edit2->setReadOnly(true);
  line_edit2a = new QLineEdit;
  line_edit2a->setReadOnly(true);
  line_edit3 = new QLineEdit;
  line_edit3->setReadOnly(true);
  line_edit3a = new QLineEdit;
  line_edit3a->setReadOnly(true);
  line_edit4 = new QLineEdit;
  line_edit4->setReadOnly(true);
  line_edit4a = new QLineEdit;
  line_edit4a->setReadOnly(true);
  line_edit5 = new QLineEdit;
  line_edit5->setReadOnly(true);
  line_edit5a = new QLineEdit;
  line_edit5a->setReadOnly(true);
  line_edit6 = new QLineEdit;
  line_edit6->setReadOnly(true);
  line_edit7 = new QLineEdit;
  line_edit7->setReadOnly(true);
  line_edit8 = new QLineEdit;
  line_edit8->setReadOnly(true);
  line_edit9 = new QLineEdit;
  line_edit9->setReadOnly(true);
  line_edit10 = new QLineEdit;
  line_edit10->setReadOnly(true);
  line_edit11 = new QLineEdit;
  line_edit11->setReadOnly(true);
  line_edit12 = new QLineEdit;
  line_edit12->setReadOnly(true);
  line_edit13 = new QLineEdit;
  line_edit13->setReadOnly(true);
  line_edit20 = new QLineEdit;
  line_edit20->setReadOnly(true);
  line_edit20a = new QLineEdit;
  line_edit20a->setReadOnly(true);
  line_edit21 = new QLineEdit;
  line_edit21->setReadOnly(true);
  line_edit21a = new QLineEdit;
  line_edit21a->setReadOnly(true);
  line_edit22 = new QLineEdit;
  line_edit22->setReadOnly(true);
  line_edit22a = new QLineEdit;
  line_edit22a->setReadOnly(true);

  QFormLayout *flayout1 = new QFormLayout;
  flayout1->addRow("Subject code", line_edit1);
  flayout1->addRow("Sex", line_edit2);
  flayout1->addRow("Start", line_edit3);
  flayout1->addRow("Duration", line_edit4);
  flayout1->addRow("Reserved", line_edit5);
  flayout1->addRow("Birthdate", line_edit6);
  flayout1->addRow("Subject name", line_edit7);
  flayout1->addRow("Additional info", line_edit8);
  flayout1->addRow("Startdate", line_edit9);
  flayout1->addRow("Administr. code", line_edit10);
  flayout1->addRow("Technician", line_edit11);
  flayout1->addRow("Device", line_edit12);
  flayout1->addRow("Additional info", line_edit13);
  flayout1->addRow("Datarecord duration", line_edit20);
  flayout1->addRow("Datarecords", line_edit22);
  flayout1->addRow("Version", line_edit21);

  QFormLayout *flayout2 = new QFormLayout;
  flayout2->addRow("Subject", line_edit1a);
  flayout2->addRow("Recording", line_edit2a);
  flayout2->addRow("Start", line_edit3a);
  flayout2->addRow("Duration", line_edit4a);
  flayout2->addRow("Reserved", line_edit5a);
  flayout2->addRow("Datarecord duration", line_edit20a);
  flayout2->addRow("Datarecords", line_edit22a);
  flayout2->addRow("Version", line_edit21a);

  QVBoxLayout *vlayout2 = new QVBoxLayout;
  vlayout2->addLayout(flayout1);
  vlayout2->addStretch(1000);

  QVBoxLayout *vlayout3 = new QVBoxLayout;
  vlayout3->addLayout(flayout2);
  vlayout3->addStretch(1000);

  tab1 = new QWidget;
  tab1->setLayout(vlayout2);

  tab1a = new QWidget;
  tab1a->setLayout(vlayout3);

  stackwidget = new QStackedWidget;
  stackwidget->addWidget(tab1);
  stackwidget->addWidget(tab1a);

  QTabWidget *tabholder = new QTabWidget;

  tabholder->addTab(stackwidget, "General");
  tabholder->addTab(signallist, "Signals");

  QHBoxLayout *hlayout1 = new QHBoxLayout;
  hlayout1->addStretch(1000);
  hlayout1->addWidget(pushButton1);

  QVBoxLayout *vlayout1 = new QVBoxLayout;
  vlayout1->addWidget(filelist);
  vlayout1->addSpacing(20);
  vlayout1->addWidget(tabholder, 1000);
  vlayout1->addSpacing(20);
  vlayout1->addLayout(hlayout1);

  EDFhdrDialog->setLayout(vlayout1);

  QObject::connect(pushButton1, SIGNAL(clicked()),              EDFhdrDialog, SLOT(close()));
  QObject::connect(filelist,    SIGNAL(currentRowChanged(int)), this,         SLOT(show_params(int)));

  filelist->setCurrentRow(mainwindow->files_open - 1);

  EDFhdrDialog->exec();
}


void UI_EDFhdrwindow::show_params(int row)
{
  int i,
      signal_cnt;

  char str[512]={""};

  long long file_duration;

  struct date_time_struct date_time;


  if(row<0)  return;

  if(mainwindow->edfheaderlist[row]->edfplus || mainwindow->edfheaderlist[row]->bdfplus)
  {
    stackwidget->setCurrentIndex(0);

    line_edit1->setText(mainwindow->edfheaderlist[row]->plus_patientcode);
    line_edit2->setText(mainwindow->edfheaderlist[row]->plus_gender);
    line_edit6->setText(mainwindow->edfheaderlist[row]->plus_birthdate);
    line_edit7->setText(mainwindow->edfheaderlist[row]->plus_patient_name);
    line_edit8->setText(mainwindow->edfheaderlist[row]->plus_patient_additional);
    line_edit9->setText(mainwindow->edfheaderlist[row]->plus_startdate);
    line_edit10->setText(mainwindow->edfheaderlist[row]->plus_admincode);
    line_edit11->setText(mainwindow->edfheaderlist[row]->plus_technician);
    line_edit12->setText(mainwindow->edfheaderlist[row]->plus_equipment);
    line_edit13->setText(mainwindow->edfheaderlist[row]->plus_recording_additional);
  }
  else // old EDF
  {
    stackwidget->setCurrentIndex(1);

    line_edit1a->setText(mainwindow->edfheaderlist[row]->patient);
    line_edit2a->setText(mainwindow->edfheaderlist[row]->recording);
  }

  utc_to_date_time(mainwindow->edfheaderlist[row]->utc_starttime, &date_time);

  date_time.month_str[0] += 32;
  date_time.month_str[1] += 32;
  date_time.month_str[2] += 32;

  snprintf(str, 400, "%i %s %i  %2i:%02i:%02i",
          date_time.day,
          date_time.month_str,
          date_time.year,
          date_time.hour,
          date_time.minute,
          date_time.second);

  if(mainwindow->edfheaderlist[row]->starttime_offset != 0LL)
  {
#ifdef Q_OS_WIN32
    __mingw_snprintf(str + strlen(str), 100, ".%07lli", mainwindow->edfheaderlist[row]->starttime_offset);
#else
    snprintf(str + strlen(str), 100, ".%07lli", mainwindow->edfheaderlist[row]->starttime_offset);
#endif

    remove_trailing_zeros(str);
  }

  line_edit3->setText(str);
  line_edit3a->setText(str);

  file_duration = mainwindow->edfheaderlist[row]->long_data_record_duration * mainwindow->edfheaderlist[row]->datarecords;

  if((file_duration / TIME_DIMENSION) / 10)
  {
    snprintf(str, 512,
            "%2i:%02i:%02i",
            (int)((file_duration / TIME_DIMENSION)/ 3600LL),
            (int)(((file_duration / TIME_DIMENSION) % 3600LL) / 60LL),
            (int)((file_duration / TIME_DIMENSION) % 60LL));
  }
  else
  {
    snprintf(str, 512,
            "%2i:%02i:%02i.%06i",
            (int)((file_duration / TIME_DIMENSION)/ 3600LL),
            (int)(((file_duration / TIME_DIMENSION) % 3600LL) / 60LL),
            (int)((file_duration / TIME_DIMENSION) % 60LL),
            (int)((file_duration % TIME_DIMENSION) / 10LL));
  }

  line_edit4->setText(str);
  line_edit4a->setText(str);

  line_edit5->setText(mainwindow->edfheaderlist[row]->reserved);
  line_edit5a->setText(mainwindow->edfheaderlist[row]->reserved);

  snprintf(str,  512, "%.12f", mainwindow->edfheaderlist[row]->data_record_duration);

  remove_trailing_zeros(str);

  line_edit20->setText(str);
  line_edit20a->setText(str);

#ifdef Q_OS_WIN32
  __mingw_snprintf(str,  512, "%lli", mainwindow->edfheaderlist[row]->datarecords);
#else
  snprintf(str,  512, "%lli", mainwindow->edfheaderlist[row]->datarecords);
#endif

  line_edit22->setText(str);
  line_edit22a->setText(str);

  line_edit21->setText(mainwindow->edfheaderlist[row]->version);
  line_edit21a->setText(mainwindow->edfheaderlist[row]->version);

  signal_cnt = mainwindow->edfheaderlist[row]->edfsignals;

  signallist->setColumnCount(10);
  signallist->setRowCount(signal_cnt);
  signallist->setSelectionMode(QAbstractItemView::NoSelection);
  QStringList horizontallabels;
  horizontallabels += "Label";
  horizontallabels += "Samplefrequency";
  horizontallabels += "Physical maximum";
  horizontallabels += "Physical minimum";
  horizontallabels += "Physical dimension";
  horizontallabels += "Digital maximum";
  horizontallabels += "Digital minimum";
  horizontallabels += "Samples per record";
  horizontallabels += "Prefilter";
  horizontallabels += "Transducer";
  signallist->setHorizontalHeaderLabels(horizontallabels);

  QLineEdit *ql;

  for(i=0; i<signal_cnt; i++)
  {
    ql = new QLineEdit(mainwindow->edfheaderlist[row]->edfparam[i].label);
    ql->setReadOnly(true);
    ql->setCursorPosition(0);
    signallist->setCellWidget(i, 0, ql);
    convert_to_metric_suffix(str, mainwindow->edfheaderlist[row]->edfparam[i].sf_f, 3, 512);
    strlcat(str, "Hz", 512);
    remove_trailing_zeros(str);
    ql = new QLineEdit(str);
    ql->setReadOnly(true);
    ql->setCursorPosition(0);
    signallist->setCellWidget(i, 1, ql);
    snprintf(str, 512, "%+f", mainwindow->edfheaderlist[row]->edfparam[i].phys_max);
    remove_trailing_zeros(str);
    ql = new QLineEdit(str);
    ql->setReadOnly(true);
    ql->setCursorPosition(0);
    signallist->setCellWidget(i, 2, ql);
    snprintf(str, 512, "%+f", mainwindow->edfheaderlist[row]->edfparam[i].phys_min);
    remove_trailing_zeros(str);
    ql = new QLineEdit(str);
    ql->setReadOnly(true);
    ql->setCursorPosition(0);
    signallist->setCellWidget(i, 3, ql);
    ql = new QLineEdit(mainwindow->edfheaderlist[row]->edfparam[i].physdimension);
    ql->setReadOnly(true);
    ql->setCursorPosition(0);
    signallist->setCellWidget(i, 4, ql);
    snprintf(str, 512, "%+i", mainwindow->edfheaderlist[row]->edfparam[i].dig_max);
    ql = new QLineEdit(str);
    ql->setReadOnly(true);
    ql->setCursorPosition(0);
    signallist->setCellWidget(i, 5, ql);
    snprintf(str, 512, "%+i", mainwindow->edfheaderlist[row]->edfparam[i].dig_min);
    ql = new QLineEdit(str);
    ql->setReadOnly(true);
    ql->setCursorPosition(0);
    signallist->setCellWidget(i, 6, ql);
    snprintf(str, 512, "%i", mainwindow->edfheaderlist[row]->edfparam[i].smp_per_record);
    ql = new QLineEdit(str);
    ql->setReadOnly(true);
    ql->setCursorPosition(0);
    signallist->setCellWidget(i, 7, ql);
    ql = new QLineEdit(mainwindow->edfheaderlist[row]->edfparam[i].prefilter);
    ql->setReadOnly(true);
    ql->setCursorPosition(0);
    signallist->setCellWidget(i, 8, ql);
    ql = new QLineEdit(mainwindow->edfheaderlist[row]->edfparam[i].transducer);
    ql->setReadOnly(true);
    ql->setCursorPosition(0);
    signallist->setCellWidget(i, 9, ql);
  }

  signallist->resizeColumnsToContents();
}

















