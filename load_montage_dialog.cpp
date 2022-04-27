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



#include "load_montage_dialog.h"


UI_LoadMontagewindow::UI_LoadMontagewindow(QWidget *w_parent, char *path)
{
  int i;

  mainwindow = (UI_Mainwindow *)w_parent;

  mtg_path[0] = 0;

  LoadMontageDialog = NULL;

  if(path!=NULL)
  {
    strlcpy(mtg_path, path, MAX_PATH_LENGTH);
  }

  if(mainwindow->rc_cmd_in_progress)
  {
    mainwindow->rc_load_mtg_err = LoadButtonClicked();

    return;
  }

  if(mainwindow->files_open==1)
  {
    LoadButtonClicked();

    return;
  }

  LoadMontageDialog = new QDialog;

  LoadMontageDialog->setMinimumSize(600 * mainwindow->w_scaling, 180 * mainwindow->h_scaling);
  LoadMontageDialog->setWindowTitle("Load montage");
  LoadMontageDialog->setModal(true);
  LoadMontageDialog->setAttribute(Qt::WA_DeleteOnClose, true);

  QLabel *label1 = new QLabel;
  label1->setText("Choose to which file you want to apply the montage:");

  filelist = new QListWidget;
  filelist->setSelectionBehavior(QAbstractItemView::SelectRows);
  filelist->setSelectionMode(QAbstractItemView::SingleSelection);
  for(i=0; i<mainwindow->files_open; i++)
  {
    new QListWidgetItem(QString::fromLocal8Bit(mainwindow->edfheaderlist[i]->filename), filelist);
  }

  LoadButton = new QPushButton;
  LoadButton->setText("Load");

  CloseButton = new QPushButton;
  CloseButton->setText("Close");

  QObject::connect(CloseButton, SIGNAL(clicked()), LoadMontageDialog, SLOT(close()));
  QObject::connect(LoadButton,  SIGNAL(clicked()), this,              SLOT(LoadButtonClicked()));

  filelist->setCurrentRow(mainwindow->files_open - 1);

  QHBoxLayout *hlayout1 = new QHBoxLayout;
  hlayout1->addWidget(LoadButton);
  hlayout1->addStretch(1000);
  hlayout1->addWidget(CloseButton);

  QVBoxLayout *vlayout1 = new QVBoxLayout;
  vlayout1->addWidget(label1);
  vlayout1->addWidget(filelist, 1000);
  vlayout1->addSpacing(20);
  vlayout1->addLayout(hlayout1);

  LoadMontageDialog->setLayout(vlayout1);

  LoadMontageDialog->exec();
}




int UI_LoadMontagewindow::LoadButtonClicked()
{
  int i, k, n=0, p, r,
      tmp,
      skip,
      found,
      signalcomps_read=0,
      signals_read,
      signal_cnt,
      filters_read,
      math_funcs_before_read,
      math_funcs_after_read,
      math_func,
      spike_filter_cnt=0,
      filter_cnt=0,
      math_func_cnt_before=0,
      math_func_cnt_after=0,
      ravg_filter_cnt=0,
      fidfilter_cnt=0,
      order=1,
      type=0,
      model=0,
      size=0,
      amp_cat[3],
      f_ruler_cnt=0,
      holdoff=100,
      plif_powerlinefrequency,
      not_compatibel,
      sf,
      n_taps,
      sense;

  char result[XML_STRBUFLEN],
       scratchpad[2048]="",
       str[128]="",
       str2[512]="",
       *err_ptr=NULL,
       *filter_spec_ptr=NULL,
       spec_str[256]="";

  double frequency=1.0,
         frequency2=2.0,
         ripple=1.0,
         velocity=1.0,
         dthreshold,
         fir_vars[1000];


  struct xml_handle *xml_hdl=NULL;

  struct signalcompblock *newsignalcomp=NULL;

  struct edfhdrblock *edf_hdr=NULL;

  if(mainwindow->rc_cmd_in_progress)
  {
    n = mainwindow->rc_load_mtg_file_num;
  }
  else if(mainwindow->files_open==1)
    {
      n = 0;
    }
    else
    {
      n = filelist->currentRow();
    }

  edf_hdr = mainwindow->edfheaderlist[n];

  if(mtg_path[0]==0)
  {
    strlcpy(mtg_path, QFileDialog::getOpenFileName(0, "Load montage", QString::fromLocal8Bit(mainwindow->recent_montagedir), "Montage files (*.mtg *.MTG)").toLocal8Bit().data(), MAX_PATH_LENGTH);

    if(!strcmp(mtg_path, ""))
    {
      return 0;
    }

    get_directory_from_path(mainwindow->recent_montagedir, mtg_path, MAX_PATH_LENGTH);
  }

  xml_hdl = xml_get_handle(mtg_path);
  if(xml_hdl==NULL)
  {
    if(mainwindow->rc_cmd_in_progress)
    {
      return 105;
    }
    else
    {
      snprintf(scratchpad, 2048, "Can not open montage file:\n%s", mtg_path);
      QMessageBox messagewindow(QMessageBox::Critical, "Error", QString::fromLocal8Bit(scratchpad));
      messagewindow.exec();
      mainwindow->remove_recent_file_mtg_path(mtg_path);
      return 0;
    }
  }

  if(strcmp(xml_hdl->elementname[xml_hdl->level], PROGRAM_NAME "_montage"))
  {
    return format_error(__FILE__, __LINE__, NULL, xml_hdl);
  }

////////////////////////////////////////////

  for(k=0; k<mainwindow->signalcomps; )
  {
    if(mainwindow->signalcomp[k]->edfhdr == edf_hdr)
    {
      for(i=0; i<MAXCDSADOCKS; i++)
      {
        p = mainwindow->signalcomp[k]->cdsa_idx[i];

        if(p != 0)
        {
          delete mainwindow->cdsa_dock[p - 1];

          mainwindow->cdsa_dock[p - 1] = NULL;
        }
      }

      for(i=0; i<MAXSPECTRUMDOCKS; i++)
      {
        if(mainwindow->spectrumdock[i]->signalcomp == mainwindow->signalcomp[k])
        {
          mainwindow->spectrumdock[i]->clear();
          mainwindow->spectrumdock[i]->dock->hide();
        }
      }

      for(i=0; i<MAXSPECTRUMDIALOGS; i++)
      {
        p = mainwindow->signalcomp[k]->spectr_dialog[i];

        if(p != 0)
        {
          delete mainwindow->spectrumdialog[p - 1];

          mainwindow->spectrumdialog[p - 1] = NULL;
        }
      }

      for(i=0; i<MAXAVERAGECURVEDIALOGS; i++)
      {
        p = mainwindow->signalcomp[k]->avg_dialog[i];

        if(p != 0)
        {
          delete mainwindow->averagecurvedialog[p - 1];

          mainwindow->averagecurvedialog[p - 1] = NULL;
        }
      }

      if(mainwindow->signalcomp[k]->hascursor2)
      {
/*        crosshair_2_active = 0;
        crosshair_2_moving = 0;*/
      }

      if(mainwindow->signalcomp[k]->hascursor1)
      {
//         crosshair_1_active = 0;
//         crosshair_2_active = 0;
//         crosshair_1_moving = 0;
//         crosshair_2_moving = 0;

        for(i=0; i<mainwindow->signalcomps; i++)
        {
          mainwindow->signalcomp[i]->hascursor2 = 0;
        }
      }

      if(mainwindow->signalcomp[k]->spike_filter)
      {
        free_spike_filter(mainwindow->signalcomp[k]->spike_filter);

        mainwindow->signalcomp[k]->spike_filter = NULL;
      }

      if(mainwindow->signalcomp[k]->fir_filter)
      {
        free_fir_filter(mainwindow->signalcomp[k]->fir_filter);

        mainwindow->signalcomp[k]->fir_filter = NULL;
      }

      for(i=0; i<mainwindow->signalcomp[k]->filter_cnt; i++)
      {
        free(mainwindow->signalcomp[k]->filter[i]);
      }

      mainwindow->signalcomp[k]->filter_cnt = 0;

      for(i=0; i<mainwindow->signalcomp[k]->math_func_cnt_before; i++)
      {
        free_math_func(mainwindow->signalcomp[k]->math_func_before[i]);
      }

      mainwindow->signalcomp[k]->math_func_cnt_before = 0;

      for(i=0; i<mainwindow->signalcomp[k]->math_func_cnt_after; i++)
      {
        free_math_func(mainwindow->signalcomp[k]->math_func_after[i]);
      }

      mainwindow->signalcomp[k]->math_func_cnt_after = 0;

      for(i=0; i<mainwindow->signalcomp[k]->ravg_filter_cnt; i++)
      {
        free_ravg_filter(mainwindow->signalcomp[k]->ravg_filter[i]);
      }

      mainwindow->signalcomp[k]->ravg_filter_cnt = 0;

      for(i=0; i<mainwindow->signalcomp[k]->fidfilter_cnt; i++)
      {
        free(mainwindow->signalcomp[k]->fidfilter[i]);
        fid_run_free(mainwindow->signalcomp[k]->fid_run[i]);
        fid_run_freebuf(mainwindow->signalcomp[k]->fidbuf[i]);
        fid_run_freebuf(mainwindow->signalcomp[k]->fidbuf2[i]);
      }

      mainwindow->signalcomp[k]->fidfilter_cnt = 0;

      if(mainwindow->signalcomp[k]->plif_ecg_filter)
      {
        plif_free_subtract_filter(mainwindow->signalcomp[k]->plif_ecg_filter);

        mainwindow->signalcomp[k]->plif_ecg_filter = NULL;
      }

      if(mainwindow->signalcomp[k]->plif_ecg_filter_sav)
      {
        plif_free_subtract_filter(mainwindow->signalcomp[k]->plif_ecg_filter_sav);

        mainwindow->signalcomp[k]->plif_ecg_filter_sav = NULL;
      }

      if(mainwindow->signalcomp[k]->ecg_filter != NULL)
      {
        free_ecg_filter(mainwindow->signalcomp[k]->ecg_filter);

        mainwindow->signalcomp[k]->ecg_filter = NULL;

        strlcpy(mainwindow->signalcomp[k]->signallabel, mainwindow->signalcomp[k]->signallabel_bu, 512);
        mainwindow->signalcomp[k]->signallabellen = mainwindow->signalcomp[k]->signallabellen_bu;
      }

      free(mainwindow->signalcomp[k]);

      for(i=k; i<mainwindow->signalcomps - 1; i++)
      {
        mainwindow->signalcomp[i] = mainwindow->signalcomp[i + 1];
      }

      mainwindow->signalcomps--;

      k = 0;
    }
    else
    {
      k++;
    }
  }

////////////////////////////////////////////

  while(1)
  {
    skip = 0;

    xml_goto_root(xml_hdl);

    signals_read = 0;

    if(xml_goto_nth_element_inside(xml_hdl, "signalcomposition", signalcomps_read))
    {
      break;
    }

    newsignalcomp = (struct signalcompblock *)calloc(1, sizeof(struct signalcompblock));
    if(newsignalcomp==NULL)
    {
      if(mainwindow->rc_cmd_in_progress)
      {
        xml_close(xml_hdl);
        return 206;
      }
      else
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "Internal error: Memory allocation error:\n\"new signal composition\"");
        messagewindow.exec();
        xml_close(xml_hdl);
        return 0;
      }
    }

    newsignalcomp->uid = mainwindow->uid_seq++;

    if(xml_goto_nth_element_inside(xml_hdl, "num_of_signals", 0))
    {
      return format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
    }
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      return format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
    }
    signal_cnt = atoi(result);
    if((signal_cnt<1)||(signal_cnt>MAXSIGNALS))
    {
      return format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
    }
    newsignalcomp->edfhdr = mainwindow->edfheaderlist[n];
    newsignalcomp->edfhdr = edf_hdr;
    newsignalcomp->num_of_signals = signal_cnt;
    newsignalcomp->hascursor1 = 0;
    newsignalcomp->hascursor2 = 0;
    newsignalcomp->hasoffsettracking = 0;
    newsignalcomp->hasgaintracking = 0;
    newsignalcomp->screen_offset = 0;
    newsignalcomp->math_func_cnt_before = 0;
    newsignalcomp->math_func_cnt_after = 0;
    newsignalcomp->filter_cnt = 0;
    newsignalcomp->ravg_filter_cnt = 0;
    newsignalcomp->plif_ecg_filter = NULL;
    newsignalcomp->plif_ecg_filter_sav = NULL;
    newsignalcomp->ecg_filter = NULL;
    newsignalcomp->fir_filter = NULL;
    newsignalcomp->fidfilter_cnt = 0;
    newsignalcomp->hasruler = 0;
    newsignalcomp->polarity = 1;

    xml_go_up(xml_hdl);
    if(!(xml_goto_nth_element_inside(xml_hdl, "alias", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        return format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }
      if(result[0] != 0)
      {
        strncpy(newsignalcomp->alias, result, 16);
        newsignalcomp->alias[16] = 0;
        latin1_to_ascii(newsignalcomp->alias, strlen(newsignalcomp->alias));
        trim_spaces(newsignalcomp->alias);
      }
      xml_go_up(xml_hdl);
    }

    if(xml_goto_nth_element_inside(xml_hdl, "voltpercm", 0))
    {
      return format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
    }
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      return format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
    }
    newsignalcomp->voltpercm = atof(result);
    if(newsignalcomp->voltpercm==0.0)  newsignalcomp->voltpercm = 0.000000001;
    xml_go_up(xml_hdl);
    if(xml_goto_nth_element_inside(xml_hdl, "screen_offset", 0))
    {
      return format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
    }
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      return format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
    }
    newsignalcomp->screen_offset = atof(result);
    xml_go_up(xml_hdl);

    if(!(xml_goto_nth_element_inside(xml_hdl, "polarity", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        return format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }
      newsignalcomp->polarity = atoi(result);
      if(newsignalcomp->polarity != -1)
      {
        newsignalcomp->polarity = 1;
      }
      xml_go_up(xml_hdl);
    }

    if(xml_goto_nth_element_inside(xml_hdl, "color", 0))
    {
      return format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
    }
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      return format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
    }
    newsignalcomp->color = atoi(result);
    if((newsignalcomp->color < 2) || (newsignalcomp->color > 18))
    {
      newsignalcomp->color = 2;
    }

    xml_go_up(xml_hdl);

    if(!(xml_goto_nth_element_inside(xml_hdl, "math_func_cnt_before", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        return format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }
      math_func_cnt_before = atoi(result);
      if(math_func_cnt_before < 0)  math_func_cnt_before = 0;
      if(math_func_cnt_before > MAXMATHFUNCS)  math_func_cnt_before = MAXMATHFUNCS;

      xml_go_up(xml_hdl);
    }

    if(!(xml_goto_nth_element_inside(xml_hdl, "math_func_cnt_after", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        return format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }
      math_func_cnt_after = atoi(result);
      if(math_func_cnt_after < 0)  math_func_cnt_after = 0;
      if(math_func_cnt_after > MAXMATHFUNCS)  math_func_cnt_after = MAXMATHFUNCS;

      xml_go_up(xml_hdl);
    }

    if(!(xml_goto_nth_element_inside(xml_hdl, "filter_cnt", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        return format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }
      filter_cnt = atoi(result);
      if(filter_cnt < 0)  filter_cnt = 0;
      if(filter_cnt > MAXFILTERS)  filter_cnt = MAXFILTERS;

      xml_go_up(xml_hdl);
    }

    if(!(xml_goto_nth_element_inside(xml_hdl, "spike_filter_cnt", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        return format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }
      spike_filter_cnt = atoi(result);
      if(spike_filter_cnt < 0)  spike_filter_cnt = 0;
      if(spike_filter_cnt > 1)  spike_filter_cnt = 1;

      xml_go_up(xml_hdl);
    }

    if(!(xml_goto_nth_element_inside(xml_hdl, "ravg_filter_cnt", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        return format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }
      ravg_filter_cnt = atoi(result);
      if(ravg_filter_cnt < 0)  ravg_filter_cnt = 0;
      if(ravg_filter_cnt > MAXFILTERS)  ravg_filter_cnt = MAXFILTERS;

      xml_go_up(xml_hdl);
    }

    if(filter_cnt)
    {
      fidfilter_cnt = 0;
    }
    else
    {
      if(!(xml_goto_nth_element_inside(xml_hdl, "fidfilter_cnt", 0)))
      {
        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          return format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
        }
        fidfilter_cnt = atoi(result);
        if(fidfilter_cnt < 0)  fidfilter_cnt = 0;
        if(fidfilter_cnt > MAXFILTERS)  fidfilter_cnt = MAXFILTERS;

        xml_go_up(xml_hdl);
      }
    }

    for(signals_read=0; signals_read<signal_cnt; signals_read++)
    {
      if(xml_goto_nth_element_inside(xml_hdl, "signal", signals_read))
      {
        return format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }

      if(xml_goto_nth_element_inside(xml_hdl, "factor", 0))
      {
        return format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        return format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }
      newsignalcomp->factor[signals_read] = atof(result);
      if(newsignalcomp->factor[signals_read] < -128)
      {
        newsignalcomp->factor[signals_read] = -128;
      }
      if(newsignalcomp->factor[signals_read] > 128)
      {
        newsignalcomp->factor[signals_read] = 128;
      }
      if((newsignalcomp->factor[signals_read] < 0.001) && (newsignalcomp->factor[signals_read] > -0.001))
      {
        newsignalcomp->factor[signals_read] = 1;
      }

      xml_go_up(xml_hdl);

      if(xml_goto_nth_element_inside(xml_hdl, "edfindex", 0))
      {
        if(xml_goto_nth_element_inside(xml_hdl, "label", 0))
        {
          return format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
        }
        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          return format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
        }

        remove_trailing_spaces(result);

        found = 0;

        for(i=0; i<newsignalcomp->edfhdr->edfsignals; i++)
        {
          strlcpy(scratchpad, newsignalcomp->edfhdr->edfparam[i].label, 2048);

          remove_trailing_spaces(scratchpad);

          if(!strcmp(scratchpad, result))
          {
            newsignalcomp->edfsignal[signals_read] = i;

            if(newsignalcomp->edfhdr->edfparam[newsignalcomp->edfsignal[signals_read]].annotation)
            {
              found = 0;
            }
            else
            {
              found = 1;
            }
            break;
          }
        }
      }
      else
      {
        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          return format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
        }

        newsignalcomp->edfsignal[signals_read] = atoi(result);

        if((newsignalcomp->edfsignal[signals_read] < 0) || (newsignalcomp->edfsignal[signals_read] >= newsignalcomp->edfhdr->edfsignals))
        {
          found = 0;
        }
        else
        {
          if(newsignalcomp->edfhdr->edfparam[newsignalcomp->edfsignal[signals_read]].annotation)
          {
            found = 0;
          }
          else
          {
            found = 1;
          }
        }
      }

      if(!found)
      {
        free(newsignalcomp);
        newsignalcomp = NULL;
        skip = 1;
        signalcomps_read++;
        xml_go_up(xml_hdl);
        xml_go_up(xml_hdl);
        break;
      }

      if(signals_read)
      {
        if(newsignalcomp->edfhdr->edfparam[newsignalcomp->edfsignal[signals_read]].smp_per_record
          != newsignalcomp->edfhdr->edfparam[newsignalcomp->edfsignal[0]].smp_per_record)
        {
          free(newsignalcomp);
          newsignalcomp = NULL;
          skip = 1;
          signalcomps_read++;
          xml_go_up(xml_hdl);
          xml_go_up(xml_hdl);
          break;
        }

        if(dblcmp(newsignalcomp->edfhdr->edfparam[newsignalcomp->edfsignal[signals_read]].bitvalue,
                  newsignalcomp->edfhdr->edfparam[newsignalcomp->edfsignal[0]].bitvalue))
        {
          free(newsignalcomp);
          newsignalcomp = NULL;
          skip = 1;
          signalcomps_read++;
          xml_go_up(xml_hdl);
          xml_go_up(xml_hdl);
          break;
        }

        if(strcmp(newsignalcomp->edfhdr->edfparam[newsignalcomp->edfsignal[signals_read]].physdimension,
                  newsignalcomp->edfhdr->edfparam[newsignalcomp->edfsignal[0]].physdimension))
        {
          free(newsignalcomp);
          newsignalcomp = NULL;
          skip = 1;
          signalcomps_read++;
          xml_go_up(xml_hdl);
          xml_go_up(xml_hdl);
          break;
        }
      }

      if(!signals_read)
      {
        newsignalcomp->sensitivity = newsignalcomp->edfhdr->edfparam[newsignalcomp->edfsignal[0]].bitvalue / (newsignalcomp->voltpercm * mainwindow->y_pixelsizefactor);

        newsignalcomp->signallabel[0] = 0;
      }

      if(signal_cnt>1)
      {
        if(newsignalcomp->factor[signals_read] < 0)
        {
          strlcat(newsignalcomp->signallabel, "- ", 512);
        }
        else
        {
          if(signals_read)
          {
            strlcat(newsignalcomp->signallabel, "+ ", 512);
          }
        }
      }
      strlcpy(str, newsignalcomp->edfhdr->edfparam[newsignalcomp->edfsignal[signals_read]].label, 128);
      if(mainwindow->strip_label_types)
      {
        newsignalcomp->signallabel_type_stripped = strip_types_from_label(str);
      }
      strlcat(newsignalcomp->signallabel, str, 512);
      remove_trailing_spaces(newsignalcomp->signallabel);
      strlcat(newsignalcomp->signallabel, " ", 512);

      newsignalcomp->file_duration = newsignalcomp->edfhdr->long_data_record_duration * newsignalcomp->edfhdr->datarecords;

      xml_go_up(xml_hdl);
      xml_go_up(xml_hdl);
    }

    if(skip || (!newsignalcomp))  continue;

    remove_trailing_spaces(newsignalcomp->signallabel);
    newsignalcomp->signallabellen = strlen(newsignalcomp->signallabel);

    strlcpy(newsignalcomp->physdimension, newsignalcomp->edfhdr->edfparam[newsignalcomp->edfsignal[0]].physdimension, 9);
    remove_trailing_spaces(newsignalcomp->physdimension);

    if(spike_filter_cnt)
    {
      if(xml_goto_nth_element_inside(xml_hdl, "spike_filter", 0))
      {
        return format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }

      if(xml_goto_nth_element_inside(xml_hdl, "velocity", 0))
      {
        return format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        return format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }
      velocity = atof(result);
      if(velocity < 0.0001)  velocity = 0.0001;
      if(velocity > 10E9)  velocity = 10E9;

      xml_go_up(xml_hdl);
      if(xml_goto_nth_element_inside(xml_hdl, "holdoff", 0))
      {
        return format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        return format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }
      holdoff = atoi(result);
      if(holdoff < 10)  holdoff = 10;
      if(holdoff > 1000)  holdoff = 1000;

      newsignalcomp->spike_filter = create_spike_filter(newsignalcomp->edfhdr->edfparam[newsignalcomp->edfsignal[0]].sf_f,
        velocity / newsignalcomp->edfhdr->edfparam[newsignalcomp->edfsignal[0]].bitvalue,
        holdoff, NULL);

      if(newsignalcomp->spike_filter == NULL)
      {
        return format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }

      newsignalcomp->spike_filter_velocity = velocity;

      newsignalcomp->spike_filter_holdoff = holdoff;

      xml_go_up(xml_hdl);
      xml_go_up(xml_hdl);
    }

    for(math_funcs_before_read=0; math_funcs_before_read<math_func_cnt_before; math_funcs_before_read++)
    {
      if(xml_goto_nth_element_inside(xml_hdl, "math_func_before", math_funcs_before_read))
      {
        return format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }

      if(xml_goto_nth_element_inside(xml_hdl, "func", 0))
      {
        return format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        return format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }
      math_func = atoi(result);
      if((math_func < 0) || (math_func >= MATH_MAX_FUNCS))
      {
        return format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }

      newsignalcomp->math_func_before[newsignalcomp->math_func_cnt_before] = create_math_func(math_func);
      if(newsignalcomp->math_func_before[newsignalcomp->math_func_cnt_before] == NULL)
      {
        return format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }
      newsignalcomp->math_func_cnt_before++;

      xml_go_up(xml_hdl);
      xml_go_up(xml_hdl);
    }

    for(math_funcs_after_read=0; math_funcs_after_read<math_func_cnt_after; math_funcs_after_read++)
    {
      if(xml_goto_nth_element_inside(xml_hdl, "math_func_after", math_funcs_after_read))
      {
        return format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }

      if(xml_goto_nth_element_inside(xml_hdl, "func", 0))
      {
        return format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        return format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }
      math_func = atoi(result);
      if((math_func < 0) || (math_func >= MATH_MAX_FUNCS))
      {
        return format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }

      newsignalcomp->math_func_after[newsignalcomp->math_func_cnt_after] = create_math_func(math_func);
      if(newsignalcomp->math_func_after[newsignalcomp->math_func_cnt_after] == NULL)
      {
        return format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }
      newsignalcomp->math_func_cnt_after++;

      xml_go_up(xml_hdl);
      xml_go_up(xml_hdl);
    }

    for(filters_read=0; filters_read<filter_cnt; filters_read++)
    {
      if(xml_goto_nth_element_inside(xml_hdl, "filter", filters_read))
      {
        return format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }

      if(xml_goto_nth_element_inside(xml_hdl, "LPF", 0))
      {
        return format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        return format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }
      type = atoi(result);

      xml_go_up(xml_hdl);
      if(xml_goto_nth_element_inside(xml_hdl, "frequency", 0))
      {
        return format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        return format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }
      frequency = atof(result);

      if((type   < 0) || (type   >   1) || (frequency < 0.0001))
      {
        return format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }

      if(frequency >= newsignalcomp->edfhdr->edfparam[newsignalcomp->edfsignal[0]].sf_f / 2.0)
      {
        return format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }

      if(type == 0)
      {
        snprintf(spec_str, 256, "HpBu1/%f", frequency);
      }

      if(type == 1)
      {
        snprintf(spec_str, 256, "LpBu1/%f", frequency);
      }

      filter_spec_ptr = spec_str;

      err_ptr = fid_parse(newsignalcomp->edfhdr->edfparam[newsignalcomp->edfsignal[0]].sf_f,
                      &filter_spec_ptr,
                      &newsignalcomp->fidfilter[filters_read]);

      if(err_ptr != NULL)
      {
        if(mainwindow->rc_cmd_in_progress)
        {
          free(err_ptr);
          err_ptr = NULL;
          free(newsignalcomp);
          newsignalcomp = NULL;
          xml_close(xml_hdl);
          return 206;
        }
        else
        {
          snprintf(str2, 512, "%s\nFile: %s line: %i", err_ptr, __FILE__, __LINE__);
          QMessageBox messagewindow(QMessageBox::Critical, "Error", str2);
          messagewindow.exec();
          free(err_ptr);
          err_ptr = NULL;
          free(newsignalcomp);
          newsignalcomp = NULL;
          xml_close(xml_hdl);
          return 0;
        }
      }

      newsignalcomp->fid_run[filters_read] = fid_run_new(newsignalcomp->fidfilter[filters_read],
                                                                         &newsignalcomp->fidfuncp[filters_read]);

      newsignalcomp->fidbuf[filters_read] = fid_run_newbuf(newsignalcomp->fid_run[filters_read]);
      newsignalcomp->fidbuf2[filters_read] = fid_run_newbuf(newsignalcomp->fid_run[filters_read]);

      newsignalcomp->fidfilter_freq[filters_read] = frequency;

      newsignalcomp->fidfilter_freq2[filters_read] = frequency * 1.12;

      newsignalcomp->fidfilter_ripple[filters_read] = -1.0;

      newsignalcomp->fidfilter_order[filters_read] = 1;

      newsignalcomp->fidfilter_type[filters_read] = type;

      newsignalcomp->fidfilter_model[filters_read] = 0;

      newsignalcomp->fidfilter_setup[filters_read] = 1;

      newsignalcomp->fidfilter_cnt = filters_read + 1;

      xml_go_up(xml_hdl);
      xml_go_up(xml_hdl);
    }

    for(filters_read=0; filters_read<ravg_filter_cnt; filters_read++)
    {
      if(xml_goto_nth_element_inside(xml_hdl, "ravg_filter", filters_read))
      {
        return format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }

      if(xml_goto_nth_element_inside(xml_hdl, "type", 0))
      {
        return format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        return format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }
      type = atoi(result);

      xml_go_up(xml_hdl);
      if(xml_goto_nth_element_inside(xml_hdl, "size", 0))
      {
        return format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        return format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }
      size = atoi(result);

      if((type   < 0) || (type   >   1) || (size < 2) || (size > 10000))
      {
        return format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }

      newsignalcomp->ravg_filter[filters_read] = create_ravg_filter(type, size);
      if(newsignalcomp->ravg_filter[filters_read] == NULL)
      {
        return format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }

      newsignalcomp->ravg_filter_size[filters_read] = size;

      newsignalcomp->ravg_filter_type[filters_read] = type;

      newsignalcomp->ravg_filter_cnt = filters_read + 1;

      xml_go_up(xml_hdl);
      xml_go_up(xml_hdl);
    }

    for(filters_read=0; filters_read<fidfilter_cnt; filters_read++)
    {
      if(xml_goto_nth_element_inside(xml_hdl, "fidfilter", filters_read))
      {
        return format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }

      if(xml_goto_nth_element_inside(xml_hdl, "type", 0))
      {
        return format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        return format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }
      type = atoi(result);

      xml_go_up(xml_hdl);
      if(xml_goto_nth_element_inside(xml_hdl, "model", 0))
      {
        return format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        return format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }
      model = atoi(result);

      xml_go_up(xml_hdl);
      if(xml_goto_nth_element_inside(xml_hdl, "frequency", 0))
      {
        return format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        return format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }
      frequency = atof(result);

      xml_go_up(xml_hdl);
      if(xml_goto_nth_element_inside(xml_hdl, "frequency2", 0))
      {
        return format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        return format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }
      frequency2 = atof(result);

      xml_go_up(xml_hdl);
      if(xml_goto_nth_element_inside(xml_hdl, "ripple", 0))
      {
        return format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        return format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }
      ripple = atof(result);

      xml_go_up(xml_hdl);
      if(xml_goto_nth_element_inside(xml_hdl, "order", 0))
      {
        return format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        return format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }
      order = atoi(result);

      if((model  < 0)         || (model  >   2)       ||
         (order  < 1)         || (order  > 100)       ||
         (type   < 0)         || (type   >   4)       ||
         (ripple < (-6.0))    || (ripple > (-0.1))    ||
         (frequency < 0.0001) || (frequency2 < 0.0001))
      {
        return format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }

      if(frequency >= newsignalcomp->edfhdr->edfparam[newsignalcomp->edfsignal[0]].sf_f / 2.0)
      {
        if(mainwindow->rc_cmd_in_progress)
        {
          free(newsignalcomp);
          newsignalcomp = NULL;
          xml_close(xml_hdl);
          return 108;
        }
        else
        {
          snprintf(str2, 512, "There seems to be an error in this montage file.\nThe frequency of the filter(s) must be less than: samplerate / 2\nFile: %s line: %i", __FILE__, __LINE__);
          QMessageBox messagewindow(QMessageBox::Critical, "Error", str2);
          messagewindow.exec();
          free(newsignalcomp);
          newsignalcomp = NULL;
          xml_close(xml_hdl);
          return 0;
        }
      }

      if(type > 2)
      {
        if(frequency2 >= newsignalcomp->edfhdr->edfparam[newsignalcomp->edfsignal[0]].sf_f / 2.0)
        {
          if(mainwindow->rc_cmd_in_progress)
          {
            free(newsignalcomp);
            newsignalcomp = NULL;
            xml_close(xml_hdl);
            return 108;
          }
          else
          {
            snprintf(str2, 512, "There seems to be an error in this montage file.\nThe frequency of the filter(s) must be less than: samplerate / 2\nFile: %s line: %i", __FILE__, __LINE__);
            QMessageBox messagewindow(QMessageBox::Critical, "Error", str2);
            messagewindow.exec();
            free(newsignalcomp);
            newsignalcomp = NULL;
            xml_close(xml_hdl);
          }
          return 0;
        }
      }

      if((type == 0) || (type == 1))
      {
        if(order > 8)
        {
          order = 8;
        }
      }

      if(type == 0)
      {
        if(model == 0)
        {
          snprintf(spec_str, 256, "HpBu%i/%f", order, frequency);
        }

        if(model == 1)
        {
          snprintf(spec_str, 256, "HpCh%i/%f/%f", order, ripple, frequency);
        }

        if(model == 2)
        {
          snprintf(spec_str, 256, "HpBe%i/%f", order, frequency);
        }
      }

      if(type == 1)
      {
        if(model == 0)
        {
          snprintf(spec_str, 256, "LpBu%i/%f", order, frequency);
        }

        if(model == 1)
        {
          snprintf(spec_str, 256, "LpCh%i/%f/%f", order, ripple, frequency);
        }

        if(model == 2)
        {
          snprintf(spec_str, 256, "LpBe%i/%f", order, frequency);
        }
      }

      if(type == 2)
      {
        if(order > 100)
        {
          order = 100;
        }

        if(order < 3)
        {
          order = 3;
        }

        if(model == 0)
        {
          snprintf(spec_str, 256, "BsRe/%i/%f", order, frequency);
        }
      }

      if((type == 3) || (type == 4))
      {
        if(order < 2)
        {
          order = 2;
        }

        if(order % 2)
        {
          order++;
        }

        if(order > 16)
        {
          order = 16;
        }
      }

      if(type == 3)
      {
        if(model == 0)
        {
          snprintf(spec_str, 256, "BpBu%i/%f-%f", order, frequency, frequency2);
        }

        if(model == 1)
        {
          snprintf(spec_str, 256, "BpCh%i/%f/%f-%f", order, ripple, frequency, frequency2);
        }

        if(model == 2)
        {
          snprintf(spec_str, 256, "BpBe%i/%f-%f", order, frequency, frequency2);
        }
      }

      if(type == 4)
      {
        if(model == 0)
        {
          snprintf(spec_str, 256, "BsBu%i/%f-%f", order, frequency, frequency2);
        }

        if(model == 1)
        {
          snprintf(spec_str, 256, "BsCh%i/%f/%f-%f", order, ripple, frequency, frequency2);
        }

        if(model == 2)
        {
          snprintf(spec_str, 256, "BsBe%i/%f-%f", order, frequency, frequency2);
        }
      }

      filter_spec_ptr = spec_str;

      err_ptr = fid_parse(newsignalcomp->edfhdr->edfparam[newsignalcomp->edfsignal[0]].sf_f,
                      &filter_spec_ptr,
                      &newsignalcomp->fidfilter[filters_read]);

      if(err_ptr != NULL)
      {
        if(mainwindow->rc_cmd_in_progress)
        {
          free(err_ptr);
          err_ptr = NULL;
          free(newsignalcomp);
          newsignalcomp = NULL;
          xml_close(xml_hdl);
          return 206;
        }
        else
        {
          QMessageBox messagewindow(QMessageBox::Critical, "Error", err_ptr);
          messagewindow.exec();
          free(err_ptr);
          err_ptr = NULL;
          free(newsignalcomp);
          newsignalcomp = NULL;
          xml_close(xml_hdl);
          return 0;
        }
      }

      newsignalcomp->fid_run[filters_read] = fid_run_new(newsignalcomp->fidfilter[filters_read],
                                                                         &newsignalcomp->fidfuncp[filters_read]);

      newsignalcomp->fidbuf[filters_read] = fid_run_newbuf(newsignalcomp->fid_run[filters_read]);
      newsignalcomp->fidbuf2[filters_read] = fid_run_newbuf(newsignalcomp->fid_run[filters_read]);

      newsignalcomp->fidfilter_freq[filters_read] = frequency;

      newsignalcomp->fidfilter_freq2[filters_read] = frequency2;

      newsignalcomp->fidfilter_ripple[filters_read] = ripple;

      newsignalcomp->fidfilter_order[filters_read] = order;

      newsignalcomp->fidfilter_type[filters_read] = type;

      newsignalcomp->fidfilter_model[filters_read] = model;

      newsignalcomp->fidfilter_setup[filters_read] = 1;

      newsignalcomp->fidfilter_cnt = filters_read + 1;

      xml_go_up(xml_hdl);
      xml_go_up(xml_hdl);
    }

    if(!xml_goto_nth_element_inside(xml_hdl, "plif_ecg_filter", 0))
    {
      not_compatibel = 0;

      sf = newsignalcomp->edfhdr->edfparam[newsignalcomp->edfsignal[0]].sf_int;

      if(!sf)
      {
        not_compatibel = 1;
      }

      if(xml_goto_nth_element_inside(xml_hdl, "plf", 0))
      {
        return format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        return format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }
      plif_powerlinefrequency = atoi(result);
      if((plif_powerlinefrequency != 0) && (plif_powerlinefrequency != 1))
      {
        return format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }
      plif_powerlinefrequency *= 10;
      plif_powerlinefrequency += 50;
      xml_go_up(xml_hdl);

      if(sf % plif_powerlinefrequency)  not_compatibel = 1;

      strlcpy(str, newsignalcomp->edfhdr->edfparam[newsignalcomp->edfsignal[0]].physdimension, 128);

      remove_trailing_spaces(str);

      if(!strcmp(str, "uV"))
      {
        dthreshold = 10.0 / newsignalcomp->edfhdr->edfparam[newsignalcomp->edfsignal[0]].bitvalue;
      }
      else if(!strcmp(str, "mV"))
        {
          dthreshold = 1e-2 / newsignalcomp->edfhdr->edfparam[newsignalcomp->edfsignal[0]].bitvalue;
        }
        else if(!strcmp(str, "V"))
          {
            dthreshold = 1e-5 / newsignalcomp->edfhdr->edfparam[newsignalcomp->edfsignal[0]].bitvalue;
          }
          else
          {
            not_compatibel = 1;
          }

      if(!not_compatibel)
      {
        newsignalcomp->plif_ecg_filter = plif_create_subtract_filter(sf, plif_powerlinefrequency, dthreshold);
        if(newsignalcomp->plif_ecg_filter == NULL)
        {
          if(mainwindow->rc_cmd_in_progress)
          {
            free(newsignalcomp);
            newsignalcomp = NULL;
            xml_close(xml_hdl);
            return 206;
          }
          else
          {
            snprintf(str2, 512, "A memory allocation error occurred when creating a powerline interference removal filter.\n"
                          "File: %s line: %i", __FILE__, __LINE__);
            QMessageBox messagewindow(QMessageBox::Critical, "Error", str2);
            messagewindow.exec();
            free(newsignalcomp);
            newsignalcomp = NULL;
            xml_close(xml_hdl);
            return 0;
          }
        }

        newsignalcomp->plif_ecg_filter_sav = plif_create_subtract_filter(sf, plif_powerlinefrequency, dthreshold);
        if(newsignalcomp->plif_ecg_filter_sav == NULL)
        {
          if(mainwindow->rc_cmd_in_progress)
          {
            free(newsignalcomp);
            newsignalcomp = NULL;
            xml_close(xml_hdl);
            return 206;
          }
          else
          {
            snprintf(str2, 512, "A memory allocation error occurred when creating a powerline interference removal filter.\n"
                          "File: %s line: %i", __FILE__, __LINE__);
            QMessageBox messagewindow(QMessageBox::Critical, "Error", str2);
            messagewindow.exec();
            free(newsignalcomp);
            newsignalcomp = NULL;
            xml_close(xml_hdl);
            return 0;
          }
        }

        newsignalcomp->plif_ecg_subtract_filter_plf = plif_powerlinefrequency / 60;
      }

      xml_go_up(xml_hdl);
    }

    if(!xml_goto_nth_element_inside(xml_hdl, "fir_filter", 0))
    {
      if(xml_goto_nth_element_inside(xml_hdl, "size", 0))
      {
        return format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        return format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }
      n_taps = atoi(result);
      if((n_taps < 2) || (n_taps > 1000))
      {
        return format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }

      xml_go_up(xml_hdl);

      for(r=0; r<n_taps; r++)
      {
        if(xml_goto_nth_element_inside(xml_hdl, "tap", r))
        {
          return format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
        }
        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          return format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
        }
        fir_vars[r] = atof(result);

        xml_go_up(xml_hdl);
      }

      newsignalcomp->fir_filter = create_fir_filter(fir_vars, n_taps);
      if(newsignalcomp->fir_filter == NULL)
      {
        return format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }

      xml_go_up(xml_hdl);
    }

    if(!xml_goto_nth_element_inside(xml_hdl, "ecg_filter", 0))
    {
      if(xml_goto_nth_element_inside(xml_hdl, "type", 0))
      {
        return format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        return format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
      }
      type = atoi(result);

      if(type == 1)
      {
        if(newsignalcomp->edfhdr->edfparam[newsignalcomp->edfsignal[0]].sf_f >= 199.999)
        {
          strlcpy(str2, newsignalcomp->edfhdr->edfparam[newsignalcomp->edfsignal[0]].physdimension, 32);
          trim_spaces(str2);
          sense = 0;
          if((!strcmp(str2, "uV")) || (!strcmp(str2, "ECG uV")) || (!strcmp(str2, "EEG uV")))
          {
            sense = 1;
          }
          else if((!strcmp(str2, "mV")) || (!strcmp(str2, "ECG mV")) || (!strcmp(str2, "EEG mV")))
            {
              sense = 1000;
            }
            else if((!strcmp(str2, "V")) || (!strcmp(str2, "ECG V")) || (!strcmp(str2, "EEG V")))
              {
                sense = 1000000;
              }

          if(sense > 0)
          {
            newsignalcomp->ecg_filter = create_ecg_filter(newsignalcomp->edfhdr->edfparam[newsignalcomp->edfsignal[0]].sf_f,
                                                          newsignalcomp->edfhdr->edfparam[newsignalcomp->edfsignal[0]].bitvalue,
                                                          sense);
            if(newsignalcomp->ecg_filter == NULL)
            {
              if(mainwindow->rc_cmd_in_progress)
              {
                free(newsignalcomp);
                newsignalcomp = NULL;
                xml_close(xml_hdl);
                return 206;
              }
              else
              {
                snprintf(str2, 512, "Could not create an ECG filter.\nFile: %s line: %i", __FILE__, __LINE__);
                QMessageBox messagewindow(QMessageBox::Critical, "Error", str2);
                messagewindow.exec();
                free(newsignalcomp);
                newsignalcomp = NULL;
                xml_close(xml_hdl);
                return 0;
              }
            }

            strlcpy(newsignalcomp->signallabel_bu, newsignalcomp->signallabel, 512);
            newsignalcomp->signallabellen_bu = newsignalcomp->signallabellen;
            strlcpy(newsignalcomp->signallabel, "HR", 512);
            newsignalcomp->signallabellen = strlen(newsignalcomp->signallabel);
            strlcpy(newsignalcomp->physdimension_bu, newsignalcomp->physdimension, 9);
            strlcpy(newsignalcomp->physdimension, "bpm", 9);
          }
        }
      }

      xml_go_up(xml_hdl);
      xml_go_up(xml_hdl);
    }

    if(newsignalcomp->ecg_filter == NULL)
    {
      if(!xml_goto_nth_element_inside(xml_hdl, "zratio_filter", 0))
      {
        if(xml_goto_nth_element_inside(xml_hdl, "type", 0))
        {
          return format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
        }
        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          return format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
        }
        type = atoi(result);

        xml_go_up(xml_hdl);

        if(type == 1)
        {
          if(xml_goto_nth_element_inside(xml_hdl, "crossoverfreq", 0))
          {
            return format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
          }
          if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
          {
            return format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
          }
          newsignalcomp->zratio_crossoverfreq = atof(result);

          if((newsignalcomp->zratio_crossoverfreq < 5.0) || (newsignalcomp->zratio_crossoverfreq > 9.5))
          {
            newsignalcomp->zratio_crossoverfreq = 7.5;
          }

          newsignalcomp->zratio_filter = create_zratio_filter(newsignalcomp->edfhdr->edfparam[newsignalcomp->edfsignal[0]].smp_per_record,
                                                              newsignalcomp->edfhdr->long_data_record_duration,
                                                              newsignalcomp->zratio_crossoverfreq,
            newsignalcomp->edfhdr->edfparam[newsignalcomp->edfsignal[0]].bitvalue);

          if(newsignalcomp->zratio_filter == NULL)
          {
            if(mainwindow->rc_cmd_in_progress)
            {
              free(newsignalcomp);
              newsignalcomp = NULL;
              xml_close(xml_hdl);
              return 206;
            }
            else
            {
              snprintf(str2, 512, "A memory allocation error occurred when creating a Z-ratio filter.\nFile: %s line: %i", __FILE__, __LINE__);
              QMessageBox messagewindow(QMessageBox::Critical, "Error", str2);
              messagewindow.exec();
              free(newsignalcomp);
              newsignalcomp = NULL;
              xml_close(xml_hdl);
              return 0;
            }
          }

          strlcpy(newsignalcomp->signallabel_bu, newsignalcomp->signallabel, 512);
          newsignalcomp->signallabellen_bu = newsignalcomp->signallabellen;
          strlcpy(newsignalcomp->signallabel, "Z-ratio ", 512);
          strlcat(newsignalcomp->signallabel, newsignalcomp->signallabel_bu, 512);
          newsignalcomp->signallabellen = strlen(newsignalcomp->signallabel);
          strlcpy(newsignalcomp->physdimension_bu, newsignalcomp->physdimension, 9);
          strlcpy(newsignalcomp->physdimension, "", 9);

          xml_go_up(xml_hdl);
        }

        xml_go_up(xml_hdl);
      }
    }

    if(f_ruler_cnt == 0)
    {
      if(!xml_goto_nth_element_inside(xml_hdl, "floating_ruler", 0))
      {
        if(xml_goto_nth_element_inside(xml_hdl, "hasruler", 0))
        {
          return format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
        }
        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          return format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
        }
        tmp = atoi(result);

        xml_go_up(xml_hdl);

        if(tmp == 1)
        {
          f_ruler_cnt = 1;

          mainwindow->maincurve->ruler_x_position = 200;
          mainwindow->maincurve->ruler_y_position = 200;
          mainwindow->maincurve->float_ruler_more = 0;

          if(!xml_goto_nth_element_inside(xml_hdl, "ruler_x_position", 0))
          {
            if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
            {
              return format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
            }
            tmp = atoi(result);

            if((tmp >= 0) && (tmp < 5000))
            {
              mainwindow->maincurve->ruler_x_position = tmp;
            }

            xml_go_up(xml_hdl);
          }

          if(!xml_goto_nth_element_inside(xml_hdl, "ruler_y_position", 0))
          {
          if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
          {
            return format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
          }
            tmp = atoi(result);

            if((tmp >= 0) && (tmp < 5000))
            {
              mainwindow->maincurve->ruler_y_position = tmp;
            }

            xml_go_up(xml_hdl);
          }

          if(!xml_goto_nth_element_inside(xml_hdl, "floating_ruler_value", 0))
          {
            if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
            {
              return format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
            }
            tmp = atoi(result);

            if((tmp >= 0) && (tmp < 2))
            {
              mainwindow->maincurve->float_ruler_more = tmp;
            }

            xml_go_up(xml_hdl);
          }

          newsignalcomp->hasruler = 1;
        }

        xml_go_up(xml_hdl);
      }
    }

    mainwindow->signalcomp[mainwindow->signalcomps] = newsignalcomp;

    mainwindow->signalcomps++;

    signalcomps_read++;
  }

  newsignalcomp = NULL;

  xml_goto_root(xml_hdl);

  if(!(xml_goto_nth_element_inside(xml_hdl, "pagetime", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      return format_error(__FILE__, __LINE__, NULL, xml_hdl);
    }
    mainwindow->pagetime = atoll(result);
    if(mainwindow->pagetime < 10000LL)
    {
      mainwindow->pagetime = 10000LL;
    }
  }

  xml_goto_root(xml_hdl);

  struct spectrumdocksettings settings[MAXSPECTRUMDOCKS];

  memset(settings, 0, sizeof(struct spectrumdocksettings[MAXSPECTRUMDOCKS]));

  for(i=0; i<MAXSPECTRUMDOCKS; i++)
  {
    settings[i].signalnr = -1;
  }

  for(i=0; i<MAXSPECTRUMDOCKS; i++)
  {
    if(!(xml_goto_nth_element_inside(xml_hdl, "powerspectrumdock", i)))
    {
      if(xml_goto_nth_element_inside(xml_hdl, "signalnum", 0))
      {
        return format_error(__FILE__, __LINE__, NULL, xml_hdl);
      }
      else
      {
        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          return format_error(__FILE__, __LINE__, NULL, xml_hdl);
        }

        settings[i].signalnr = atoi(result);

        xml_go_up(xml_hdl);
      }

      if(!xml_goto_nth_element_inside(xml_hdl, "dashboard", 0))
      {
        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          return format_error(__FILE__, __LINE__, NULL, xml_hdl);
        }

        settings[i].dashboard = atoi(result);

        xml_go_up(xml_hdl);
      }

      if(xml_goto_nth_element_inside(xml_hdl, "amp", 0))
      {
        return format_error(__FILE__, __LINE__, NULL, xml_hdl);
      }
      else
      {
        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          return format_error(__FILE__, __LINE__, NULL, xml_hdl);
        }

        settings[i].amp = atoi(result);

        xml_go_up(xml_hdl);
      }

      if(xml_goto_nth_element_inside(xml_hdl, "log_min_sl", 0))
      {
        settings[i].log_min_sl = 1000;
      }
      else
      {
        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          return format_error(__FILE__, __LINE__, NULL, xml_hdl);
        }

        settings[i].log_min_sl = atoi(result);

        xml_go_up(xml_hdl);
      }

      if(xml_goto_nth_element_inside(xml_hdl, "wheel", 0))
      {
        return format_error(__FILE__, __LINE__, NULL, xml_hdl);
      }
      else
      {
        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          return format_error(__FILE__, __LINE__, NULL, xml_hdl);
        }

        settings[i].wheel = atoi(result);

        xml_go_up(xml_hdl);
      }

      if(xml_goto_nth_element_inside(xml_hdl, "span", 0))
      {
        return format_error(__FILE__, __LINE__, NULL, xml_hdl);
      }
      else
      {
        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          return format_error(__FILE__, __LINE__, NULL, xml_hdl);
        }

        settings[i].span = atoi(result);

        xml_go_up(xml_hdl);
      }

      if(xml_goto_nth_element_inside(xml_hdl, "center", 0))
      {
        return format_error(__FILE__, __LINE__, NULL, xml_hdl);
      }
      else
      {
        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          return format_error(__FILE__, __LINE__, NULL, xml_hdl);
        }

        settings[i].center = atoi(result);

        xml_go_up(xml_hdl);
      }

      if(xml_goto_nth_element_inside(xml_hdl, "log", 0))
      {
        return format_error(__FILE__, __LINE__, NULL, xml_hdl);
      }
      else
      {
        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          return format_error(__FILE__, __LINE__, NULL, xml_hdl);
        }

        settings[i].log = atoi(result);

        xml_go_up(xml_hdl);
      }

      if(xml_goto_nth_element_inside(xml_hdl, "sqrt", 0))
      {
        return format_error(__FILE__, __LINE__, NULL, xml_hdl);
      }
      else
      {
        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          return format_error(__FILE__, __LINE__, NULL, xml_hdl);
        }

        settings[i].sqrt = atoi(result);

        xml_go_up(xml_hdl);
      }

      if(xml_goto_nth_element_inside(xml_hdl, "colorbar", 0))
      {
        return format_error(__FILE__, __LINE__, NULL, xml_hdl);
      }
      else
      {
        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          return format_error(__FILE__, __LINE__, NULL, xml_hdl);
        }

        settings[i].colorbar = atoi(result);

        xml_go_up(xml_hdl);
      }

      settings[i].blocksize_predefined = 0;

      if(!xml_goto_nth_element_inside(xml_hdl, "blocksize_predefined", 0))
      {
        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          return format_error(__FILE__, __LINE__, NULL, xml_hdl);
        }

        settings[i].blocksize_predefined = atoi(result);
        if((settings[i].blocksize_predefined < 0) || (settings[i].blocksize_predefined > 9))
        {
          return format_error(__FILE__, __LINE__, NULL, xml_hdl);
        }

        xml_go_up(xml_hdl);
      }

      settings[i].dftblocksize = 200;

      if(!xml_goto_nth_element_inside(xml_hdl, "dftblocksize", 0))
      {
        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          return format_error(__FILE__, __LINE__, NULL, xml_hdl);
        }

        settings[i].dftblocksize = atoi(result);
        if(settings[i].dftblocksize < 2)
        {
          return format_error(__FILE__, __LINE__, NULL, xml_hdl);
        }

        xml_go_up(xml_hdl);
      }

      settings[i].overlap = 1;

      if(!xml_goto_nth_element_inside(xml_hdl, "overlap", 0))
      {
        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          return format_error(__FILE__, __LINE__, NULL, xml_hdl);
        }

        settings[i].overlap = atoi(result);
        if((settings[i].overlap < 1) || (settings[i].overlap > 5))
        {
          return format_error(__FILE__, __LINE__, NULL, xml_hdl);
        }

        xml_go_up(xml_hdl);
      }

      settings[i].window_type = 0;

      if(!xml_goto_nth_element_inside(xml_hdl, "window_type", 0))
      {
        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          return format_error(__FILE__, __LINE__, NULL, xml_hdl);
        }

        settings[i].window_type = atoi(result);
        if((settings[i].window_type < 0) || (settings[i].window_type > 12))
        {
          return format_error(__FILE__, __LINE__, NULL, xml_hdl);
        }

        xml_go_up(xml_hdl);
      }

      if(xml_goto_nth_element_inside(xml_hdl, "maxvalue", 0))
      {
        return format_error(__FILE__, __LINE__, NULL, xml_hdl);
      }
      else
      {
        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          return format_error(__FILE__, __LINE__, NULL, xml_hdl);
        }

        settings[i].maxvalue = atof(result);

        xml_go_up(xml_hdl);
      }

      if(xml_goto_nth_element_inside(xml_hdl, "maxvalue_sqrt", 0))
      {
        return format_error(__FILE__, __LINE__, NULL, xml_hdl);
      }
      else
      {
        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          return format_error(__FILE__, __LINE__, NULL, xml_hdl);
        }

        settings[i].maxvalue_sqrt = atof(result);

        xml_go_up(xml_hdl);
      }

      if(xml_goto_nth_element_inside(xml_hdl, "maxvalue_vlog", 0))
      {
        return format_error(__FILE__, __LINE__, NULL, xml_hdl);
      }
      else
      {
        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          return format_error(__FILE__, __LINE__, NULL, xml_hdl);
        }

        settings[i].maxvalue_vlog = atof(result);

        xml_go_up(xml_hdl);
      }

      if(xml_goto_nth_element_inside(xml_hdl, "maxvalue_sqrt_vlog", 0))
      {
        return format_error(__FILE__, __LINE__, NULL, xml_hdl);
      }
      else
      {
        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          return format_error(__FILE__, __LINE__, NULL, xml_hdl);
        }

        settings[i].maxvalue_sqrt_vlog = atof(result);

        xml_go_up(xml_hdl);
      }

      if(xml_goto_nth_element_inside(xml_hdl, "minvalue_vlog", 0))
      {
        return format_error(__FILE__, __LINE__, NULL, xml_hdl);
      }
      else
      {
        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          return format_error(__FILE__, __LINE__, NULL, xml_hdl);
        }

        settings[i].minvalue_vlog = atof(result);

        xml_go_up(xml_hdl);
      }

      if(xml_goto_nth_element_inside(xml_hdl, "minvalue_sqrt_vlog", 0))
      {
        return format_error(__FILE__, __LINE__, NULL, xml_hdl);
      }
      else
      {
        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          return format_error(__FILE__, __LINE__, NULL, xml_hdl);
        }

        settings[i].minvalue_sqrt_vlog = atof(result);

        xml_go_up(xml_hdl);
      }

      xml_go_up(xml_hdl);
    }
    else
    {
      break;
    }
  }

  xml_close(xml_hdl);

  if(LoadMontageDialog!=NULL) LoadMontageDialog->close();

  mainwindow->setMainwindowTitle(mainwindow->edfheaderlist[mainwindow->sel_viewtime]);

  if(mainwindow->files_open == 1)
  {
    strlcpy(&mainwindow->recent_file_mtg_path[0][0], mtg_path, MAX_PATH_LENGTH);
  }

  mainwindow->timescale_doubler = round_125_cat(mainwindow->pagetime);

  for(i=0; i<3; i++)
  {
    amp_cat[i] = 0;
  }

  for(i=0; i<mainwindow->signalcomps; i++)
  {
    tmp = round_125_cat(mainwindow->signalcomp[i]->voltpercm);

    switch(tmp)
    {
      case 10 : amp_cat[0]++;
                break;
      case 20 : amp_cat[1]++;
                break;
      case 50 : amp_cat[2]++;
                break;
    }
  }

  mainwindow->amplitude_doubler = 10;

  if((amp_cat[1] > amp_cat[0]) && (amp_cat[1] >= amp_cat[2]))
  {
    mainwindow->amplitude_doubler = 20;
  }

  if((amp_cat[2] > amp_cat[0]) && (amp_cat[2] > amp_cat[1]))
  {
    mainwindow->amplitude_doubler = 50;
  }

  if(f_ruler_cnt == 1)
  {
    mainwindow->maincurve->ruler_active = 1;
  }

  for(i=0; i<MAXSPECTRUMDOCKS; i++)
  {
    if(settings[i].signalnr >= 0)
    {
      if(mainwindow->spectrumdock[i]->dock->isVisible())
      {
        mainwindow->spectrumdock[i]->clear();
        mainwindow->spectrumdock[i]->dock->hide();
      }
    }
  }

  mainwindow->setup_viewbuf();

  for(i=0; i<MAXSPECTRUMDOCKS; i++)
  {
    if((settings[i].signalnr >= 0) && (settings[i].signalnr < MAXSIGNALS) && (settings[i].signalnr < mainwindow->signalcomps))
    {
      mainwindow->spectrumdock[i]->init(settings[i].signalnr);

      mainwindow->spectrumdock[i]->setsettings(settings[i]);
    }
  }

  return 0;
}


int UI_LoadMontagewindow::format_error(const char *file_name, int line_number, struct signalcompblock *sigcomp, struct xml_handle *hdl)
{
  char str[1024]="";

  if(mainwindow->rc_cmd_in_progress)
  {
    free(sigcomp);
    xml_close(hdl);
    return 108;
  }
  else
  {
    snprintf(str, 1024, "There seems to be an error in this montage file.\nFile: %s line: %i", file_name, line_number);
    QMessageBox messagewindow(QMessageBox::Critical, "Error", str);
    messagewindow.exec();
    free(sigcomp);
    xml_close(hdl);
    return 0;
  }
}












