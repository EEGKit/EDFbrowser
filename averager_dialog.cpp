/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2011 - 2021 Teunis van Beelen
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




#include "averager_dialog.h"



UI_AveragerWindow::UI_AveragerWindow(QWidget *w_parent, struct annotationblock *annot)
{
  int i;

  mainwindow = (UI_Mainwindow *)w_parent;

  edf_hdr = (struct edfhdrblock *)(annot->edfhdr);

  averager_dialog = new QDialog(w_parent);

  averager_dialog->setMinimumSize(600 * mainwindow->w_scaling, 400 * mainwindow->h_scaling);
  averager_dialog->setWindowTitle("Average waveforms");
  averager_dialog->setModal(true);
  averager_dialog->setAttribute(Qt::WA_DeleteOnClose, true);
  averager_dialog->setSizeGripEnabled(true);

  annot_name_line_edit = new QLineEdit;

  QLabel *signalLabel = new QLabel;
  signalLabel->setText("Select signal(s):");

  list = new QListWidget;
  list->setSelectionBehavior(QAbstractItemView::SelectRows);
  list->setSelectionMode(QAbstractItemView::ExtendedSelection);

  time1.setHMS(0, 0, 0, 0);

  time2.setHMS((edf_hdr->recording_len_sec / 3600) % 24, (edf_hdr->recording_len_sec % 3600) / 60, edf_hdr->recording_len_sec % 60, 0);

  day_spinbox1 = new QSpinBox;
  day_spinbox1->setRange(0, 9);
  day_spinbox1->setToolTip("Days (24-hour units)");

  timeEdit1 = new QTimeEdit;
  timeEdit1->setDisplayFormat("hh:mm:ss.zzz");
  timeEdit1->setMinimumTime(QTime(0, 0, 0, 0));

  day_spinbox2 = new QSpinBox;
  day_spinbox2->setRange(0, 9);
  day_spinbox2->setToolTip("Days (24-hour units)");
  day_spinbox2->setValue(edf_hdr->recording_len_sec / 86400);

  timeEdit2 = new QTimeEdit;
  timeEdit2->setDisplayFormat("hh:mm:ss.zzz");
  timeEdit2->setMinimumTime(QTime(0, 0, 0, 0));
  timeEdit2->setTime(time2);

  ratioBox = new QComboBox;
  ratioBox->addItem("10 / 90");
  ratioBox->addItem("25 / 75");
  ratioBox->addItem("50 / 50");
  ratioBox->setCurrentIndex(mainwindow->average_ratio);

  avg_periodspinbox = new QSpinBox;
  avg_periodspinbox->setRange(10, 300000);
  avg_periodspinbox->setSuffix(" mSec");
  avg_periodspinbox->setValue(mainwindow->average_period);

  CloseButton = new QPushButton;
  CloseButton->setText("Cancel");

  StartButton = new QPushButton;
  StartButton->setText("Start");

  set_start_button = new QPushButton;
  set_start_button->setText("Set to minimum");

  set_end_button = new QPushButton;
  set_end_button->setText("Set to maximum");

  set_display_range_button = new QPushButton;
  set_display_range_button->setText("as displayed");

  for(i=0; i<mainwindow->signalcomps; i++)
  {
    if(mainwindow->signalcomp[i]->alias[0] != 0)
    {
      new QListWidgetItem(mainwindow->signalcomp[i]->alias, list);
    }
    else
    {
      new QListWidgetItem(mainwindow->signalcomp[i]->signallabel, list);
    }
  }

  list->setCurrentRow(0, QItemSelectionModel::Select);

  strlcpy(annot_str, annot->description, MAX_ANNOTATION_LEN + 1);
  trim_spaces(annot_str);

  annot_name_line_edit->setText(annot_str);

  QHBoxLayout *hlayout3 = new QHBoxLayout;
  hlayout3->addWidget(day_spinbox1);
  hlayout3->addWidget(timeEdit1);
  hlayout3->addWidget(set_start_button);

  QHBoxLayout *hlayout4 = new QHBoxLayout;
  hlayout4->addWidget(day_spinbox2);
  hlayout4->addWidget(timeEdit2);
  hlayout4->addWidget(set_end_button);

  QHBoxLayout *hlayout5 = new QHBoxLayout;
  hlayout5->addWidget(set_display_range_button);
  hlayout5->addStretch(1000);

  QFormLayout *flayout = new QFormLayout;
  flayout->addRow(" ", (QWidget *)NULL);
  flayout->addRow("Use annotation:", annot_name_line_edit);
  flayout->addRow(" ", (QWidget *)NULL);
  flayout->addRow("Average period:", avg_periodspinbox);
  flayout->addRow(" ", (QWidget *)NULL);
  flayout->addRow("Ratio of time before and after trigger:", ratioBox);
  flayout->addRow(" ", (QWidget *)NULL);
  flayout->addRow("Range from: (d:hh:mm:ss.mmm)", hlayout3);
  flayout->addRow(" ", (QWidget *)NULL);
  flayout->addRow("Range to: (d:hh:mm:ss.mmm)", hlayout4);
  flayout->addRow(" ", (QWidget *)NULL);
  flayout->addRow("Set range:", hlayout5);
  flayout->addRow(" ", (QWidget *)NULL);

  QHBoxLayout *hlayout2 = new QHBoxLayout;
  hlayout2->addWidget(CloseButton);
  hlayout2->addStretch(1000);

  QVBoxLayout *vlayout1 = new QVBoxLayout;
  vlayout1->addLayout(flayout);
  vlayout1->addStretch(1000);
  vlayout1->addSpacing(20);
  vlayout1->addLayout(hlayout2);

  QVBoxLayout *vlayout2 = new QVBoxLayout;
  vlayout2->addWidget(signalLabel);
  vlayout2->addStretch(1000);
  vlayout2->addWidget(StartButton);

  QHBoxLayout *hlayout1 = new QHBoxLayout;
  hlayout1->addLayout(vlayout1);
  hlayout1->addSpacing(20);
  hlayout1->addLayout(vlayout2);
  hlayout1->addWidget(list);

  averager_dialog->setLayout(hlayout1);

  QObject::connect(CloseButton,              SIGNAL(clicked()), averager_dialog, SLOT(close()));
  QObject::connect(StartButton,              SIGNAL(clicked()), this,            SLOT(startButtonClicked()));
  QObject::connect(set_start_button,         SIGNAL(clicked()), this,            SLOT(set_start_button_clicked()));
  QObject::connect(set_end_button,           SIGNAL(clicked()), this,            SLOT(set_end_button_clicked()));
  QObject::connect(set_display_range_button, SIGNAL(clicked()), this,            SLOT(set_display_range_button_clicked()));

  averager_dialog->exec();
}



void UI_AveragerWindow::startButtonClicked()
{
  int i, j, n,
      avg_cnt=0,
      samples_on_screen,
      trigger_position_ratio=4,
      dialoglistnumber,
      itemCnt,
      progress_steps;

  char str[MAX_ANNOTATION_LEN + 1],
       scratchpad[1024];

  double avg_max_value,
         avg_min_value;

  long long backup_viewtime,
            backup_timescale,
            l_time1,
            l_time2;

  struct annotationblock *annot_ptr;

  QList<QListWidgetItem *> itemList;

  strlcpy(annot_str, annot_name_line_edit->text().toUtf8().data(), MAX_ANNOTATION_LEN + 1);
  trim_spaces(annot_str);
  if(!strlen(annot_str))
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "You have to enter an annotation.");
    messagewindow.exec();
    return;
  }

  mainwindow->average_period = avg_periodspinbox->value();
  mainwindow->average_ratio = ratioBox->currentIndex();

  itemList = list->selectedItems();

  itemCnt = itemList.size();

  if(itemCnt < 1)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "You have to select at least one signal.");
    messagewindow.exec();
    return;
  }

  if(itemCnt > MAXAVERAGECURVEDIALOGS)
  {
    snprintf(scratchpad, 1024, "You cannot select more than %i signals.", MAXAVERAGECURVEDIALOGS);
    QMessageBox messagewindow(QMessageBox::Critical, "Error", scratchpad);
    messagewindow.exec();
    return;
  }

  time1 = timeEdit1->time();
  time2 = timeEdit2->time();

  l_time1 = (((time1.hour() * 3600) + (time1.minute() * 60) + (time1.second())) * TIME_DIMENSION) + (time1.msec() * TIME_DIMENSION / 1000LL) + (day_spinbox1->value() * 86400LL * TIME_DIMENSION);
  l_time2 = (((time2.hour() * 3600) + (time2.minute() * 60) + (time2.second())) * TIME_DIMENSION) + (time2.msec() * TIME_DIMENSION / 1000LL) + (day_spinbox2->value() * 86400LL * TIME_DIMENSION);

  if(l_time1 >= l_time2)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Start time is higher or equal to stop time.");
    messagewindow.exec();
    return;
  }

  n = ratioBox->currentIndex();

  switch(n)
  {
    case 0 : trigger_position_ratio = 10;
            break;
    case 1 : trigger_position_ratio = 4;
            break;
    case 2 : trigger_position_ratio = 2;
            break;
  }

  backup_viewtime = edf_hdr->viewtime;

  backup_timescale = mainwindow->pagetime;

  mainwindow->pagetime = avg_periodspinbox->value() * (TIME_DIMENSION / 1000LL);

  mainwindow->setup_viewbuf();

  mainwindow->signal_averaging_active = 1;

  n = edfplus_annotation_size(&(edf_hdr->annot_list));

  avg_cnt = 0;

  for(i=0; i<n; i++)
  {
    annot_ptr = edfplus_annotation_get_item(&(edf_hdr->annot_list), i);

    if(((annot_ptr->onset - edf_hdr->starttime_offset) >= l_time1)
      && ((annot_ptr->onset - edf_hdr->starttime_offset) <= l_time2))
    {
      strlcpy(str, annot_ptr->description, MAX_ANNOTATION_LEN + 1);

      trim_spaces(str);

      if(!strcmp(str, annot_str))
      {
        avg_cnt++;
      }
    }
  }

  QProgressDialog progress("Averaging ...", "Abort", 0, avg_cnt, averager_dialog);
  progress.setWindowModality(Qt::WindowModal);
  progress.setMinimumDuration(200);
  progress.reset();

  progress_steps = avg_cnt / 100;
  if(progress_steps < 1)
  {
    progress_steps = 1;
  }

  for(j=0; j<itemCnt; j++)
  {
    for(i=0; i<MAXAVERAGECURVEDIALOGS; i++)
    {
      if(mainwindow->averagecurvedialog[i] == NULL)
      {
        dialoglistnumber = i;

        break;
      }
    }

    if(i >= MAXAVERAGECURVEDIALOGS)
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "Too many \"Average\" windows are open.\nClose some first.");
      messagewindow.exec();

      edf_hdr->viewtime = backup_viewtime;
      mainwindow->pagetime = backup_timescale;
      mainwindow->signal_averaging_active = 0;
      mainwindow->setup_viewbuf();

      return;
    }

    signal_nr = list->row(itemList.at(j));

    if(mainwindow->signalcomp[signal_nr]->samples_on_screen > 2147483646LL)
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "Too many samples in buf.");
      messagewindow.exec();

      edf_hdr->viewtime = backup_viewtime;
      mainwindow->pagetime = backup_timescale;
      mainwindow->signal_averaging_active = 0;
      mainwindow->setup_viewbuf();

      return;
    }

    samples_on_screen = mainwindow->signalcomp[signal_nr]->samples_on_screen;

    avgbuf = (double *)calloc(1, sizeof(double) * samples_on_screen);
    if(avgbuf == NULL)
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "The system was not able to provide enough resources (memory) to perform the requested action.");
      messagewindow.exec();

      edf_hdr->viewtime = backup_viewtime;
      mainwindow->pagetime = backup_timescale;
      mainwindow->signal_averaging_active = 0;
      mainwindow->setup_viewbuf();

      return;
    }

    n = edfplus_annotation_size(&(edf_hdr->annot_list));

    avg_cnt = 0;

    for(i=0; i<n; i++)
    {
      annot_ptr = edfplus_annotation_get_item(&(edf_hdr->annot_list), i);

      if(annot_ptr->hided_in_list)
      {
        continue;
      }

      if(((annot_ptr->onset - edf_hdr->starttime_offset) >= l_time1)
        && ((annot_ptr->onset - edf_hdr->starttime_offset) <= l_time2))
      {
        strlcpy(str, annot_ptr->description, MAX_ANNOTATION_LEN + 1);

        trim_spaces(str);

        if(!strcmp(str, annot_str))
        {
          if(!(avg_cnt % progress_steps))
          {
            progress.setValue(avg_cnt);

            qApp->processEvents();

            if(progress.wasCanceled() == true)
            {
              free(avgbuf);

              edf_hdr->viewtime = backup_viewtime;

              mainwindow->pagetime = backup_timescale;

              mainwindow->signal_averaging_active = 0;

              mainwindow->setup_viewbuf();

              return;
            }
          }

          edf_hdr->viewtime = annot_ptr->onset;

          edf_hdr->viewtime -= edf_hdr->starttime_offset;

          edf_hdr->viewtime -= (mainwindow->pagetime / trigger_position_ratio);

          mainwindow->setup_viewbuf();

          process_avg(mainwindow->signalcomp[signal_nr]);

          avg_cnt++;
        }
      }
    }

    progress.reset();

    if(!avg_cnt)
    {
      snprintf(scratchpad, 1024,
               "The selected annotation/trigger \"%s\" is not found in the selected timewindow\n"
               "%i:%02i:%02i - %i:%02i:%02i",
               annot_str,
               timeEdit1->time().hour(), timeEdit1->time().minute(), timeEdit1->time().second(),
               timeEdit2->time().hour(), timeEdit2->time().minute(), timeEdit2->time().second());

      QMessageBox messagewindow(QMessageBox::Critical, "Error", scratchpad);
      messagewindow.exec();

      free(avgbuf);

      edf_hdr->viewtime = backup_viewtime;

      mainwindow->pagetime = backup_timescale;

      mainwindow->signal_averaging_active = 0;

      mainwindow->setup_viewbuf();

      return;
    }

    avg_max_value = -100000000.0;
    avg_min_value =  100000000.0;

    for(i=0; i<samples_on_screen; i++)
    {
      avgbuf[i] /= avg_cnt;

      if(avgbuf[i] > avg_max_value)
      {
        avg_max_value = avgbuf[i];
      }

      if(avgbuf[i] < avg_min_value)
      {
        avg_min_value = avgbuf[i];
      }
    }

    mainwindow->averagecurvedialog[dialoglistnumber] = new UI_AverageCurveWindow(mainwindow->signalcomp[signal_nr], mainwindow, dialoglistnumber,
                                                                                 avgbuf,
                                                                                 avg_max_value,
                                                                                 avg_min_value,
                                                                                 mainwindow->pagetime,
                                                                                 mainwindow->signalcomp[signal_nr]->samples_on_screen,
                                                                                 avg_cnt,
                                                                                 trigger_position_ratio,
                                                                                 annot_str,
                                                                                 avg_periodspinbox->value());

    for(i=0; i<MAXAVERAGECURVEDIALOGS; i++)
    {
      if(mainwindow->signalcomp[signal_nr]->avg_dialog[i] == 0)
      {
        mainwindow->signalcomp[signal_nr]->avg_dialog[i] = dialoglistnumber + 1;

        break;
      }
    }
  }

  edf_hdr->viewtime = backup_viewtime;

  mainwindow->pagetime = backup_timescale;

  mainwindow->signal_averaging_active = 0;

  mainwindow->setup_viewbuf();

  averager_dialog->close();
}


UI_AveragerWindow::~UI_AveragerWindow()
{
  int i;

  for(i=0; i<MAXAVERAGECURVEDIALOGS; i++)
  {
    if(mainwindow->averagecurvedialog[i] != NULL)
    {
      mainwindow->averagecurvedialog[i]->averager_curve_dialog->move(((i % 15) * 30) + 200, ((i % 15) * 30) + 200);
      mainwindow->averagecurvedialog[i]->averager_curve_dialog->raise();
      mainwindow->averagecurvedialog[i]->averager_curve_dialog->activateWindow();
    }
  }
}


void UI_AveragerWindow::process_avg(struct signalcompblock *signalcomp)
{
  int j, k;

  char *viewbuf;

  unsigned long long s, s2;

  double dig_value=0.0,
         f_tmp=0.0;

  union {
          unsigned int one;
          signed int one_signed;
          unsigned short two[2];
          signed short two_signed[2];
          unsigned char four[4];
        } var;

  viewbuf = mainwindow->viewbuf;

  if(viewbuf == NULL)
  {
    return;
  }

  if(avgbuf == NULL)
  {
    return;
  }

  for(s=signalcomp->sample_start; s<signalcomp->samples_on_screen; s++)
  {
    if(s>=signalcomp->sample_stop)  break;

    dig_value = 0.0;
    s2 = s + signalcomp->sample_timeoffset - signalcomp->sample_start;

    for(j=0; j<signalcomp->num_of_signals; j++)
    {
      if(signalcomp->edfhdr->bdf)
      {
        var.two[0] = *((unsigned short *)(
          viewbuf
          + signalcomp->viewbufoffset
          + (signalcomp->edfhdr->recordsize * (s2 / signalcomp->edfhdr->edfparam[signalcomp->edfsignal[j]].smp_per_record))
          + signalcomp->edfhdr->edfparam[signalcomp->edfsignal[j]].buf_offset
          + ((s2 % signalcomp->edfhdr->edfparam[signalcomp->edfsignal[j]].smp_per_record) * 3)));

        var.four[2] = *((unsigned char *)(
          viewbuf
          + signalcomp->viewbufoffset
          + (signalcomp->edfhdr->recordsize * (s2 / signalcomp->edfhdr->edfparam[signalcomp->edfsignal[j]].smp_per_record))
          + signalcomp->edfhdr->edfparam[signalcomp->edfsignal[j]].buf_offset
          + ((s2 % signalcomp->edfhdr->edfparam[signalcomp->edfsignal[j]].smp_per_record) * 3)
          + 2));

        if(var.four[2]&0x80)
        {
          var.four[3] = 0xff;
        }
        else
        {
          var.four[3] = 0x00;
        }

        f_tmp = var.one_signed;
      }

      if(signalcomp->edfhdr->edf)
      {
        f_tmp = *(((short *)(
          viewbuf
          + signalcomp->viewbufoffset
          + (signalcomp->edfhdr->recordsize * (s2 / signalcomp->edfhdr->edfparam[signalcomp->edfsignal[j]].smp_per_record))
          + signalcomp->edfhdr->edfparam[signalcomp->edfsignal[j]].buf_offset))
          + (s2 % signalcomp->edfhdr->edfparam[signalcomp->edfsignal[j]].smp_per_record));
      }

      f_tmp += signalcomp->edfhdr->edfparam[signalcomp->edfsignal[j]].offset;
      f_tmp *= signalcomp->factor[j];

      dig_value += f_tmp;
    }

    if(signalcomp->spike_filter)
    {
      if(s==signalcomp->sample_start)
      {
        spike_filter_restore_buf(signalcomp->spike_filter);
      }

      dig_value = run_spike_filter(dig_value, signalcomp->spike_filter);
    }

    for(k=0; k<signalcomp->filter_cnt; k++)
    {
      dig_value = first_order_filter(dig_value, signalcomp->filter[k]);
    }

    for(k=0; k<signalcomp->ravg_filter_cnt; k++)
    {
      if(s==signalcomp->sample_start)
      {
        ravg_filter_restore_buf(signalcomp->ravg_filter[k]);
      }

      dig_value = run_ravg_filter(dig_value, signalcomp->ravg_filter[k]);
    }

    for(k=0; k<signalcomp->fidfilter_cnt; k++)
    {
      if(s==signalcomp->sample_start)
      {
        memcpy(signalcomp->fidbuf[k], signalcomp->fidbuf2[k], fid_run_bufsize(signalcomp->fid_run[k]));
      }

      dig_value = signalcomp->fidfuncp[k](signalcomp->fidbuf[k], dig_value);
    }

    if(signalcomp->fir_filter != NULL)
    {
      if(s==signalcomp->sample_start)
      {
        fir_filter_restore_buf(signalcomp->fir_filter);
      }

      dig_value = run_fir_filter(dig_value, signalcomp->fir_filter);
    }

    if(signalcomp->plif_ecg_filter)
    {
      if(s==signalcomp->sample_start)
      {
        if(signalcomp->edfhdr->viewtime<=0)
        {
          plif_reset_subtract_filter(signalcomp->plif_ecg_filter, 0);
        }
        else
        {
          plif_subtract_filter_state_copy(signalcomp->plif_ecg_filter, signalcomp->plif_ecg_filter_sav);
        }
      }

      dig_value = plif_run_subtract_filter(dig_value, signalcomp->plif_ecg_filter);
    }

    if(signalcomp->ecg_filter != NULL)
    {
      if(s==signalcomp->sample_start)
      {
        ecg_filter_restore_buf(signalcomp->ecg_filter);
      }

      dig_value = run_ecg_filter(dig_value, signalcomp->ecg_filter);
    }

    avgbuf[s] += (dig_value * signalcomp->edfhdr->edfparam[signalcomp->edfsignal[0]].bitvalue);
  }
}


void UI_AveragerWindow::set_start_button_clicked()
{
  time1.setHMS(0, 0, 0, 0);

  timeEdit1->setTime(time1);

  day_spinbox1->setValue(0);
}


void UI_AveragerWindow::set_end_button_clicked()
{
  time2.setHMS((edf_hdr->recording_len_sec / 3600) % 24, (edf_hdr->recording_len_sec % 3600) / 60, edf_hdr->recording_len_sec % 60, 0);

  timeEdit2->setTime(time2);

  day_spinbox2->setValue(edf_hdr->recording_len_sec / 86400);
}


void UI_AveragerWindow::set_display_range_button_clicked()
{
  long long starttime = edf_hdr->viewtime / TIME_DIMENSION;

  long long endtime = (edf_hdr->viewtime + mainwindow->pagetime) / TIME_DIMENSION;

  time1.setHMS((starttime / 3600) % 24, (starttime % 3600) / 60, starttime % 60, 0);

  timeEdit1->setTime(time1);

  day_spinbox1->setValue(starttime / 86400);

  time2.setHMS((endtime / 3600) % 24, (endtime % 3600) / 60, endtime % 60, 0);

  timeEdit2->setTime(time2);

  day_spinbox2->setValue(endtime / 86400);
}



















