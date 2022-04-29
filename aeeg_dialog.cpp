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



#include "aeeg_dialog.h"





UI_aeeg_window::UI_aeeg_window(QWidget *w_parent, struct signalcompblock *signal_comp, int numb, struct aeeg_dock_param_struct *p_par)
{
  char str[1024]="";

  myobjectDialog = NULL;

  mainwindow = (UI_Mainwindow *)w_parent;

  no_dialog_params = p_par;

  signalcomp = signal_comp;

  aeeg_instance_nr = numb;

  sf = signalcomp->edfhdr->edfparam[signalcomp->edfsignal[0]].sf_int;
  if(!sf)
  {
    sf = signalcomp->edfhdr->edfparam[signalcomp->edfsignal[0]].sf_f + 0.5;
  }

  if(no_dialog_params)
  {
    no_dialog_params->sf = sf;

    start_button_clicked();

    return;
  }

  snprintf(str, 1024, "aEEG   %s", signalcomp->signallabel);

  myobjectDialog = new QDialog;

  myobjectDialog->setMinimumSize(350 * mainwindow->w_scaling, 400 * mainwindow->h_scaling);
  myobjectDialog->setWindowTitle(str);
  myobjectDialog->setModal(true);
  myobjectDialog->setAttribute(Qt::WA_DeleteOnClose, true);
  myobjectDialog->setSizeGripEnabled(true);

  segmentlen_spinbox = new QSpinBox;
  segmentlen_spinbox->setSuffix(" sec");
  segmentlen_spinbox->setMinimum(5);
  if(signalcomp->edfhdr->recording_len_sec < 300)
  {
    segmentlen_spinbox->setMaximum(signalcomp->edfhdr->recording_len_sec);
  }
  else
  {
    segmentlen_spinbox->setMaximum(300);
  }
  segmentlen_spinbox->setValue(mainwindow->aeeg_segmentlen);

  min_hz_spinbox = new QDoubleSpinBox;
  min_hz_spinbox->setSuffix(" Hz");
  min_hz_spinbox->setMinimum(1);
  min_hz_spinbox->setMaximum((sf / 2) - 1);
  min_hz_spinbox->setDecimals(1);
  min_hz_spinbox->setValue(mainwindow->aeeg_min_hz);

  max_hz_spinbox = new QDoubleSpinBox;
  max_hz_spinbox->setSuffix(" Hz");
  max_hz_spinbox->setMinimum(2);
  max_hz_spinbox->setMaximum(sf / 2);
  max_hz_spinbox->setDecimals(1);
  max_hz_spinbox->setValue(mainwindow->aeeg_max_hz);

  strlcpy(str, " ", 128);
  strlcat(str, signalcomp->edfhdr->edfparam[signalcomp->edfsignal[0]].physdimension, 128);
  remove_trailing_spaces(str);

  close_button = new QPushButton;
  close_button->setText("Close");

  default_button = new QPushButton;
  default_button->setText("Default");

  start_button = new QPushButton;
  start_button->setText("Start");

  flayout = new QFormLayout;
  flayout->addRow("Segment length", segmentlen_spinbox);
  flayout->addRow("Min. freq.", min_hz_spinbox);
  flayout->addRow("Max. freq.", max_hz_spinbox);

  QHBoxLayout *hlayout2 = new QHBoxLayout;
  hlayout2->addLayout(flayout, 1000);

  QHBoxLayout *hlayout1 = new QHBoxLayout;
  hlayout1->addWidget(close_button);
  hlayout1->addStretch(1000);
  hlayout1->addWidget(default_button);
  hlayout1->addStretch(1000);
  hlayout1->addWidget(start_button);

  QVBoxLayout *vlayout1 = new QVBoxLayout;
  vlayout1->addLayout(hlayout2);
  vlayout1->addStretch(1000);
  vlayout1->addSpacing(20);
  vlayout1->addLayout(hlayout1);

  myobjectDialog->setLayout(vlayout1);

  QObject::connect(close_button, SIGNAL(clicked()), myobjectDialog, SLOT(close()));

  if(sf >= 30)
  {
    QObject::connect(default_button,     SIGNAL(clicked()),            this, SLOT(default_button_clicked()));
    QObject::connect(start_button,       SIGNAL(clicked()),            this, SLOT(start_button_clicked()));
    QObject::connect(segmentlen_spinbox, SIGNAL(valueChanged(int)),    this, SLOT(segmentlen_spinbox_changed(int)));
    QObject::connect(min_hz_spinbox,     SIGNAL(valueChanged(double)), this, SLOT(min_hz_spinbox_changed(double)));
    QObject::connect(max_hz_spinbox,     SIGNAL(valueChanged(double)), this, SLOT(max_hz_spinbox_changed(double)));
  }

  myobjectDialog->exec();
}


void UI_aeeg_window::segmentlen_spinbox_changed(int)
{
  QObject::blockSignals(true);

//   if(blocklen_spinbox->value() > (value / 3))
//   {
//     blocklen_spinbox->setValue(value / 3);
//   }

  QObject::blockSignals(false);
}


void UI_aeeg_window::min_hz_spinbox_changed(double value)
{
  QObject::blockSignals(true);

  if(max_hz_spinbox->value() <= value)
  {
    max_hz_spinbox->setValue(value + 1);
  }

  QObject::blockSignals(false);
}


void UI_aeeg_window::max_hz_spinbox_changed(double value)
{
  QObject::blockSignals(true);

  if(min_hz_spinbox->value() >= value)
  {
    min_hz_spinbox->setValue(value - 1);
  }

  QObject::blockSignals(false);
}


void UI_aeeg_window::default_button_clicked()
{
  QObject::blockSignals(true);

  segmentlen_spinbox->setValue(15);
  min_hz_spinbox->setValue(2);
  max_hz_spinbox->setValue(15);

  mainwindow->aeeg_segmentlen = 15;
  mainwindow->aeeg_min_hz = 2;
  mainwindow->aeeg_max_hz = 15;

  QObject::blockSignals(false);
}


void UI_aeeg_window::start_button_clicked()
{
  int i, j,
      err,
      len,
      smpls_in_segment,
      segments_in_recording,
      segmentlen,
      ret_err=0;

  long long samples_in_file;

  double v_scale,
         d_tmp,
         hz_min,
         hz_max,
         *smplbuf=NULL;

  char str[1024]={""},
       filt_spec_str[256]={""},
       *filt_spec_ptr=NULL,
       *fid_err=NULL;

  struct aeeg_dock_param_struct dock_param;

  if(myobjectDialog)
  {
    segmentlen = segmentlen_spinbox->value();
    mainwindow->aeeg_segmentlen = segmentlen;

    hz_min = min_hz_spinbox->value();
    mainwindow->aeeg_min_hz = hz_min;

    hz_max = max_hz_spinbox->value();
    mainwindow->aeeg_max_hz = hz_max;
  }
  else  // no dialog
  {
    segmentlen = no_dialog_params->segment_len;
    mainwindow->aeeg_segmentlen = segmentlen;

    hz_min = no_dialog_params->min_hz;
    mainwindow->aeeg_min_hz = hz_min;

    hz_max = no_dialog_params->max_hz;
    mainwindow->aeeg_max_hz = hz_max;
  }

  smpls_in_segment = sf * segmentlen;

  samples_in_file = (long long)signalcomp->edfhdr->datarecords * (long long)signalcomp->edfhdr->edfparam[signalcomp->edfsignal[0]].smp_per_record;

  segments_in_recording = samples_in_file / (long long)smpls_in_segment;

//   printf("start_button_clicked(): samples_in_file: %lli\n", samples_in_file);
//
//   printf("start_button_clicked(): sf: %i\n", sf);
//
//   printf("start_button_clicked(): segmentlen: %i\n", segmentlen);
//
//   printf("start_button_clicked(): datarecords in file: %lli\n", signalcomp->edfhdr->datarecords);
//
//   printf("start_button_clicked(): samples_in_file: %lli\n", samples_in_file);
//
//   printf("start_button_clicked(): smpls_in_segment: %i\n", smpls_in_segment);
//
//   printf("start_button_clicked(): segments_in_recording: %i\n", segments_in_recording);

  FilteredBlockReadClass fbr;

  ret_err = 0;

  smplbuf = fbr.init_signalcomp(signalcomp, smpls_in_segment, 0, 1, &ret_err);
  if(smplbuf == NULL)
  {
    if(ret_err == 1)
    {
      QMessageBox msgBox(QMessageBox::Critical, "Error", "The system was not able to provide enough resources (memory) to perform the requested action.", QMessageBox::Close);
      msgBox.exec();
    }
    else
    {
      QMessageBox msgBox(QMessageBox::Critical, "Error", "Internal error (-1)", QMessageBox::Close);
      msgBox.exec();
    }
    return;
  }

  FidFilter *fidfilter=NULL;
  FidRun *fid_run=NULL;
  FidFunc *fidfuncp=NULL;
  void *fidbuf=NULL;

  snprintf(filt_spec_str, 256, "BpBu%i/%f-%f", 8, hz_min, hz_max);

  filt_spec_ptr = filt_spec_str;

  fid_err = fid_parse(sf, &filt_spec_ptr, &fidfilter);
  if(fid_err != NULL)
  {
    QMessageBox msgBox(QMessageBox::Critical, "Error", "Internal error (-2)", QMessageBox::Close);
    msgBox.exec();
    free(fid_err);
    return;
  }

  fid_run = fid_run_new(fidfilter, &fidfuncp);

  fidbuf = fid_run_newbuf(fid_run);

  QProgressDialog progress("Processing...", "Abort", 0, segments_in_recording);
  progress.setWindowModality(Qt::WindowModal);
  progress.setMinimumDuration(500);

  for(i=0; i<segments_in_recording; i++)
  {
    progress.setValue(i);

    qApp->processEvents();

    if(progress.wasCanceled() == true)
    {
      progress.reset();
      return;
    }

    err = fbr.process_signalcomp(i * smpls_in_segment);
    if(err)
    {
      snprintf(str, 1024, "Internal error (-3)  fbr() error: %i file: %s line %i", err, __FILE__, __LINE__);
      progress.reset();
      QMessageBox::critical(myobjectDialog, "Error", str, QMessageBox::Close);
      return;
    }

    for(j=0; j<smpls_in_segment; j++)
    {
      smplbuf[j] = fidfuncp(fidbuf, smplbuf[j]);
    }
  }

  free(fidfilter);
  fid_run_free(fid_run);
  fid_run_freebuf(fidbuf);

  progress.reset();

  dock_param.signalcomp = signalcomp;
  dock_param.sf = sf;
  dock_param.min_hz = mainwindow->aeeg_min_hz;
  dock_param.max_hz = mainwindow->aeeg_max_hz;
  dock_param.segment_len = segmentlen;
  dock_param.segments_in_recording = segments_in_recording;
  dock_param.instance_num = aeeg_instance_nr;
  strlcpy(dock_param.unit, signalcomp->edfhdr->edfparam[signalcomp->edfsignal[0]].physdimension, 32);
  remove_trailing_spaces(dock_param.unit);

  mainwindow->aeeg_dock[aeeg_instance_nr] = new UI_aeeg_dock(mainwindow, dock_param);

  signalcomp->aeeg_idx[aeeg_instance_nr] = aeeg_instance_nr + 1;

  if(myobjectDialog)
  {
    myobjectDialog->close();
  }
}





























