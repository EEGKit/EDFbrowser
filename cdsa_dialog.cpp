/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2020 - 2022 Teunis van Beelen
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



#include "cdsa_dialog.h"





UI_cdsa_window::UI_cdsa_window(QWidget *w_parent, struct signalcompblock *signal_comp, int numb)
{
  char str[1024]={""};

  mainwindow = (UI_Mainwindow *)w_parent;

  signalcomp = signal_comp;

  cdsa_instance_nr = numb;

  export_data = 0;

  sf = signalcomp->edfhdr->edfparam[signalcomp->edfsignal[0]].sf_int;
  if(!sf)
  {
    sf = signalcomp->edfhdr->edfparam[signalcomp->edfsignal[0]].sf_f + 0.5;
  }

  snprintf(str, 1024, "Color Density Spectral Array   %s", signalcomp->signallabel);

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
  segmentlen_spinbox->setValue(mainwindow->cdsa_segmentlen);
  segmentlen_spinbox->setToolTip("Time resolution of the CDSA");

  blocklen_spinbox = new QSpinBox;
  blocklen_spinbox->setSuffix(" sec");
  blocklen_spinbox->setMinimum(1);
  blocklen_spinbox->setMaximum(10);
  blocklen_spinbox->setValue(mainwindow->cdsa_blocklen);
  blocklen_spinbox->setToolTip("FFT block size in seconds, FFT resolution in Herz is the inverse of this value");

  overlap_combobox = new QComboBox;
  overlap_combobox->addItem(" 0 %");
  overlap_combobox->addItem("50 %");
  overlap_combobox->addItem("67 %");
  overlap_combobox->addItem("75 %");
  overlap_combobox->addItem("80 %");
  overlap_combobox->setCurrentIndex(mainwindow->cdsa_overlap - 1);
  overlap_combobox->setToolTip("Percentage of an FFT block that will overlap the next FFT block");

  windowfunc_combobox = new QComboBox;
  windowfunc_combobox->addItem("Rectangular");
  windowfunc_combobox->addItem("Hamming");
  windowfunc_combobox->addItem("4-term Blackman-Harris");
  windowfunc_combobox->addItem("7-term Blackman-Harris");
  windowfunc_combobox->addItem("Nuttall3b");
  windowfunc_combobox->addItem("Nuttall4c");
  windowfunc_combobox->addItem("Hann");
  windowfunc_combobox->addItem("HFT223D");
  windowfunc_combobox->addItem("HFT95");
  windowfunc_combobox->addItem("Kaiser2");
  windowfunc_combobox->addItem("Kaiser3");
  windowfunc_combobox->addItem("Kaiser4");
  windowfunc_combobox->addItem("Kaiser5");
  windowfunc_combobox->setCurrentIndex(mainwindow->cdsa_window_func);
  windowfunc_combobox->setToolTip("Smoothing (taper) function that will be applied before the FFT");

  min_hz_spinbox = new QSpinBox;
  min_hz_spinbox->setSuffix(" Hz");
  min_hz_spinbox->setMinimum(0);
  min_hz_spinbox->setMaximum((sf / 2) - 1);
  min_hz_spinbox->setValue(mainwindow->cdsa_min_hz);
  min_hz_spinbox->setToolTip("Lowest frequency output (bin) of the FFT that will be used to display the CDSA");

  max_hz_spinbox = new QSpinBox;
  max_hz_spinbox->setSuffix(" Hz");
  max_hz_spinbox->setMinimum(1);
  max_hz_spinbox->setMaximum(sf / 2);
  max_hz_spinbox->setValue(mainwindow->cdsa_max_hz);
  max_hz_spinbox->setToolTip("Highest frequency output (bin) of the FFT that will be used to display the CDSA");

  strlcpy(str, " ", 128);
  strlcat(str, signalcomp->edfhdr->edfparam[signalcomp->edfsignal[0]].physdimension, 128);
  remove_trailing_spaces(str);

  max_voltage_spinbox = new QDoubleSpinBox;
  max_voltage_spinbox->setSuffix(str);
  max_voltage_spinbox->setDecimals(7);
  max_voltage_spinbox->setMinimum(0.0000001);
  max_voltage_spinbox->setMaximum(100000.0);
  max_voltage_spinbox->setValue(mainwindow->cdsa_max_voltage);
  max_voltage_spinbox->setToolTip("The highest level that can be displayed (white). Higher levels will clip to white");

  strlcpy(str, " dB", 128);
  strlcat(str, signalcomp->edfhdr->edfparam[signalcomp->edfsignal[0]].physdimension, 128);
  remove_trailing_spaces(str);

  max_pwr_spinbox = new QSpinBox;
  max_pwr_spinbox->setSuffix(str);
  max_pwr_spinbox->setMinimum(-159);
  max_pwr_spinbox->setMaximum(160);
  max_pwr_spinbox->setValue(mainwindow->cdsa_max_pwr);
  max_pwr_spinbox->setToolTip("The highest level that can be displayed (white). Higher levels will clip to white");

  min_pwr_spinbox = new QSpinBox;
  min_pwr_spinbox->setSuffix(str);
  min_pwr_spinbox->setMinimum(-160);
  min_pwr_spinbox->setMaximum(159);
  min_pwr_spinbox->setValue(mainwindow->cdsa_min_pwr);
  min_pwr_spinbox->setToolTip("The lowest level that can be displayed (black). Lower levels will clip to black");

  log_checkbox = new QCheckBox;
  log_checkbox->setTristate(false);
  if(mainwindow->cdsa_log)
  {
    log_checkbox->setCheckState(Qt::Checked);
    max_pwr_spinbox->setEnabled(true);
    min_pwr_spinbox->setEnabled(true);
    max_voltage_spinbox->setEnabled(false);
  }
  else
  {
    log_checkbox->setCheckState(Qt::Unchecked);
    max_pwr_spinbox->setEnabled(false);
    min_pwr_spinbox->setEnabled(false);
    max_voltage_spinbox->setEnabled(true);
  }
  log_checkbox->setToolTip("Use the base-10 logarithm of the output of the FFT (can be used to increase the dynamic range)");

  pwr_voltage_checkbox = new QCheckBox;
  pwr_voltage_checkbox->setTristate(false);
  if(mainwindow->cdsa_pwr_voltage)
  {
    pwr_voltage_checkbox->setCheckState(Qt::Checked);
  }
  else
  {
    pwr_voltage_checkbox->setCheckState(Qt::Unchecked);
  }
  pwr_voltage_checkbox->setToolTip("Display power instead of voltage");
  if(mainwindow->cdsa_log)
  {
    pwr_voltage_checkbox->setCheckState(Qt::Checked);
    pwr_voltage_checkbox->setEnabled(false);
  }

  export_data_checkbox = new QCheckBox;
  export_data_checkbox->setTristate(false);
  export_data_checkbox->setToolTip("Export the data generated for the CDSA to file.");

  close_button = new QPushButton;
  close_button->setText("Close");

  default_button = new QPushButton;
  default_button->setText("Default");

  start_button = new QPushButton;
  start_button->setText("Start");

  flayout = new QFormLayout;
  flayout->addRow("Segment length", segmentlen_spinbox);
  flayout->labelForField(segmentlen_spinbox)->setToolTip("Time resolution of the CDSA");
  flayout->addRow("Block length", blocklen_spinbox);
  flayout->labelForField(blocklen_spinbox)->setToolTip("FFT block size in seconds, FFT resolution in Herz is the inverse of this value");
  flayout->addRow("Overlap", overlap_combobox);
  flayout->labelForField(overlap_combobox)->setToolTip("Percentage of an FFT block that will overlap the next FFT block");
  flayout->addRow("Window", windowfunc_combobox);
  flayout->labelForField(windowfunc_combobox)->setToolTip("Smoothing (taper) function that will be applied before the FFT");
  flayout->addRow("Min. freq.", min_hz_spinbox);
  flayout->labelForField(min_hz_spinbox)->setToolTip("Lowest frequency output (bin) of the FFT that will be used to display the CDSA");
  flayout->addRow("Max. freq.", max_hz_spinbox);
  flayout->labelForField(max_hz_spinbox)->setToolTip("Highest frequency output (bin) of the FFT that will be used to display the CDSA");
  flayout->addRow("Max. level", max_pwr_spinbox);
  flayout->labelForField(max_pwr_spinbox)->setToolTip("The highest level that can be displayed (white). Higher levels will clip to white");
  flayout->addRow("Min. level", min_pwr_spinbox);
  flayout->labelForField(min_pwr_spinbox)->setToolTip("The lowest level that can be displayed (black). Lower levels will clip to black");
  flayout->addRow("Max. level", max_voltage_spinbox);
  flayout->labelForField(max_voltage_spinbox)->setToolTip("The highest level that can be displayed (white). Higher levels will clip to white");
  flayout->addRow("Logarithmic", log_checkbox);
  flayout->labelForField(log_checkbox)->setToolTip("Use the base-10 logarithm of the output of the FFT (can be used to increase the dynamic range)");
  flayout->addRow("Power", pwr_voltage_checkbox);
  flayout->labelForField(pwr_voltage_checkbox)->setToolTip("Display power instead of voltage");
  flayout->addRow("Export data", export_data_checkbox);
  flayout->labelForField(export_data_checkbox)->setToolTip("Export the data generated for the CDSA to a file.");

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
    QObject::connect(default_button,     SIGNAL(clicked()),         this, SLOT(default_button_clicked()));
    QObject::connect(start_button,       SIGNAL(clicked()),         this, SLOT(start_button_clicked()));
    QObject::connect(segmentlen_spinbox, SIGNAL(valueChanged(int)), this, SLOT(segmentlen_spinbox_changed(int)));
    QObject::connect(blocklen_spinbox,   SIGNAL(valueChanged(int)), this, SLOT(blocklen_spinbox_changed(int)));
    QObject::connect(min_hz_spinbox,     SIGNAL(valueChanged(int)), this, SLOT(min_hz_spinbox_changed(int)));
    QObject::connect(max_hz_spinbox,     SIGNAL(valueChanged(int)), this, SLOT(max_hz_spinbox_changed(int)));
    QObject::connect(min_pwr_spinbox,    SIGNAL(valueChanged(int)), this, SLOT(min_pwr_spinbox_changed(int)));
    QObject::connect(max_pwr_spinbox,    SIGNAL(valueChanged(int)), this, SLOT(max_pwr_spinbox_changed(int)));
    QObject::connect(log_checkbox,       SIGNAL(stateChanged(int)), this, SLOT(log_checkbox_changed(int)));
  }

  myobjectDialog->exec();
}


void UI_cdsa_window::segmentlen_spinbox_changed(int value)
{
  QObject::blockSignals(true);

  if(blocklen_spinbox->value() > (value / 3))
  {
    blocklen_spinbox->setValue(value / 3);
  }

  QObject::blockSignals(false);
}


void UI_cdsa_window::blocklen_spinbox_changed(int value)
{
  QObject::blockSignals(true);

  if(segmentlen_spinbox->value() < (value * 3))
  {
    segmentlen_spinbox->setValue(value * 3);
  }

  QObject::blockSignals(false);
}


void UI_cdsa_window::min_hz_spinbox_changed(int value)
{
  QObject::blockSignals(true);

  if(max_hz_spinbox->value() <= value)
  {
    max_hz_spinbox->setValue(value + 1);
  }

  QObject::blockSignals(false);
}


void UI_cdsa_window::max_hz_spinbox_changed(int value)
{
  QObject::blockSignals(true);

  if(min_hz_spinbox->value() >= value)
  {
    min_hz_spinbox->setValue(value - 1);
  }

  QObject::blockSignals(false);
}


void UI_cdsa_window::min_pwr_spinbox_changed(int value)
{
  QObject::blockSignals(true);

  if(max_pwr_spinbox->value() <= value)
  {
    max_pwr_spinbox->setValue(value + 1);
  }

  QObject::blockSignals(false);
}


void UI_cdsa_window::max_pwr_spinbox_changed(int value)
{
  QObject::blockSignals(true);

  if(min_pwr_spinbox->value() >= value)
  {
    min_pwr_spinbox->setValue(value - 1);
  }

  QObject::blockSignals(false);
}


void UI_cdsa_window::log_checkbox_changed(int state)
{
  if(state == Qt::Checked)
  {
    pwr_voltage_checkbox->setCheckState(Qt::Checked);
    pwr_voltage_checkbox->setEnabled(false);
    mainwindow->cdsa_log = 1;

    max_pwr_spinbox->setEnabled(true);
    min_pwr_spinbox->setEnabled(true);
    max_voltage_spinbox->setEnabled(false);
  }
  else
  {
    pwr_voltage_checkbox->setEnabled(true);
    mainwindow->cdsa_log = 0;

    max_pwr_spinbox->setEnabled(false);
    min_pwr_spinbox->setEnabled(false);
    max_voltage_spinbox->setEnabled(true);
  }
}


void UI_cdsa_window::default_button_clicked()
{
  QObject::blockSignals(true);

  segmentlen_spinbox->setValue(30);
  blocklen_spinbox->setValue(2);
  overlap_combobox->setCurrentIndex(4);
  windowfunc_combobox->setCurrentIndex(9);
  min_hz_spinbox->setValue(1);
  max_hz_spinbox->setValue(30);
  max_pwr_spinbox->setValue(13);
  min_pwr_spinbox->setValue(-5);
  max_voltage_spinbox->setValue(5.0);
  log_checkbox->setCheckState(Qt::Checked);
  pwr_voltage_checkbox->setCheckState(Qt::Checked);
  pwr_voltage_checkbox->setEnabled(false);
  max_pwr_spinbox->setEnabled(true);
  min_pwr_spinbox->setEnabled(true);
  max_voltage_spinbox->setEnabled(false);
  export_data_checkbox->setCheckState(Qt::Unchecked);

  mainwindow->cdsa_segmentlen = 30;
  mainwindow->cdsa_blocklen = 2;
  mainwindow->cdsa_overlap = 5;
  mainwindow->cdsa_window_func = 9;
  mainwindow->cdsa_min_hz = 1;
  mainwindow->cdsa_max_hz = 30;
  mainwindow->cdsa_max_pwr = 13;
  mainwindow->cdsa_min_pwr = -5;
  mainwindow->cdsa_max_voltage = 5.0;
  mainwindow->cdsa_log = 1;
  mainwindow->cdsa_pwr_voltage = 1;

  QObject::blockSignals(false);
}


void UI_cdsa_window::start_button_clicked()
{
  int i, j, w, h, h_min, h_max, err, len,
      smpls_in_segment,
      segments_in_recording,
      segmentlen,
      blocklen,
      smpl_in_block,
      overlap,
      window_func,
      log_density=0,
      power_density=0,
      ret_err=0;

  long long samples_in_file;

  int rgb_map[1786][3],
      rgb_idx;

  double v_scale,
         d_tmp,
         *smplbuf=NULL,
         log_minimum_offset=0;

  char str[1024]={""},
       path[MAX_PATH_LENGTH]={""};

  struct fft_wrap_settings_struct *dft;

  QPixmap *pxm=NULL;

  struct cdsa_dock_param_struct dock_param;

  FILE *dat_f=NULL;

  if(export_data_checkbox->checkState() == Qt::Checked)
  {
    export_data = 1;
  }
  else
  {
    export_data = 0;
  }

  for(i=0; i<256; i++)
  {
    rgb_map[i][0] = i;
    rgb_map[i][1] = 0;
    rgb_map[i][2] = i;
  }

  for(i=256; i<511; i++)
  {
    rgb_map[i][0] = 510 - i;
    rgb_map[i][1] = 0;
    rgb_map[i][2] = 255;
  }

  for(i=511; i<766; i++)
  {
    rgb_map[i][0] = 0;
    rgb_map[i][1] = i - 510;
    rgb_map[i][2] = 255;
  }

  for(i=766; i<1021; i++)
  {
    rgb_map[i][0] = 0;
    rgb_map[i][1] = 255;
    rgb_map[i][2] = 1020 - i;
  }

  for(i=1021; i<1276; i++)
  {
    rgb_map[i][0] = i - 1020;
    rgb_map[i][1] = 255;
    rgb_map[i][2] = 0;
  }

  for(i=1276; i<1531; i++)
  {
    rgb_map[i][0] = 255;
    rgb_map[i][1] = 1530 - i;
    rgb_map[i][2] = 0;
  }

  for(i=1531; i<1786; i++)
  {
    rgb_map[i][0] = 255;
    rgb_map[i][1] = i - 1530;
    rgb_map[i][2] = i - 1530;
  }

  if(log_checkbox->checkState() == Qt::Checked)
  {
    log_density = 1;
  }
  mainwindow->cdsa_log = log_density;

  if(pwr_voltage_checkbox->checkState() == Qt::Checked)
  {
    power_density = 1;
  }
  mainwindow->cdsa_pwr_voltage = power_density;

  samples_in_file = (long long)signalcomp->edfhdr->datarecords * (long long)signalcomp->edfhdr->edfparam[signalcomp->edfsignal[0]].smp_per_record;

//  printf("start_button_clicked(): samples_in_file: %lli\n", samples_in_file);

  segmentlen = segmentlen_spinbox->value();
  mainwindow->cdsa_segmentlen = segmentlen;

  blocklen = blocklen_spinbox->value();
  mainwindow->cdsa_blocklen = blocklen;

  h_min = min_hz_spinbox->value();
  mainwindow->cdsa_min_hz = h_min;

  h_max = max_hz_spinbox->value();
  mainwindow->cdsa_max_hz = h_max;

  if(power_density)
  {
    if(log_density)
    {
      v_scale = 1785.0 / ((max_pwr_spinbox->value() - min_pwr_spinbox->value()) / 10.0);

      log_minimum_offset = min_pwr_spinbox->value() / 10.0;
    }
    else
    {
      v_scale = 1785.0 / (max_voltage_spinbox->value() * max_voltage_spinbox->value());
    }
  }
  else
  {
    if(log_density)
    {
      v_scale = 1785.0 / ((max_pwr_spinbox->value() - min_pwr_spinbox->value()) / 20.0);

      log_minimum_offset = min_pwr_spinbox->value() / 20.0;
    }
    else
    {
      v_scale = 1785.0 / max_voltage_spinbox->value();
    }
  }

  mainwindow->cdsa_max_pwr = max_pwr_spinbox->value();
  mainwindow->cdsa_min_pwr = min_pwr_spinbox->value();
  mainwindow->cdsa_max_voltage = max_voltage_spinbox->value();

  window_func = windowfunc_combobox->currentIndex();
  mainwindow->cdsa_window_func = window_func;

  overlap = overlap_combobox->currentIndex() + 1;
  mainwindow->cdsa_overlap = overlap;

  smpls_in_segment = sf * segmentlen;

  smpl_in_block = sf * blocklen;

  segments_in_recording = samples_in_file / (long long)smpls_in_segment;

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
//   printf("start_button_clicked(): smpl_in_block: %i\n", smpl_in_block);
//
//   printf("start_button_clicked(): segments_in_recording: %i\n", segments_in_recording);

  w = segments_in_recording;

  h_max *= blocklen;

  h_min *= blocklen;

  h = h_max - h_min;

  FilteredBlockReadClass fbr;

  ret_err = 0;

  if(export_data)
  {
    path[0] = 0;
    if(mainwindow->recent_savedir[0]!=0)
    {
      strlcpy(path, mainwindow->recent_savedir, MAX_PATH_LENGTH);
      strlcat(path, "/", MAX_PATH_LENGTH);
    }
    len = strlen(path);
    get_filename_from_path(path + len, signalcomp->edfhdr->filename, MAX_PATH_LENGTH - len);
    remove_extension_from_filename(path);
    strlcat(path, "_", MAX_PATH_LENGTH);
    strlcpy(str, signalcomp->signallabel, 1024);
    trim_spaces(str);
    strlcat(path, str, MAX_PATH_LENGTH);
    strlcat(path, "_data.txt", MAX_PATH_LENGTH);

    strlcpy(path, QFileDialog::getSaveFileName(0, "Export data", QString::fromLocal8Bit(path), "Text/CSV files (*.txt *.TXT *.csv *.CSV)").toLocal8Bit().data(), MAX_PATH_LENGTH);
    if(!strcmp(path, ""))
    {
      return;
    }

    get_directory_from_path(mainwindow->recent_savedir, path, MAX_PATH_LENGTH);

    dat_f = fopen(path, "wb");
    if(dat_f == NULL)
    {
      QMessageBox::critical(myobjectDialog, "Error", "Cannot create the output file for writing.", QMessageBox::Close);
      return;
    }

    fprintf(dat_f,
            "Signal:         %s\n"
            "Segments:       %i\n"
            "Segment length: %i sec.\n"
            "Block length:   %i sec.\n"
            "Overlap:        %s\n"
            "Window:         %s\n"
            "Min. freq.:     %i Hz\n"
            "Max. freq.:     %i Hz\n",
            signalcomp->signallabel,
            segments_in_recording,
            segmentlen,
            blocklen,
            overlap_combobox->currentText().toLatin1().data(),
            windowfunc_combobox->currentText().toLatin1().data(),
            min_hz_spinbox->value(),
            max_hz_spinbox->value());

    if(power_density)
    {
      fprintf(dat_f, "Power:          yes\n");
    }
    else
    {
      fprintf(dat_f, "Power:          no\n");
    }

    if(log_density)
    {
      fprintf(dat_f, "Logarithmic:    yes\n");
    }
    else
    {
      fprintf(dat_f, "Logarithmic:    no\n");
    }

    if(log_density)
    {
      fprintf(dat_f, "Unit:           dB%s\n", signalcomp->physdimension);
    }
    else
    {
      fprintf(dat_f, "Unit:           %s\n", signalcomp->physdimension);
    }

    fprintf(dat_f, "seconds,");
    for(i=h_min; i<h_max; i++)
    {
      fprintf(dat_f, "%0.3f Hz,", (double)i / (double)blocklen);
    }
    fprintf(dat_f, "\n");
  }

  smplbuf = fbr.init_signalcomp(signalcomp, smpls_in_segment, 0, 1, &ret_err);
  if(smplbuf == NULL)
  {
    if(ret_err == 1)
    {
      QMessageBox::critical(myobjectDialog, "Error",
                            "The system was not able to provide enough resources (memory) to perform the requested action.",
                            QMessageBox::Close);
    }
    else
    {
      QMessageBox::critical(myobjectDialog, "Error", "Internal error (-1)", QMessageBox::Close);
    }
    if(dat_f)
    {
      fclose(dat_f);
      dat_f = NULL;
    }
    return;
  }

  pxm = new QPixmap(w, h);
  pxm->fill(Qt::black);

  QPainter painter(pxm);

  dft = fft_wrap_create(smplbuf, smpls_in_segment, smpl_in_block, window_func, overlap);
  if(dft == NULL)
  {
    QMessageBox::critical(myobjectDialog, "Error", "Internal error (-2)", QMessageBox::Close);
    if(dat_f)
    {
      fclose(dat_f);
      dat_f = NULL;
    }
    return;
  }

  QProgressDialog progress("Processing...", "Abort", 0, segments_in_recording);
  progress.setWindowModality(Qt::WindowModal);
  progress.setMinimumDuration(500);

  for(i=0; i<segments_in_recording; i++)
  {
    progress.setValue(i);

    qApp->processEvents();

    if(progress.wasCanceled() == true)
    {
      painter.end();
      progress.reset();
      free_fft_wrap(dft);
      delete pxm;
      if(dat_f)
      {
        fclose(dat_f);
        dat_f = NULL;
      }
      return;
    }

    err = fbr.process_signalcomp(i * smpls_in_segment);
    if(err)
    {
      snprintf(str, 1024, "Internal error (-3)  fbr() error: %i file: %s line %i", err, __FILE__, __LINE__);
      progress.reset();
      QMessageBox::critical(myobjectDialog, "Error", str, QMessageBox::Close);
      if(dat_f)
      {
        fclose(dat_f);
        dat_f = NULL;
      }
      return;
    }

    fft_wrap_run(dft);

    dft->buf_out[0] /= 2.0;  // DC!

    if(dat_f)
    {
      fprintf(dat_f, "%i,", i * segmentlen);
    }

    for(j=0; j<h; j++)
    {
      if(power_density)
      {
        d_tmp = dft->buf_out[j + h_min] / dft->dft_sz;

        if(log_density)
        {
          if(d_tmp < 1E-13)
          {
            d_tmp = 1E-13;
          }

          d_tmp = log10(d_tmp);

          rgb_idx = (d_tmp - log_minimum_offset) * v_scale;

          if(dat_f)
          {
            fprintf(dat_f, "%.1f,", d_tmp * 10);
          }
        }
        else
        {
          rgb_idx = d_tmp * v_scale;

          if(dat_f)
          {
            fprintf(dat_f, "%e,", d_tmp);
          }
        }
      }
      else
      {
        d_tmp = sqrt(dft->buf_out[j + h_min] / dft->dft_sz);

        if(log_density)
        {
          if(d_tmp < 1E-13)
          {
            d_tmp = 1E-13;
          }

          d_tmp = log10(d_tmp);

          rgb_idx = (d_tmp - log_minimum_offset) * v_scale;

          if(dat_f)
          {
            fprintf(dat_f, "%.1f,", d_tmp * 20);
          }
        }
        else
        {
          rgb_idx = d_tmp * v_scale;

          if(dat_f)
          {
            fprintf(dat_f, "%e,", d_tmp);
          }
        }
      }

      if(rgb_idx > 1785)  rgb_idx = 1785;

      if(rgb_idx < 0)  rgb_idx = 0;

      painter.setPen(QColor(rgb_map[rgb_idx][0], rgb_map[rgb_idx][1], rgb_map[rgb_idx][2]));

      painter.drawPoint(i, (h - 1) - j);
    }

    if(dat_f)
    {
      fputc('\n', dat_f);
    }
  }

  painter.end();

  progress.reset();

  free_fft_wrap(dft);

  dock_param.signalcomp = signalcomp;
  dock_param.sf = sf;
  dock_param.min_hz = mainwindow->cdsa_min_hz;
  dock_param.max_hz = mainwindow->cdsa_max_hz;
  dock_param.max_pwr = mainwindow->cdsa_max_pwr;
  dock_param.min_pwr = mainwindow->cdsa_min_pwr;
  dock_param.max_voltage = mainwindow->cdsa_max_voltage;
  dock_param.log = mainwindow->cdsa_log;
  dock_param.block_len = mainwindow->cdsa_blocklen;
  dock_param.overlap = mainwindow->cdsa_overlap;
  dock_param.window_func = mainwindow->cdsa_window_func;
  dock_param.power_voltage = mainwindow->cdsa_pwr_voltage;
  dock_param.pxm = pxm;
  dock_param.segment_len = segmentlen;
  dock_param.segments_in_recording = segments_in_recording;
  dock_param.instance_nr = cdsa_instance_nr;

  if(log_density)
  {
    strlcpy(dock_param.unit, "dB", 32);
    strlcat(dock_param.unit, signalcomp->edfhdr->edfparam[signalcomp->edfsignal[0]].physdimension, 32);
  }
  else
  {
    strlcpy(dock_param.unit, signalcomp->edfhdr->edfparam[signalcomp->edfsignal[0]].physdimension, 32);
  }
  remove_trailing_spaces(dock_param.unit);

  mainwindow->cdsa_dock[cdsa_instance_nr] = new UI_cdsa_dock(mainwindow, dock_param);

  signalcomp->cdsa_dock[cdsa_instance_nr] = cdsa_instance_nr + 1;

  pxm = NULL;

  if(dat_f)
  {
    fclose(dat_f);
    dat_f = NULL;
  }

  myobjectDialog->close();
}





























