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

  if(no_dialog_params != NULL)
  {
    no_dialog_params->sf = sf;

    start_button_clicked();

    return;
  }

  snprintf(str, 1024, "aEEG   %s", signalcomp->signallabel);

  myobjectDialog = new QDialog;

  myobjectDialog->setMinimumSize(300 * mainwindow->w_scaling, 250 * mainwindow->h_scaling);
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

  bp_min_hz_spinbox = new QDoubleSpinBox;
  bp_min_hz_spinbox->setSuffix(" Hz");
  bp_min_hz_spinbox->setDecimals(1);
  bp_min_hz_spinbox->setMinimum(1);
  bp_min_hz_spinbox->setMaximum((sf / 2) - 2);
  bp_min_hz_spinbox->setValue(mainwindow->aeeg_bp_min_hz);

  bp_max_hz_spinbox = new QDoubleSpinBox;
  bp_max_hz_spinbox->setSuffix(" Hz");
  bp_max_hz_spinbox->setDecimals(1);
  bp_max_hz_spinbox->setMinimum(2);
  bp_max_hz_spinbox->setMaximum((sf / 2) - 1);
  bp_max_hz_spinbox->setValue(mainwindow->aeeg_bp_max_hz);

  lp_hz_spinbox = new QDoubleSpinBox;
  lp_hz_spinbox->setSuffix(" Hz");
  lp_hz_spinbox->setDecimals(3);
  lp_hz_spinbox->setMinimum(0.001);
  lp_hz_spinbox->setMaximum(2);
  lp_hz_spinbox->setValue(mainwindow->aeeg_lp_hz);

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
  flayout->addRow(" ", (QWidget *)(NULL));
  flayout->addRow("Segment length", segmentlen_spinbox);
  flayout->addRow(" ", (QWidget *)(NULL));
  flayout->addRow("BP min. freq.", bp_min_hz_spinbox);
  flayout->addRow("BP max. freq.", bp_max_hz_spinbox);
  flayout->addRow(" ", (QWidget *)(NULL));
  flayout->addRow("LP freq.", lp_hz_spinbox);
  flayout->addRow(" ", (QWidget *)(NULL));

  QHBoxLayout *hlayout2 = new QHBoxLayout;
  hlayout2->addLayout(flayout, 1000);
  hlayout2->addStretch(1000);

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

  if((sf >= 100) && (signalcomp->edfhdr->recording_len_sec >= 30))
  {
    QObject::connect(default_button, SIGNAL(clicked()), this, SLOT(default_button_clicked()));
    QObject::connect(start_button,   SIGNAL(clicked()), this, SLOT(start_button_clicked()));
  }

  myobjectDialog->exec();
}


void UI_aeeg_window::default_button_clicked()
{
  segmentlen_spinbox->setValue(15);
  bp_min_hz_spinbox->setValue(2);
  bp_max_hz_spinbox->setValue(15);
  lp_hz_spinbox->setValue(0.5);

  mainwindow->aeeg_segmentlen = 15;
  mainwindow->aeeg_bp_min_hz = 2;
  mainwindow->aeeg_bp_max_hz = 15;
  mainwindow->aeeg_lp_hz = 0.5;
}


void UI_aeeg_window::start_button_clicked()
{
  int i, j,
      err,
      smpls_in_segment,
      segments_in_recording,
      segmentlen,
      ret_err=0;

  long long samples_in_file;

  double bp_hz_min,
         bp_hz_max,
         lp_hz,
         *smplbuf=NULL,
         *min_max_val=NULL;

  char str[1024]={""},
       filt_spec_str_bp[256]={""},
       filt_spec_str_lp[256]={""},
       *filt_spec_ptr_bp=NULL,
       *filt_spec_ptr_lp=NULL,
       *fid_err_lp=NULL,
       *fid_err_bp=NULL;

  struct aeeg_dock_param_struct dock_param;

  if(myobjectDialog != NULL)
  {
    segmentlen = segmentlen_spinbox->value();
    bp_hz_min = bp_min_hz_spinbox->value();
    bp_hz_max = bp_max_hz_spinbox->value();
    lp_hz = lp_hz_spinbox->value();

    if((bp_hz_max - bp_hz_min) <= 4.999)
    {
      QMessageBox msgBox(QMessageBox::Critical, "Error", "(Max.freq. - min.freq.) must be >= 5 Hz.", QMessageBox::Close);
      msgBox.exec();
      return;
    }
  }
  else  // no dialog
  {
    segmentlen = no_dialog_params->segment_len;
    bp_hz_min = no_dialog_params->bp_min_hz;
    bp_hz_max = no_dialog_params->bp_max_hz;
    lp_hz = no_dialog_params->lp_hz;
  }
  mainwindow->aeeg_segmentlen = segmentlen;
  mainwindow->aeeg_bp_min_hz = bp_hz_min;
  mainwindow->aeeg_bp_max_hz = bp_hz_max;
  mainwindow->aeeg_lp_hz = lp_hz;

  smpls_in_segment = sf * segmentlen;

  samples_in_file = signalcomp->edfhdr->edfparam[signalcomp->edfsignal[0]].smpls;

  segments_in_recording = samples_in_file / (long long)smpls_in_segment;

//   printf("start_button_clicked(): samples_in_file: %lli\n", samples_in_file);
//
//   printf("start_button_clicked(): sf: %i\n", sf);
//
//   printf("start_button_clicked(): segmentlen: %i\n", segmentlen);
//
//   printf("start_button_clicked(): samples_in_file: %lli\n", samples_in_file);
//
//   printf("start_button_clicked(): smpls_in_segment: %i\n", smpls_in_segment);
//
//   printf("start_button_clicked(): segments_in_recording: %i\n", segments_in_recording);

  min_max_val = (double *)malloc(segments_in_recording * 2 * sizeof(double));
  if(min_max_val == NULL)
  {
    QMessageBox msgBox(QMessageBox::Critical, "Error", "The system was not able to provide enough resources (memory) to perform the requested action.", QMessageBox::Close);
    msgBox.exec();
    return;
  }

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

  FidFilter *fidfilter_bp=NULL;
  FidFilter *fidfilter_lp=NULL;
  FidRun *fid_run_bp=NULL;
  FidRun *fid_run_lp=NULL;
  FidFunc *fidfuncp_bp=NULL;
  FidFunc *fidfuncp_lp=NULL;
  void *fidbuf_bp=NULL;
  void *fidbuf_lp=NULL;

  snprintf(filt_spec_str_bp, 256, "BpBu%i/%f-%f", 8, bp_hz_min, bp_hz_max);
  snprintf(filt_spec_str_lp, 256, "LpBu%i/%f", 8, lp_hz);

  filt_spec_ptr_bp = filt_spec_str_bp;
  filt_spec_ptr_lp = filt_spec_str_lp;

  fid_err_bp = fid_parse(sf, &filt_spec_ptr_bp, &fidfilter_bp);
  if(fid_err_bp != NULL)
  {
    QMessageBox msgBox(QMessageBox::Critical, "Error", "Internal error (-2)", QMessageBox::Close);
    msgBox.exec();
    free(fid_err_bp);
    return;
  }
  fid_err_lp = fid_parse(sf, &filt_spec_ptr_lp, &fidfilter_lp);
  if(fid_err_lp != NULL)
  {
    QMessageBox msgBox(QMessageBox::Critical, "Error", "Internal error (-3)", QMessageBox::Close);
    msgBox.exec();
    free(fid_err_lp);
    return;
  }

  fid_run_bp = fid_run_new(fidfilter_bp, &fidfuncp_bp);
  fid_run_lp = fid_run_new(fidfilter_lp, &fidfuncp_lp);

  fidbuf_bp = fid_run_newbuf(fid_run_bp);
  fidbuf_lp = fid_run_newbuf(fid_run_lp);

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
      free(min_max_val);
      free(fidfilter_bp);
      free(fidfilter_lp);
      fid_run_free(fid_run_bp);
      fid_run_free(fid_run_lp);
      fid_run_freebuf(fidbuf_bp);
      fid_run_freebuf(fidbuf_lp);
      return;
    }

    err = fbr.process_signalcomp(i * smpls_in_segment);
    if(err)
    {
      snprintf(str, 1024, "Internal error (-4)  fbr() error: %i file: %s line %i", err, __FILE__, __LINE__);
      progress.reset();
      QMessageBox::critical(myobjectDialog, "Error", str, QMessageBox::Close);
      return;
    }

    min_max_val[(i * 2) + 1] = 0;

    min_max_val[i * 2] = DBL_MAX;

    for(j=0; j<smpls_in_segment; j++)
    {
      smplbuf[j] = fidfuncp_bp(fidbuf_bp, smplbuf[j]);  // bandpass 2 - 15 Hz

      smplbuf[j] = fabs(smplbuf[j]);  // rectifier

      smplbuf[j] = fidfuncp_lp(fidbuf_lp, smplbuf[j]) * 2;  // lowpass 0.5 Hz, gain = 2

      if(min_max_val[(i * 2) + 1] < smplbuf[j])
      {
        min_max_val[(i * 2) + 1] = smplbuf[j];
      }

      if(min_max_val[i * 2] > smplbuf[j])
      {
        min_max_val[i * 2] = smplbuf[j];
      }
    }
  }

  free(fidfilter_bp);
  free(fidfilter_lp);
  fid_run_free(fid_run_bp);
  fid_run_free(fid_run_lp);
  fid_run_freebuf(fidbuf_bp);
  fid_run_freebuf(fidbuf_lp);

  progress.reset();

  dock_param.signalcomp = signalcomp;
  dock_param.sf = sf;
  dock_param.bp_min_hz = mainwindow->aeeg_bp_min_hz;
  dock_param.bp_max_hz = mainwindow->aeeg_bp_max_hz;
  dock_param.lp_hz = mainwindow->aeeg_lp_hz;
  dock_param.segment_len = segmentlen;
  dock_param.segments_in_recording = segments_in_recording;
  dock_param.instance_num = aeeg_instance_nr;
  dock_param.min_max_val = min_max_val;
  strlcpy(dock_param.unit, signalcomp->edfhdr->edfparam[signalcomp->edfsignal[0]].physdimension, 32);
  remove_trailing_spaces(dock_param.unit);

  mainwindow->aeeg_dock[aeeg_instance_nr] = new UI_aeeg_dock(mainwindow, dock_param);

  signalcomp->aeeg_idx[aeeg_instance_nr] = aeeg_instance_nr + 1;

  if(myobjectDialog != NULL)
  {
    myobjectDialog->close();
  }
}





























