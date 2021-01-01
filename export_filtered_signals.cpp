/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2017 - 2021 Teunis van Beelen
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



#include "export_filtered_signals.h"



UI_ExportFilteredSignalsWindow::UI_ExportFilteredSignalsWindow(QWidget *w_parent)
{
  mainwindow = (UI_Mainwindow *)w_parent;

  recent_savedir = mainwindow->recent_savedir;

  myobjectDialog = new QDialog;

  myobjectDialog->setMinimumSize(600 * mainwindow->w_scaling, 500 * mainwindow->h_scaling);
  myobjectDialog->setWindowTitle("Export Filtered Signals");
  myobjectDialog->setModal(true);
  myobjectDialog->setAttribute(Qt::WA_DeleteOnClose, true);

  tree = new QTreeView;
  tree->setHeaderHidden(true);
  tree->setSelectionMode(QAbstractItemView::NoSelection);
  tree->setEditTriggers(QAbstractItemView::NoEditTriggers);
  tree->setSortingEnabled(false);
  tree->setDragDropMode(QAbstractItemView::NoDragDrop);
  tree->setAlternatingRowColors(true);

  t_model = new QStandardItemModel(this);

  label1 = new QLabel;

  label2 = new QLabel;
  label2->setText("from datarecord");
  label2->setEnabled(false);

  label3 = new QLabel;
  label3->setText("to datarecord");
  label3->setEnabled(false);

  label4 = new QLabel;
  label4->setEnabled(false);

  label5 = new QLabel;
  label5->setEnabled(false);

  radioButton1 = new QRadioButton("whole duration", myobjectDialog);
  radioButton1->setChecked(true);
  radioButton1->setEnabled(false);

  radioButton2 = new QRadioButton("selection", myobjectDialog);
  radioButton2->setEnabled(false);

  spinBox1 = new QSpinBox;
  spinBox1->setRange(1, 2147483647);
  spinBox1->setValue(1);
  spinBox1->setEnabled(false);

  spinBox2 = new QSpinBox;
  spinBox2->setRange(1, 2147483647);
  spinBox2->setValue(2147483647);
  spinBox2->setEnabled(false);

  pushButton1 = new QPushButton;
  pushButton1->setText("Select File");
  if(mainwindow->files_open < 2)
  {
    pushButton1->setEnabled(false);
  }

  pushButton2 = new QPushButton;
  pushButton2->setText("Close");

  pushButton3 = new QPushButton;
  pushButton3->setText("Export");
  pushButton3->setEnabled(false);

  QVBoxLayout *vlayout4 = new QVBoxLayout;
  vlayout4->addStretch(1000);
  vlayout4->addWidget(label4);
  vlayout4->addStretch(1000);
  vlayout4->addWidget(label5);

  QVBoxLayout *vlayout3 = new QVBoxLayout;
  vlayout3->addWidget(label2);
  vlayout3->addWidget(spinBox1);
  vlayout3->addWidget(label3);
  vlayout3->addWidget(spinBox2);

  QHBoxLayout *hlayout3 = new QHBoxLayout;
  hlayout3->addLayout(vlayout3);
  hlayout3->addLayout(vlayout4);

  QVBoxLayout *vlayout2 = new QVBoxLayout;
  vlayout2->addStretch(1000);
  vlayout2->addWidget(radioButton1);
  vlayout2->addWidget(radioButton2);
  vlayout2->addStretch(100);
  vlayout2->addLayout(hlayout3);
  vlayout2->addStretch(400);

  QHBoxLayout *hlayout2 = new QHBoxLayout;
  hlayout2->addWidget(tree, 1000);
  hlayout2->addLayout(vlayout2);

  QHBoxLayout *hlayout1 = new QHBoxLayout;
  hlayout1->addWidget(pushButton1);
  hlayout1->addStretch(1000);
  hlayout1->addWidget(pushButton3);
  hlayout1->addStretch(1000);
  hlayout1->addWidget(pushButton2);

  QVBoxLayout *vlayout1 = new QVBoxLayout;
  vlayout1->addWidget(label1);
  vlayout1->addLayout(hlayout2, 1000);
  vlayout1->addSpacing(20);
  vlayout1->addLayout(hlayout1);

  myobjectDialog->setLayout(vlayout1);

  QObject::connect(pushButton1,  SIGNAL(clicked()),         this,           SLOT(SelectFileButton()));
  QObject::connect(pushButton2,  SIGNAL(clicked()),         myobjectDialog, SLOT(close()));
  QObject::connect(pushButton3,  SIGNAL(clicked()),         this,           SLOT(StartExport()));
  QObject::connect(spinBox1,     SIGNAL(valueChanged(int)), this,           SLOT(spinBox1changed(int)));
  QObject::connect(spinBox2,     SIGNAL(valueChanged(int)), this,           SLOT(spinBox2changed(int)));
  QObject::connect(radioButton1, SIGNAL(toggled(bool)),     this,           SLOT(radioButton1Toggled(bool)));
  QObject::connect(radioButton2, SIGNAL(toggled(bool)),     this,           SLOT(radioButton2Toggled(bool)));

  edfhdr = NULL;
  inputfile = NULL;
  outputfile = NULL;
  file_num = -1;

  inputpath[0] = 0;

  if(mainwindow->files_open == 1)
  {
    SelectFileButton();
  }

  myobjectDialog->exec();
}


void UI_ExportFilteredSignalsWindow::spinBox1changed(int value)
{
  long long seconds,
            milliSec;

  int days;

  char scratchpad[256];

  QObject::disconnect(spinBox2, SIGNAL(valueChanged(int)), this, SLOT(spinBox2changed(int)));
  spinBox2->setMinimum(value);
  QObject::connect(spinBox2,    SIGNAL(valueChanged(int)), this, SLOT(spinBox2changed(int)));

  if(edfhdr == NULL)
  {
    return;
  }
  days = (int)((((value - 1) * edfhdr->long_data_record_duration) / TIME_DIMENSION) / 86400LL);
  seconds = ((value - 1) * edfhdr->long_data_record_duration) / TIME_DIMENSION;
  if(seconds < 0)
  {
    seconds = 0;
  }
  seconds %= 86400LL;
  milliSec = ((value - 1) * edfhdr->long_data_record_duration) % TIME_DIMENSION;
  milliSec /= 10000LL;
  if(days)
  {
    snprintf(scratchpad, 256, "%id %i:%02i:%02i.%03i", days, (int)(seconds / 3600), (int)((seconds % 3600) / 60), (int)(seconds % 60), (int)milliSec);
  }
  else
  {
    snprintf(scratchpad, 256, "%i:%02i:%02i.%03i", (int)(seconds / 3600), (int)((seconds % 3600) / 60), (int)(seconds % 60), (int)milliSec);
  }
  label4->setText(scratchpad);
}


void UI_ExportFilteredSignalsWindow::spinBox2changed(int value)
{
  long long seconds,
            milliSec;

  int days;

  char scratchpad[256];

  QObject::disconnect(spinBox1, SIGNAL(valueChanged(int)), this, SLOT(spinBox1changed(int)));
  spinBox1->setMaximum(value);
  QObject::connect(spinBox1,    SIGNAL(valueChanged(int)), this, SLOT(spinBox1changed(int)));

  if(edfhdr == NULL)
  {
    return;
  }
  days = (int)(((value * edfhdr->long_data_record_duration) / TIME_DIMENSION) / 86400LL);
  seconds = (value * edfhdr->long_data_record_duration) / TIME_DIMENSION;
  seconds %= 86400LL;
  milliSec = (value * edfhdr->long_data_record_duration) % TIME_DIMENSION;
  milliSec /= 10000LL;
  if(days)
  {
    snprintf(scratchpad, 256, "%id %i:%02i:%02i.%03i", days, (int)(seconds / 3600), (int)((seconds % 3600) / 60), (int)(seconds % 60), (int)milliSec);
  }
  else
  {
    snprintf(scratchpad, 256, "%i:%02i:%02i.%03i", (int)(seconds / 3600), (int)((seconds % 3600) / 60), (int)(seconds % 60), (int)milliSec);
  }
  label5->setText(scratchpad);
}


void UI_ExportFilteredSignalsWindow::radioButton1Toggled(bool checked)
{
  long long seconds,
            milliSec;

  int days;

  char scratchpad[256];

  if(checked == true)
  {
    spinBox1->setEnabled(false);
    spinBox2->setEnabled(false);
    label2->setEnabled(false);
    label3->setEnabled(false);
    label4->setEnabled(false);
    label5->setEnabled(false);

    if(edfhdr == NULL)
    {
      return;
    }
    spinBox1->setValue(1);
    spinBox2->setMaximum(edfhdr->datarecords);
    spinBox2->setValue(edfhdr->datarecords);
    spinBox1->setMaximum(edfhdr->datarecords);

    days = (int)(((edfhdr->datarecords * edfhdr->long_data_record_duration) / TIME_DIMENSION) / 86400LL);
    seconds = (edfhdr->datarecords * edfhdr->long_data_record_duration) / TIME_DIMENSION;
    seconds %= 86400LL;
    milliSec = (edfhdr->datarecords * edfhdr->long_data_record_duration) % TIME_DIMENSION;
    milliSec /= 10000LL;

    if(days > 0)
    {
      label4->setText("0d 0:00:00.000");

      snprintf(scratchpad, 256, "%id %i:%02i:%02i.%03i", days, (int)(seconds / 3600), (int)((seconds % 3600) / 60), (int)(seconds % 60), (int)milliSec);
    }
    else
    {
      label4->setText("0:00:00.000");

      snprintf(scratchpad, 256, "%i:%02i:%02i.%03i", (int)(seconds / 3600), (int)((seconds % 3600) / 60), (int)(seconds % 60), (int)milliSec);
    }

    label5->setText(scratchpad);
  }
}


void UI_ExportFilteredSignalsWindow::radioButton2Toggled(bool checked)
{
  if(checked == true)
  {
    spinBox1->setEnabled(true);
    spinBox2->setEnabled(true);
    label2->setEnabled(true);
    label3->setEnabled(true);
    label4->setEnabled(true);
    label5->setEnabled(true);
  }
}


void UI_ExportFilteredSignalsWindow::SelectFileButton()
{
  int days;

  long long seconds,
            milliSec;

  char txt_string[2048];

  label1->clear();
  label4->clear();
  label5->clear();

  inputfile = NULL;
  outputfile = NULL;

  inputpath[0] = 0;

  edfhdr = NULL;

  file_num = -1;

  pushButton3->setEnabled(false);
  spinBox1->setEnabled(false);
  spinBox2->setEnabled(false);
  radioButton1->setChecked(true);
  radioButton1->setEnabled(false);
  radioButton2->setEnabled(false);
  label2->setEnabled(false);
  label3->setEnabled(false);
  label4->setEnabled(false);
  label5->setEnabled(false);

  t_model->clear();

  if(mainwindow->files_open > 1)
  {
    UI_activeFileChooserWindow afchooser(&file_num, mainwindow);

    if(file_num < 0)
    {
      return;
    }
  }
  else
  {
    file_num = 0;
  }

  edfhdr = mainwindow->edfheaderlist[file_num];

  strlcpy(inputpath, edfhdr->filename, MAX_PATH_LENGTH);

  inputfile = edfhdr->file_hdl;
  if(inputfile==NULL)
  {
    snprintf(txt_string, 2048, "Can not open file %s for reading.", inputpath);
    QMessageBox messagewindow(QMessageBox::Critical, "Error", QString::fromLocal8Bit(txt_string));
    messagewindow.exec();

    inputpath[0] = 0;

    edfhdr = NULL;

    file_num = -1;

    return;
  }

  if(edfhdr->datarecords > 2147483647LL)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Failure", "This file contains more than 2147483647 datarecords.\n"
                                                                "This tool can not handle more than 2147483647 datarecords.");
    messagewindow.exec();

    inputfile = NULL;

    inputpath[0] = 0;

    edfhdr = NULL;

    file_num = -1;

    return;
  }

/***************** load signalproperties ******************************/

  label1->setText(inputpath);

  pushButton3->setEnabled(true);

  spinBox1->setValue(1);
  spinBox2->setMaximum(edfhdr->datarecords);
  spinBox2->setValue(edfhdr->datarecords);
  spinBox1->setMaximum(edfhdr->datarecords);

  radioButton1->setEnabled(true);
  radioButton2->setEnabled(true);

  label4->setText("0:00:00.000");
  days = (int)(((edfhdr->datarecords * edfhdr->long_data_record_duration) / TIME_DIMENSION) / 86400LL);
  seconds = (edfhdr->datarecords * edfhdr->long_data_record_duration) / TIME_DIMENSION;
  seconds %= 86400LL;
  milliSec = (edfhdr->datarecords * edfhdr->long_data_record_duration) % TIME_DIMENSION;
  milliSec /= 10000LL;
  if(days)
  {
    snprintf(txt_string, 2048, "%id %i:%02i:%02i.%03i", days, (int)(seconds / 3600), (int)((seconds % 3600) / 60), (int)(seconds % 60), (int)milliSec);
  }
  else
  {
    snprintf(txt_string, 2048, "%i:%02i:%02i.%03i", (int)(seconds / 3600), (int)((seconds % 3600) / 60), (int)(seconds % 60), (int)milliSec);
  }
  label5->setText(txt_string);

  populate_tree_view();
}


void UI_ExportFilteredSignalsWindow::StartExport()
{
  int i, j, k, p,
      type,
      new_edfsignals,
      datarecords=0,
      start_datarecord=0,
      annot_smp_per_record,
      annot_recordsize,
      timestamp_digits=0,
      timestamp_decimals=0,
      annot_len,
      annot_cnt=0,
      tallen=0,
      len,
      annots_per_datrec=0,
      smplrt,
      progress_steps,
      datrecs_processed,
      annot_list_sz=0,
      smp_per_record[MAXSIGNALS],
      signalslist[MAXSIGNALS],
      digmin,
      digmax,
      value;

  char scratchpad[4096];

  double *filtered_blockread_buf[MAXSIGNALS],
         bitvalue,
         phys_offset,
         frequency,
         frequency2;

  long long new_starttime,
            time_diff,
            onset_diff,
            taltime,
            l_temp,
            endtime=0;

  struct date_time_struct dts;

  struct annotation_list new_annot_list,
                         *annot_list=NULL;

  struct annotationblock *annot_ptr=NULL;

  FilteredBlockReadClass *block_reader[MAXSIGNALS];

  struct signalcompblock *signalcomp[MAXSIGNALS];

  for(i=0; i<MAXSIGNALS; i++)
  {
    block_reader[i] = NULL;
  }

  memset(&new_annot_list, 0, sizeof(struct annotation_list));

  QProgressDialog progress("Processing file...", "Abort", 0, 1);
  progress.setWindowModality(Qt::WindowModal);
  progress.setMinimumDuration(200);
  progress.reset();

  pushButton3->setEnabled(false);
  spinBox1->setEnabled(false);
  spinBox2->setEnabled(false);
  radioButton1->setEnabled(false);
  radioButton2->setEnabled(false);
  label2->setEnabled(false);
  label3->setEnabled(false);

  if(edfhdr==NULL)
  {
    return;
  }

  if(file_num < 0)
  {
    return;
  }

  annot_smp_per_record = 0;

  time_diff = (long long)(spinBox1->value() - 1) * edfhdr->long_data_record_duration;

  taltime = (time_diff + edfhdr->starttime_offset) % TIME_DIMENSION;

  endtime = (long long)(spinBox2->value() - (spinBox1->value() - 1)) * edfhdr->long_data_record_duration + taltime;

  for(i=0, new_edfsignals=0; i<mainwindow->signalcomps; i++)
  {
    if(mainwindow->signalcomp[i]->edfhdr != edfhdr)  continue;

    signalcomp[new_edfsignals] = mainwindow->signalcomp[i];

    signalslist[new_edfsignals] = signalcomp[new_edfsignals]->edfsignal[0];

    block_reader[new_edfsignals] = new FilteredBlockReadClass;

    filtered_blockread_buf[new_edfsignals] = block_reader[new_edfsignals]->init_signalcomp(signalcomp[new_edfsignals], 1, 0);

    smp_per_record[new_edfsignals] = block_reader[new_edfsignals]->samples_in_buf();

    new_edfsignals++;
  }

  if(!new_edfsignals)
  {
    showpopupmessage("Error", "No signals present on screen for selected file.");
    goto END_1;
  }

  start_datarecord = spinBox1->value() - 1;

  datarecords = spinBox2->value() - start_datarecord;

  if(edfhdr->edfplus || edfhdr->bdfplus)
  {
    timestamp_decimals = edfplus_annotation_get_tal_timestamp_decimal_cnt(edfhdr);
    if(timestamp_decimals < 0)
    {
      showpopupmessage("Error", "Internal error, get_tal_timestamp_decimal_cnt()");
      goto END_1;
    }

    timestamp_digits = edfplus_annotation_get_tal_timestamp_digit_cnt(edfhdr);
    if(timestamp_digits < 0)
    {
      showpopupmessage("Error", "Internal error, get_tal_timestamp_digit_cnt()");
      goto END_1;
    }

    annot_list = &mainwindow->edfheaderlist[file_num]->annot_list;

    annot_list_sz = edfplus_annotation_size(annot_list);

    for(i=0; i<annot_list_sz; i++)
    {
      annot_ptr = edfplus_annotation_get_item(annot_list, i);

      l_temp = annot_ptr->onset - time_diff;

      if((l_temp >= 0LL) && (l_temp <= endtime))
      {
        edfplus_annotation_add_item(&new_annot_list, *(edfplus_annotation_get_item(annot_list, i)));
      }
    }

    new_starttime = edfhdr->utc_starttime + ((time_diff + edfhdr->starttime_offset) / TIME_DIMENSION);

    onset_diff = (new_starttime - edfhdr->utc_starttime) * TIME_DIMENSION;

    annot_list_sz = edfplus_annotation_size(&new_annot_list);

    if(annot_list_sz > 0)
    {
      for(i=0; i<annot_list_sz; i++)
      {
        annot_ptr = edfplus_annotation_get_item(&new_annot_list, i);

        annot_ptr->onset -= onset_diff;
      }

      edfplus_annotation_sort(&new_annot_list, NULL);

      annots_per_datrec = annot_list_sz / datarecords;

      if(annot_list_sz % datarecords)
      {
        annots_per_datrec++;
      }
    }
    else
    {
      annots_per_datrec = 0;
    }

    annot_len = edfplus_annotation_get_max_annotation_strlen(&new_annot_list);

    annot_recordsize = (annot_len * annots_per_datrec) + timestamp_digits + timestamp_decimals + 4;

    if(timestamp_decimals)
    {
      annot_recordsize++;
    }

    if(edfhdr->edf)
    {
      annot_smp_per_record = annot_recordsize / 2;

      if(annot_recordsize % annot_smp_per_record)
      {
        annot_smp_per_record++;

        annot_recordsize = annot_smp_per_record * 2;
      }
    }
    else
    {
      annot_smp_per_record = annot_recordsize / 3;

      if(annot_recordsize % annot_smp_per_record)
      {
        annot_smp_per_record++;

        annot_recordsize = annot_smp_per_record * 3;
      }
    }
  }
  else
  {
    annot_smp_per_record = 0;

    annot_recordsize = 0;
  }

///////////////////////////////////////////////////////////////////

  outputpath[0] = 0;
  if(recent_savedir[0]!=0)
  {
    strlcpy(outputpath, recent_savedir, MAX_PATH_LENGTH);
    strlcat(outputpath, "/", MAX_PATH_LENGTH);
  }
  len = strlen(outputpath);
  get_filename_from_path(outputpath + len, inputpath, MAX_PATH_LENGTH - len);
  remove_extension_from_filename(outputpath);
  if(edfhdr->edf)
  {
    strlcat(outputpath, "_filtered.edf", MAX_PATH_LENGTH);

    strlcpy(outputpath, QFileDialog::getSaveFileName(0, "Save file", QString::fromLocal8Bit(outputpath), "EDF files (*.edf *.EDF)").toLocal8Bit().data(), MAX_PATH_LENGTH);
  }
  else
  {
    strlcat(outputpath, "_filtered.bdf", MAX_PATH_LENGTH);

    strlcpy(outputpath, QFileDialog::getSaveFileName(0, "Save file", QString::fromLocal8Bit(outputpath), "BDF files (*.bdf *.BDF)").toLocal8Bit().data(), MAX_PATH_LENGTH);
  }

  if(!strcmp(outputpath, ""))
  {
    goto END_3;
  }

  get_directory_from_path(recent_savedir, outputpath, MAX_PATH_LENGTH);

  if(mainwindow->file_is_opened(outputpath))
  {
    showpopupmessage("Error", "Selected file is in use.");
    goto END_3;
  }

  outputfile = fopeno(outputpath, "wb");
  if(outputfile==NULL)
  {
    showpopupmessage("Error", "Can not open outputfile for writing.");
    goto END_3;
  }

  new_starttime = edfhdr->utc_starttime + ((time_diff + edfhdr->starttime_offset) / TIME_DIMENSION);

  utc_to_date_time(new_starttime, &dts);

  rewind(inputfile);
  if(fread(scratchpad, 168, 1, inputfile)!=1)
  {
    showpopupmessage("Error", "Read error (1).");
    goto END_4;
  }

  if(edfhdr->edfplus || edfhdr->bdfplus)
  {
    if(scratchpad[98] != 'X')
    {
      snprintf(scratchpad + 98, 4096 - 98, "%02i-%s-%04i", dts.day, dts.month_str, dts.year);

      scratchpad[109] = ' ';
    }
  }

  if(fwrite(scratchpad, 168, 1, outputfile)!=1)
  {
    showpopupmessage("Error", "Write error (1).");
    goto END_4;
  }

  fprintf(outputfile, "%02i.%02i.%02i%02i.%02i.%02i",
          dts.day,
          dts.month,
          dts.year % 100,
          dts.hour,
          dts.minute,
          dts.second);

  if(edfhdr->edfplus || edfhdr->bdfplus)
  {
    fprintf(outputfile, "%-8i", new_edfsignals * 256 + 512);
  }
  else
  {
    fprintf(outputfile, "%-8i", new_edfsignals * 256 + 256);
  }
  if(edfhdr->edfplus)
  {
    fprintf(outputfile, "EDF+C");
    for(i=0; i<39; i++)
    {
      fputc(' ', outputfile);
    }
  }
  if(edfhdr->bdfplus)
  {
    fprintf(outputfile, "BDF+C");
    for(i=0; i<39; i++)
    {
      fputc(' ', outputfile);
    }
  }
  if((!edfhdr->edfplus) && (!edfhdr->bdfplus))
  {
    for(i=0; i<44; i++)
    {
      fputc(' ', outputfile);
    }
  }
  fprintf(outputfile, "%-8i", datarecords);
  snprintf(scratchpad, 256, "%f", edfhdr->data_record_duration);
  convert_trailing_zeros_to_spaces(scratchpad);
  if(scratchpad[7]=='.')
  {
    scratchpad[7] = ' ';
  }
  scratchpad[8] = 0;

  fprintf(outputfile, "%s", scratchpad);
  if(edfhdr->edfplus || edfhdr->bdfplus)
  {
    fprintf(outputfile, "%-4i", new_edfsignals + 1);
  }
  else
  {
    fprintf(outputfile, "%-4i", new_edfsignals);
  }

  for(i=0; i<new_edfsignals; i++)
  {
    strlcpy(scratchpad, signalcomp[i]->signallabel, 4096);
    scratchpad[16] = 0;
    strlcat(scratchpad, "                ", 4096);
    scratchpad[16] = 0;
    fprintf(outputfile, "%s", scratchpad);
  }
  if(edfhdr->edfplus)
  {
    fprintf(outputfile, "EDF Annotations ");
  }
  if(edfhdr->bdfplus)
  {
    fprintf(outputfile, "BDF Annotations ");
  }
  for(i=0; i<new_edfsignals; i++)
  {
    fprintf(outputfile, "%s", edfhdr->edfparam[signalslist[i]].transducer);
  }
  if(edfhdr->edfplus || edfhdr->bdfplus)
  {
    for(i=0; i<80; i++)
    {
      fputc(' ', outputfile);
    }
  }
  for(i=0; i<new_edfsignals; i++)
  {
    fprintf(outputfile, "%s", edfhdr->edfparam[signalslist[i]].physdimension);
  }
  if(edfhdr->edfplus || edfhdr->bdfplus)
  {
    for(i=0; i<8; i++)
    {
      fputc(' ', outputfile);
    }
  }
  for(i=0; i<new_edfsignals; i++)
  {
    snprintf(scratchpad, 256, "%f", edfhdr->edfparam[signalslist[i]].phys_min);
    convert_trailing_zeros_to_spaces(scratchpad);
    if(scratchpad[7]=='.')
    {
      scratchpad[7] = ' ';
    }
    scratchpad[8] = 0;
    fprintf(outputfile, "%s", scratchpad);
  }
  if(edfhdr->edfplus || edfhdr->bdfplus)
  {
    fprintf(outputfile, "-1      ");
  }
  for(i=0; i<new_edfsignals; i++)
  {
    snprintf(scratchpad, 256, "%f", edfhdr->edfparam[signalslist[i]].phys_max);
    convert_trailing_zeros_to_spaces(scratchpad);
    if(scratchpad[7]=='.')
    {
      scratchpad[7] = ' ';
    }
    scratchpad[8] = 0;
    fprintf(outputfile, "%s", scratchpad);
  }
  if(edfhdr->edfplus || edfhdr->bdfplus)
  {
    fprintf(outputfile, "1       ");
  }
  for(i=0; i<new_edfsignals; i++)
  {
    fprintf(outputfile, "%-8i", edfhdr->edfparam[signalslist[i]].dig_min);
  }
  if(edfhdr->edfplus)
  {
    fprintf(outputfile, "-32768  ");
  }
  if(edfhdr->bdfplus)
  {
    fprintf(outputfile, "-8388608");
  }
  for(i=0; i<new_edfsignals; i++)
  {
    fprintf(outputfile, "%-8i", edfhdr->edfparam[signalslist[i]].dig_max);
  }
  if(edfhdr->edfplus)
  {
    fprintf(outputfile, "32767   ");
  }
  if(edfhdr->bdfplus)
  {
    fprintf(outputfile, "8388607 ");
  }
  for(i=0; i<new_edfsignals; i++)
  {
//    fprintf(outputfile, "%s", edfhdr->edfparam[signalslist[i]].prefilter);

    strlcpy(scratchpad, edfhdr->edfparam[signalslist[i]].prefilter, 4096);
    strlcat(scratchpad, "                                                                                ", 4096);
    for(p = strlen(scratchpad) - 1; p>=0; p--)
    {
      if(scratchpad[p]!=' ')  break;
    }
    p++;
    if(p) p++;

    for(j=0; j<signalcomp[i]->filter_cnt; j++)
    {
      if(signalcomp[i]->filter[j]->is_LPF == 1)
      {
        p += snprintf(scratchpad + p, 4096 - p, "LP:%f", signalcomp[i]->filter[j]->cutoff_frequency);
      }

      if(signalcomp[i]->filter[j]->is_LPF == 0)
      {
        p += snprintf(scratchpad + p, 4096 - p, "HP:%f", signalcomp[i]->filter[j]->cutoff_frequency);
      }

      for(k=(p-1); k>0; k--)
      {
        if(scratchpad[k]!='0')  break;
      }

      if(scratchpad[k]=='.')  scratchpad[k] = 0;
      else  scratchpad[k+1] = 0;

      strlcat(scratchpad, "Hz ", 4096);

      p = strlen(scratchpad);

      if(p>80)  break;
    }

    for(j=0; j<signalcomp[i]->fidfilter_cnt; j++)
    {
      type = signalcomp[i]->fidfilter_type[j];

      frequency = signalcomp[i]->fidfilter_freq[j];

      frequency2 = signalcomp[i]->fidfilter_freq2[j];

      if(type == 0)
      {
        p += snprintf(scratchpad + p, 4096 -p, "HP:%f", frequency);
      }

      if(type == 1)
      {
        p += snprintf(scratchpad + p, 4096 -p, "LP:%f", frequency);
      }

      if(type == 2)
      {
        p += snprintf(scratchpad + p, 4096 -p, "N:%f", frequency);
      }

      if(type == 3)
      {
        p += snprintf(scratchpad + p, 4096 -p, "BP:%f", frequency);
      }

      if(type == 4)
      {
        p += snprintf(scratchpad + p, 4096 -p, "BS:%f", frequency);
      }

      for(k=(p-1); k>0; k--)
      {
        if(scratchpad[k]!='0')  break;
      }

      if(scratchpad[k]=='.')  scratchpad[k] = 0;
      else  scratchpad[k+1] = 0;

      p = strlen(scratchpad);

      if((type == 3) || (type == 4))
      {
        p += snprintf(scratchpad + p, 4096 -p, "-%f", frequency2);

        for(k=(p-1); k>0; k--)
        {
          if(scratchpad[k]!='0')  break;
        }

        if(scratchpad[k]=='.')  scratchpad[k] = 0;
        else  scratchpad[k+1] = 0;
      }

      strlcat(scratchpad, "Hz ", 4096);

      p = strlen(scratchpad);

      if(p>80)  break;
    }

    for(j=0; j<signalcomp[i]->ravg_filter_cnt; j++)
    {
      if(signalcomp[i]->ravg_filter_type[j] == 0)
      {
        p += snprintf(scratchpad + p, 4096 - p, "HP:%iSmpls ", signalcomp[i]->ravg_filter[j]->size);
      }

      if(signalcomp[i]->ravg_filter_type[j] == 1)
      {
        p += snprintf(scratchpad + p, 4096 - p, "LP:%iSmpls ", signalcomp[i]->ravg_filter[j]->size);
      }

      p = strlen(scratchpad);

      if(p>80)  break;
    }

    if(signalcomp[i]->fir_filter)
    {
      p += snprintf(scratchpad + p, 4096 - p, "FIR ");
    }

    if(signalcomp[i]->ecg_filter != NULL)
    {
      p += snprintf(scratchpad + p, 4096 - p, "ECG:HR ");
    }

    if(signalcomp[i]->zratio_filter != NULL)
    {
      p += snprintf(scratchpad + p, 4096 - p, "Z-ratio ");
    }

    for(;p<81; p++)
    {
      scratchpad[p] = ' ';
    }

    if(fwrite(scratchpad, 80, 1, outputfile)!=1)
    {
      showpopupmessage("Error", "Write error (2).");
      goto END_4;
    }
  }
  if(edfhdr->edfplus || edfhdr->bdfplus)
  {
    for(i=0; i<80; i++)
    {
      fputc(' ', outputfile);
    }
  }
  for(i=0; i<new_edfsignals; i++)
  {
    fprintf(outputfile, "%-8i", edfhdr->edfparam[signalslist[i]].smp_per_record);
  }
  if(edfhdr->edfplus || edfhdr->bdfplus)
  {
    fprintf(outputfile, "%-8i", annot_smp_per_record);
  }
  for(i=0; i<(new_edfsignals * 32); i++)
  {
   fputc(' ', outputfile);
  }
  if(edfhdr->edfplus || edfhdr->bdfplus)
  {
    for(i=0; i<32; i++)
    {
      fputc(' ', outputfile);
    }
  }
///////////////////////////////////////////////////////////////////

  progress.setRange(0, datarecords);
  progress.setValue(0);

  progress_steps = datarecords / 100;
  if(progress_steps < 1)
  {
    progress_steps = 1;
  }

  for(datrecs_processed=0; datrecs_processed<datarecords; datrecs_processed++)
  {
    if(!(datrecs_processed % progress_steps))
    {
      progress.setValue(datrecs_processed);

      qApp->processEvents();

      if(progress.wasCanceled() == true)
      {
        goto END_4;
      }
    }

    for(i=0; i<new_edfsignals; i++)
    {
      if(block_reader[i]->process_signalcomp(start_datarecord))
      {
        progress.reset();
        showpopupmessage("Error", "Read error (2).");
        goto END_4;
      }
    }

    start_datarecord++;

    for(i=0; i<new_edfsignals; i++)
    {
      digmax = edfhdr->edfparam[signalslist[i]].dig_max;

      digmin = edfhdr->edfparam[signalslist[i]].dig_min;

      bitvalue = edfhdr->edfparam[signalslist[i]].bitvalue;

      phys_offset = edfhdr->edfparam[signalslist[i]].offset;

      smplrt = smp_per_record[i];

      for(j=0; j<smplrt; j++)
      {
        value = (filtered_blockread_buf[i][j] / bitvalue) - phys_offset;

        if(value>digmax)
        {
          value = digmax;
        }

        if(value<digmin)
        {
          value = digmin;
        }

        fputc(value&0xff, outputfile);

        if(fputc((value>>8)&0xff, outputfile)==EOF)
        {
          progress.reset();
          showpopupmessage("Error", "Write error (4).");
          goto END_4;
        }

        if(edfhdr->bdf)
        {
          fputc((value>>16)&0xff, outputfile);
        }
      }
    }

    if(edfhdr->edfplus || edfhdr->bdfplus)
    {
      switch(timestamp_decimals)
      {
        case 0 : tallen = fprintf(outputfile, "+%i", (int)(taltime / TIME_DIMENSION));
                  break;
        case 1 : tallen = fprintf(outputfile, "+%i.%01i", (int)(taltime / TIME_DIMENSION), (int)((taltime % TIME_DIMENSION) / 1000000LL));
                  break;
        case 2 : tallen = fprintf(outputfile, "+%i.%02i", (int)(taltime / TIME_DIMENSION), (int)((taltime % TIME_DIMENSION) / 100000LL));
                  break;
        case 3 : tallen = fprintf(outputfile, "+%i.%03i", (int)(taltime / TIME_DIMENSION), (int)((taltime % TIME_DIMENSION) / 10000LL));
                  break;
        case 4 : tallen = fprintf(outputfile, "+%i.%04i", (int)(taltime / TIME_DIMENSION), (int)((taltime % TIME_DIMENSION) / 1000LL));
                  break;
        case 5 : tallen = fprintf(outputfile, "+%i.%05i", (int)(taltime / TIME_DIMENSION), (int)((taltime % TIME_DIMENSION) / 100LL));
                  break;
        case 6 : tallen = fprintf(outputfile, "+%i.%06i", (int)(taltime / TIME_DIMENSION), (int)((taltime % TIME_DIMENSION) / 10LL));
                  break;
        case 7 : tallen = fprintf(outputfile, "+%i.%07i", (int)(taltime / TIME_DIMENSION), (int)(taltime % TIME_DIMENSION));
                  break;
      }

      fputc(20, outputfile);
      fputc(20, outputfile);
      fputc(0, outputfile);

      tallen += 3;

      if(annot_cnt < annot_list_sz)
      {
        for(i=0; i<annots_per_datrec; i++)
        {
          if(annot_cnt >= annot_list_sz)  break;

          annot_ptr = edfplus_annotation_get_item(&new_annot_list, annot_cnt++);

          len = snprintf(scratchpad, 256, "%+i.%07i",
          (int)(annot_ptr->onset / TIME_DIMENSION),
          (int)(annot_ptr->onset % TIME_DIMENSION));

          for(j=0; j<7; j++)
          {
            if(scratchpad[len - j - 1] != '0')
            {
              break;
            }
          }

          if(j)
          {
            len -= j;

            if(j == 7)
            {
              len--;
            }
          }

          if(fwrite(scratchpad, len, 1, outputfile) != 1)
          {
            progress.reset();
            showpopupmessage("Error", "Write error (5).");
            goto END_4;
          }

          tallen += len;

          if(annot_ptr->duration[0]!=0)
          {
            fputc(21, outputfile);
            tallen++;

            tallen += fprintf(outputfile, "%s", annot_ptr->duration);
          }

          fputc(20, outputfile);
          tallen++;

          tallen += fprintf(outputfile, "%s", annot_ptr->description);

          fputc(20, outputfile);
          fputc(0, outputfile);
          tallen += 2;
        }
      }

      for(k=tallen; k<annot_recordsize; k++)
      {
        fputc(0, outputfile);
      }

      taltime += edfhdr->long_data_record_duration;
    }
  }

  progress.reset();
  showpopupmessage("Ready", "Done.");

END_4:

  fclose(outputfile);
  outputfile = NULL;

END_3:

//END_2:

END_1:

  inputfile = NULL;

  inputpath[0] = 0;

  label1->clear();
  label4->clear();
  label5->clear();

  file_num = -1;

  edfhdr = NULL;

  edfplus_annotation_empty_list(&new_annot_list);

  for(i=0; i<MAXSIGNALS; i++)
  {
    if(block_reader[i] != NULL)
    {
      delete block_reader[i];
    }
  }
}


void UI_ExportFilteredSignalsWindow::showpopupmessage(const char *str1, const char *str2)
{
  QMessageBox messagewindow(QMessageBox::NoIcon, str1, str2);

  messagewindow.exec();
}


void UI_ExportFilteredSignalsWindow::populate_tree_view()
{
  int i, j, k,
      type,
      model,
      order,
      n_taps;

  char txtbuf[2048]="",
       str[64]="";

  double frequency,
         frequency2,
         ripple;

  QStandardItem *parentItem,
                *signalItem,
                *filterItem,
                *firfilterItem;

  t_model->clear();

  parentItem = t_model->invisibleRootItem();

  for(i=0; i<mainwindow->signalcomps; i++)
  {
    if(mainwindow->signalcomp[i]->edfhdr != edfhdr)  continue;

    txtbuf[0] = 0;

    if(mainwindow->signalcomp[i]->alias[0] != 0)
    {
      strlcpy(txtbuf, "alias: ", 2048);
      strlcat(txtbuf, mainwindow->signalcomp[i]->alias, 2048);
      strlcat(txtbuf, "   ", 2048);
    }

    for(j=0; j<mainwindow->signalcomp[i]->num_of_signals; j++)
    {
      snprintf(str, 64, "%+f", mainwindow->signalcomp[i]->factor[j]);

      remove_trailing_zeros(str);

      snprintf(txtbuf + strlen(txtbuf), 2048 - strlen(txtbuf), "%sx %s",
              str,
              mainwindow->signalcomp[i]->edfhdr->edfparam[mainwindow->signalcomp[i]->edfsignal[j]].label);

      remove_trailing_spaces(txtbuf);

      strlcat(txtbuf, "   ", 2048);
    }

    signalItem = new QStandardItem(txtbuf);

    switch(mainwindow->signalcomp[i]->color)
    {
      case Qt::white       : signalItem->setIcon(QIcon(":/images/white_icon_16x16"));
                             break;
      case Qt::black       : signalItem->setIcon(QIcon(":/images/black_icon_16x16"));
                             break;
      case Qt::red         : signalItem->setIcon(QIcon(":/images/red_icon_16x16"));
                             break;
      case Qt::darkRed     : signalItem->setIcon(QIcon(":/images/darkred_icon_16x16"));
                             break;
      case Qt::green       : signalItem->setIcon(QIcon(":/images/green_icon_16x16"));
                             break;
      case Qt::darkGreen   : signalItem->setIcon(QIcon(":/images/darkgreen_icon_16x16"));
                             break;
      case Qt::blue        : signalItem->setIcon(QIcon(":/images/blue_icon_16x16"));
                             break;
      case Qt::darkBlue    : signalItem->setIcon(QIcon(":/images/darkblue_icon_16x16"));
                             break;
      case Qt::cyan        : signalItem->setIcon(QIcon(":/images/cyan_icon_16x16"));
                             break;
      case Qt::darkCyan    : signalItem->setIcon(QIcon(":/images/darkcyan_icon_16x16"));
                             break;
      case Qt::magenta     : signalItem->setIcon(QIcon(":/images/magenta_icon_16x16"));
                             break;
      case Qt::darkMagenta : signalItem->setIcon(QIcon(":/images/darkmagenta_icon_16x16"));
                             break;
      case Qt::yellow      : signalItem->setIcon(QIcon(":/images/yellow_icon_16x16"));
                             break;
      case Qt::darkYellow  : signalItem->setIcon(QIcon(":/images/darkyellow_icon_16x16"));
                             break;
      case Qt::gray        : signalItem->setIcon(QIcon(":/images/gray_icon_16x16"));
                             break;
      case Qt::darkGray    : signalItem->setIcon(QIcon(":/images/darkgray_icon_16x16"));
                             break;
      case Qt::lightGray   : signalItem->setIcon(QIcon(":/images/lightgray_icon_16x16"));
                             break;
    }

    parentItem->appendRow(signalItem);

    filterItem = new QStandardItem("Filters");

    filterItem->setIcon(QIcon(":/images/filter_lowpass_small.png"));

    signalItem->appendRow(filterItem);

    if(mainwindow->signalcomp[i]->spike_filter)
    {
      snprintf(txtbuf, 2048, "Spike: %.8f", mainwindow->signalcomp[i]->spike_filter_velocity);

      remove_trailing_zeros(txtbuf);

      snprintf(txtbuf + strlen(txtbuf), 2048 - strlen(txtbuf), " %s/0.5mSec.  Hold-off: %i mSec.",
              mainwindow->signalcomp[i]->physdimension,
              mainwindow->signalcomp[i]->spike_filter_holdoff);

      filterItem->appendRow(new QStandardItem(txtbuf));
    }

    for(j=0; j<mainwindow->signalcomp[i]->filter_cnt; j++)
    {
      if(mainwindow->signalcomp[i]->filter[j]->is_LPF == 1)
      {
        snprintf(txtbuf, 2048, "LPF: %fHz", mainwindow->signalcomp[i]->filter[j]->cutoff_frequency);
      }

      if(mainwindow->signalcomp[i]->filter[j]->is_LPF == 0)
      {
        snprintf(txtbuf, 2048, "HPF: %fHz", mainwindow->signalcomp[i]->filter[j]->cutoff_frequency);
      }

      remove_trailing_zeros(txtbuf);

      filterItem->appendRow(new QStandardItem(txtbuf));
    }

    for(j=0; j<mainwindow->signalcomp[i]->ravg_filter_cnt; j++)
    {
      if(mainwindow->signalcomp[i]->ravg_filter_type[j] == 0)
      {
        snprintf(txtbuf, 2048, "highpass moving average %i smpls", mainwindow->signalcomp[i]->ravg_filter[j]->size);
      }

      if(mainwindow->signalcomp[i]->ravg_filter_type[j] == 1)
      {
        snprintf(txtbuf, 2048, "lowpass moving average %i smpls", mainwindow->signalcomp[i]->ravg_filter[j]->size);
      }

      filterItem->appendRow(new QStandardItem(txtbuf));
    }

    for(j=0; j<mainwindow->signalcomp[i]->fidfilter_cnt; j++)
    {
      type = mainwindow->signalcomp[i]->fidfilter_type[j];

      model = mainwindow->signalcomp[i]->fidfilter_model[j];

      frequency = mainwindow->signalcomp[i]->fidfilter_freq[j];

      frequency2 = mainwindow->signalcomp[i]->fidfilter_freq2[j];

      order = mainwindow->signalcomp[i]->fidfilter_order[j];

      ripple = mainwindow->signalcomp[i]->fidfilter_ripple[j];

      if(type == 0)
      {
        if(model == 0)
        {
          snprintf(txtbuf, 2048, "highpass Butterworth %fHz %ith order", frequency, order);
        }

        if(model == 1)
        {
          snprintf(txtbuf, 2048, "highpass Chebyshev %fHz %ith order %fdB ripple", frequency, order, ripple);
        }

        if(model == 2)
        {
          snprintf(txtbuf, 2048, "highpass Bessel %fHz %ith order", frequency, order);
        }
      }

      if(type == 1)
      {
        if(model == 0)
        {
          snprintf(txtbuf, 2048, "lowpass Butterworth %fHz %ith order", frequency, order);
        }

        if(model == 1)
        {
          snprintf(txtbuf, 2048, "lowpass Chebyshev %fHz %ith order %fdB ripple", frequency, order, ripple);
        }

        if(model == 2)
        {
          snprintf(txtbuf, 2048, "lowpass Bessel %fHz %ith order", frequency, order);
        }
      }

      if(type == 2)
      {
        snprintf(txtbuf, 2048, "notch %fHz Q-factor %i", frequency, order);
      }

      if(type == 3)
      {
        if(model == 0)
        {
          snprintf(txtbuf, 2048, "bandpass Butterworth %f-%fHz %ith order", frequency, frequency2, order);
        }

        if(model == 1)
        {
          snprintf(txtbuf, 2048, "bandpass Chebyshev %f-%fHz %ith order %fdB ripple", frequency, frequency2, order, ripple);
        }

        if(model == 2)
        {
          snprintf(txtbuf, 2048, "bandpass Bessel %f-%fHz %ith order", frequency, frequency2, order);
        }
      }

      if(type == 4)
      {
        if(model == 0)
        {
          snprintf(txtbuf, 2048, "bandstop Butterworth %f-%fHz %ith order", frequency, frequency2, order);
        }

        if(model == 1)
        {
          snprintf(txtbuf, 2048, "bandstop Chebyshev %f-%fHz %ith order %fdB ripple", frequency, frequency2, order, ripple);
        }

        if(model == 2)
        {
          snprintf(txtbuf, 2048, "bandstop Bessel %f-%fHz %ith order", frequency, frequency2, order);
        }
      }

      remove_trailing_zeros(txtbuf);

      filterItem->appendRow(new QStandardItem(txtbuf));
    }

    if(mainwindow->signalcomp[i]->fir_filter != NULL)
    {
      n_taps = fir_filter_size(mainwindow->signalcomp[i]->fir_filter);

      snprintf(txtbuf, 2048, "Custom FIR filter with %i taps", n_taps);

      firfilterItem = new QStandardItem(txtbuf);

      filterItem->appendRow(firfilterItem);

      for(k=0; k<n_taps; k++)
      {
        snprintf(txtbuf, 2048, " %.20f ", fir_filter_tap(k, mainwindow->signalcomp[i]->fir_filter));

        firfilterItem->appendRow(new QStandardItem(txtbuf));
      }
    }

    if(mainwindow->signalcomp[i]->ecg_filter != NULL)
    {
      snprintf(txtbuf, 2048, "ECG heartrate detection");

      filterItem->appendRow(new QStandardItem(txtbuf));
    }

    if(mainwindow->signalcomp[i]->plif_ecg_filter != NULL)
    {
      snprintf(txtbuf, 2048, "Powerline interference removal: %iHz",
              (mainwindow->signalcomp[i]->plif_ecg_subtract_filter_plf * 10) + 50);

      filterItem->appendRow(new QStandardItem(txtbuf));
    }

    if(mainwindow->signalcomp[i]->zratio_filter != NULL)
    {
      snprintf(txtbuf, 2048, "Z-ratio  cross-over frequency is %.1f Hz", mainwindow->signalcomp[i]->zratio_crossoverfreq);

      filterItem->appendRow(new QStandardItem(txtbuf));
    }
  }

  tree->setModel(t_model);

  tree->expandAll();
}


















