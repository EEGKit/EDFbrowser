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


#include "view_montage_dialog.h"



UI_ViewMontagewindow::UI_ViewMontagewindow(QWidget *w_parent)
{
  mainwindow = (UI_Mainwindow *)w_parent;

  mtg_path[0] = 0;
  mtg_dir[0] = 0;

  if(mainwindow->recent_montagedir[0]!=0)
  {
    strlcpy(mtg_dir, mainwindow->recent_montagedir, MAX_PATH_LENGTH);
  }

  ViewMontageDialog = new QDialog;
  ViewMontageDialog->setMinimumSize(500 * mainwindow->w_scaling, 300 * mainwindow->h_scaling);
  ViewMontageDialog->setWindowTitle("View montage");
  ViewMontageDialog->setModal(true);
  ViewMontageDialog->setAttribute(Qt::WA_DeleteOnClose, true);
  ViewMontageDialog->setSizeGripEnabled(true);

  t_model = new QStandardItemModel(this);

  tree = new QTreeView;
  tree->setHeaderHidden(true);
  tree->setSelectionMode(QAbstractItemView::NoSelection);
  tree->setEditTriggers(QAbstractItemView::NoEditTriggers);
  tree->setSortingEnabled(false);
  tree->setDragDropMode(QAbstractItemView::NoDragDrop);
  tree->setModel(t_model);
  tree->setAlternatingRowColors(true);

  SelectButton = new QPushButton;
  SelectButton->setText("Select montage");

  CloseButton = new QPushButton;
  CloseButton->setText("Close");

  QHBoxLayout *hlayout1 = new QHBoxLayout;
  hlayout1->addWidget(SelectButton);
  hlayout1->addStretch(1000);
  hlayout1->addWidget(CloseButton);

  QVBoxLayout *vlayout1 = new QVBoxLayout;
  vlayout1->addWidget(tree, 1000);
  vlayout1->addSpacing(20);
  vlayout1->addLayout(hlayout1);

  ViewMontageDialog->setLayout(vlayout1);

  QObject::connect(CloseButton,  SIGNAL(clicked()), ViewMontageDialog, SLOT(close()));
  QObject::connect(SelectButton, SIGNAL(clicked()), this,              SLOT(SelectButtonClicked()));

  ViewMontageDialog->exec();
}




void UI_ViewMontagewindow::SelectButtonClicked()
{
  int r,
      signalcomps_read=0,
      signals_read,
      signal_cnt,
      filters_read,
      color,
      filter_cnt=0,
      spike_filter_cnt=0,
      ravg_filter_cnt=0,
      fidfilter_cnt=0,
      islpf,
      signalcomps,
      screen_offset,
      order,
      model,
      type,
      size,
      polarity=1,
      holdoff=100,
      plif_powerlinefrequency,
      plif_linear_threshold,
      n_taps,
      idx_used=0;

  char result[XML_STRBUFLEN]="",
       composition_txt[2048]="",
       label[256]="",
       str2[2048]="",
       str3[64]="";

  double frequency,
         frequency2,
         voltpercm,
         ripple,
         timescale,
         d_tmp,
         velocity,
         factor[MAXSIGNALS],
         fir_vars[1000];

  QStandardItem *parentItem,
                *signalItem,
                *filterItem,
                *firfilterItem;

  struct xml_handle *xml_hdl;


  strlcpy(mtg_path, QFileDialog::getOpenFileName(0, "Choose a montage", QString::fromLocal8Bit(mtg_dir), "Montage files (*.mtg *.MTG)").toLocal8Bit().data(), MAX_PATH_LENGTH);

  if(!strcmp(mtg_path, ""))
  {
    return;
  }

  get_directory_from_path(mtg_dir, mtg_path, MAX_PATH_LENGTH);

  xml_hdl = xml_get_handle(mtg_path);
  if(xml_hdl==NULL)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Can not open file for reading.");
    messagewindow.exec();
    return;
  }

  if(strcmp(xml_hdl->elementname[xml_hdl->level], PROGRAM_NAME "_montage"))
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file.");
    messagewindow.exec();
    xml_close(xml_hdl);
    return;
  }

  ViewMontageDialog->setWindowTitle(mtg_path);

  t_model->clear();

  parentItem = t_model->invisibleRootItem();

  for(signalcomps=0; ; signalcomps++)
  {
    xml_goto_root(xml_hdl);

    signals_read = 0;

    filter_cnt = 0;
    ravg_filter_cnt = 0;
    fidfilter_cnt = 0;

    if(xml_goto_nth_element_inside(xml_hdl, "signalcomposition", signalcomps_read))
    {
      break;
    }

    if(xml_goto_nth_element_inside(xml_hdl, "num_of_signals", 0))
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file.");
      messagewindow.exec();
      xml_close(xml_hdl);
      return;
    }
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file.");
      messagewindow.exec();
      xml_close(xml_hdl);
      return;
    }
    signal_cnt = atoi(result);
    if((signal_cnt<1)||(signal_cnt>256))
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file.");
      messagewindow.exec();
      xml_close(xml_hdl);
      return;
    }

    xml_go_up(xml_hdl);
    if(xml_goto_nth_element_inside(xml_hdl, "voltpercm", 0))
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file.");
      messagewindow.exec();
      xml_close(xml_hdl);
      return;
    }
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file.");
      messagewindow.exec();
      xml_close(xml_hdl);
      return;
    }
    voltpercm = atof(result);

    xml_go_up(xml_hdl);
    if(xml_goto_nth_element_inside(xml_hdl, "screen_offset", 0))
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file.");
      messagewindow.exec();
      xml_close(xml_hdl);
      return;
    }
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file.");
      messagewindow.exec();
      xml_close(xml_hdl);
      return;
    }
    screen_offset = atoi(result);
    xml_go_up(xml_hdl);

    if(!(xml_goto_nth_element_inside(xml_hdl, "polarity", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file.");
        messagewindow.exec();
        xml_close(xml_hdl);
        return;
      }
      polarity = atoi(result);
      if(polarity != -1)
      {
        polarity = 1;
      }
      xml_go_up(xml_hdl);
    }

    if(xml_goto_nth_element_inside(xml_hdl, "color", 0))
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file.");
      messagewindow.exec();
      xml_close(xml_hdl);
      return;
    }
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file.");
      messagewindow.exec();
      xml_close(xml_hdl);
      return;
    }
    color = atoi(result);
    xml_go_up(xml_hdl);

    if(!(xml_goto_nth_element_inside(xml_hdl, "spike_filter_cnt", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file.");
        messagewindow.exec();
        xml_close(xml_hdl);
        return;
      }
      spike_filter_cnt = atoi(result);
      xml_go_up(xml_hdl);
    }

    if(!(xml_goto_nth_element_inside(xml_hdl, "filter_cnt", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file.");
        messagewindow.exec();
        xml_close(xml_hdl);
        return;
      }
      filter_cnt = atoi(result);
      xml_go_up(xml_hdl);
    }

    if(!(xml_goto_nth_element_inside(xml_hdl, "ravg_filter_cnt", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file.");
        messagewindow.exec();
        xml_close(xml_hdl);
        return;
      }
      ravg_filter_cnt = atoi(result);
      xml_go_up(xml_hdl);
    }

    if(!(xml_goto_nth_element_inside(xml_hdl, "fidfilter_cnt", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file.");
        messagewindow.exec();
        xml_close(xml_hdl);
        return;
      }
      fidfilter_cnt = atoi(result);
      xml_go_up(xml_hdl);
    }

    composition_txt[0] = 0;

    if(!(xml_goto_nth_element_inside(xml_hdl, "alias", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file.");
        messagewindow.exec();
        xml_close(xml_hdl);
        return;
      }
      if(result[0] != 0)
      {
        strlcpy(composition_txt, "alias: ", 2048);
        strlcat(composition_txt, result, 2048);
        strlcat(composition_txt, "   ", 2048);
      }
      xml_go_up(xml_hdl);
    }

    for(signals_read=0; signals_read<signal_cnt; signals_read++)
    {
      idx_used = 0;

      if(xml_goto_nth_element_inside(xml_hdl, "signal", signals_read))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file.");
        messagewindow.exec();
        xml_close(xml_hdl);
        return;
      }

      if(xml_goto_nth_element_inside(xml_hdl, "factor", 0))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file.");
        messagewindow.exec();
        xml_close(xml_hdl);
        return;
      }
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file.");
        messagewindow.exec();
        xml_close(xml_hdl);
        return;
      }
      factor[signals_read] = atof(result);

      xml_go_up(xml_hdl);
      if(xml_goto_nth_element_inside(xml_hdl, "label", 0))
      {
        if(xml_goto_nth_element_inside(xml_hdl, "edfindex", 0))
        {
          QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file.");
          messagewindow.exec();
          xml_close(xml_hdl);
          return;
        }

        idx_used = 1;
      }
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file.");
        messagewindow.exec();
        xml_close(xml_hdl);
        return;
      }
      if(idx_used)
      {
        strlcpy(label, "index: ", 256);
      }
      else
      {
        strlcpy(label, "label: ", 256);
      }
      strlcat(label, result, 256);

      snprintf(str3, 64, "%+f", factor[signals_read]);

      remove_trailing_zeros(str3);

      snprintf(composition_txt + strlen(composition_txt), 2048 - strlen(composition_txt), "%sx %s", str3, label);

      remove_trailing_spaces(composition_txt);

      strlcat(composition_txt, "   ", 2048);

      xml_go_up(xml_hdl);
      xml_go_up(xml_hdl);
    }

    if(polarity == -1)
    {
      strlcat(composition_txt, "inverted: yes", 2048);
    }

    signalItem = new QStandardItem(composition_txt);

    parentItem->appendRow(signalItem);

    snprintf(composition_txt, 2048, "amplitude: %f", voltpercm);

    remove_trailing_zeros(composition_txt);

    snprintf(composition_txt + strlen(composition_txt), 2048 - strlen(composition_txt), "/cm  offset: %f", (double)screen_offset * mainwindow->y_pixelsizefactor * voltpercm);

    remove_trailing_zeros(composition_txt);

    strlcat(composition_txt, "  color: ", 2048);

    switch(color)
    {
      case Qt::white       : strlcat(composition_txt, "white", 2048);
                             signalItem->setIcon(QIcon(":/images/white_icon_16x16.png"));
                             break;
      case Qt::black       : strlcat(composition_txt, "black", 2048);
                             signalItem->setIcon(QIcon(":/images/black_icon_16x16.png"));
                             break;
      case Qt::red         : strlcat(composition_txt, "red", 2048);
                             signalItem->setIcon(QIcon(":/images/red_icon_16x16.png"));
                             break;
      case Qt::darkRed     : strlcat(composition_txt, "dark red", 2048);
                             signalItem->setIcon(QIcon(":/images/darkred_icon_16x16.png"));
                             break;
      case Qt::green       : strlcat(composition_txt, "green", 2048);
                             signalItem->setIcon(QIcon(":/images/green_icon_16x16.png"));
                             break;
      case Qt::darkGreen   : strlcat(composition_txt, "dark green", 2048);
                             signalItem->setIcon(QIcon(":/images/darkgreen_icon_16x16.png"));
                             break;
      case Qt::blue        : strlcat(composition_txt, "blue", 2048);
                             signalItem->setIcon(QIcon(":/images/blue_icon_16x16.png"));
                             break;
      case Qt::darkBlue    : strlcat(composition_txt, "dark blue", 2048);
                             signalItem->setIcon(QIcon(":/images/darkblue_icon_16x16.png"));
                             break;
      case Qt::cyan        : strlcat(composition_txt, "cyan", 2048);
                             signalItem->setIcon(QIcon(":/images/cyan_icon_16x16.png"));
                             break;
      case Qt::darkCyan    : strlcat(composition_txt, "dark cyan", 2048);
                             signalItem->setIcon(QIcon(":/images/darkcyan_icon_16x16.png"));
                             break;
      case Qt::magenta     : strlcat(composition_txt, "magenta", 2048);
                             signalItem->setIcon(QIcon(":/images/magenta_icon_16x16.png"));
                             break;
      case Qt::darkMagenta : strlcat(composition_txt, "dark magenta", 2048);
                             signalItem->setIcon(QIcon(":/images/darkmagenta_icon_16x16.png"));
                             break;
      case Qt::yellow      : strlcat(composition_txt, "yellow", 2048);
                             signalItem->setIcon(QIcon(":/images/yellow_icon_16x16.png"));
                             break;
      case Qt::darkYellow  : strlcat(composition_txt, "dark yellow", 2048);
                             signalItem->setIcon(QIcon(":/images/darkyellow_icon_16x16.png"));
                             break;
      case Qt::gray        : strlcat(composition_txt, "gray", 2048);
                             signalItem->setIcon(QIcon(":/images/gray_icon_16x16.png"));
                             break;
      case Qt::darkGray    : strlcat(composition_txt, "dark gray", 2048);
                             signalItem->setIcon(QIcon(":/images/darkgray_icon_16x16.png"));
                             break;
      case Qt::lightGray   : strlcat(composition_txt, "light gray", 2048);
                             signalItem->setIcon(QIcon(":/images/lightgray_icon_16x16.png"));
                             break;
    }

    signalItem->appendRow(new QStandardItem(composition_txt));

    filterItem = new QStandardItem("Filters");

    filterItem->setIcon(QIcon(":/images/filter_lowpass_small.png"));

    signalItem->appendRow(filterItem);

    for(filters_read=0; filters_read<spike_filter_cnt; filters_read++)
    {
      if(xml_goto_nth_element_inside(xml_hdl, "spike_filter", filters_read))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file. (filter)");
        messagewindow.exec();
        xml_close(xml_hdl);
        return;
      }

      if(xml_goto_nth_element_inside(xml_hdl, "velocity", 0))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file. (velocity)");
        messagewindow.exec();
        xml_close(xml_hdl);
        return;
      }
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file.");
        messagewindow.exec();
        xml_close(xml_hdl);
        return;
      }
      velocity = atof(result);
      if(velocity < 0.0001)  velocity = 0.0001;
      if(velocity > 10E9)  velocity = 10E9;

      xml_go_up(xml_hdl);
      if(xml_goto_nth_element_inside(xml_hdl, "holdoff", 0))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file. (holdoff)");
        messagewindow.exec();
        xml_close(xml_hdl);
        return;
      }
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file.");
        messagewindow.exec();
        xml_close(xml_hdl);
        return;
      }
      holdoff = atoi(result);
      if(holdoff < 10)  holdoff = 10;
      if(holdoff > 1000)  holdoff = 1000;

      snprintf(composition_txt, 2048, "Spike: Velocity: %.8f", velocity);

      remove_trailing_zeros(composition_txt);

      snprintf(composition_txt + strlen(composition_txt), 2048 - strlen(composition_txt), "  Hold-off: %i milli-Sec.", holdoff);

      filterItem->appendRow(new QStandardItem(composition_txt));

      xml_go_up(xml_hdl);
      xml_go_up(xml_hdl);
    }

    for(filters_read=0; filters_read<filter_cnt; filters_read++)
    {
      if(xml_goto_nth_element_inside(xml_hdl, "filter", filters_read))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file. (filter)");
        messagewindow.exec();
        xml_close(xml_hdl);
        return;
      }

      if(xml_goto_nth_element_inside(xml_hdl, "LPF", 0))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file. (LPF)");
        messagewindow.exec();
        xml_close(xml_hdl);
        return;
      }
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file.");
        messagewindow.exec();
        xml_close(xml_hdl);
        return;
      }
      islpf = atoi(result);

      xml_go_up(xml_hdl);
      if(xml_goto_nth_element_inside(xml_hdl, "frequency", 0))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file. (frequency)");
        messagewindow.exec();
        xml_close(xml_hdl);
        return;
      }
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file.");
        messagewindow.exec();
        xml_close(xml_hdl);
        return;
      }
      frequency = atof(result);

      if(islpf)
      {
        snprintf(composition_txt, 2048, "LPF: %fHz", frequency);
      }
      else
      {
        snprintf(composition_txt, 2048, "HPF: %fHz", frequency);
      }

      remove_trailing_zeros(composition_txt);

      filterItem->appendRow(new QStandardItem(composition_txt));

      xml_go_up(xml_hdl);
      xml_go_up(xml_hdl);
    }

    for(filters_read=0; filters_read<ravg_filter_cnt; filters_read++)
    {
      if(xml_goto_nth_element_inside(xml_hdl, "ravg_filter", filters_read))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file. (filter)");
        messagewindow.exec();
        xml_close(xml_hdl);
        return;
      }

      if(xml_goto_nth_element_inside(xml_hdl, "type", 0))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file. (type)");
        messagewindow.exec();
        xml_close(xml_hdl);
        return;
      }
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file.");
        messagewindow.exec();
        xml_close(xml_hdl);
        return;
      }
      type = atoi(result);

      xml_go_up(xml_hdl);
      if(xml_goto_nth_element_inside(xml_hdl, "size", 0))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file. (size)");
        messagewindow.exec();
        xml_close(xml_hdl);
        return;
      }
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file.");
        messagewindow.exec();
        xml_close(xml_hdl);
        return;
      }
      size = atoi(result);

      if(type == 0)
      {
        snprintf(composition_txt, 2048, "highpass moving average %ismpls", size);
      }

      if(type == 1)
      {
        snprintf(composition_txt, 2048, "lowpass moving average %ismpls", size);
      }

      filterItem->appendRow(new QStandardItem(composition_txt));

      xml_go_up(xml_hdl);
      xml_go_up(xml_hdl);
    }

    for(filters_read=0; filters_read<fidfilter_cnt; filters_read++)
    {
      if(xml_goto_nth_element_inside(xml_hdl, "fidfilter", filters_read))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file. (fidfilter)");
        messagewindow.exec();
        xml_close(xml_hdl);
        return;
      }

      if(xml_goto_nth_element_inside(xml_hdl, "type", 0))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file. (type)");
        messagewindow.exec();
        xml_close(xml_hdl);
        return;
      }
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file.");
        messagewindow.exec();
        xml_close(xml_hdl);
        return;
      }
      type = atoi(result);

      xml_go_up(xml_hdl);
      if(xml_goto_nth_element_inside(xml_hdl, "frequency", 0))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file. (frequency)");
        messagewindow.exec();
        xml_close(xml_hdl);
        return;
      }
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file.");
        messagewindow.exec();
        xml_close(xml_hdl);
        return;
      }
      frequency = atof(result);

      xml_go_up(xml_hdl);
      if(xml_goto_nth_element_inside(xml_hdl, "frequency2", 0))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file. (frequency2)");
        messagewindow.exec();
        xml_close(xml_hdl);
        return;
      }
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file.");
        messagewindow.exec();
        xml_close(xml_hdl);
        return;
      }
      frequency2 = atof(result);

      xml_go_up(xml_hdl);
      if(xml_goto_nth_element_inside(xml_hdl, "ripple", 0))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file. (ripple)");
        messagewindow.exec();
        xml_close(xml_hdl);
        return;
      }
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file.");
        messagewindow.exec();
        xml_close(xml_hdl);
        return;
      }
      ripple = atof(result);

      xml_go_up(xml_hdl);
      if(xml_goto_nth_element_inside(xml_hdl, "order", 0))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file. (order)");
        messagewindow.exec();
        xml_close(xml_hdl);
        return;
      }
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file.");
        messagewindow.exec();
        xml_close(xml_hdl);
        return;
      }
      order = atoi(result);

      xml_go_up(xml_hdl);
      if(xml_goto_nth_element_inside(xml_hdl, "model", 0))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file. (model)");
        messagewindow.exec();
        xml_close(xml_hdl);
        return;
      }
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file.");
        messagewindow.exec();
        xml_close(xml_hdl);
        return;
      }
      model = atoi(result);

      if(type == 0)
      {
        if(model == 0)
        {
          snprintf(composition_txt, 2048, "highpass Butterworth %fHz %ith order  ", frequency, order);
        }

        if(model == 1)
        {
          snprintf(composition_txt, 2048, "highpass Chebyshev %fHz %ith order %fdB ripple  ", frequency, order, ripple);
        }

        if(model == 2)
        {
          snprintf(composition_txt, 2048, "highpass Bessel %fHz %ith order  ", frequency, order);
        }
      }

      if(type == 1)
      {
        if(model == 0)
        {
          snprintf(composition_txt, 2048, "lowpass Butterworth %fHz %ith order  ", frequency, order);
        }

        if(model == 1)
        {
          snprintf(composition_txt, 2048, "lowpass Chebyshev %fHz %ith order %fdB ripple  ", frequency, order, ripple);
        }

        if(model == 2)
        {
          snprintf(composition_txt, 2048, "lowpass Bessel %fHz %ith order  ", frequency, order);
        }
      }

      if(type == 2)
      {
        snprintf(composition_txt, 2048, "notch %fHz Q-factor %i  ", frequency, order);
      }

      if(type == 3)
      {
        if(model == 0)
        {
          snprintf(composition_txt, 2048, "bandpass Butterworth %f-%fHz %ith order  ", frequency, frequency2, order);
        }

        if(model == 1)
        {
          snprintf(composition_txt, 2048, "bandpass Chebyshev %f-%fHz %ith order %fdB ripple  ", frequency, frequency2, order, ripple);
        }

        if(model == 2)
        {
          snprintf(composition_txt, 2048, "bandpass Bessel %f-%fHz %ith order  ", frequency, frequency2, order);
        }
      }

      if(type == 4)
      {
        if(model == 0)
        {
          snprintf(composition_txt, 2048, "bandstop Butterworth %f-%fHz %ith order  ", frequency, frequency2, order);
        }

        if(model == 1)
        {
          snprintf(composition_txt, 2048, "bandstop Chebyshev %f-%fHz %ith order %fdB ripple  ", frequency, frequency2, order, ripple);
        }

        if(model == 2)
        {
          snprintf(composition_txt, 2048, "bandstop Bessel %f-%fHz %ith order  ", frequency, frequency2, order);
        }
      }

      remove_trailing_zeros(composition_txt);

      filterItem->appendRow(new QStandardItem(composition_txt));

      xml_go_up(xml_hdl);
      xml_go_up(xml_hdl);
    }

    if(!xml_goto_nth_element_inside(xml_hdl, "plif_ecg_filter", 0))
    {
      if(xml_goto_nth_element_inside(xml_hdl, "plf", 0))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file.");
        messagewindow.exec();
        xml_close(xml_hdl);
        return;
      }
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file.");
        messagewindow.exec();
        xml_close(xml_hdl);
        return;
      }
      plif_powerlinefrequency = atoi(result);
      if((plif_powerlinefrequency != 0) && (plif_powerlinefrequency != 1))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file.");
        messagewindow.exec();
        xml_close(xml_hdl);
        return;
      }
      plif_powerlinefrequency *= 10;
      plif_powerlinefrequency += 50;
      xml_go_up(xml_hdl);

      if(xml_goto_nth_element_inside(xml_hdl, "linear_threshold", 0))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file.");
        messagewindow.exec();
        xml_close(xml_hdl);
        return;
      }
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file.");
        messagewindow.exec();
        xml_close(xml_hdl);
        return;
      }
      plif_linear_threshold = atoi(result);
      if((plif_linear_threshold < 10) || (plif_linear_threshold > 200))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file.");
        messagewindow.exec();
        xml_close(xml_hdl);
        return;
      }
      xml_go_up(xml_hdl);

      snprintf(composition_txt, 2048, "Powerline interference removal: %iHz  threshold: %iuV", plif_powerlinefrequency, plif_linear_threshold);

      filterItem->appendRow(new QStandardItem(composition_txt));

      xml_go_up(xml_hdl);
    }

    if(!xml_goto_nth_element_inside(xml_hdl, "fir_filter", 0))
    {
      if(xml_goto_nth_element_inside(xml_hdl, "size", 0))
      {
        snprintf(str2, 2048, "There seems to be an error in this montage file.\nFile: %s line: %i", __FILE__, __LINE__);
        QMessageBox messagewindow(QMessageBox::Critical, "Error", str2);
        messagewindow.exec();
        xml_close(xml_hdl);
        return;
      }
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file.");
        messagewindow.exec();
        xml_close(xml_hdl);
        return;
      }
      n_taps = atoi(result);
      if((n_taps < 2) || (n_taps > 1000))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file.");
        messagewindow.exec();
        xml_close(xml_hdl);
        return;
      }

      xml_go_up(xml_hdl);

      for(r=0; r<n_taps; r++)
      {
        if(xml_goto_nth_element_inside(xml_hdl, "tap", r))
        {
          snprintf(str2, 2048, "There seems to be an error in this montage file.\nFile: %s line: %i", __FILE__, __LINE__);
          QMessageBox messagewindow(QMessageBox::Critical, "Error", str2);
          messagewindow.exec();
          xml_close(xml_hdl);
          return;
        }
        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file.");
          messagewindow.exec();
          xml_close(xml_hdl);
          return;
        }
        fir_vars[r] = atof(result);

        xml_go_up(xml_hdl);
      }

      snprintf(str2, 2048, "Custom FIR filter with %i taps", n_taps);

      firfilterItem = new QStandardItem(str2);

      filterItem->appendRow(firfilterItem);

      for(r=0; r<n_taps; r++)
      {
        snprintf(str2, 2048, " %.20f ", fir_vars[r]);

        firfilterItem->appendRow(new QStandardItem(str2));
      }

      xml_go_up(xml_hdl);
    }

    if(!xml_goto_nth_element_inside(xml_hdl, "ecg_filter", 0))
    {
      if(xml_goto_nth_element_inside(xml_hdl, "type", 0))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file.");
        messagewindow.exec();
        xml_close(xml_hdl);
        return;
      }
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file.");
        messagewindow.exec();
        xml_close(xml_hdl);
        return;
      }
      type = atoi(result);

      if(type == 1)
      {
        snprintf(composition_txt, 2048, "ECG heartrate detection");
      }

      filterItem->appendRow(new QStandardItem(composition_txt));

      xml_go_up(xml_hdl);
      xml_go_up(xml_hdl);
    }

    if(!xml_goto_nth_element_inside(xml_hdl, "zratio_filter", 0))
    {
      if(xml_goto_nth_element_inside(xml_hdl, "type", 0))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file.");
        messagewindow.exec();
        xml_close(xml_hdl);
        return;
      }
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file.");
        messagewindow.exec();
        xml_close(xml_hdl);
        return;
      }
      type = atoi(result);

      xml_go_up(xml_hdl);

      if(type == 1)
      {
        if(xml_goto_nth_element_inside(xml_hdl, "crossoverfreq", 0))
        {
          QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file.");
          messagewindow.exec();
          xml_close(xml_hdl);
          return;
        }
        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file.");
          messagewindow.exec();
          xml_close(xml_hdl);
          return;
        }
        d_tmp = atof(result);

        snprintf(composition_txt, 2048, "Z-ratio  cross-over frequency is %.1f", d_tmp);
      }

      filterItem->appendRow(new QStandardItem(composition_txt));

      xml_go_up(xml_hdl);
      xml_go_up(xml_hdl);
    }

    signalcomps_read++;
  }

  xml_goto_root(xml_hdl);

  if(!(xml_goto_nth_element_inside(xml_hdl, "pagetime", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file.");
      messagewindow.exec();
      xml_close(xml_hdl);
      return;
    }
    timescale = atof(result);
    timescale /= TIME_DIMENSION;
    snprintf(composition_txt, 2048, "Timescale: %f seconds", timescale);
    remove_trailing_zeros(composition_txt);
    parentItem->appendRow(new QStandardItem(composition_txt));
  }

  xml_close(xml_hdl);

  tree->expandAll();
}













