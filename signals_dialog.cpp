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



#include "signals_dialog.h"

#define DEFAULT_COLOR_LIST_SZ  (6)


static const char derivation_err_str[]={
  "It is only possible to make derivations from signals which:\n"
  " - are from the same file\n"
  " - have the same samplerate\n"
  " - have the same physical dimension (e.g. uV)\n"
  " - have the same resolution (e.g. uV/bit)"};



UI_Signalswindow::UI_Signalswindow(QWidget *w_parent)
{
  int i, tmp;

  mainwindow = (UI_Mainwindow *)w_parent;

  smp_per_record = 0;

  color_selected = 0;

  default_color_idx = 0;

  default_color_list[0] = Qt::yellow;
  default_color_list[1] = Qt::green;
  default_color_list[2] = Qt::red;
  default_color_list[3] = Qt::cyan;
  default_color_list[4] = Qt::magenta;
  default_color_list[5] = Qt::blue;

  if(mainwindow->signalcomps > 0)
  {
    tmp = mainwindow->signalcomp[mainwindow->signalcomps - 1]->color;

    for(i=0; i<DEFAULT_COLOR_LIST_SZ; i++)
    {
      if(default_color_list[i] == tmp)  break;
    }

    if(i < DEFAULT_COLOR_LIST_SZ)
    {
      default_color_idx = (i + 1) % DEFAULT_COLOR_LIST_SZ;
    }
  }

  SignalsDialog = new QDialog;

  SignalsDialog->setMinimumSize(850 * mainwindow->w_scaling, 500 * mainwindow->h_scaling);
  SignalsDialog->setWindowTitle("Add signals");
  SignalsDialog->setModal(true);
  SignalsDialog->setAttribute(Qt::WA_DeleteOnClose, true);
  SignalsDialog->setSizeGripEnabled(true);

  filelist = new QListWidget;
  filelist->setMaximumHeight(75);
  for(i=0; i<mainwindow->files_open; i++)
  {
    new QListWidgetItem(QString::fromLocal8Bit(mainwindow->edfheaderlist[i]->filename), filelist);
  }

  label1 = new QLabel;

  label2 = new QLabel;

  label3 = new QLabel;

  label4 = new QLabel;

  label5 = new QLabel;
  label5->setText("Signals in file");

  label6 = new QLabel;
  label6->setText("Signal Composition (derivation)");

  signallist = new QListWidget;
  signallist->setFont(*mainwindow->monofont);
  signallist->setSelectionBehavior(QAbstractItemView::SelectRows);
  signallist->setSelectionMode(QAbstractItemView::ExtendedSelection);

  CloseButton = new QPushButton;
  CloseButton->setText("&Close");

  SelectAllButton = new QPushButton;
  SelectAllButton->setText("&Select All");

  HelpButton = new QPushButton;
  HelpButton->setText("&Help");

  DisplayButton = new QPushButton;
  DisplayButton->setText("&Add signal(s)");
  DisplayButton->setToolTip("Add the above selected signals to the screen (unipolar)");

  DisplayCompButton = new QPushButton;
  DisplayCompButton->setText("&Make derivation");
  DisplayCompButton->setToolTip("Make a derivation of the above selected signals");

  AddButton = new QPushButton;
  AddButton->setText("Add->");

  SubtractButton = new QPushButton;
  SubtractButton->setText("Subtract->");

  RemoveButton = new QPushButton;
  RemoveButton->setText("Remove<-");

  ColorButton = new SpecialButton;
  ColorButton->setMinimumHeight(25);
  if(mainwindow->use_diverse_signal_colors)
  {
    ColorButton->setColor(127);
  }
  else
  {
    ColorButton->setColor((Qt::GlobalColor)mainwindow->maincurve->signal_color);
  }
  ColorButton->setToolTip("Click to select the trace color");
  ColorButton->setText("Trace color");

  compositionlist = new QTableWidget;
  compositionlist->setSelectionBehavior(QAbstractItemView::SelectRows);
  compositionlist->setSelectionMode(QAbstractItemView::ExtendedSelection);
  compositionlist->setColumnCount(4);
  compositionlist->setColumnWidth(0, 30);
  compositionlist->setColumnWidth(1, 150);
  compositionlist->setColumnWidth(2, 100);
  compositionlist->setColumnWidth(3, 100);

  QStringList horizontallabels;
  horizontallabels += "Signal";
  horizontallabels += "Label";
  horizontallabels += "Factor";
  horizontallabels += "Samplerate";
  compositionlist->setHorizontalHeaderLabels(horizontallabels);
  compositionlist->resizeColumnsToContents();

  QHBoxLayout *hlayout1 = new QHBoxLayout;
  hlayout1->addWidget(label1);
  hlayout1->addStretch(1000);

  QHBoxLayout *hlayout2 = new QHBoxLayout;
  hlayout2->addWidget(label2);
  hlayout2->addStretch(1000);
  hlayout2->addWidget(HelpButton);

  QHBoxLayout *hlayout3 = new QHBoxLayout;
  hlayout3->addWidget(label3);
  hlayout3->addStretch(100);
  hlayout3->addWidget(label4);
  hlayout3->addStretch(900);

  QHBoxLayout *hlayout5 = new QHBoxLayout;
  hlayout5->addWidget(SelectAllButton);
  hlayout5->addStretch(1000);
  hlayout5->addWidget(DisplayButton);

  QHBoxLayout *hlayout6 = new QHBoxLayout;
  hlayout6->addWidget(DisplayCompButton);
  hlayout6->addStretch(1000);
  hlayout6->addWidget(CloseButton);

  QVBoxLayout *vlayout2 = new QVBoxLayout;
  vlayout2->addWidget(label5);
  vlayout2->addWidget(signallist, 1000);
  vlayout2->addSpacing(35);
  vlayout2->addLayout(hlayout5);

  QVBoxLayout *vlayout3 = new QVBoxLayout;
  vlayout3->addStretch(1000);
  vlayout3->addWidget(AddButton);
  vlayout3->addWidget(SubtractButton);
  vlayout3->addWidget(RemoveButton);
  vlayout3->addSpacing(35);
  vlayout3->addWidget(ColorButton);
  vlayout3->addStretch(1000);

  QVBoxLayout *vlayout4 = new QVBoxLayout;
  vlayout4->addWidget(label6);
  vlayout4->addWidget(compositionlist, 1000);
  vlayout4->addSpacing(35);
  vlayout4->addLayout(hlayout6);

  QHBoxLayout *hlayout4 = new QHBoxLayout;
  hlayout4->addLayout(vlayout2);
  hlayout4->addLayout(vlayout3);
  hlayout4->addLayout(vlayout4);

  QVBoxLayout *vlayout1 = new QVBoxLayout;
  vlayout1->addWidget(filelist);
  vlayout1->addLayout(hlayout1);
  vlayout1->addLayout(hlayout2);
  vlayout1->addLayout(hlayout3);
  vlayout1->addSpacing(10);
  vlayout1->addLayout(hlayout4, 1000);

  SignalsDialog->setLayout(vlayout1);

  QObject::connect(CloseButton,       SIGNAL(clicked()),                SignalsDialog, SLOT(close()));
  QObject::connect(SelectAllButton,   SIGNAL(clicked()),                this,          SLOT(SelectAllButtonClicked()));
  QObject::connect(HelpButton,        SIGNAL(clicked()),                this,          SLOT(HelpButtonClicked()));
  QObject::connect(DisplayButton,     SIGNAL(clicked()),                this,          SLOT(DisplayButtonClicked()));
  QObject::connect(DisplayCompButton, SIGNAL(clicked()),                this,          SLOT(DisplayCompButtonClicked()));
  QObject::connect(AddButton,         SIGNAL(clicked()),                this,          SLOT(AddButtonClicked()));
  QObject::connect(SubtractButton,    SIGNAL(clicked()),                this,          SLOT(SubtractButtonClicked()));
  QObject::connect(RemoveButton,      SIGNAL(clicked()),                this,          SLOT(RemoveButtonClicked()));
  QObject::connect(ColorButton,       SIGNAL(clicked(SpecialButton *)), this,          SLOT(ColorButtonClicked(SpecialButton *)));
  QObject::connect(filelist,          SIGNAL(currentRowChanged(int)),   this,          SLOT(show_signals(int)));

  curve_color = mainwindow->maincurve->signal_color;

  filelist->setCurrentRow(mainwindow->files_open - 1);

  signallist->setFocus();

  SignalsDialog->exec();
}


void UI_Signalswindow::ColorButtonClicked(SpecialButton *)
{
  int color;

  UI_ColorMenuDialog colormenudialog(&color, mainwindow, 1);

  if(color < 0)  return;

  if(color == 127)
  {
    color_selected = 0;

    mainwindow->use_diverse_signal_colors = 1;

    ColorButton->setColor(127);

    return;
  }

  mainwindow->use_diverse_signal_colors = 0;

  ColorButton->setColor((Qt::GlobalColor)color);

  curve_color = color;

  color_selected = 1;
}


void UI_Signalswindow::DisplayCompButtonClicked()
{
  int i, j, n, edfsignal=0;

  char str[512]="",
       str2[128]="";

  struct signalcompblock *newsignalcomp;


  n = compositionlist->rowCount();

  if(!n)
  {
    return;
  }

  for(i=0; i<n; i++)
  {
    if((((QDoubleSpinBox *)(compositionlist->cellWidget(i, 2)))->value() < 0.001) &&
       (((QDoubleSpinBox *)(compositionlist->cellWidget(i, 2)))->value() > -0.001))
    {
      strlcpy(str2, ((QLabel *)(compositionlist->cellWidget(i, 0)))->text().toLatin1().data(), 128);

      snprintf(str, 512, "Signal %i in the Composition list has a Factor too close to zero.", atoi(str2));
      QMessageBox messagewindow(QMessageBox::Critical, "Error", str);
      messagewindow.exec();
      return;
    }
  }

  newsignalcomp = (struct signalcompblock *)calloc(1, sizeof(struct signalcompblock));
  if(newsignalcomp==NULL)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Internal error: Memory allocation error:\n\"new signal composition\"");
    messagewindow.exec();
    SignalsDialog->close();
    return;
  }

  newsignalcomp->uid = mainwindow->uid_seq++;
  newsignalcomp->num_of_signals = n;
  newsignalcomp->edfhdr = mainwindow->edfheaderlist[filelist->currentRow()];
  newsignalcomp->file_duration = newsignalcomp->edfhdr->long_data_record_duration * newsignalcomp->edfhdr->datarecords;
  newsignalcomp->voltpercm = mainwindow->default_amplitude;
  if(mainwindow->use_diverse_signal_colors && (!color_selected))
  {
    newsignalcomp->color = default_color_list[default_color_idx++];
    default_color_idx %= DEFAULT_COLOR_LIST_SZ;
  }
  else
  {
    newsignalcomp->color = curve_color;
  }
  newsignalcomp->hasruler = 0;
  newsignalcomp->polarity = 1;

  for(i=0; i<n; i++)
  {
    strlcpy(str, ((QLabel *)(compositionlist->cellWidget(i, 0)))->text().toLatin1().data(), 512);

    edfsignal = atoi(str) - 1;

    strlcpy(str, ((QLabel *)(compositionlist->cellWidget(i, 1)))->text().toLatin1().data(), 512);

    for(j=0; j<newsignalcomp->edfhdr->edfsignals; j++)
    {
      if(!strcmp(newsignalcomp->edfhdr->edfparam[j].label, str))
      {
        if(j != edfsignal)  continue;

        newsignalcomp->edfsignal[i] = j;
        newsignalcomp->factor[i] = ((QDoubleSpinBox *)(compositionlist->cellWidget(i, 2)))->value();
        if(newsignalcomp->factor[i] < 0.0001)
        {
          strlcat(newsignalcomp->signallabel, "- ", 512);
        }
        else
        {
          if(i)
          {
            strlcat(newsignalcomp->signallabel, "+ ", 512);
          }
        }
        strlcpy(str2, newsignalcomp->edfhdr->edfparam[j].label, 128);
        strip_types_from_label(str2);
        strlcat(newsignalcomp->signallabel, str2, 512);
        remove_trailing_spaces(newsignalcomp->signallabel);
        strlcat(newsignalcomp->signallabel, " ", 512);

        if(newsignalcomp->edfhdr->edfparam[j].bitvalue < 0.0)
        {
          newsignalcomp->voltpercm = mainwindow->default_amplitude * -1;
        }
        newsignalcomp->sensitivity[i] = newsignalcomp->edfhdr->edfparam[j].bitvalue / ((double)newsignalcomp->voltpercm * mainwindow->y_pixelsizefactor);
      }
    }
  }

  remove_trailing_spaces(newsignalcomp->signallabel);
  newsignalcomp->signallabellen = strlen(newsignalcomp->signallabel);

  strlcpy(newsignalcomp->physdimension, newsignalcomp->edfhdr->edfparam[newsignalcomp->edfsignal[0]].physdimension, 9);
  remove_trailing_spaces(newsignalcomp->physdimension);

  mainwindow->signalcomp[mainwindow->signalcomps] = newsignalcomp;
  mainwindow->signalcomps++;

  while(compositionlist->rowCount())
  {
    compositionlist->removeRow(0);
  }

  mainwindow->setup_viewbuf();

  smp_per_record = 0;
}


void UI_Signalswindow::DisplayButtonClicked()
{
  int i, n, s, old_scomps;

  struct signalcompblock *newsignalcomp;

  QListWidgetItem *item;

  QList<QListWidgetItem *> selectedlist;

  selectedlist = signallist->selectedItems();

  n = selectedlist.size();

  if(!n)
  {
    SignalsDialog->close();
    return;
  }

  old_scomps = mainwindow->signalcomps;

  for(i=0; i<n; i++)
  {
    newsignalcomp = (struct signalcompblock *)calloc(1, sizeof(struct signalcompblock));
    if(newsignalcomp==NULL)
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "Internal error: Memory allocation error:\n\"new signal composition\"");
      messagewindow.exec();
      SignalsDialog->close();
      return;
    }

    newsignalcomp->uid = mainwindow->uid_seq++;
    newsignalcomp->num_of_signals = 1;
    newsignalcomp->edfhdr = mainwindow->edfheaderlist[filelist->currentRow()];
    newsignalcomp->file_duration = newsignalcomp->edfhdr->long_data_record_duration * newsignalcomp->edfhdr->datarecords;
    newsignalcomp->voltpercm = mainwindow->default_amplitude;
    if(mainwindow->use_diverse_signal_colors && (!color_selected))
    {
      newsignalcomp->color = default_color_list[default_color_idx++];
      default_color_idx %= DEFAULT_COLOR_LIST_SZ;
    }
    else
    {
      newsignalcomp->color = curve_color;
    }
    newsignalcomp->hasruler = 0;
    newsignalcomp->polarity = 1;

    item = selectedlist.at(i);
    s = item->data(Qt::UserRole).toInt();
    newsignalcomp->edfsignal[0] = s;
    newsignalcomp->factor[0] = 1;
    if(newsignalcomp->edfhdr->edfparam[s].bitvalue < 0.0)
    {
      newsignalcomp->voltpercm = mainwindow->default_amplitude * -1;
    }
    newsignalcomp->sensitivity[0] = newsignalcomp->edfhdr->edfparam[s].bitvalue / ((double)newsignalcomp->voltpercm * mainwindow->y_pixelsizefactor);

    strlcpy(newsignalcomp->signallabel, newsignalcomp->edfhdr->edfparam[s].label, 256);
    strip_types_from_label(newsignalcomp->signallabel);
    remove_trailing_spaces(newsignalcomp->signallabel);

    newsignalcomp->file_duration = newsignalcomp->edfhdr->long_data_record_duration * newsignalcomp->edfhdr->datarecords;

    newsignalcomp->signallabellen = strlen(newsignalcomp->signallabel);

    strlcpy(newsignalcomp->physdimension, newsignalcomp->edfhdr->edfparam[s].physdimension, 9);
    remove_trailing_spaces(newsignalcomp->physdimension);

    mainwindow->signalcomp[mainwindow->signalcomps] = newsignalcomp;
    mainwindow->signalcomps++;
  }

  if((i) && (mainwindow->files_open == 1) && (old_scomps == 0))
  {
    if((mainwindow->signalcomp[0]->file_duration / TIME_DIMENSION) < 5)
    {
      mainwindow->pagetime = mainwindow->signalcomp[0]->file_duration;
    }
  }

  SignalsDialog->close();

  mainwindow->setup_viewbuf();
}


void UI_Signalswindow::RemoveButtonClicked()
{
  compositionlist->removeRow(compositionlist->currentRow());
}


void UI_Signalswindow::AddButtonClicked()
{
  AddSubtractButtonsClicked(0);
}


void UI_Signalswindow::SubtractButtonClicked()
{
  AddSubtractButtonsClicked(1);
}


void UI_Signalswindow::AddSubtractButtonsClicked(int subtract)
{
  int i, j, k, n, s, row, duplicate;

  char str[256];

  QListWidgetItem *item;

  QList<QListWidgetItem *> selectedlist;

  selectedlist = signallist->selectedItems();

  n = selectedlist.size();

  if(!n)  return;

  if(!compositionlist->rowCount())
  {
    smp_per_record = 0;
    physdimension[0] = 0;
    bitvalue = 0.0;
  }

  for(i=0; i<n; i++)
  {
    item = selectedlist.at(i);
    s = item->data(Qt::UserRole).toInt();
    row = filelist->currentRow();

    if(smp_per_record)
    {
      if(smp_per_record!=mainwindow->edfheaderlist[row]->edfparam[s].smp_per_record)
      {
        QMessageBox messagewindow(QMessageBox::Warning, "Warning", derivation_err_str);
        messagewindow.exec();
        continue;
      }
    }
    else
    {
      smp_per_record = mainwindow->edfheaderlist[row]->edfparam[s].smp_per_record;
    }

    if(physdimension[0])
    {
      if(strcmp(physdimension, mainwindow->edfheaderlist[row]->edfparam[s].physdimension))
      {
        QMessageBox messagewindow(QMessageBox::Warning, "Warning", derivation_err_str);
        messagewindow.exec();
        continue;
      }
    }
    else
    {
      strlcpy(physdimension, mainwindow->edfheaderlist[row]->edfparam[s].physdimension, 64);
    }

    if(bitvalue!=0.0)
    {
      if(dblcmp(bitvalue, mainwindow->edfheaderlist[row]->edfparam[s].bitvalue))
      {
        QMessageBox messagewindow(QMessageBox::Warning, "Warning", derivation_err_str);
        messagewindow.exec();
        messagewindow.setIcon(QMessageBox::Information);
        messagewindow.setWindowTitle("Help");
        messagewindow.setText("In this case the problem is that the resolution of the signals selected for the derivation,"
                              "have different values. You can correct this with the tool \"Unify resolution\" in the tools menu."
                              "Have a look at the manual for the details.");
        messagewindow.exec();
        continue;
      }
    }
    else
    {
      bitvalue = mainwindow->edfheaderlist[row]->edfparam[s].bitvalue;
    }

    duplicate = 0;

    k = compositionlist->rowCount();
    for(j=0; j<k; j++)
    {
      strlcpy(str, ((QLabel *)(compositionlist->cellWidget(j, 0)))->text().toLatin1().data(), 256);

      if((s + 1) == atoi(str))
      {
        duplicate = 1;
        break;
      }
    }
    if(duplicate)
    {
      if(subtract)
      {
        ((QDoubleSpinBox *)(compositionlist->cellWidget(j, 2)))->setValue(((QDoubleSpinBox *)(compositionlist->cellWidget(j, 2)))->value() - 1);
      }
      else
      {
        ((QDoubleSpinBox *)(compositionlist->cellWidget(j, 2)))->setValue(((QDoubleSpinBox *)(compositionlist->cellWidget(j, 2)))->value() + 1);
      }
    }
    else
    {
      compositionlist->insertRow(k);
      snprintf(str, 200, "%i", s + 1);
      compositionlist->setCellWidget(k, 0, new QLabel(str));
      ((QLabel *)(compositionlist->cellWidget(k, 0)))->setAlignment(Qt::AlignCenter);
      compositionlist->setCellWidget(k, 1, new QLabel(mainwindow->edfheaderlist[row]->edfparam[s].label));
      ((QLabel *)(compositionlist->cellWidget(k, 1)))->setAlignment(Qt::AlignCenter);

      compositionlist->setCellWidget(k, 2, new QDoubleSpinBox);
      ((QDoubleSpinBox *)(compositionlist->cellWidget(k, 2)))->setAlignment(Qt::AlignCenter);
      ((QDoubleSpinBox *)(compositionlist->cellWidget(k, 2)))->setPrefix("x");
      ((QDoubleSpinBox *)(compositionlist->cellWidget(k, 2)))->setDecimals(3);
      ((QDoubleSpinBox *)(compositionlist->cellWidget(k, 2)))->setRange(-128.0, 128.0);
      if(subtract)
      {
        ((QDoubleSpinBox *)(compositionlist->cellWidget(k, 2)))->setValue(-1);
      }
      else
      {
        ((QDoubleSpinBox *)(compositionlist->cellWidget(k, 2)))->setValue(1);
      }

      convert_to_metric_suffix(str, mainwindow->edfheaderlist[row]->edfparam[s].sf_f, 3, 256);
      remove_trailing_zeros(str);
      strlcat(str, "Hz", 256);
      compositionlist->setCellWidget(k, 3, new QLabel(str));
      ((QLabel *)(compositionlist->cellWidget(k, 3)))->setAlignment(Qt::AlignCenter);
    }
  }

  compositionlist->resizeColumnsToContents();
}


void UI_Signalswindow::show_signals(int row)
{
  int i, j, len, skip, signal_cnt;

  char str[256];

  long long file_duration;

  QListWidgetItem *item;

  struct date_time_struct date_time;


  if(row<0)  return;

  while(signallist->count())
  {
    delete signallist->item(0);
  }

  while(compositionlist->rowCount())
  {
    compositionlist->removeRow(0);
  }

  if((mainwindow->edfheaderlist[row]->edfplus)||(mainwindow->edfheaderlist[row]->bdfplus))
  {
    strlcpy(str, "Subject    ", 256);
    strlcat(str, mainwindow->edfheaderlist[row]->plus_patient_name, 256);
    strlcat(str, "  ", 256);
    strlcat(str, mainwindow->edfheaderlist[row]->plus_birthdate, 256);
    strlcat(str, "  ", 256);
    strlcat(str, mainwindow->edfheaderlist[row]->plus_patientcode, 256);
    label1->setText(str);
    strlcpy(str, "Recording  ", 256);
    strlcat(str, mainwindow->edfheaderlist[row]->plus_admincode, 256);
    label2->setText(str);
  }
  else
  {
    strlcpy(str, "Subject    ", 256);
    strlcat(str, mainwindow->edfheaderlist[row]->patient, 256);
    len = strlen(str);
    for(j=0; j<len; j++)
    {
      if(str[j]=='_')
      {
        str[j] = ' ';
      }
    }
    label1->setText(str);
    strlcpy(str, "Recording  ", 256);
    strlcat(str, mainwindow->edfheaderlist[row]->recording, 256);
    len = strlen(str);
    for(j=0; j<len; j++)
    {
      if(str[j]=='_')
      {
        str[j] = ' ';
      }
    }
    label2->setText(str);
  }

  utc_to_date_time(mainwindow->edfheaderlist[row]->utc_starttime, &date_time);

  date_time.month_str[0] += 32;
  date_time.month_str[1] += 32;
  date_time.month_str[2] += 32;

  snprintf(str, 256, "Start      %i %s %i   %2i:%02i:%02i",
          date_time.day,
          date_time.month_str,
          date_time.year,
          date_time.hour,
          date_time.minute,
          date_time.second);

  label3->setText(str);

  strlcpy(str, "Duration   ", 256);
  file_duration = mainwindow->edfheaderlist[row]->long_data_record_duration * mainwindow->edfheaderlist[row]->datarecords;
  if((file_duration / TIME_DIMENSION) / 10)
  {
    snprintf(str + 11, 240,
            "%2i:%02i:%02i",
            (int)((file_duration / TIME_DIMENSION)/ 3600LL),
            (int)(((file_duration / TIME_DIMENSION) % 3600LL) / 60LL),
            (int)((file_duration / TIME_DIMENSION) % 60LL));
  }
  else
  {
    snprintf(str + 11, 240,
            "%2i:%02i:%02i.%06i",
            (int)((file_duration / TIME_DIMENSION)/ 3600LL),
            (int)(((file_duration / TIME_DIMENSION) % 3600LL) / 60LL),
            (int)((file_duration / TIME_DIMENSION) % 60LL),
            (int)((file_duration % TIME_DIMENSION) / 10LL));
  }
  label4->setText(str);

  skip = 0;

  signal_cnt = mainwindow->edfheaderlist[row]->edfsignals;

  for(i=0; i<signal_cnt; i++)
  {
    if(mainwindow->edfheaderlist[row]->edfplus || mainwindow->edfheaderlist[row]->bdfplus)
    {
      for(j=0; j<mainwindow->edfheaderlist[row]->nr_annot_chns; j++)
      {
        if(i==mainwindow->edfheaderlist[row]->annot_ch[j])
        {
          skip = 1;
          break;
        }
      }
      if(skip)
      {
        skip = 0;
        continue;
      }
    }

    snprintf(str, 256, "%-3i ", i + 1);
    strlcat(str, mainwindow->edfheaderlist[row]->edfparam[i].label, 256);
    strlcat(str, "   ", 256);
    convert_to_metric_suffix(str + strlen(str), mainwindow->edfheaderlist[row]->edfparam[i].sf_f, 6, 256 - strlen(str));
    remove_trailing_zeros(str);
    strlcat(str, "Hz", 256);
    item = new QListWidgetItem;
    item->setText(str);
    item->setData(Qt::UserRole, QVariant(i));
    signallist->addItem(item);
  }

  SelectAllButtonClicked();
}


void UI_Signalswindow::SelectAllButtonClicked()
{
  int row, i;

  QListWidgetItem *item;

  row = signallist->count();

  for(i=0; i<row; i++)
  {
    item = signallist->item(i);
    item->setSelected(true);
  }
}


void UI_Signalswindow::HelpButtonClicked()
{
  QMessageBox messagewindow(QMessageBox::NoIcon, "Help",
  "On top you will see a list of opened files.\n"
  "Select (highlight) the file from which you want to add signals.\n"
  "At the left part of the dialog you see a list of all the signals which are in the selected file.\n"
  "Select one or more signals and click on the \"Add signals\" button.\n"
  "Now you will see the selected signals on the screen.\n"

  "\nWhen you want to make a combination (derivation) of two or more signals, do as follows.\n"
  "Select the file from which you want to add signals.\n"
  "For example, we choose an EEG file which contains the signals \"P3\" and \"C3\"\n"
  "and we want to subtract \"C3\" from \"P3\".\n"
  "- Select (highlight) the signal \"P3\".\n"
  "- Click on the \"Add\" button.\n"
  "- Select (highlight) the signal \"C3\".\n"
  "- Click on the \"Subtract\" button.\n"
  "- Click on the \"Make derivation\" button.\n"
  "- The result of \"P3\" minus \"C3\" will appear on the screen.");

  messagewindow.exec();
}


void UI_Signalswindow::strip_types_from_label(char *label)
{
  int i,
      len;


  len = strlen(label);
  if(len<16)
  {
    return;
  }

  if((!(strncmp(label, "EEG ", 4)))
   ||(!(strncmp(label, "ECG ", 4)))
   ||(!(strncmp(label, "EOG ", 4)))
   ||(!(strncmp(label, "ERG ", 4)))
   ||(!(strncmp(label, "EMG ", 4)))
   ||(!(strncmp(label, "MEG ", 4)))
   ||(!(strncmp(label, "MCG ", 4))))
  {
    if(label[4]!=' ')
    {
      for(i=0; i<(len-4); i++)
      {
        label[i] = label[i+4];
      }

      for(; i<len; i++)
      {
        label[i] = ' ';
      }
    }
  }
}












