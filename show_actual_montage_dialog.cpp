/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2007 - 2022 Teunis van Beelen
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



#include "show_actual_montage_dialog.h"




UI_ShowActualMontagewindow::UI_ShowActualMontagewindow(QWidget *w_parent)
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
                *firfilterItem,
                *math_item_before=NULL,
                *math_item_after=NULL;

  mainwindow = (UI_Mainwindow *)w_parent;

  ShowMontageDialog = new QDialog;
  ShowMontageDialog->setMinimumSize(500 * mainwindow->w_scaling, 300 * mainwindow->h_scaling);
  ShowMontageDialog->setWindowTitle("Show montage");
  ShowMontageDialog->setModal(true);
  ShowMontageDialog->setAttribute(Qt::WA_DeleteOnClose, true);
  ShowMontageDialog->setSizeGripEnabled(true);

  tree = new QTreeView;
  tree->setHeaderHidden(true);
  tree->setSelectionMode(QAbstractItemView::NoSelection);
  tree->setEditTriggers(QAbstractItemView::NoEditTriggers);
  tree->setSortingEnabled(false);
  tree->setDragDropMode(QAbstractItemView::NoDragDrop);
  tree->setAlternatingRowColors(true);

  CloseButton = new QPushButton;
  CloseButton->setText("Close");

  QHBoxLayout *hlayout1 = new QHBoxLayout;
  hlayout1->addStretch(1000);
  hlayout1->addWidget(CloseButton);

  QVBoxLayout *vlayout1 = new QVBoxLayout;
  vlayout1->addWidget(tree, 1000);
  vlayout1->addSpacing(20);
  vlayout1->addLayout(hlayout1);

  ShowMontageDialog->setLayout(vlayout1);

  QObject::connect(CloseButton,  SIGNAL(clicked()), ShowMontageDialog, SLOT(close()));

  t_model = new QStandardItemModel(this);

  parentItem = t_model->invisibleRootItem();

  for(i=0; i<mainwindow->signalcomps; i++)
  {
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

    if(mainwindow->signalcomp[i]->polarity == -1)
    {
      strlcat(txtbuf, "Inverted: yes", 2048);
    }

    signalItem = new QStandardItem(txtbuf);

    parentItem->appendRow(signalItem);

    snprintf(txtbuf, 2048, "amplitude: %f", mainwindow->signalcomp[i]->voltpercm);

    strlcat(txtbuf, mainwindow->signalcomp[i]->physdimension, 2048);

    remove_trailing_spaces(txtbuf);

    snprintf(txtbuf + strlen(txtbuf), 2048 - strlen(txtbuf), "/cm offset: %f%s",
            mainwindow->signalcomp[i]->screen_offset * mainwindow->y_pixelsizefactor * mainwindow->signalcomp[i]->voltpercm,
            mainwindow->signalcomp[i]->physdimension);

    remove_trailing_zeros(txtbuf);

    remove_trailing_spaces(txtbuf);

    strlcat(txtbuf, "  color: ", 2048);

    switch(mainwindow->signalcomp[i]->color)
    {
      case Qt::white       : strlcat(txtbuf, "white", 2048);
                             signalItem->setIcon(QIcon(":/images/white_icon_16x16.png"));
                             break;
      case Qt::black       : strlcat(txtbuf, "black", 2048);
                             signalItem->setIcon(QIcon(":/images/black_icon_16x16.png"));
                             break;
      case Qt::red         : strlcat(txtbuf, "red", 2048);
                             signalItem->setIcon(QIcon(":/images/red_icon_16x16.png"));
                             break;
      case Qt::darkRed     : strlcat(txtbuf, "dark red", 2048);
                             signalItem->setIcon(QIcon(":/images/darkred_icon_16x16.png"));
                             break;
      case Qt::green       : strlcat(txtbuf, "green", 2048);
                             signalItem->setIcon(QIcon(":/images/green_icon_16x16.png"));
                             break;
      case Qt::darkGreen   : strlcat(txtbuf, "dark green", 2048);
                             signalItem->setIcon(QIcon(":/images/darkgreen_icon_16x16.png"));
                             break;
      case Qt::blue        : strlcat(txtbuf, "blue", 2048);
                             signalItem->setIcon(QIcon(":/images/blue_icon_16x16.png"));
                             break;
      case Qt::darkBlue    : strlcat(txtbuf, "dark blue", 2048);
                             signalItem->setIcon(QIcon(":/images/darkblue_icon_16x16.png"));
                             break;
      case Qt::cyan        : strlcat(txtbuf, "cyan", 2048);
                             signalItem->setIcon(QIcon(":/images/cyan_icon_16x16.png"));
                             break;
      case Qt::darkCyan    : strlcat(txtbuf, "dark cyan", 2048);
                             signalItem->setIcon(QIcon(":/images/darkcyan_icon_16x16.png"));
                             break;
      case Qt::magenta     : strlcat(txtbuf, "magenta", 2048);
                             signalItem->setIcon(QIcon(":/images/magenta_icon_16x16.png"));
                             break;
      case Qt::darkMagenta : strlcat(txtbuf, "dark magenta", 2048);
                             signalItem->setIcon(QIcon(":/images/darkmagenta_icon_16x16.png"));
                             break;
      case Qt::yellow      : strlcat(txtbuf, "yellow", 2048);
                             signalItem->setIcon(QIcon(":/images/yellow_icon_16x16.png"));
                             break;
      case Qt::darkYellow  : strlcat(txtbuf, "dark yellow", 2048);
                             signalItem->setIcon(QIcon(":/images/darkyellow_icon_16x16.png"));
                             break;
      case Qt::gray        : strlcat(txtbuf, "gray", 2048);
                             signalItem->setIcon(QIcon(":/images/gray_icon_16x16.png"));
                             break;
      case Qt::darkGray    : strlcat(txtbuf, "dark gray", 2048);
                             signalItem->setIcon(QIcon(":/images/darkgray_icon_16x16.png"));
                             break;
      case Qt::lightGray   : strlcat(txtbuf, "light gray", 2048);
                             signalItem->setIcon(QIcon(":/images/lightgray_icon_16x16.png"));
                             break;
    }

    signalItem->appendRow(new QStandardItem(txtbuf));

    if(mainwindow->signalcomp[i]->math_func_cnt_before)
    {
      math_item_before = new QStandardItem("Math functions (before filtering)");

      signalItem->appendRow(math_item_before);

      for(j=0; j<mainwindow->signalcomp[i]->math_func_cnt_before; j++)
      {
        if(mainwindow->signalcomp[i]->math_func_before[j]->func == MATH_FUNC_SQUARE)
        {
          math_item_before->appendRow(new QStandardItem("Math function: Square"));
        }
        else if(mainwindow->signalcomp[i]->math_func_before[j]->func == MATH_FUNC_SQRT)
          {
            math_item_before->appendRow(new QStandardItem("Math function: Square Root"));
          }
          else if(mainwindow->signalcomp[i]->math_func_before[j]->func == MATH_FUNC_ABS)
            {
              math_item_before->appendRow(new QStandardItem("Math function: Absolute"));
            }
            else if(mainwindow->signalcomp[i]->math_func_before[j]->func == MATH_FUNC_NONE)
              {
                math_item_before->appendRow(new QStandardItem("Math function: None"));
              }
      }
    }

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

    if(mainwindow->signalcomp[i]->math_func_cnt_after)
    {
      math_item_after = new QStandardItem("Math functions (after filtering)");

      signalItem->appendRow(math_item_after);

      for(j=0; j<mainwindow->signalcomp[i]->math_func_cnt_after; j++)
      {
        if(mainwindow->signalcomp[i]->math_func_after[j]->func == MATH_FUNC_SQUARE)
        {
          math_item_after->appendRow(new QStandardItem("Math function: Square"));
        }
        else if(mainwindow->signalcomp[i]->math_func_after[j]->func == MATH_FUNC_SQRT)
          {
            math_item_after->appendRow(new QStandardItem("Math function: Square Root"));
          }
          else if(mainwindow->signalcomp[i]->math_func_after[j]->func == MATH_FUNC_ABS)
            {
              math_item_after->appendRow(new QStandardItem("Math function: Absolute"));
            }
            else if(mainwindow->signalcomp[i]->math_func_after[j]->func == MATH_FUNC_NONE)
              {
                math_item_after->appendRow(new QStandardItem("Math function: None"));
              }
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

  snprintf(txtbuf, 2048, "timescale: %f seconds", (double)mainwindow->pagetime / (double)TIME_DIMENSION);
  remove_trailing_zeros(txtbuf);
  parentItem->appendRow(new QStandardItem(txtbuf));

  tree->setModel(t_model);

//  tree->expandAll();

  ShowMontageDialog->exec();
}














