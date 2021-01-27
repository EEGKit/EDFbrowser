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



#include "cdsa_dialog.h"





UI_cdsa_window::UI_cdsa_window(QWidget *w_parent, struct signalcompblock *signal_comp, int numb)
{
  char str[1024]={""};

  mainwindow = (UI_Mainwindow *)w_parent;

  signalcomp = signal_comp;

  cdsa_instance_nr = numb;

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
  blocklen_spinbox->setToolTip("FFT block length in seconds, affects the FFT resolution");

  overlap_combobox = new QComboBox;
  overlap_combobox->addItem(" 0 %");
  overlap_combobox->addItem("50 %");
  overlap_combobox->addItem("67 %");
  overlap_combobox->addItem("75 %");
  overlap_combobox->addItem("80 %");
  overlap_combobox->setCurrentIndex(mainwindow->cdsa_overlap -1);
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

  close_button = new QPushButton;
  close_button->setText("Close");

  default_button = new QPushButton;
  default_button->setText("Default");

  start_button = new QPushButton;
  start_button->setText("Start");

  flayout = new QFormLayout;
  flayout->addRow("Segment length", segmentlen_spinbox);
  flayout->addRow("Block length", blocklen_spinbox);
  flayout->addRow("Overlap", overlap_combobox);
  flayout->addRow("Window", windowfunc_combobox);
  flayout->addRow("Min. freq.", min_hz_spinbox);
  flayout->addRow("Max. freq.", max_hz_spinbox);
  flayout->addRow("Max. level", max_pwr_spinbox);
  flayout->addRow("Min. level", min_pwr_spinbox);
  flayout->addRow("Max. level", max_voltage_spinbox);
  flayout->addRow("Logarithmic", log_checkbox);
  flayout->addRow("Power", pwr_voltage_checkbox);

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
    QObject::connect(default_button,       SIGNAL(clicked()),            this, SLOT(default_button_clicked()));
    QObject::connect(start_button,         SIGNAL(clicked()),            this, SLOT(start_button_clicked()));
    QObject::connect(segmentlen_spinbox,   SIGNAL(valueChanged(int)),    this, SLOT(segmentlen_spinbox_changed(int)));
    QObject::connect(blocklen_spinbox,     SIGNAL(valueChanged(int)),    this, SLOT(blocklen_spinbox_changed(int)));
    QObject::connect(min_hz_spinbox,       SIGNAL(valueChanged(int)),    this, SLOT(min_hz_spinbox_changed(int)));
    QObject::connect(max_hz_spinbox,       SIGNAL(valueChanged(int)),    this, SLOT(max_hz_spinbox_changed(int)));
    QObject::connect(min_pwr_spinbox,      SIGNAL(valueChanged(int)),    this, SLOT(min_pwr_spinbox_changed(int)));
    QObject::connect(max_pwr_spinbox,      SIGNAL(valueChanged(int)),    this, SLOT(max_pwr_spinbox_changed(int)));
    QObject::connect(log_checkbox,         SIGNAL(stateChanged(int)),    this, SLOT(log_checkbox_changed(int)));
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
  int i, j, w, h, h_min, h_max, err,
      smpls_in_segment,
      segments_in_recording,
      segmentlen,
      blocklen,
      smpl_in_block,
      overlap,
      window_func,
      log_density=0,
      power_density=0;

  long long samples_in_file;

  int rgb_map[1786][3],
      rgb_idx;

  double v_scale,
         d_tmp,
         *smplbuf=NULL,
         log_minimum_offset=0;

  struct fft_wrap_settings_struct *dft;

  QPixmap *pxm=NULL;

  struct cdsa_dock_param_struct dock_param;

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

  w = segments_in_recording;

  h_max *= blocklen;

  h_min *= blocklen;

  h = h_max - h_min;

  FilteredBlockReadClass fbr;

  smplbuf = fbr.init_signalcomp(signalcomp, smpls_in_segment, 0, 1);
  if(smplbuf == NULL)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Internal error (-1)");
    messagewindow.exec();
    return;
  }

  pxm = new QPixmap(w, h);
  pxm->fill(Qt::black);

  QPainter painter(pxm);

  dft = fft_wrap_create(smplbuf, smpls_in_segment, smpl_in_block, window_func, overlap);
  if(dft == NULL)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Internal error (-2)");
    messagewindow.exec();
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
      return;
    }

    err = fbr.process_signalcomp(i * smpls_in_segment);
    if(err)
    {
      progress.reset();
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "Internal error (-3)");
      messagewindow.exec();
      return;
    }

    fft_wrap_run(dft);

    dft->buf_out[0] /= 2.0;  // DC!

    for(j=0; j<h; j++)
    {
      if(power_density)
      {
        if(log_density)
        {
          d_tmp = dft->buf_out[j + h_min] / dft->dft_sz;

          if(d_tmp < 1E-13)
          {
            rgb_idx = (log10(1E-13) - log_minimum_offset) * v_scale;
          }
          else
          {
            rgb_idx = (log10(d_tmp) - log_minimum_offset) * v_scale;
          }
        }
        else
        {
          rgb_idx = (dft->buf_out[j + h_min] / dft->dft_sz) * v_scale;
        }
      }
      else
      {
        if(log_density)
        {
          d_tmp = sqrt(dft->buf_out[j + h_min] / dft->dft_sz);

          if(d_tmp < 1E-13)
          {
            rgb_idx = (log10(1E-13) - log_minimum_offset) * v_scale;
          }
          else
          {
            rgb_idx = (log10(d_tmp) - log_minimum_offset) * v_scale;
          }
        }
        else
        {
          rgb_idx = sqrt(dft->buf_out[j + h_min] / dft->dft_sz) * v_scale;
        }
      }

      if(rgb_idx > 1785)  rgb_idx = 1785;

      if(rgb_idx < 0)  rgb_idx = 0;

      painter.setPen(QColor(rgb_map[rgb_idx][0], rgb_map[rgb_idx][1], rgb_map[rgb_idx][2]));

      painter.drawPoint(i, (h - 1) - j);
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

  myobjectDialog->close();
}





























