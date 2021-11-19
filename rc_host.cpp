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
  int n, len, n_sub_cmds, err;

  char rx_msg_str[512]="",
       tx_msg_str[512]="",
       cmd_args[512]="",
       cmds_parsed[CMD_MAX_SUB_CMDS][CMD_PARSE_STR_LEN]={"","","","","","","",""};

  for(n=1; n>0; )
  {
    n = rc_host_sock->readLine(rx_msg_str, 511);
    if(n < 1)  break;

    if(rx_msg_str[n-1] == '\n')
    {
      if(--n < 1)  break;
      rx_msg_str[n] = 0;
    }

    if(rx_msg_str[n-1] == '\r')
    {
      if(--n < 1)  break;
      rx_msg_str[n] = 0;
    }

    n_sub_cmds = parse_rc_command(rx_msg_str, cmds_parsed, cmd_args, 512);

/*****************************************************************************/
//     int i;
//     printf("n_sub_cmds: %i\n", n_sub_cmds);
//     for(i=0; i<CMD_MAX_SUB_CMDS; i++)
//     {
//       if(cmds_parsed[i][0] == 0)  break;
//
//       printf("sub command %i: ->%s<-\n", i, cmds_parsed[i]);
//     }
//     printf("cmd_args: ->%s<-\n", cmd_args);
/*****************************************************************************/

    if(n_sub_cmds < 1)
    {
      continue;
    }

    if((n_sub_cmds == 1) && !strlen(cmd_args) && !strcmp(cmds_parsed[0], "LIST"))
    {
      len = snprintf(tx_msg_str, 512,
              "*IDN?\n"
              "QUIT\n"
              "FILE:OPEN <path>\n"
              "FILE:CLOSE:ALL\n"
              "MONTAGE:LOAD <path>\n"
              "SIGNAL:ADD:LABEL <label>\n"
              "SIGNAL:AMPLITUDE:ALL <units>\n"
              "SIGNAL:AMPLITUDE:LABEL <label> <units>\n"
              "SIGNAL:AMPLITUDE:FIT:ALL\n"
              "SIGNAL:AMPLITUDE:FIT:LABEL <label>\n"
              "SIGNAL:OFFSET:ADJUST:ALL\n"
              "SIGNAL:OFFSET:ADJUST:LABEL <label>\n"
              "SIGNAL:OFFSET:ZERO:ALL\n"
              "SIGNAL:OFFSET:ZERO:LABEL <label>\n"
              "SIGNAL:REMOVE:LABEL <label>\n"
              "SIGNAL:REMOVE:ALL\n"
              "TIMESCALE?\n"
              "TIMESCALE <seconds>\n"
              "VIEWTIME?\n"
              "VIEWTIME <seconds>\n");

      rc_host_sock->write(tx_msg_str, len);
      continue;
    }

    if((n_sub_cmds == 1) && !strlen(cmd_args) && !strcmp(cmds_parsed[0], "*IDN?"))
    {
      len = snprintf(tx_msg_str, 512, PROGRAM_NAME "," PROGRAM_VERSION "," THIS_APP_BITS_W "\n");
      rc_host_sock->write(tx_msg_str, len);
      continue;
    }

    if((n_sub_cmds == 1) && !strlen(cmd_args) && !strcmp(cmds_parsed[0], "QUIT"))
    {
      exit_program();
      continue;
    }

    if(!strcmp(cmds_parsed[0], "FILE"))
    {
      err = process_rc_cmd_file(cmds_parsed, cmd_args, n_sub_cmds);
      continue;
    }

    if(!strcmp(cmds_parsed[0], "MONTAGE"))
    {
      err = process_rc_cmd_montage(cmds_parsed, cmd_args, n_sub_cmds);
      continue;
    }

    if(!strcmp(cmds_parsed[0], "SIGNAL"))
    {
      err = process_rc_cmd_signal(cmds_parsed, cmd_args, n_sub_cmds);
      if(err)
      {
        printf("process_rc_cmd_signal() returns: %i\n", err);
      }
      continue;
    }

    if(!strcmp(cmds_parsed[0], "TIMESCALE") || !strcmp(cmds_parsed[0], "TIMESCALE?"))
    {
      err = process_rc_cmd_timescale(cmds_parsed, cmd_args, n_sub_cmds);
      continue;
    }

    if(!strcmp(cmds_parsed[0], "VIEWTIME") || !strcmp(cmds_parsed[0], "VIEWTIME?"))
    {
      err = process_rc_cmd_viewtime(cmds_parsed, cmd_args, n_sub_cmds);
      continue;
    }
  }
}


int UI_Mainwindow::parse_rc_command(const char *cmd_str, char cmd_parsed_str[CMD_MAX_SUB_CMDS][CMD_PARSE_STR_LEN], char *arg_n_str, int arg_n_len)
{
  int i, j, k,
      last_char_colon=0;

  cmd_parsed_str[0][0] = 0;

  arg_n_str[0] = 0;

  for(j=0; j<CMD_MAX_SUB_CMDS; j++)
  {
    cmd_parsed_str[j][0] = 0;
  }

  for(i=0, j=0, k=0; i<511; i++)
  {
    if(j >= CMD_MAX_SUB_CMDS)
    {
      return -2;
    }

    if(k >= (CMD_PARSE_STR_LEN - 1))
    {
      cmd_parsed_str[j][CMD_PARSE_STR_LEN - 1] = 0;

      return -3;
    }

    cmd_parsed_str[j][k] = cmd_str[i];

    if(cmd_parsed_str[j][k] == 0)
    {
      if(!k)
      {
        return -4;
      }
      else
      {
        if(cmd_parsed_str[j][k-1] == ':')
        {
          return -5;
        }
      }

      ascii_toupper(cmd_parsed_str[j++]);

      return j;
    }

    if(cmd_parsed_str[j][k] == ':')
    {
      if(last_char_colon || !k)
      {
        cmd_parsed_str[j][0] = 0;

        return -6;
      }

      last_char_colon++;

      cmd_parsed_str[j][k] = 0;

      ascii_toupper(cmd_parsed_str[j++]);

      if(j >= CMD_MAX_SUB_CMDS)
      {
        return -5;
      }

      k = 0;

      continue;
    }
    else
    {
      last_char_colon = 0;
    }

    if(cmd_parsed_str[j][k] == ' ')
    {
      cmd_parsed_str[j][k] = 0;

      if(!k)
      {
        return -6;
      }

      if(cmd_parsed_str[j][k-1] == ':')
      {
        return -7;
      }

      ascii_toupper(cmd_parsed_str[j++]);

      strlcpy(arg_n_str, cmd_str + i + 1, arg_n_len);

      return j;
    }

    k++;
  }

  return -1;
}


int UI_Mainwindow::process_rc_cmd_file(const char cmds_parsed[CMD_MAX_SUB_CMDS][CMD_PARSE_STR_LEN], const char *cmd_args, int n_sub_cmds)
{
  if(n_sub_cmds < 2)
  {
    return -1;
  }

  if(!strcmp(cmds_parsed[1], "CLOSE"))
  {
    if((n_sub_cmds == 3) && !strcmp(cmds_parsed[2], "ALL") && !strlen(cmd_args))
    {
      close_all_files();
      return 0;
    }
    else
    {
      return -2;
    }
  }
  else if(!strcmp(cmds_parsed[1], "OPEN"))
    {
      if((n_sub_cmds == 2) && strlen(cmd_args))
      {
        if(files_open >= MAXFILES)  return 0;
        if(annot_editor_active && files_open)  return 0;
        if((files_open > 0) && (live_stream_active))  return 0;

        strlcpy(drop_path, cmd_args, MAX_PATH_LENGTH);
        rc_file_open_requested = 1;
        open_new_file();
        return 0;
      }
      else
      {
        return -3;
      }
    }
    else
    {
      return -4;
    }

  return 0;
}


int UI_Mainwindow::process_rc_cmd_montage(const char cmds_parsed[CMD_MAX_SUB_CMDS][CMD_PARSE_STR_LEN], const char *cmd_args, int n_sub_cmds)
{
  if(n_sub_cmds < 2)
  {
    return -1;
  }

  if((n_sub_cmds == 2) && !strcmp(cmds_parsed[1], "LOAD") && strlen(cmd_args))
  {
    if(!files_open)  return 0;
    if(signalcomps >= MAXSIGNALS)  return 0;
    strlcpy(montagepath, cmd_args, MAX_PATH_LENGTH);
    UI_LoadMontagewindow load_mtg(this, montagepath);
    montagepath[0] = 0;
    return 0;
  }
  else
  {
    return -2;
  }

  return 0;
}


int UI_Mainwindow::process_rc_cmd_signal(const char cmds_parsed[CMD_MAX_SUB_CMDS][CMD_PARSE_STR_LEN], const char *cmd_args, int n_sub_cmds)
{
  int i, j, n, len;

  char str1[512]="",
       str2[512]="",
       *ptr=NULL;

  double value2=100,
         original_value=100;

  struct signalcompblock *newsignalcomp=NULL;

  if(n_sub_cmds < 3)
  {
    return -1;
  }

  if(!strcmp(cmds_parsed[1], "ADD"))
  {
    if((n_sub_cmds == 3) && !strcmp(cmds_parsed[2], "LABEL") && strlen(cmd_args))
    {
      if(!files_open)  return 0;
      if(signalcomps >= MAXSIGNALS)  return 0;
      strlcpy(str1, cmd_args, 512);
      trim_spaces(str1);

      for(i=0; i<edfheaderlist[0]->edfsignals; i++)
      {
        strlcpy(str2, edfheaderlist[0]->edfparam[i].label, 512);
        if((!strncmp(str2, "EDF Annotations ", 16)) || (!strncmp(str2, "BDF Annotations ", 16)))
        {
          continue;
        }

        trim_spaces(str2);
        if(!strcmp(str2, str1))
        {
          newsignalcomp = (struct signalcompblock *)calloc(1, sizeof(struct signalcompblock));
          if(newsignalcomp==NULL)  return 0;
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

      return 0;
    }
    else
    {
      return -2;
    }
  }

  if(!strcmp(cmds_parsed[1], "REMOVE"))
  {
    if((n_sub_cmds == 3) && !strcmp(cmds_parsed[2], "ALL") && !strlen(cmd_args))
    {
      remove_all_signals();
      return 0;
    }
    else if((n_sub_cmds == 3) && !strcmp(cmds_parsed[2], "LABEL") && strlen(cmd_args))
      {
        i = get_signalcomp_number(cmd_args);
        if(i >= 0)
        {
          remove_signalcomp(i);
          setup_viewbuf();
          return 0;
        }
        else
        {
          return -4;
        }
      }
      else
      {
        return -5;
      }
  }

  if(!strcmp(cmds_parsed[1], "AMPLITUDE"))
  {
    if((n_sub_cmds == 3) && !strcmp(cmds_parsed[2], "ALL") && strlen(cmd_args))
    {
      if(!signalcomps)  return 0;

      if(is_number(cmd_args))  return -6;

      value2 = atof(cmd_args);

      if((value2 > 1000000.001) || (value2 < 0.0000000999))  return -7;

      for(i=0; i<signalcomps; i++)
      {
        if(signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[0]].bitvalue < 0.0)
        {
          value2 *= -1.0;
        }

        for(j=0; j<signalcomp[i]->num_of_signals; j++)
        {
          signalcomp[i]->sensitivity[j] = (signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[j]].bitvalue / value2) / y_pixelsizefactor;
        }

        original_value = signalcomp[i]->voltpercm;

        signalcomp[i]->voltpercm = value2;

        signalcomp[i]->screen_offset *= (original_value / value2);
      }

      maincurve->drawCurve_stage_1();

      return 0;
    }

    if((n_sub_cmds == 3) && !strcmp(cmds_parsed[2], "LABEL") && (strlen(cmd_args) > 2))
    {
      strlcpy(str1, cmd_args, 512);

      len = strlen(str1);

      for(i=0, ptr=NULL; i<len; i++)
      {
        if(str1[i] == ' ')
        {
          ptr = &str1[i];
        }
      }

      if(ptr == NULL)  return -8;

      *ptr = 0;

      ptr++;

      if(!strlen(ptr))  return -9;

      if(is_number(ptr))  return -10;

      if(!strlen(str1))  return -11;

      value2 = atof(ptr);
      if((value2 > 1000000.001) || (value2 < 0.0000000999))  return -12;

      n = get_signalcomp_number(str1);
      if(n < 0)  return 0;

      if(signalcomp[n]->edfhdr->edfparam[signalcomp[n]->edfsignal[0]].bitvalue < 0.0)
      {
        value2 *= -1.0;
      }

      for(j=0; j<signalcomp[n]->num_of_signals; j++)
      {
        signalcomp[n]->sensitivity[j] = (signalcomp[n]->edfhdr->edfparam[signalcomp[n]->edfsignal[j]].bitvalue / value2) / y_pixelsizefactor;
      }

      original_value = signalcomp[n]->voltpercm;

      signalcomp[n]->voltpercm = value2;

      signalcomp[n]->screen_offset *= (original_value / value2);

      maincurve->drawCurve_stage_1();

      return 0;
    }

    if((n_sub_cmds == 4) && !strcmp(cmds_parsed[2], "FIT") && !strcmp(cmds_parsed[3], "ALL") && !strlen(cmd_args))
    {
      fit_signals_to_pane();
      return 0;
    }

    if((n_sub_cmds == 4) && !strcmp(cmds_parsed[2], "FIT") && !strcmp(cmds_parsed[3], "LABEL") && strlen(cmd_args))
    {
      n = get_signalcomp_number(cmd_args);
      if(n >= 0)
      {
        fit_signals_to_pane(n);
        return 0;
      }
      else
      {
        return -13;
      }
    }
  }

  if(!strcmp(cmds_parsed[1], "OFFSET"))
  {
    if((n_sub_cmds == 4) && !strcmp(cmds_parsed[2], "ADJUST") && !strcmp(cmds_parsed[3], "ALL") && !strlen(cmd_args))
    {
      fit_signals_dc_offset();
      return 0;
    }

    if((n_sub_cmds == 4) && !strcmp(cmds_parsed[2], "ADJUST") && !strcmp(cmds_parsed[3], "LABEL") && strlen(cmd_args))
    {
      n = get_signalcomp_number(cmd_args);
      if(n >= 0)
      {
        fit_signals_dc_offset(n);
        return 0;
      }
      else
      {
        return -14;
      }
    }

    if((n_sub_cmds == 4) && !strcmp(cmds_parsed[2], "ZERO") && !strcmp(cmds_parsed[3], "ALL") && !strlen(cmd_args))
    {
      set_dc_offset_to_zero();
      return 0;
    }

    if((n_sub_cmds == 4) && !strcmp(cmds_parsed[2], "ZERO") && !strcmp(cmds_parsed[3], "LABEL") && strlen(cmd_args))
    {
      n = get_signalcomp_number(cmd_args);
      if(n >= 0)
      {
        set_dc_offset_to_zero(n);
        return 0;
      }
      else
      {
        return -15;
      }
    }
  }

  if(!strcmp(cmds_parsed[1], "INVERT"))
  {
    if((n_sub_cmds == 3) && !strcmp(cmds_parsed[2], "ALL") && strlen(cmd_args))
    {
      if(is_integer_number(cmd_args))  return -16;

      n = atoi(cmd_args);
      if((n < 0) || (n > 1))  return -17;

      //FIXME  TODO

      return 0;
    }
    else
    {
      return -18;
    }
  }


  return 0;
}


int UI_Mainwindow::process_rc_cmd_timescale(const char cmds_parsed[CMD_MAX_SUB_CMDS][CMD_PARSE_STR_LEN], const char *cmd_args, int n_sub_cmds)
{
  int len;

  long long ltmp;

  char tx_msg_str[512]="";

  if(n_sub_cmds < 1)
  {
    return -1;
  }

  if((n_sub_cmds == 1) && !strcmp(cmds_parsed[0], "TIMESCALE?") && !strlen(cmd_args))
  {
#ifdef Q_OS_WIN32
    len = _mingw_snprintf(tx_msg_str, 512, "%llu.%llu\n", pagetime / TIME_DIMENSION, pagetime % TIME_DIMENSION);
#else
    len = snprintf(tx_msg_str, 512, "%llu.%llu\n", pagetime / TIME_DIMENSION, pagetime % TIME_DIMENSION);
#endif
    rc_host_sock->write(tx_msg_str, len);
    return 0;
  }
  else if((n_sub_cmds == 1) && !strcmp(cmds_parsed[0], "TIMESCALE") && strlen(cmd_args))
    {
      if(is_number(cmd_args))  return 0;
      ltmp = atoll_x(cmd_args, TIME_DIMENSION);
      if((ltmp <= 100LL) || (ltmp >= (3600LL * TIME_DIMENSION)))  return 0;
      pagetime = ltmp;
      setup_viewbuf();
        return 0;
    }
    else
    {
      return -2;
    }

  return 0;
}


int UI_Mainwindow::process_rc_cmd_viewtime(const char cmds_parsed[CMD_MAX_SUB_CMDS][CMD_PARSE_STR_LEN], const char *cmd_args, int n_sub_cmds)
{
  int len;

  long long ltmp;

  char tx_msg_str[512]="";

  if(n_sub_cmds < 1)
  {
    return -1;
  }

  if((n_sub_cmds == 1) && !strcmp(cmds_parsed[0], "VIEWTIME?") && !strlen(cmd_args))
  {
    if(!files_open)
    {
      len = snprintf(tx_msg_str, 512, "0\n");
      rc_host_sock->write(tx_msg_str, len);
      return 0;
    }
#ifdef Q_OS_WIN32
    len = _mingw_snprintf(tx_msg_str, 512, "%lli.%lli\n", edfheaderlist[sel_viewtime]->viewtime / TIME_DIMENSION, edfheaderlist[sel_viewtime]->viewtime % TIME_DIMENSION);
#else
    len = snprintf(tx_msg_str, 512, "%lli.%lli\n", edfheaderlist[sel_viewtime]->viewtime / TIME_DIMENSION, edfheaderlist[sel_viewtime]->viewtime % TIME_DIMENSION);
#endif
    rc_host_sock->write(tx_msg_str, len);
    return 0;
  }
  else if((n_sub_cmds == 1) && !strcmp(cmds_parsed[0], "VIEWTIME") && strlen(cmd_args))
    {
      if(is_number(cmd_args))  return 0;
      if(!files_open)  return 0;
      ltmp = atoll_x(cmd_args, TIME_DIMENSION);
      if((ltmp <= (-30LL * TIME_DIMENSION)) || (ltmp >= (3600LL * 24LL * 7LL * TIME_DIMENSION)))  return 0;
      set_viewtime(ltmp);
      return 0;
    }
    else
    {
      return -2;
    }

  return 0;
}




















