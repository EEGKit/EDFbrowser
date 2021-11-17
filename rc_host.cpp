/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2021 Teunis van Beelen
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


#include "mainwindow.h"


void UI_Mainwindow::rc_host_server_new_connection()
{
  printf("rc host server: new connection\n");

  if(rc_host_sock == NULL)
  {
    rc_host_sock = rc_host_server->nextPendingConnection();

    if(rc_host_sock != NULL)
    {
      printf("setup rc host socket\n");

      QObject::connect(rc_host_sock, SIGNAL(disconnected()), this, SLOT(rc_host_sock_disconnected_handler()));
      QObject::connect(rc_host_sock, SIGNAL(readyRead()),    this, SLOT(rc_host_sock_rxdata_handler()));
    }
  }
}


void UI_Mainwindow::rc_host_sock_disconnected_handler()
{
  printf("rc host socket disconnected\n");

  rc_host_sock = NULL;
}


void UI_Mainwindow::rc_host_sock_rxdata_handler()
{
  int i, n, len;

  long long ltmp;

  char cmd_str[512]="",
       response_str[512]="",
       str[512]="";

  struct signalcompblock *newsignalcomp=NULL;

  for(n=1; n>0; )
  {
    n = rc_host_sock->readLine(cmd_str, 511);
    if(n < 1)  break;

    if(cmd_str[n-1] == '\n')
    {
      if(--n < 1)  break;
      cmd_str[n] = 0;
    }

    if(cmd_str[n-1] == '\r')
    {
      if(--n < 1)  break;
      cmd_str[n] = 0;
    }

//    ascii_toupper(cmd_str);

    if(!strcmp(cmd_str, "*IDN?"))
    {
      len = snprintf(response_str, 512, PROGRAM_NAME "," PROGRAM_VERSION "," THIS_APP_BITS_W "\n");
      rc_host_sock->write(response_str, len);
      continue;
    }
    if(!strcmp(cmd_str, "QUIT"))
    {
      exit_program();
      continue;
    }
    if(!strcmp(cmd_str, "FILE:CLOSE:ALL"))
    {
      close_all_files();
      continue;
    }
    if((!strncmp(cmd_str, "FILE:OPEN ", 10)) && (strlen(cmd_str) > 10))
    {
      if(files_open >= MAXFILES)  continue;
      if(annot_editor_active && files_open)  continue;
      if((files_open > 0) && (live_stream_active))  continue;

      strlcpy(drop_path, cmd_str + 10, MAX_PATH_LENGTH);
      rc_file_open_requested = 1;
      open_new_file();
      continue;
    }
    if((!strncmp(cmd_str, "MONTAGE:LOAD ", 13)) && (strlen(cmd_str) > 13))
    {
      if(!files_open)  continue;
      if(signalcomps >= MAXSIGNALS)  continue;
      strlcpy(montagepath, cmd_str + 13, MAX_PATH_LENGTH);
      UI_LoadMontagewindow load_mtg(this, montagepath);
      montagepath[0] = 0;
      continue;
    }
    if((!strncmp(cmd_str, "SIGNAL:ADD:LABEL ", 17)) && (strlen(cmd_str) > 17))
    {
      if(!files_open)  continue;
      if(signalcomps >= MAXSIGNALS)  continue;
      strlcpy(response_str, cmd_str + 17, 512);
      trim_spaces(response_str);

      for(i=0; i<edfheaderlist[0]->edfsignals; i++)
      {
        strlcpy(str, edfheaderlist[0]->edfparam[i].label, 512);
        if((!strncmp(str, "EDF Annotations ", 16)) || (!strncmp(str, "BDF Annotations ", 16)))
        {
          continue;
        }

        trim_spaces(str);
        if(!strcmp(str, response_str))
        {
          newsignalcomp = (struct signalcompblock *)calloc(1, sizeof(struct signalcompblock));
          if(newsignalcomp==NULL)  return;
          newsignalcomp->uid = uid_seq++;
          newsignalcomp->num_of_signals = 1;
          newsignalcomp->edfhdr = edfheaderlist[0];
          newsignalcomp->file_duration = newsignalcomp->edfhdr->long_data_record_duration * newsignalcomp->edfhdr->datarecords;
          newsignalcomp->voltpercm = default_amplitude;
          newsignalcomp->color = maincurve->signal_color;
          newsignalcomp->hasruler = 0;
          newsignalcomp->polarity = 1;

          newsignalcomp->edfsignal[0] = i;
          newsignalcomp->factor[0] = 1;
          if(newsignalcomp->edfhdr->edfparam[i].bitvalue < 0.0)
          {
            newsignalcomp->voltpercm = default_amplitude * -1;
          }
          newsignalcomp->sensitivity[0] = newsignalcomp->edfhdr->edfparam[i].bitvalue / ((double)newsignalcomp->voltpercm * y_pixelsizefactor);

          strlcpy(newsignalcomp->signallabel, newsignalcomp->edfhdr->edfparam[i].label, 256);
          newsignalcomp->signallabel_type_stripped = strip_types_from_label(newsignalcomp->signallabel);
          remove_trailing_spaces(newsignalcomp->signallabel);

          newsignalcomp->file_duration = newsignalcomp->edfhdr->long_data_record_duration * newsignalcomp->edfhdr->datarecords;

          newsignalcomp->signallabellen = strlen(newsignalcomp->signallabel);

          strlcpy(newsignalcomp->physdimension, newsignalcomp->edfhdr->edfparam[i].physdimension, 9);
          remove_trailing_spaces(newsignalcomp->physdimension);

          signalcomp[signalcomps] = newsignalcomp;
          signalcomps++;

          setup_viewbuf();

          break;
        }
      }
      continue;
    }
    if(!strcmp(cmd_str, "SIGNAL:REMOVE:ALL"))
    {
      remove_all_signals();
      continue;
    }
    if(!strcmp(cmd_str, "TIMESCALE?"))
    {
#ifdef Q_OS_WIN32
      len = _mingw_snprintf(response_str, 512, "%llu.%llu\n", pagetime / TIME_DIMENSION, pagetime % TIME_DIMENSION);
#else
      len = snprintf(response_str, 512, "%llu.%llu\n", pagetime / TIME_DIMENSION, pagetime % TIME_DIMENSION);
#endif
      rc_host_sock->write(response_str, len);
      continue;
    }
    if((!strncmp(cmd_str, "TIMESCALE ", 10)) && (strlen(cmd_str) > 10))
    {
      if(is_number(cmd_str + 10))  continue;
      ltmp = atoll_x(cmd_str + 10, TIME_DIMENSION);
      if((ltmp <= 100LL) || (ltmp >= (3600LL * TIME_DIMENSION)))  continue;
      pagetime = ltmp;
      setup_viewbuf();
      continue;
    }
    if(!strcmp(cmd_str, "VIEWTIME?"))
    {
      if(!files_open)
      {
        len = snprintf(response_str, 512, "0\n");
        rc_host_sock->write(response_str, len);
        continue;
      }
#ifdef Q_OS_WIN32
      len = _mingw_snprintf(response_str, 512, "%lli.%lli\n", edfheaderlist[sel_viewtime]->viewtime / TIME_DIMENSION, edfheaderlist[sel_viewtime]->viewtime % TIME_DIMENSION);
#else
      len = snprintf(response_str, 512, "%lli.%lli\n", edfheaderlist[sel_viewtime]->viewtime / TIME_DIMENSION, edfheaderlist[sel_viewtime]->viewtime % TIME_DIMENSION);
#endif
      rc_host_sock->write(response_str, len);
      continue;
    }
    if((!strncmp(cmd_str, "VIEWTIME ", 9)) && (strlen(cmd_str) > 9))
    {
      if(is_number(cmd_str + 9))  continue;
      if(!files_open)  continue;
      ltmp = atoll_x(cmd_str + 9, TIME_DIMENSION);
      if((ltmp <= (-30LL * TIME_DIMENSION)) || (ltmp >= (3600LL * 24LL * 7LL * TIME_DIMENSION)))  continue;
      set_viewtime(ltmp);
      continue;
    }
  }
}





















