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


#include "mainwindow.h"


UI_Mainwindow::~UI_Mainwindow()
{
  delete splash_pixmap;
  delete splash;
  delete startup_timer;
  delete myfont;
  delete monofont;
  delete maincurve;
  delete annotationEditDock;
  for(int i=0; i<MAXSPECTRUMDOCKS; i++)
  {
    delete spectrumdock[i];
  }
  delete live_stream_timer;
  delete video_poll_timer;
  delete playback_realtime_time;
  delete playback_realtime_timer;
  delete dig_min_max_overflow_timer;
  delete update_checker;
  free(toolbar_stats.ival);
  delete rc_host_server;
}


void UI_Mainwindow::exit_program()
{
  close();
}


void UI_Mainwindow::closeEvent(QCloseEvent *cl_event)
{
  int i, button_nr=0;

  if(annotations_edited)
  {
    QMessageBox messagewindow;
    messagewindow.setText("There are unsaved annotations,\n are you sure you want to quit?");
    messagewindow.setIcon(QMessageBox::Question);
    messagewindow.setStandardButtons(QMessageBox::Cancel | QMessageBox::Discard);
    messagewindow.setDefaultButton(QMessageBox::Cancel);
    button_nr = messagewindow.exec();
  }

  if(button_nr == QMessageBox::Cancel)
  {
    cl_event->ignore();

    return;
  }

  exit_in_progress = 1;

  for(i=0; i<MAXSPECTRUMDIALOGS; i++)
  {
    if(spectrumdialog[i] != NULL)
    {
      delete spectrumdialog[i];

      spectrumdialog[i] = NULL;
    }
  }

  for(i=0; i<MAXCDSADOCKS; i++)
  {
    if(cdsa_dock[i] != NULL)
    {
      delete cdsa_dock[i];

      cdsa_dock[i] = NULL;
    }
  }

  for(i=0; i<MAXAVERAGECURVEDIALOGS; i++)
  {
    if(averagecurvedialog[i] != NULL)
    {
      delete averagecurvedialog[i];

      averagecurvedialog[i] = NULL;
    }
  }

  for(i=0; i<MAXZSCOREDIALOGS; i++)
  {
    if(zscoredialog[i] != NULL)
    {
      delete zscoredialog[i];

      zscoredialog[i] = NULL;
    }
  }

  annotations_edited = 0;

  close_all_files();

  window_width_sav_rest = width();
  window_height_sav_rest = height();

  write_settings();

  free(spectrum_colorbar);
  free(zoomhistory);
  free(import_annotations_var);
  free(export_annotations_var);
  free(video_player);
  free(annot_filter);

  cl_event->accept();
}


// void UI_Mainwindow::search_pattern()
// {
//   if(!signalcomps)
//   {
//     return;
//   }
//
//
//
// }


void UI_Mainwindow::Escape_fun()
{
  int i;

  for(i=0; i<signalcomps; i++)
  {
    signalcomp[i]->hascursor1 = 0;
    signalcomp[i]->hascursor2 = 0;
    signalcomp[i]->hasoffsettracking = 0;
  }
  maincurve->crosshair_1.active = 0;
  maincurve->crosshair_2.active = 0;
  maincurve->crosshair_1.moving = 0;
  maincurve->crosshair_2.moving = 0;
  maincurve->use_move_events = 0;
  maincurve->setMouseTracking(false);

  for(i=0; i<signalcomps; i++)
  {
    signalcomp[i]->hasruler = 0;
  }
  maincurve->ruler_active = 0;
  maincurve->ruler_moving = 0;

  maincurve->update();
}


void UI_Mainwindow::open_stream()
{
  if(files_open)
  {
    QMessageBox::critical(this, "Error", "Close all files before opening a stream.");
    return;
  }

  live_stream_active = 1;

  open_new_file();

  if(files_open == 1)
  {
    toolsmenu->setEnabled(false);
    timemenu->setEnabled(false);
//    windowmenu->setEnabled(false);
    windowmenu->actions().at(0)->setEnabled(false);  // Annotations window
    windowmenu->actions().at(1)->setEnabled(false);  // Annotation editor
    former_page_Act->setEnabled(false);
    shift_page_left_Act->setEnabled(false);
    shift_page_right_Act->setEnabled(false);
    next_page_Act->setEnabled(false);
//     shift_page_up_Act->setEnabled(false);
//     shift_page_down_Act->setEnabled(false);
    printmenu->setEnabled(false);
    recent_filesmenu->setEnabled(false);
    recent_session_menu->setEnabled(false);
    playback_file_Act->setEnabled(false);

    live_stream_timer->start(live_stream_update_interval);
  }
  else
  {
    live_stream_active = 0;
  }
}


void UI_Mainwindow::live_stream_timer_func()
{
  long long datarecords_old,
            datarecords_new;


  if((!live_stream_active) || (files_open != 1))
  {
    return;
  }

  if(!signalcomps)
  {
    live_stream_timer->start(live_stream_update_interval);

    return;
  }

  datarecords_old = edfheaderlist[0]->datarecords;

  datarecords_new = check_edf_file_datarecords(edfheaderlist[0]);

  if((datarecords_new > datarecords_old) && (datarecords_new > 0))
  {
    jump_to_end();
  }

  live_stream_timer->start(live_stream_update_interval);
}


long long UI_Mainwindow::check_edf_file_datarecords(struct edfhdrblock *hdr)
{
  long long datarecords;


  if(fseeko(hdr->file_hdl, 0LL, SEEK_END) == -1LL)
  {
    hdr->datarecords = 0LL;

    return 0LL;
  }

  datarecords = ftello(hdr->file_hdl);

  if(datarecords < 1LL)
  {
    hdr->datarecords = 0LL;

    return 0LL;
  }

  datarecords -= (long long)hdr->hdrsize;

  datarecords /= (long long)hdr->recordsize;

  if(datarecords < 1LL)
  {
    hdr->datarecords = 0LL;

    return 0LL;
  }

  hdr->datarecords = datarecords;

  return datarecords;
}


void UI_Mainwindow::save_file()
{
  int len;

  char f_path[MAX_PATH_LENGTH];

  struct edfhdrblock *hdr;

  FILE *outputfile;


  if((!annotations_edited)||(!files_open))
  {
    save_act->setEnabled(false);

    return;
  }

  hdr = edfheaderlist[0];

  strlcpy(f_path, recent_savedir, MAX_PATH_LENGTH);
  strlcat(f_path, "/", MAX_PATH_LENGTH);
  len = strlen(f_path);
  get_filename_from_path(f_path + len, hdr->filename, MAX_PATH_LENGTH - len);
  remove_extension_from_filename(f_path);
  if(hdr->edf)
  {
    strlcat(f_path, "_edited.edf", MAX_PATH_LENGTH);
  }
  else
  {
    strlcat(f_path, "_edited.bdf", MAX_PATH_LENGTH);
  }

  strlcpy(f_path, QFileDialog::getSaveFileName(this, "Save file", QString::fromLocal8Bit(f_path), "EDF/BDF files (*.edf *.EDF *.bdf *.BDF *.rec *.REC)").toLocal8Bit().data(), MAX_PATH_LENGTH);

  if(!strcmp(f_path, ""))
  {
    return;
  }

  get_directory_from_path(recent_savedir, f_path, MAX_PATH_LENGTH);

  if(file_is_opened(f_path))
  {
    QMessageBox::critical(this, "Error", "Selected file is in use.");
    return;
  }

  outputfile = fopeno(f_path, "wb");
  if(outputfile==NULL)
  {
    QMessageBox::critical(this, "Error", "Cannot create a file for writing.");
    return;
  }

  if(save_annotations(this, outputfile, hdr))
  {
    QMessageBox::critical(this, "Error", "An error occurred during saving.");
    fclose(outputfile);
    return;
  }

  fclose(outputfile);

  edfplus_annotation_empty_list(&hdr->annot_list);

  if(annotationlist_backup != NULL)
  {
    hdr->annot_list = *annotationlist_backup;

    free(annotationlist_backup);

    annotationlist_backup = NULL;
  }

  annotations_dock[0]->updateList(0);

  annotations_edited = 0;

  save_act->setEnabled(false);

  delete annotationEditDock;
  annotationEditDock = NULL;

  maincurve->update();
}


void UI_Mainwindow::save_session()
{
  int i, j, k, hdr_idx=0, sigcomp_idx=0, use_index=0, present=0, position=0, button_nr=0;

  QAction *act=NULL;

  if(!files_open)
  {
    save_session_act->setEnabled(false);
    return;
  }

  char session_path[MAX_PATH_LENGTH]="",
       path_relative[MAX_PATH_LENGTH]="";

  FILE *pro_file=NULL;

  if(annotations_edited)
  {
    QMessageBox messagewindow;
    messagewindow.setText("There are unsaved annotations which will not be stored in the session.\n"
                          "It's strongly recommended you save them first.");
    messagewindow.setIcon(QMessageBox::Question);
    messagewindow.setStandardButtons(QMessageBox::Cancel | QMessageBox::Discard);
    messagewindow.setDefaultButton(QMessageBox::Cancel);
    button_nr = messagewindow.exec();
  }

  if(button_nr == QMessageBox::Cancel)
  {
    return;
  }

  strlcpy(session_path, recent_sessiondir, MAX_PATH_LENGTH);
  strlcat(session_path, "/my_session.esf", MAX_PATH_LENGTH);

  strlcpy(session_path, QFileDialog::getSaveFileName(0, "Save session", QString::fromLocal8Bit(session_path), "Session files (*.esf *.ESF)").toLocal8Bit().data(), MAX_PATH_LENGTH);

  if(!strcmp(session_path, ""))
  {
    return;
  }

  if(strlen(session_path) > 4)
  {
    if(strcmp(session_path + strlen(session_path) - 4, ".esf"))
    {
      strlcat(session_path, ".esf", MAX_PATH_LENGTH);
    }
  }

  get_directory_from_path(recent_sessiondir, session_path, MAX_PATH_LENGTH);

  pro_file = fopeno(session_path, "wb");
  if(pro_file==NULL)
  {
    QMessageBox::critical(this, "Error", "Cannot create session file for writing.");
    return;
  }

  fprintf(pro_file, "<?xml version=\"1.0\"?>\n<" PROGRAM_NAME "_session>\n");

  fprintf(pro_file, "  <relative_path>%i</relative_path>\n", session_relative_paths);

  fprintf(pro_file, "  <edf_files>\n");
  for(i=0; i<files_open; i++)
  {
    fprintf(pro_file, "    <file>");
    if(session_relative_paths)
    {
      get_relative_path_from_absolut_paths(path_relative, session_path, edfheaderlist[i]->filename, MAX_PATH_LENGTH);
      xml_fwrite_encode_entity(pro_file, path_relative);
//      printf("src1: ->%s<-\nsrc2: ->%s<-\ndest: ->%s<-\n", session_path, edfheaderlist[i]->filename, path_relative);  //FIXME
    }
    else
    {
      xml_fwrite_encode_entity(pro_file, edfheaderlist[i]->filename);
    }
    fprintf(pro_file, "</file>\n");
  }
  for(i=0; i<files_open; i++)
  {
#ifdef Q_OS_WIN32
    __mingw_fprintf(pro_file, "    <viewtime>%lli</viewtime>\n", edfheaderlist[i]->viewtime);
#else
    fprintf(pro_file, "    <viewtime>%lli</viewtime>\n", edfheaderlist[i]->viewtime);
#endif
  }
  fprintf(pro_file, "  </edf_files>\n");

  fprintf(pro_file, "  <ref_file>%i</ref_file>\n", sel_viewtime);

  fprintf(pro_file, "  <timesync_mode>%i</timesync_mode>\n", viewtime_sync);

#ifdef Q_OS_WIN32
  __mingw_fprintf(pro_file, "  <pagetime>%lli</pagetime>\n", pagetime);
#else
  fprintf(pro_file, "  <pagetime>%lli</pagetime>\n", pagetime);
#endif

  for(i=0; i<signalcomps; i++)
  {
    fprintf(pro_file, "  <signalcomposition>\n");

    fprintf(pro_file, "    <file>");

    if(session_relative_paths)
    {
      get_relative_path_from_absolut_paths(path_relative, session_path, signalcomp[i]->edfhdr->filename, MAX_PATH_LENGTH);
      xml_fwrite_encode_entity(pro_file, path_relative);
//      printf("src1: ->%s<-\nsrc2: ->%s<-\ndest: ->%s<-\n", session_path, signalcomp[i]->edfhdr->filename, path_relative);  //FIXME
    }
    else
    {
      xml_fwrite_encode_entity(pro_file, signalcomp[i]->edfhdr->filename);
    }

    fprintf(pro_file, "</file>\n");

    fprintf(pro_file, "    <alias>");

    xml_fwrite_encode_entity(pro_file, signalcomp[i]->alias);

    fprintf(pro_file, "</alias>\n");

    fprintf(pro_file, "    <num_of_signals>%i</num_of_signals>\n", signalcomp[i]->num_of_signals);

    fprintf(pro_file, "    <voltpercm>%f</voltpercm>\n", signalcomp[i]->voltpercm);

    fprintf(pro_file, "    <screen_offset>%f</screen_offset>\n", signalcomp[i]->screen_offset);

    fprintf(pro_file, "    <polarity>%i</polarity>\n", signalcomp[i]->polarity);

    fprintf(pro_file, "    <color>%i</color>\n", signalcomp[i]->color);

    if(signalcomp[i]->spike_filter)
    {
      fprintf(pro_file, "    <spike_filter_cnt>%i</spike_filter_cnt>\n", 1);
    }
    else
    {
      fprintf(pro_file, "    <spike_filter_cnt>%i</spike_filter_cnt>\n", 0);
    }

    fprintf(pro_file, "    <filter_cnt>%i</filter_cnt>\n", signalcomp[i]->filter_cnt);

    fprintf(pro_file, "    <fidfilter_cnt>%i</fidfilter_cnt>\n", signalcomp[i]->fidfilter_cnt);

    fprintf(pro_file, "    <ravg_filter_cnt>%i</ravg_filter_cnt>\n", signalcomp[i]->ravg_filter_cnt);

    for(j=0; j<signalcomp[i]->num_of_signals; j++)
    {
      fprintf(pro_file, "    <signal>\n");

      if(use_index)
      {
        fprintf(pro_file, "      <edfindex>%i</edfindex>\n", signalcomp[i]->edfsignal[j]);
      }
      else
      {
        fprintf(pro_file, "      <label>");

        xml_fwrite_encode_entity(pro_file, signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[j]].label);

        fprintf(pro_file, "</label>\n");
      }

      fprintf(pro_file, "      <factor>%e</factor>\n", signalcomp[i]->factor[j]);

      fprintf(pro_file, "    </signal>\n");
    }

    if(signalcomp[i]->spike_filter)
    {
      fprintf(pro_file, "    <spike_filter>\n");

      fprintf(pro_file, "      <velocity>%.16f</velocity>\n", signalcomp[i]->spike_filter_velocity);

      fprintf(pro_file, "      <holdoff>%i</holdoff>\n", signalcomp[i]->spike_filter_holdoff);

      fprintf(pro_file, "    </spike_filter>\n");
    }

    for(j=0; j<signalcomp[i]->filter_cnt; j++)
    {
      fprintf(pro_file, "    <filter>\n");

      fprintf(pro_file, "      <LPF>%i</LPF>\n", signalcomp[i]->filter[j]->is_LPF);

      fprintf(pro_file, "      <frequency>%.16f</frequency>\n", signalcomp[i]->filter[j]->cutoff_frequency);

      fprintf(pro_file, "    </filter>\n");
    }

    for(j=0; j<signalcomp[i]->ravg_filter_cnt; j++)
    {
      fprintf(pro_file, "    <ravg_filter>\n");

      fprintf(pro_file, "      <type>%i</type>\n", signalcomp[i]->ravg_filter[j]->type);

      fprintf(pro_file, "      <size>%i</size>\n", signalcomp[i]->ravg_filter[j]->size);

      fprintf(pro_file, "    </ravg_filter>\n");
    }

    for(j=0; j<signalcomp[i]->fidfilter_cnt; j++)
    {
      fprintf(pro_file, "    <fidfilter>\n");

      fprintf(pro_file, "      <type>%i</type>\n", signalcomp[i]->fidfilter_type[j]);

      fprintf(pro_file, "      <frequency>%.16f</frequency>\n", signalcomp[i]->fidfilter_freq[j]);

      fprintf(pro_file, "      <frequency2>%.16f</frequency2>\n", signalcomp[i]->fidfilter_freq2[j]);

      fprintf(pro_file, "      <ripple>%.16f</ripple>\n", signalcomp[i]->fidfilter_ripple[j]);

      fprintf(pro_file, "      <order>%i</order>\n", signalcomp[i]->fidfilter_order[j]);

      fprintf(pro_file, "      <model>%i</model>\n", signalcomp[i]->fidfilter_model[j]);

      fprintf(pro_file, "    </fidfilter>\n");
    }

    if(signalcomp[i]->fir_filter != NULL)
    {
      k = fir_filter_size(signalcomp[i]->fir_filter);

      fprintf(pro_file, "    <fir_filter>\n");

      fprintf(pro_file, "      <size>%i</size>\n", k);

      for(j=0; j<k; j++)
      {
        fprintf(pro_file, "      <tap>%.20f</tap>\n", fir_filter_tap(j, signalcomp[i]->fir_filter));
      }

      fprintf(pro_file, "    </fir_filter>\n");
    }

    if(signalcomp[i]->plif_ecg_filter != NULL)
    {
      fprintf(pro_file, "    <plif_ecg_filter>\n");

      fprintf(pro_file, "      <plf>%i</plf>\n", signalcomp[i]->plif_ecg_subtract_filter_plf);

      fprintf(pro_file, "    </plif_ecg_filter>\n");
    }

    if(signalcomp[i]->ecg_filter != NULL)
    {
      fprintf(pro_file, "    <ecg_filter>\n");

      fprintf(pro_file, "      <type>1</type>\n");

      fprintf(pro_file, "    </ecg_filter>\n");
    }

    if(signalcomp[i]->zratio_filter != NULL)
    {
      fprintf(pro_file, "    <zratio_filter>\n");

      fprintf(pro_file, "      <type>1</type>\n");

      fprintf(pro_file, "      <crossoverfreq>%f</crossoverfreq>\n", signalcomp[i]->zratio_crossoverfreq);

      fprintf(pro_file, "    </zratio_filter>\n");
    }

    if(signalcomp[i]->hasruler)
    {
      fprintf(pro_file, "    <floating_ruler>\n");

      fprintf(pro_file, "      <hasruler>1</hasruler>\n");

      fprintf(pro_file, "      <ruler_x_position>%i</ruler_x_position>\n", maincurve->ruler_x_position);

      fprintf(pro_file, "      <ruler_y_position>%i</ruler_y_position>\n", maincurve->ruler_y_position);

      fprintf(pro_file, "      <floating_ruler_value>%i</floating_ruler_value>\n", maincurve->float_ruler_more);

      fprintf(pro_file, "    </floating_ruler>\n");
    }

    fprintf(pro_file, "  </signalcomposition>\n");
  }

  if((video_player->status == VIDEO_STATUS_PLAYING) || (video_player->status == VIDEO_STATUS_PAUSED))
  {
    fprintf(pro_file, "  <video_file>");
    if(session_relative_paths)
    {
      get_relative_path_from_absolut_paths(path_relative, session_path, videopath, MAX_PATH_LENGTH);
      xml_fwrite_encode_entity(pro_file, path_relative);
//      printf("src1: ->%s<-\nsrc2: ->%s<-\ndest: ->%s<-\n", session_path, videopath, path_relative);  //FIXME
    }
    else
    {
      xml_fwrite_encode_entity(pro_file, videopath);
    }
    fprintf(pro_file, "</video_file>\n");

    fprintf(pro_file, "  <video_seek>%i</video_seek>\n", video_player->fpos);
  }

  struct spectrumdocksettings settings;

  for(i=0; i<MAXSPECTRUMDOCKS; i++)
  {
    if(spectrumdock[i]->dock->isVisible())
    {
      spectrumdock[i]->getsettings(&settings);

      if(settings.signalnr >= 0)
      {
        fprintf(pro_file, "  <powerspectrumdock>\n");

        fprintf(pro_file, "    <signalnum>%i</signalnum>\n", settings.signalnr);

        fprintf(pro_file, "    <amp>%i</amp>\n", settings.amp);

        fprintf(pro_file, "    <log_min_sl>%i</log_min_sl>\n", settings.log_min_sl);

        fprintf(pro_file, "    <wheel>%i</wheel>\n", settings.wheel);

        fprintf(pro_file, "    <span>%i</span>\n", settings.span);

        fprintf(pro_file, "    <center>%i</center>\n", settings.center);

        fprintf(pro_file, "    <log>%i</log>\n", settings.log);

        fprintf(pro_file, "    <sqrt>%i</sqrt>\n", settings.sqrt);

        fprintf(pro_file, "    <colorbar>%i</colorbar>\n", settings.colorbar);

        fprintf(pro_file, "    <maxvalue>%.10e</maxvalue>\n", settings.maxvalue);

        fprintf(pro_file, "    <maxvalue_sqrt>%.10e</maxvalue_sqrt>\n", settings.maxvalue_sqrt);

        fprintf(pro_file, "    <maxvalue_vlog>%.10e</maxvalue_vlog>\n", settings.maxvalue_vlog);

        fprintf(pro_file, "    <maxvalue_sqrt_vlog>%.10e</maxvalue_sqrt_vlog>\n", settings.maxvalue_sqrt_vlog);

        fprintf(pro_file, "    <minvalue_vlog>%.10e</minvalue_vlog>\n", settings.minvalue_vlog);

        fprintf(pro_file, "    <minvalue_sqrt_vlog>%.10e</minvalue_sqrt_vlog>\n", settings.minvalue_sqrt_vlog);

        fprintf(pro_file, "  </powerspectrumdock>\n");
      }
    }
  }

  for(i=0; i<MAXHYPNOGRAMDOCKS; i++)
  {
    if(hypnogram_dock[i] == NULL)  continue;

    for(hdr_idx=0; hdr_idx<files_open; hdr_idx++)
    {
      if(edfheaderlist[hdr_idx] == hypnogram_dock[i]->param.edfhdr)
      {
        break;
      }
    }
    if(hdr_idx == files_open)  continue;

    fprintf(pro_file, "  <hypnogram>\n");

    fprintf(pro_file, "    <instance_num>%i</instance_num>\n", hypnogram_dock[i]->param.instance_num);

    for(j=0; j<HYPNOGRAM_STAGENUM; j++)
    {
      fprintf(pro_file, "    <stage_name>");
      xml_fwrite_encode_entity(pro_file, hypnogram_dock[i]->param.stage_name[j]);
      fprintf(pro_file, "</stage_name>\n");
    }

    for(j=0; j<HYPNOGRAM_STAGENUM; j++)
    {
      fprintf(pro_file, "    <annot_name>");
      xml_fwrite_encode_entity(pro_file, hypnogram_dock[i]->param.annot_name[j]);
      fprintf(pro_file, "</annot_name>\n");
    }

    fprintf(pro_file, "    <hdr_idx>%i</hdr_idx>\n", hdr_idx);

    fprintf(pro_file, "  </hypnogram>\n");
  }

  for(i=0; i<MAXCDSADOCKS; i++)
  {
    if(cdsa_dock[i] == NULL)  continue;

    for(sigcomp_idx=0; sigcomp_idx<signalcomps; sigcomp_idx++)
    {
      if(signalcomp[sigcomp_idx] == cdsa_dock[i]->param.signalcomp)
      {
        break;
      }
    }
    if(sigcomp_idx == signalcomps)  continue;

    fprintf(pro_file, "  <cdsa>\n");

    fprintf(pro_file, "    <sigcomp_idx>%i</sigcomp_idx>\n", sigcomp_idx);

    fprintf(pro_file, "    <min_hz>%i</min_hz>\n", cdsa_dock[i]->param.min_hz);

    fprintf(pro_file, "    <max_hz>%i</max_hz>\n", cdsa_dock[i]->param.max_hz);

    fprintf(pro_file, "    <segment_len>%i</segment_len>\n", cdsa_dock[i]->param.segment_len);

    fprintf(pro_file, "    <block_len>%i</block_len>\n", cdsa_dock[i]->param.block_len);

    fprintf(pro_file, "    <overlap>%i</overlap>\n", cdsa_dock[i]->param.overlap);

    fprintf(pro_file, "    <window_func>%i</window_func>\n", cdsa_dock[i]->param.window_func);

    fprintf(pro_file, "    <max_voltage>%.10e</max_voltage>\n", cdsa_dock[i]->param.max_voltage);

    fprintf(pro_file, "    <max_pwr>%i</max_pwr>\n", cdsa_dock[i]->param.max_pwr);

    fprintf(pro_file, "    <min_pwr>%i</min_pwr>\n", cdsa_dock[i]->param.min_pwr);

    fprintf(pro_file, "    <log>%i</log>\n", cdsa_dock[i]->param.log);

    fprintf(pro_file, "    <power_voltage>%i</power_voltage>\n", cdsa_dock[i]->param.power_voltage);

    fprintf(pro_file, "    <instance_num>%i</instance_num>\n", cdsa_dock[i]->param.instance_num);

    fprintf(pro_file, "  </cdsa>\n");
  }

  fprintf(pro_file, "</" PROGRAM_NAME "_session>\n");

  fclose(pro_file);

  for(i=0, present=0; i<MAX_RECENTFILES; i++)
  {
//    printf("mainwindow.cpp: load_session(): recent_session_path[i]: ->%s<-\nsession_path: ->%s<-\n", recent_session_path[i], session_path);  //FIXME

    if(!strcmp(&recent_session_path[i][0], session_path))
    {
      present = 1;

      position = i;

      break;
    }
  }

  if(present)
  {
    for(i=position; i>0; i--)
    {
      strlcpy(&recent_session_path[i][0], &recent_session_path[i-1][0], MAX_PATH_LENGTH);
    }
  }
  else
  {
    for(i=MAX_RECENTFILES-1; i>0; i--)
    {
      strlcpy(&recent_session_path[i][0], &recent_session_path[i-1][0], MAX_PATH_LENGTH);
    }
  }

  strlcpy(&recent_session_path[0][0], session_path, MAX_PATH_LENGTH);

  recent_session_menu->clear();

  for(i=0; i<MAX_RECENTFILES; i++)
  {
    if(recent_session_path[i][0] == 0)
    {
      break;
    }
    act = new QAction(QString::fromLocal8Bit(&recent_session_path[i][0]), recent_session_menu);
    act->setData(QVariant(i));
    recent_session_menu->addAction(act);
  }
  recent_session_menu->addSeparator();
  act = new QAction("Clear list", recent_session_menu);
  act->setData(QVariant(MAX_RECENTFILES));
  recent_session_menu->addAction(act);
}


void UI_Mainwindow::load_session()
{
  int i, button_nr=0, err, present=0, position=0;

  char session_path[MAX_PATH_LENGTH]="";

  QAction *act=NULL;

  if(annotations_edited)
  {
    QMessageBox messagewindow;
    messagewindow.setText("There are unsaved annotations,\n are you sure you want to close this file?");
    messagewindow.setIcon(QMessageBox::Question);
    messagewindow.setStandardButtons(QMessageBox::Cancel | QMessageBox::Discard);
    messagewindow.setDefaultButton(QMessageBox::Cancel);
    button_nr = messagewindow.exec();
  }
  else if(files_open)
    {
      QMessageBox messagewindow;
      messagewindow.setText("This will close all files, continue?");
      messagewindow.setIcon(QMessageBox::Question);
      messagewindow.setStandardButtons(QMessageBox::Cancel | QMessageBox::Close);
      messagewindow.setDefaultButton(QMessageBox::Cancel);
      button_nr = messagewindow.exec();
    }

  if(button_nr == QMessageBox::Cancel)
  {
    return;
  }

  annotations_edited = 0;

  session_start_video = 0;

  session_video_seek = 0;

  close_all_files();

  strlcpy(session_path, QFileDialog::getOpenFileName(0, "Load session", QString::fromLocal8Bit(recent_sessiondir), "Session files (*.esf *.ESF)").toLocal8Bit().data(), MAX_PATH_LENGTH);

  if(!strcmp(session_path, ""))
  {
    return;
  }

  get_directory_from_path(recent_sessiondir, session_path, MAX_PATH_LENGTH);

  for(i=0, present=0; i<MAX_RECENTFILES; i++)
  {
//    printf("mainwindow.cpp: load_session(): recent_session_path[i]: ->%s<-\nsession_path: ->%s<-\n", recent_session_path[i], session_path);  //FIXME

    if(!strcmp(&recent_session_path[i][0], session_path))
    {
      present = 1;

      position = i;

      break;
    }
  }

  if(present)
  {
    for(i=position; i>0; i--)
    {
      strlcpy(&recent_session_path[i][0], &recent_session_path[i-1][0], MAX_PATH_LENGTH);
    }
  }
  else
  {
    for(i=MAX_RECENTFILES-1; i>0; i--)
    {
      strlcpy(&recent_session_path[i][0], &recent_session_path[i-1][0], MAX_PATH_LENGTH);
    }
  }

  strlcpy(&recent_session_path[0][0], session_path, MAX_PATH_LENGTH);

  recent_session_menu->clear();

  for(i=0; i<MAX_RECENTFILES; i++)
  {
    if(recent_session_path[i][0] == 0)
    {
      break;
    }
    act = new QAction(QString::fromLocal8Bit(&recent_session_path[i][0]), recent_session_menu);
    act->setData(QVariant(i));
    recent_session_menu->addAction(act);
  }
  recent_session_menu->addSeparator();
  act = new QAction("Clear list", recent_session_menu);
  act->setData(QVariant(MAX_RECENTFILES));
  recent_session_menu->addAction(act);

  err = read_session_file(session_path);

  if(err)
  {
    printf("load_session() returned error: %i\n", err);
  }
}


void UI_Mainwindow::slider_moved(int value)
{
  long long new_viewtime, tmp;

  if(!signalcomps)  return;

  new_viewtime = edfheaderlist[sel_viewtime]->long_data_record_duration * edfheaderlist[sel_viewtime]->datarecords;

  new_viewtime -= pagetime;

  if(new_viewtime<0)
  {
    new_viewtime = 0;
  }
  else
  {
    new_viewtime /= 1000000;

    new_viewtime *= value;
  }

  if(pagetime >= (3600LL * TIME_DIMENSION))
  {
    tmp = new_viewtime % (360LL * TIME_DIMENSION);

    new_viewtime -= tmp;
  }
  else if(pagetime >= (1200LL * TIME_DIMENSION))
    {
      tmp = new_viewtime % (120LL * TIME_DIMENSION);

      new_viewtime -= tmp;
    }
    else if(pagetime >= (300LL * TIME_DIMENSION))
      {
        tmp = new_viewtime % (30LL * TIME_DIMENSION);

        new_viewtime -= tmp;
      }
      else if(pagetime >= (60LL * TIME_DIMENSION))
        {
          tmp = new_viewtime % (6LL * TIME_DIMENSION);

          new_viewtime -= tmp;
        }
        else if(pagetime >= (30LL * TIME_DIMENSION))
          {
            tmp = new_viewtime % (3LL * TIME_DIMENSION);

            new_viewtime -= tmp;
          }
          else if(pagetime >= (20LL * TIME_DIMENSION))
            {
              tmp = new_viewtime % (2LL * TIME_DIMENSION);

              new_viewtime -= tmp;
            }
            else if(pagetime >= (10LL * TIME_DIMENSION))
              {
                tmp = new_viewtime % TIME_DIMENSION;

                new_viewtime -= tmp;
              }
              else if(pagetime >= TIME_DIMENSION)
                {
                  tmp = new_viewtime % (TIME_DIMENSION / 10LL);

                  new_viewtime -= tmp;
                }
                else if(pagetime >= (TIME_DIMENSION / 10LL))
                  {
                    tmp = new_viewtime % (TIME_DIMENSION / 100LL);

                    new_viewtime -= tmp;
                  }
                  else if(pagetime >= (TIME_DIMENSION / 100LL))
                    {
                      tmp = new_viewtime % (TIME_DIMENSION / 1000LL);

                      new_viewtime -= tmp;
                    }

  if(video_player->status == VIDEO_STATUS_PLAYING)
  {
    video_player_seek((int)((new_viewtime + (pagetime / 2LL)) / TIME_DIMENSION));

    return;
  }

  if(video_player->status == VIDEO_STATUS_PAUSED)
  {
    video_player_seek((int)((new_viewtime + (pagetime / 2LL)) / TIME_DIMENSION));
  }

  set_viewtime(new_viewtime);
}


void UI_Mainwindow::set_timesync_reference(QAction *action)
{
  int i;

  for(i=0; i<files_open; i++)
  {
    if(!strcmp(edfheaderlist[i]->filename, action->text().toLocal8Bit().data()))
    {
      break;
    }
  }

  sel_viewtime = i;

  setMainwindowTitle(edfheaderlist[sel_viewtime]);

  setup_viewbuf();
}


void UI_Mainwindow::set_timesync_reference(int ref)
{
  if((ref < 0) || (ref >= files_open))
  {
    return;
  }

  sel_viewtime = ref;

  setMainwindowTitle(edfheaderlist[sel_viewtime]);

  setup_viewbuf();
}


void UI_Mainwindow::set_timesync(QAction *)
{
  int i;

  if(no_timesync_act->isChecked())
  {
    viewtime_sync = VIEWTIME_UNSYNCED;
  }

  if(offset_timesync_act->isChecked())
  {
    viewtime_sync = VIEWTIME_SYNCED_OFFSET;

    for(i=0; i<files_open; i++)
    {
      edfheaderlist[i]->viewtime = edfheaderlist[sel_viewtime]->viewtime;
    }
  }

  if(absolut_timesync_act->isChecked())
  {
    viewtime_sync = VIEWTIME_SYNCED_ABSOLUT;

    for(i=0; i<files_open; i++)
    {
      if(i!=sel_viewtime)
      {
        edfheaderlist[i]->viewtime = edfheaderlist[sel_viewtime]->viewtime + edfheaderlist[sel_viewtime]->starttime_offset - ((edfheaderlist[i]->utc_starttime - edfheaderlist[sel_viewtime]->utc_starttime) * TIME_DIMENSION) - edfheaderlist[i]->starttime_offset;
      }
    }
  }

  if(user_def_sync_act->isChecked())
  {
    viewtime_sync = VIEWTIME_USER_DEF_SYNCED;
  }

  setup_viewbuf();
}


void UI_Mainwindow::set_timesync(int mode)
{
  int i;

  if(mode == VIEWTIME_UNSYNCED)
  {
    viewtime_sync = VIEWTIME_UNSYNCED;
  }
  else if(mode == VIEWTIME_SYNCED_OFFSET)
    {
      viewtime_sync = VIEWTIME_SYNCED_OFFSET;

      for(i=0; i<files_open; i++)
      {
        edfheaderlist[i]->viewtime = edfheaderlist[sel_viewtime]->viewtime;
      }
    }
    else if(mode == VIEWTIME_SYNCED_ABSOLUT)
      {
        viewtime_sync = VIEWTIME_SYNCED_ABSOLUT;

        for(i=0; i<files_open; i++)
        {
          if(i!=sel_viewtime)
          {
            edfheaderlist[i]->viewtime = edfheaderlist[sel_viewtime]->viewtime + edfheaderlist[sel_viewtime]->starttime_offset - ((edfheaderlist[i]->utc_starttime - edfheaderlist[sel_viewtime]->utc_starttime) * TIME_DIMENSION) - edfheaderlist[i]->starttime_offset;
          }
        }
      }
      else if(mode == VIEWTIME_USER_DEF_SYNCED)
        {
          viewtime_sync = VIEWTIME_USER_DEF_SYNCED;
        }

  switch(viewtime_sync)
  {
    case VIEWTIME_SYNCED_OFFSET :   offset_timesync_act->setChecked(true);
                                    break;
    case VIEWTIME_SYNCED_ABSOLUT :  absolut_timesync_act->setChecked(true);
                                    break;
    case VIEWTIME_UNSYNCED :        no_timesync_act->setChecked(true);
                                    break;
    case VIEWTIME_USER_DEF_SYNCED : user_def_sync_act->setChecked(true);
                                    break;
    default                       : break;
  }

  setup_viewbuf();
}


void UI_Mainwindow::sync_by_crosshairs()
{
  if(files_open<2)  return;
  if(signalcomps<2)  return;

  if(maincurve->crosshair_1.active&&maincurve->crosshair_2.active)
  {
    if(maincurve->crosshair_1.edf_hdr != maincurve->crosshair_2.edf_hdr)
    {
      viewtime_sync = VIEWTIME_USER_DEF_SYNCED;

      maincurve->crosshair_2.edf_hdr->viewtime
      -= ((maincurve->crosshair_1.time - maincurve->crosshair_1.edf_hdr->l_starttime - maincurve->crosshair_1.edf_hdr->viewtime - maincurve->crosshair_1.edf_hdr->starttime_offset)
      - (maincurve->crosshair_2.time - maincurve->crosshair_2.edf_hdr->l_starttime - maincurve->crosshair_2.edf_hdr->viewtime - maincurve->crosshair_2.edf_hdr->starttime_offset));

      maincurve->crosshair_2.x_position = maincurve->crosshair_1.x_position;

      user_def_sync_act->setChecked(true);

      viewtime_sync = VIEWTIME_USER_DEF_SYNCED;

      setup_viewbuf();
    }
  }
}


void UI_Mainwindow::show_options_dialog()
{
  UI_OptionsDialog OptionsDialog(this);
}


void UI_Mainwindow::nk2edf_converter()
{
  UI_NK2EDFwindow nk2edf(this, recent_opendir);
}


void UI_Mainwindow::convert_ascii_to_edf()
{
  UI_ASCII2EDFapp ascii2edf(this, recent_opendir, recent_savedir);
}


void UI_Mainwindow::convert_binary_to_edf()
{
  UI_RAW2EDFapp bin2edf(this, &raw2edf_var, recent_opendir, recent_savedir);
}


void UI_Mainwindow::convert_fino_to_edf()
{
  UI_FINO2EDFwindow fino2edf(this, recent_opendir, recent_savedir);
}


void UI_Mainwindow::convert_wave_to_edf()
{
  UI_WAV2EDFwindow wav2edf(this, recent_opendir, recent_savedir);
}


void UI_Mainwindow::convert_fm_audio_to_edf()
{
  UI_FMaudio2EDFwindow fma2edf(this, recent_opendir, recent_savedir);
}


void UI_Mainwindow::convert_mortara_to_edf()
{
  UI_MortaraEDFwindow mortara2edf(this, recent_opendir, recent_savedir);
}


void UI_Mainwindow::convert_ishne_to_edf()
{
  UI_IshneEDFwindow ishne2edf(this, recent_opendir, recent_savedir);
}


void UI_Mainwindow::convert_nexfin_to_edf()
{
  UI_NEXFIN2EDFwindow nexfin2edf(this, recent_opendir, recent_savedir);
}


void UI_Mainwindow::convert_emsa_to_edf()
{
  UI_EMSA2EDFwindow emsa2edf(this, recent_opendir, recent_savedir);
}


void UI_Mainwindow::convert_manscan_to_edf()
{
  UI_MANSCAN2EDFwindow manscan2edf(this, recent_opendir, recent_savedir);
}


void UI_Mainwindow::convert_mit_to_edf()
{
  UI_MIT2EDFwindow mit2edf(this, recent_opendir, recent_savedir);
}


void UI_Mainwindow::convert_biox_to_edf()
{
  UI_BIOX2EDFwindow biox2edf(this, recent_opendir, recent_savedir);
}


void UI_Mainwindow::convert_scpecg_to_edf()
{
  UI_SCPECG2EDFwindow scpecg2edf(this, recent_opendir, recent_savedir);
}


void UI_Mainwindow::edfd_converter()
{
  UI_EDFDwindow edfplusd2edfplusc(this, recent_opendir, recent_savedir);
}


void UI_Mainwindow::bdf2edf_converter()
{
  UI_BDF2EDFwindow bdf2edfconv(this);
}


void UI_Mainwindow::biosemi2bdfplus_converter()
{
  UI_BIOSEMI2BDFPLUSwindow biosemi2bdfplusconv(this);
}


void UI_Mainwindow::reduce_signals()
{
  if(!files_open)
  {
    QMessageBox::critical(this, "Error", "You have to open the file first.");
    return;
  }

  UI_ReduceSignalsWindow reduceSignals(this);
}


void UI_Mainwindow::edit_header()
{
  UI_headerEditorWindow header_edit(this);
}


void UI_Mainwindow::unisens2edf_converter()
{
  UI_UNISENS2EDFwindow unisens2edfconv(this, recent_opendir, recent_savedir);
}


void UI_Mainwindow::BI98002edf_converter()
{
  UI_BI98002EDFwindow BI98002edfconv(this, recent_opendir, recent_savedir);
}


void UI_Mainwindow::edit_predefined_montages()
{
  UI_edit_predefined_mtg_window edit_predef_mtgs_app(this);
}


void UI_Mainwindow::jump_to_dialog()
{
  if(!files_open)  return;

  UI_JumpMenuDialog jumpmenu(this);
}


void UI_Mainwindow::jump_to_start()
{
  if(!files_open)  return;

  if(video_player->status == VIDEO_STATUS_PLAYING)
  {
    video_player_seek((int)((pagetime / 2LL) / TIME_DIMENSION));

    return;
  }

  if(video_player->status == VIDEO_STATUS_PAUSED)
  {
    video_player_seek((int)((pagetime / 2LL) / TIME_DIMENSION));
  }

  set_viewtime(0LL);
}


void UI_Mainwindow::jump_to_time_millisec(long long milliseconds)
{
  if(!files_open)  return;

  set_viewtime(milliseconds * (TIME_DIMENSION / 1000));
}


void UI_Mainwindow::jump_to_end()
{
  long long new_viewtime;

  if(!files_open)  return;

  new_viewtime = edfheaderlist[sel_viewtime]->datarecords * edfheaderlist[sel_viewtime]->long_data_record_duration - pagetime;

  set_viewtime(new_viewtime);
}


void UI_Mainwindow::set_viewtime(long long new_viewtime)
{
  int i;

  if(!files_open)  return;

  if(viewtime_sync==VIEWTIME_SYNCED_OFFSET)
  {
    for(i=0; i<files_open; i++)
    {
      edfheaderlist[i]->viewtime = new_viewtime;
    }
  }

  if(viewtime_sync==VIEWTIME_UNSYNCED)
  {
    edfheaderlist[sel_viewtime]->viewtime = new_viewtime;
  }

  if(viewtime_sync==VIEWTIME_SYNCED_ABSOLUT)
  {
    edfheaderlist[sel_viewtime]->viewtime = new_viewtime;

    for(i=0; i<files_open; i++)
    {
      if(i!=sel_viewtime)
      {
        edfheaderlist[i]->viewtime = edfheaderlist[sel_viewtime]->viewtime - ((edfheaderlist[i]->utc_starttime - edfheaderlist[sel_viewtime]->utc_starttime) * TIME_DIMENSION);
      }
    }
  }

  if(viewtime_sync==VIEWTIME_USER_DEF_SYNCED)
  {
    for(i=0; i<files_open; i++)
    {
      if(i!=sel_viewtime)
      {
        edfheaderlist[i]->viewtime -= (edfheaderlist[sel_viewtime]->viewtime - new_viewtime);
      }
    }

    edfheaderlist[sel_viewtime]->viewtime = new_viewtime;
  }

  setup_viewbuf();
}


void UI_Mainwindow::add_new_filter()
{
  if(!files_open)  return;

  UI_FilterDialog filterdialog(this);
}


void UI_Mainwindow::add_plif_ecg_filter()
{
  if(!files_open)  return;

  UI_PLIF_ECG_filter_dialog plifecgfilterdialog(this);
}


void UI_Mainwindow::add_fir_filter()
{
  if(!files_open)  return;

  UI_FIRFilterDialog firfilterdialog(recent_opendir, recent_savedir, this);
}


void UI_Mainwindow::add_spike_filter()
{
  if(!files_open)  return;

  UI_SpikeFilterDialog spikefilterdialog(this);
}


// void UI_Mainwindow::add_new_math_func()
// {
// }


// void UI_Mainwindow::remove_all_math_funcs()
// {
// }


void UI_Mainwindow::zoomback()
{
  int i;

  if(!files_open)  return;

  if(!zoomhistory->history_size_tail)
  {
    return;
  }
  zoomhistory->history_size_head++;
  zoomhistory->history_size_tail--;

  for(i=0; i<files_open; i++)
  {
    zoomhistory->viewtime[zoomhistory->idx][i] = edfheaderlist[i]->viewtime;
  }
  zoomhistory->pagetime[zoomhistory->idx] = pagetime;
  for(i=0; i<signalcomps; i++)
  {
    zoomhistory->voltpercm[zoomhistory->idx][i] = signalcomp[i]->voltpercm;
    zoomhistory->screen_offset[zoomhistory->idx][i] = signalcomp[i]->screen_offset;
    zoomhistory->sensitivity[zoomhistory->idx][i] = signalcomp[i]->sensitivity;
  }

  zoomhistory->idx--;
  zoomhistory->idx += MAXZOOMHISTORY;
  zoomhistory->idx %= MAXZOOMHISTORY;

  for(i=0; i<files_open; i++)
  {
    edfheaderlist[i]->viewtime = zoomhistory->viewtime[zoomhistory->idx][i];
  }
  pagetime = zoomhistory->pagetime[zoomhistory->idx];

  for(i=0; i<signalcomps; i++)
  {
    signalcomp[i]->voltpercm = zoomhistory->voltpercm[zoomhistory->idx][i];
    signalcomp[i]->screen_offset = zoomhistory->screen_offset[zoomhistory->idx][i];
    signalcomp[i]->sensitivity = zoomhistory->sensitivity[zoomhistory->idx][i];
  }

  setup_viewbuf();
}


void UI_Mainwindow::forward()
{
  int i;

  if(!files_open)  return;

  if(!zoomhistory->history_size_head)
  {
    return;
  }
  zoomhistory->history_size_head--;
  zoomhistory->history_size_tail++;

  zoomhistory->idx++;
  zoomhistory->idx %= MAXZOOMHISTORY;

  for(i=0; i<files_open; i++)
  {
    edfheaderlist[i]->viewtime = zoomhistory->viewtime[zoomhistory->idx][i];
  }
  pagetime = zoomhistory->pagetime[zoomhistory->idx];

  for(i=0; i<signalcomps; i++)
  {
    signalcomp[i]->voltpercm = zoomhistory->voltpercm[zoomhistory->idx][i];
    signalcomp[i]->screen_offset = zoomhistory->screen_offset[zoomhistory->idx][i];
    signalcomp[i]->sensitivity = zoomhistory->sensitivity[zoomhistory->idx][i];
  }

  setup_viewbuf();
}


void UI_Mainwindow::former_page()
{
  int i;

  if(!files_open)  return;

  if(video_player->status == VIDEO_STATUS_PLAYING)
  {
    video_player_seek((int)((edfheaderlist[sel_viewtime]->viewtime - (pagetime / 2LL)) / TIME_DIMENSION));

    return;
  }

  if(video_player->status == VIDEO_STATUS_PAUSED)
  {
    video_player_seek((int)((edfheaderlist[sel_viewtime]->viewtime - (pagetime / 2LL)) / TIME_DIMENSION));
  }

  if((viewtime_sync==VIEWTIME_SYNCED_OFFSET)||(viewtime_sync==VIEWTIME_SYNCED_ABSOLUT)||(viewtime_sync==VIEWTIME_USER_DEF_SYNCED))
  {
    for(i=0; i<files_open; i++)
    {
      edfheaderlist[i]->viewtime -= pagetime;
    }
  }

  if(viewtime_sync==VIEWTIME_UNSYNCED)
  {
    edfheaderlist[sel_viewtime]->viewtime -= pagetime;
  }

  setup_viewbuf();
}


void UI_Mainwindow::shift_page_left()
{
  int i;

  if(!files_open)  return;

  if(video_player->status == VIDEO_STATUS_PLAYING)
  {
    video_player_seek((int)((edfheaderlist[sel_viewtime]->viewtime + (pagetime / 2LL) - (pagetime / 10)) / TIME_DIMENSION));

    return;
  }

  if(video_player->status == VIDEO_STATUS_PAUSED)
  {
    video_player_seek((int)((edfheaderlist[sel_viewtime]->viewtime + (pagetime / 2LL) - (pagetime / 10)) / TIME_DIMENSION));
  }

  if((viewtime_sync==VIEWTIME_SYNCED_OFFSET)||(viewtime_sync==VIEWTIME_SYNCED_ABSOLUT)||(viewtime_sync==VIEWTIME_USER_DEF_SYNCED))
  {
    for(i=0; i<files_open; i++)
    {
      edfheaderlist[i]->viewtime -= (pagetime / 10);
    }
  }

  if(viewtime_sync==VIEWTIME_UNSYNCED)
  {
    edfheaderlist[sel_viewtime]->viewtime -= (pagetime / 10);
  }

  setup_viewbuf();
}


void UI_Mainwindow::shift_page_right()
{
  int i;

  if(!files_open)  return;

  if(video_player->status == VIDEO_STATUS_PLAYING)
  {
    video_player_seek((int)((edfheaderlist[sel_viewtime]->viewtime + (pagetime / 2LL) + (pagetime / 10)) / TIME_DIMENSION));

    return;
  }

  if(video_player->status == VIDEO_STATUS_PAUSED)
  {
    video_player_seek((int)((edfheaderlist[sel_viewtime]->viewtime + (pagetime / 2LL) + (pagetime / 10)) / TIME_DIMENSION));
  }

  if((viewtime_sync==VIEWTIME_SYNCED_OFFSET)||(viewtime_sync==VIEWTIME_SYNCED_ABSOLUT)||(viewtime_sync==VIEWTIME_USER_DEF_SYNCED))
  {
    for(i=0; i<files_open; i++)
    {
      edfheaderlist[i]->viewtime += (pagetime / 10);
    }
  }

  if(viewtime_sync==VIEWTIME_UNSYNCED)
  {
    edfheaderlist[sel_viewtime]->viewtime += (pagetime / 10);
  }

  setup_viewbuf();
}


void UI_Mainwindow::stop_playback()
{
  if(playback_realtime_active)
  {
    playback_realtime_timer->stop();

    playback_realtime_active = 0;
  }
  else
  {
    if((video_player->status == VIDEO_STATUS_PLAYING) || (video_player->status == VIDEO_STATUS_PAUSED))
    {
      start_stop_video();
    }
  }
}


void UI_Mainwindow::playback_file()
{
  if(!signalcomps)
  {
    return;
  }

  if(live_stream_active)
  {
    return;
  }

  if(playback_realtime_active)
  {
    playback_realtime_timer->stop();

    playback_realtime_active = 0;

    return;
  }

  if((video_player->status == VIDEO_STATUS_PLAYING) || (video_player->status == VIDEO_STATUS_PAUSED))
  {
    video_player_toggle_pause();

    return;
  }

  playback_realtime_time->start();

  playback_realtime_timer->start();

  playback_realtime_active = 1;
}


void UI_Mainwindow::playback_realtime_timer_func()
{
  int i;

  long long elapsed_time;

  if(viewtime_sync==VIEWTIME_UNSYNCED)
  {
    edfheaderlist[sel_viewtime]->viewtime += (playback_realtime_time->restart() * 10000);
  }
  else if((viewtime_sync==VIEWTIME_SYNCED_OFFSET) ||
      (viewtime_sync==VIEWTIME_SYNCED_ABSOLUT) ||
      (viewtime_sync==VIEWTIME_USER_DEF_SYNCED))
      {
        elapsed_time = playback_realtime_time->restart() * 10000LL;

        for(i=0; i<files_open; i++)
        {
          edfheaderlist[i]->viewtime += elapsed_time;
        }

        playback_realtime_time->restart();
      }

  setup_viewbuf();
}


void UI_Mainwindow::next_page()
{
  int i;

  if(!files_open)  return;

  if(video_player->status == VIDEO_STATUS_PLAYING)
  {
    video_player_seek((int)((edfheaderlist[sel_viewtime]->viewtime + pagetime + (pagetime / 2LL)) / TIME_DIMENSION));

    return;
  }

  if(video_player->status == VIDEO_STATUS_PAUSED)
  {
    video_player_seek((int)((edfheaderlist[sel_viewtime]->viewtime + pagetime + (pagetime / 2LL)) / TIME_DIMENSION));
  }

  if((viewtime_sync==VIEWTIME_SYNCED_OFFSET)||(viewtime_sync==VIEWTIME_SYNCED_ABSOLUT)||(viewtime_sync==VIEWTIME_USER_DEF_SYNCED))
  {
    for(i=0; i<files_open; i++)
    {
      edfheaderlist[i]->viewtime += pagetime;
    }
  }

  if(viewtime_sync==VIEWTIME_UNSYNCED)
  {
    edfheaderlist[sel_viewtime]->viewtime += pagetime;
  }

  setup_viewbuf();
}


void UI_Mainwindow::shift_page_up()
{
  int i;

  if(!files_open)  return;

  for(i=0; i<signalcomps; i++)
  {
    signalcomp[i]->screen_offset += (height() / 20.0);

  }

  maincurve->drawCurve_stage_1();
}


void UI_Mainwindow::shift_page_down()
{
  int i;

  if(!files_open)  return;

  for(i=0; i<signalcomps; i++)
  {
    signalcomp[i]->screen_offset -= (height() / 20.0);

  }

  maincurve->drawCurve_stage_1();
}


void UI_Mainwindow::show_annotations()
{
  int i;

  EDF_annotations annotations;

  if(!files_open)  return;

  for(i=0; i<files_open; i++)
  {
    if(edfheaderlist[i]->annots_not_read)
    {
      if((edfheaderlist[i]->edfplus) || (edfheaderlist[i]->bdfplus))
      {
        edfheaderlist[i]->annots_not_read = 0;

        annotations.get_annotations(edfheaderlist[i], read_nk_trigger_signal);

        if(edfheaderlist[i]->annots_not_read)
        {
          edfplus_annotation_empty_list(&edfheaderlist[i]->annot_list);
        }
        else
        {
          if(annotations_dock[i] == NULL)
          {
            annotations_dock[i] = new UI_Annotationswindow(edfheaderlist[i], this);

            addDockWidget(Qt::RightDockWidgetArea, annotations_dock[i]->docklist, Qt::Vertical);
          }
        }
      }
    }

    if(edfplus_annotation_size(&edfheaderlist[i]->annot_list) > 0)
    {
      if(annotations_dock[i] != NULL)
      {
        annotations_dock[i]->docklist->show();
      }
    }
  }
}


void UI_Mainwindow::annotation_editor()
{
//  stop_video_generic(0);

//  stop_playback();

  if(playback_realtime_active)
  {
    playback_realtime_timer->stop();

    playback_realtime_active = 0;
  }

  if(files_open!=1)
  {
    QMessageBox::critical(this, "Error", "Editing annotations is possible when you have opened one file only.");
    return;
  }

  if(edfheaderlist[0]->annots_not_read)
  {
    edfplus_annotation_empty_list(&edfheaderlist[0]->annot_list);

    if(annotations_dock[0] != NULL)
    {
      annotations_dock[0]->docklist->close();
      delete annotations_dock[0];
      annotations_dock[0] = NULL;
    }

    edfheaderlist[0]->annots_not_read = 0;

    EDF_annotations annotations;

    annotations.get_annotations(edfheaderlist[0], read_nk_trigger_signal);

    if(edfheaderlist[0]->annots_not_read)
    {
      QMessageBox::critical(this, "Error", "Editing annotations is not possible when you abort the scanning of the file.");
      return;
    }
  }

  if(annotations_dock[0]==NULL)
  {
    annotations_dock[0] = new UI_Annotationswindow(edfheaderlist[0], this);

    addDockWidget(Qt::RightDockWidgetArea, annotations_dock[0]->docklist, Qt::Vertical);
  }

  maincurve->active_markers->edf_hdr = edfheaderlist[0];

  annotations_dock[0]->docklist->show();

  if(annotationEditDock == NULL)
  {
//    printf("annotationEditDock = new    file: %s  line: %i\n", __FILE__, __LINE__);

    annotationEditDock = new UI_AnnotationEditwindow(edfheaderlist[0], this);

    addToolBar(Qt::BottomToolBarArea, annotationEditDock->dockedit);

    insertToolBarBreak(annotationEditDock->dockedit);
  }
  else
  {
//    printf("annotationEditDock pointer re-used!    file: %s  line: %i\n", __FILE__, __LINE__);

    annotationEditDock->set_edf_header(edfheaderlist[0]);

    annotationEditDock->dockedit->show();
  }
}


void UI_Mainwindow::show_spectrum_dock()
{
  int i;

  if((!files_open) || (!signalcomps))  return;

  if(signalcomps == 1)
  {
    for(i=0; i<MAXSPECTRUMDOCKS; i++)
    {
      if(spectrumdock[i]->dock->isHidden())  break;
    }

    if(i<MAXSPECTRUMDOCKS)
    {
      spectrumdock[i]->init(0);
    }
    else
    {
      QMessageBox::critical(this, "Error", "The maximum number of docked Power Spectrum windows has been reached.\n"
                                           "Close one first.");
    }

    return;
  }

  UI_SignalChooser signalchooserdialog(this, 1);
}


void UI_Mainwindow::show_cdsa_dock()
{
  int i;

  if((!files_open) || (!signalcomps) || live_stream_active)  return;

  if(signalcomps == 1)
  {
    if(signalcomp[0]->ecg_filter != NULL)
    {
      return;
    }

    if(signalcomp[0]->edfhdr->edfparam[signalcomp[0]->edfsignal[0]].sf_int < 30)
    {
      QMessageBox::critical(this, "Error", "Samplefrequency must be at least 30Hz and must be an integer value.");
      return;
    }

    if(signalcomp[0]->edfhdr->recording_len_sec < 30)
    {
      QMessageBox::critical(this, "Error", "Recording length must be at least 30 seconds.");
      return;
    }

    for(i=0; i<MAXCDSADOCKS; i++)
    {
      if(cdsa_dock[i] == NULL)
      {
        UI_cdsa_window wndw(this, signalcomp[0], i);
        break;
      }
    }

    return;
  }

  UI_SignalChooser signalchooserdialog(this, 5);
}


void UI_Mainwindow::show_hypnogram()
{
  int i, file_num=0;

  if((!files_open) || live_stream_active)  return;

  for(i=0; i<MAXHYPNOGRAMDOCKS; i++)
  {
    if(hypnogram_dock[i] == NULL)
    {
      if(files_open > 1)
      {
        UI_activeFileChooserWindow choose_file_dialog(&file_num, this);

        if(file_num >= 0)
        {
          UI_hypnogram_window hypnogram_dialog(this, edfheaderlist[file_num], i);
        }
      }
      else
      {
        UI_hypnogram_window hypnogram_dialog(this, edfheaderlist[0], i);
      }

      break;
    }
  }
}


void UI_Mainwindow::open_new_file()
{
  FILE *newfile=NULL;

  int i, len, present, position, button_nr=0;

  char str[2048]="";

  QAction *act=NULL;

  struct edfhdrblock *edfhdr=NULL;

  rc_file_open_err = 0;

  if(edflib_version() != 121)  return;

  if(annot_editor_active && files_open)
  {
    if(rc_file_open_requested)
    {
      rc_file_open_err = 101;
    }
    else
    {
      QMessageBox::critical(this, "Error", "You cannot open multiple files when editing annotations.\n"
                                           "Close the annotation edit window first.");
    }
    rc_file_open_requested = 0;
    drop_path[0] = 0;
    cmdlineargument = 0;
    return;
  }

  if((files_open > 0) && (live_stream_active))
  {
    if(rc_file_open_requested)
    {
      rc_file_open_err = 102;
    }
    else
    {
      QMessageBox::critical(this, "Error", "You cannot open multiple files while a streaming file is open.\n"
                                           "Close the streaming file first.");
    }
    rc_file_open_requested = 0;
    drop_path[0] = 0;
    cmdlineargument = 0;
    return;
  }

  if(files_open >= MAXFILES)
  {
    if(rc_file_open_requested)
    {
      rc_file_open_err = 103;
    }
    else
    {
      QMessageBox::critical(this, "Error", "There are too many files opened.");
    }
    rc_file_open_requested = 0;
    drop_path[0] = 0;
    cmdlineargument = 0;
    return;
  }

  if((cmdlineargument == 0) && (drop_path[0] == 0) && (!rc_file_open_requested))
  {
    strlcpy(path, QFileDialog::getOpenFileName(this, "Open file", QString::fromLocal8Bit(recent_opendir), "EDF/BDF files (*.edf *.EDF *.bdf *.BDF *.rec *.REC)").toLocal8Bit().data(), MAX_PATH_LENGTH);

    if(!strcmp(path, ""))
    {
      return;
    }

    get_directory_from_path(recent_opendir, path, MAX_PATH_LENGTH);
  }

  if((cmdlineargument == 0) && (drop_path[0] != 0))
  {
    strlcpy(path, drop_path, MAX_PATH_LENGTH);

    get_directory_from_path(recent_opendir, path, MAX_PATH_LENGTH);
  }

  if((cmdlineargument == 0) || (cmdlineargument == 1))
  {
    montagepath[0] = 0;
  }

  present = 0;

  for(i=0; i<MAX_RECENTFILES; i++)
  {
    if(!strcmp(&recent_file_path[i][0], path))
    {
      present = 1;

      position = i;

      if(cmdlineargument != 2)
      {
        strlcpy(montagepath, &recent_file_mtg_path[i][0], MAX_PATH_LENGTH);
      }

      break;
    }
  }

  if(present)
  {
    for(i=position; i>0; i--)
    {
      strlcpy(&recent_file_path[i][0], &recent_file_path[i-1][0], MAX_PATH_LENGTH);
      strlcpy(&recent_file_mtg_path[i][0], &recent_file_mtg_path[i-1][0], MAX_PATH_LENGTH);
    }
  }
  else
  {
    for(i=MAX_RECENTFILES-1; i>0; i--)
    {
      strlcpy(&recent_file_path[i][0], &recent_file_path[i-1][0], MAX_PATH_LENGTH);
      strlcpy(&recent_file_mtg_path[i][0], &recent_file_mtg_path[i-1][0], MAX_PATH_LENGTH);
    }
  }

  strlcpy(&recent_file_path[0][0], path, MAX_PATH_LENGTH);
  strlcpy(&recent_file_mtg_path[0][0], montagepath, MAX_PATH_LENGTH);

  recent_filesmenu->clear();

  for(i=0; i<MAX_RECENTFILES; i++)
  {
    if(recent_file_path[i][0] == 0)
    {
      break;
    }
    act = new QAction(QString::fromLocal8Bit(&recent_file_path[i][0]), recent_filesmenu);
    act->setData(QVariant(i));
    recent_filesmenu->addAction(act);
  }
  recent_filesmenu->addSeparator();
  act = new QAction("Clear list", recent_filesmenu);
  act->setData(QVariant(MAX_RECENTFILES));
  recent_filesmenu->addAction(act);

  present = 0;

  for(i=0; i<files_open; i++)
  {
    if(!strcmp(edfheaderlist[i]->filename, path))
    {
      present = 1;

      break;
    }
  }

  if(!present)
  {
    len = strlen(path);

    if((strcmp(path + (len - 4), ".edf"))
       &&(strcmp(path + (len - 4), ".EDF"))
       &&(strcmp(path + (len - 4), ".rec"))
       &&(strcmp(path + (len - 4), ".REC"))
       &&(strcmp(path + (len - 4), ".bdf"))
       &&(strcmp(path + (len - 4), ".BDF")))
    {
      if(rc_file_open_requested)
      {
        rc_file_open_err = 104;
      }
      else
      {
        snprintf(str, 2048, "File has an unknown extension:  \"%s\"", path + (len - 4));

        QMessageBox::critical(this, "Error", QString::fromLocal8Bit(str));
      }
      rc_file_open_requested = 0;
      drop_path[0] = 0;
      cmdlineargument = 0;
      return;
    }

    newfile = fopeno(path, "rb");
    if(newfile==NULL)
    {
      if(rc_file_open_requested)
      {
        rc_file_open_err = 105;
      }
      else
      {
        snprintf(str, 2048, "Cannot open file for reading:\n\"%s\"\n"
                            "Check if you have the right permissions.", path);
        QMessageBox::critical(this, "Error", QString::fromLocal8Bit(str));
      }
      rc_file_open_requested = 0;
      drop_path[0] = 0;
      cmdlineargument = 0;
      return;
    }

    strlcpy(recent_opendir, path, MAX_PATH_LENGTH);

    if(len)
    {
      for(i=len-1; i>=0; i--)
      {
        if((path[i] == '/')||(path[i] == '\\'))
        {
          break;
        }
      }
    }

    recent_opendir[i] = 0;

    EDFfileCheck EDFfilechecker;

    edfhdr = EDFfilechecker.check_edf_file(newfile, str, 2048, live_stream_active, 0);
    if(edfhdr==NULL)
    {
      fclose(newfile);

      newfile = NULL;

      if(rc_file_open_requested)
      {
        rc_file_open_err = 106;
      }
      else
      {
        if(strstr(str, "If you want to import annotations from this file") == NULL)
        {
          strlcat(str, "\n File is not a valid EDF or BDF file.", 2048);
        }
        QMessageBox::critical(this, "Error", str);
      }
      rc_file_open_requested = 0;
      drop_path[0] = 0;
      cmdlineargument = 0;
      return;
    }

    if(edfhdr->discontinuous)
    {
      if(rc_file_open_requested)
      {
        rc_file_open_err = 107;
      }
      else
      {
        if(edfhdr->edf)
        {
          QMessageBox::critical(this, "Error", "EDFbrowser cannot show EDF+D (discontiguous) files.\n"
                                               "Convert this file to EDF+C first. You can find this converter\n"
                                               "in the Tools menu (EDF+D to EDF+C converter).");
        }

        if(edfhdr->bdf)
        {
          QMessageBox::critical(this, "Error", "EDFbrowser cannot show BDF+D (discontiguous) files.\n"
                                               "Convert this file to BDF+C first. You can find this converter\n"
                                               "in the Tools menu (EDF+D to EDF+C converter).");
        }
      }

      free(edfhdr->edfparam);
      free(edfhdr);
      fclose(newfile);
      newfile = NULL;
      rc_file_open_requested = 0;
      drop_path[0] = 0;
      cmdlineargument = 0;
      return;
    }

    strlcpy(edfhdr->filename, path, MAX_PATH_LENGTH);

    edfhdr->file_hdl = newfile;

    edfheaderlist[files_open] = edfhdr;

    memset(&edfheaderlist[files_open]->annot_list, 0, sizeof(struct annotation_list));

    annotations_dock[files_open] = NULL;

    if((edfhdr->edfplus || edfhdr->bdfplus) && (!live_stream_active))
    {
      if((edfhdr->datarecords * (unsigned long long)edfhdr->recordsize) <= maxfilesize_to_readin_annotations)
      {
        EDF_annotations annotations;

        annotations.get_annotations(edfhdr, read_nk_trigger_signal);

        if(edfhdr->annots_not_read)
        {
          edfplus_annotation_empty_list(&edfheaderlist[files_open]->annot_list);
        }
        else
        {
          annotations_dock[files_open] = new UI_Annotationswindow(edfheaderlist[files_open], this);

          addDockWidget(Qt::RightDockWidgetArea, annotations_dock[files_open]->docklist, Qt::Vertical);

          if(!edfheaderlist[files_open]->annot_list.sz)
          {
            annotations_dock[files_open]->docklist->hide();
          }
        }
      }
      else
      {
        edfhdr->annots_not_read = 1;
      }
    }

    if((edfhdr->bdf && (!edfhdr->bdfplus) && read_biosemi_status_signal) && (!live_stream_active))
    {
      if((edfhdr->datarecords * (unsigned long long)edfhdr->recordsize) <= maxfilesize_to_readin_annotations)
      {
        BDF_triggers bdf_triggers_obj;

        bdf_triggers_obj.get_triggers(edfhdr);

        annotations_dock[files_open] = new UI_Annotationswindow(edfheaderlist[files_open], this);

        addDockWidget(Qt::RightDockWidgetArea, annotations_dock[files_open]->docklist, Qt::Vertical);

        if(!edfhdr->annot_list.sz)
        {
          annotations_dock[files_open]->docklist->hide();
        }
      }
    }

    if(!files_open)
    {
      edfheaderlist[0]->viewtime = 0;

      setMainwindowTitle(edfhdr);
    }
    else
    {
      if(viewtime_sync==VIEWTIME_SYNCED_OFFSET)
      {
        edfheaderlist[files_open]->viewtime = edfheaderlist[sel_viewtime]->viewtime;
      }

      if(viewtime_sync==VIEWTIME_UNSYNCED)
      {
        edfheaderlist[files_open]->viewtime = 0;
      }

      if(viewtime_sync==VIEWTIME_SYNCED_ABSOLUT)
      {
        edfheaderlist[files_open]->viewtime = edfheaderlist[sel_viewtime]->viewtime + edfheaderlist[sel_viewtime]->starttime_offset - edfheaderlist[files_open]->starttime_offset - ((edfheaderlist[files_open]->utc_starttime - edfheaderlist[sel_viewtime]->utc_starttime) * TIME_DIMENSION);
      }

      if(viewtime_sync==VIEWTIME_USER_DEF_SYNCED)
      {
        edfheaderlist[files_open]->viewtime = 0;
      }
    }

    sel_viewtime_act[files_open] = new QAction(QString::fromLocal8Bit(edfhdr->filename), this);
    sel_viewtime_act[files_open]->setCheckable(true);
    if(!files_open)
    {
      sel_viewtime_act[files_open]->setChecked(true);
    }
    sel_viewtime_act_group->addAction(sel_viewtime_act[files_open]);
    timemenu->addAction(sel_viewtime_act[files_open]);

    files_open++;
  }

  if(!rc_file_open_requested)
  {
    if((montagepath[0]!=0)&&(cmdlineargument==2))
    {
      UI_LoadMontagewindow load_mtg(this, montagepath);
      strlcpy(&recent_file_mtg_path[0][0], montagepath, MAX_PATH_LENGTH);
    }
    else
    {
      if((recent_file_mtg_path[0][0] != 0) && (files_open == 1) && auto_reload_mtg)
      {
        QMessageBox messagewindow;
        messagewindow.setText("Load last used montage?");
        messagewindow.setInformativeText(QString::fromLocal8Bit(&recent_file_mtg_path[0][0]));
        messagewindow.setIcon(QMessageBox::Question);
        messagewindow.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        messagewindow.setDefaultButton(QMessageBox::Yes);
        button_nr = messagewindow.exec();

        if(button_nr == QMessageBox::No)
        {
          recent_file_mtg_path[0][0] = 0;

          UI_Signalswindow signalwindow(this);
        }
        else
        {
          UI_LoadMontagewindow load_mtg(this, &recent_file_mtg_path[0][0]);

          if(recent_file_mtg_path[0][0] == 0)
          {
            UI_Signalswindow signalwindow(this);
          }
        }
      }
      else
      {
        UI_Signalswindow signalwindow(this);

        if(!signalcomps)
        {
          setup_viewbuf();
        }
      }
    }
  }

  cmdlineargument = 0;

  montagepath[0] = 0;

  drop_path[0] = 0;

  rc_file_open_requested = 0;

  if(files_open)
  {
    save_session_act->setEnabled(true);
  }

  close_filemenu->addAction(QString::fromLocal8Bit(path));
}


void UI_Mainwindow::remove_recent_file_mtg_path(const char *mtg_path)
{
  int i;


  for(i=0; i<MAX_RECENTFILES; i++)
  {
    if(!strcmp(&recent_file_mtg_path[i][0], mtg_path))
    {
      recent_file_mtg_path[i][0] = 0;
    }
  }
}


void UI_Mainwindow::setMainwindowTitle(struct edfhdrblock *edfhdr)
{
  int i, len;

  char str[MAX_PATH_LENGTH + 64]={""};

  struct date_time_struct date_time;


  if(edfhdr==NULL)
  {
    setWindowTitle(PROGRAM_NAME);

    return;
  }

  if(mainwindow_title_type == 0)
  {
    if(edfhdr->edfplus || edfhdr->bdfplus)
    {
      snprintf(str, 256, PROGRAM_NAME "  subject %s  birthdate %s  startdate %s",
                    edfhdr->plus_patient_name,
                    edfhdr->plus_birthdate,
                    edfhdr->plus_startdate);
    }
    else
    {
      utc_to_date_time(edfhdr->utc_starttime, &date_time);

      date_time.month_str[0] += 32;
      date_time.month_str[1] += 32;
      date_time.month_str[2] += 32;

      snprintf(str, 256, PROGRAM_NAME "  %s  startdate %i %s %i",
                    edfhdr->patient,
                    date_time.day,
                    date_time.month_str,
                    date_time.year);
      len = strlen(str);
      for(i=0; i<len; i++)
      {
        if(str[i]=='_')
        {
          str[i] = ' ';
        }
      }
    }
  }

  if(mainwindow_title_type == 1)
  {
    get_filename_from_path(str, edfhdr->filename, MAX_PATH_LENGTH);

    strlcat(str, " - " PROGRAM_NAME, MAX_PATH_LENGTH + 64);
  }

  if(mainwindow_title_type == 2)
  {
    strlcpy(str, edfhdr->filename, MAX_PATH_LENGTH + 64);

    strlcat(str, " - " PROGRAM_NAME, MAX_PATH_LENGTH + 64);
  }

  setWindowTitle(str);
}


void UI_Mainwindow::signalproperties_dialog()
{
  if((!files_open) || (!signalcomps))  return;

  if(signalcomps == 1)
  {
    maincurve->exec_sidemenu(0);

    return;
  }

  UI_SignalChooser signalchooserdialog(this, 0);
}


void UI_Mainwindow::filterproperties_dialog()
{
  if((!files_open) || (!signalcomps))  return;

  if(signalcomps == 1)
  {
    AdjustFilterSettings filtersettings(signalcomp[0], maincurve);

    return;
  }

  UI_SignalChooser signalchooserdialog(this, 2);
}


void UI_Mainwindow::add_signals_dialog()
{
  if(!files_open)  return;

  UI_Signalswindow signalwindow(this);
}


void UI_Mainwindow::show_splashscreen()
{
#if QT_VERSION >= 0x050F00
  splash = new QSplashScreen(this->screen(), *splash_pixmap);
#else
  splash = new QSplashScreen(this, *splash_pixmap, Qt::WindowStaysOnTopHint);
#endif
  splash->show();
}


void UI_Mainwindow::show_file_info()
{
  if(!files_open)  return;

  UI_EDFhdrwindow showhdr(this);
}


void UI_Mainwindow::remove_all_filters()
{
  int i, j,
      update_scr=0;

  if(!files_open)  return;

  for(i=0; i<signalcomps; i++)
  {
    for(j=0; j<signalcomp[i]->filter_cnt; j++)
    {
      free(signalcomp[i]->filter[j]);

      update_scr = 1;
    }

    signalcomp[i]->filter_cnt = 0;

    for(j=0; j<signalcomp[i]->fidfilter_cnt; j++)
    {
      free(signalcomp[i]->fidfilter[j]);
      fid_run_free(signalcomp[i]->fid_run[j]);
      fid_run_freebuf(signalcomp[i]->fidbuf[j]);
      fid_run_freebuf(signalcomp[i]->fidbuf2[j]);

      update_scr = 1;
    }

    signalcomp[i]->fidfilter_cnt = 0;

    for(j=0; j<signalcomp[i]->ravg_filter_cnt; j++)
    {
      free_ravg_filter(signalcomp[i]->ravg_filter[j]);

      update_scr = 1;
    }

    signalcomp[i]->ravg_filter_cnt = 0;

    if(signalcomp[i]->fir_filter != NULL)
    {
      free_fir_filter(signalcomp[i]->fir_filter);

      signalcomp[i]->fir_filter = NULL;
    }

    if(signalcomp[i]->ecg_filter != NULL)
    {
      free_ecg_filter(signalcomp[i]->ecg_filter);

      signalcomp[i]->ecg_filter = NULL;

      strlcpy(signalcomp[i]->signallabel, signalcomp[i]->signallabel_bu, 512);
      signalcomp[i]->signallabellen = signalcomp[i]->signallabellen_bu;
      strlcpy(signalcomp[i]->physdimension, signalcomp[i]->physdimension_bu, 512);
    }

    if(signalcomp[i]->zratio_filter != NULL)
    {
      free_zratio_filter(signalcomp[i]->zratio_filter);

      signalcomp[i]->zratio_filter = NULL;

      strlcpy(signalcomp[i]->signallabel, signalcomp[i]->signallabel_bu, 512);
      signalcomp[i]->signallabellen = signalcomp[i]->signallabellen_bu;
      strlcpy(signalcomp[i]->physdimension, signalcomp[i]->physdimension_bu, 512);
    }
  }

  if(update_scr)
  {
    setup_viewbuf();
  }
}


void UI_Mainwindow::remove_all_plif_ecg_filters()
{
  int i,
      update_scr=0;

  if(!files_open)  return;

  for(i=0; i<signalcomps; i++)
  {
    if(signalcomp[i]->plif_ecg_filter)
    {
      plif_free_subtract_filter(signalcomp[i]->plif_ecg_filter);

      signalcomp[i]->plif_ecg_filter = NULL;

      update_scr = 1;
    }

    if(signalcomp[i]->plif_ecg_filter_sav)
    {
      plif_free_subtract_filter(signalcomp[i]->plif_ecg_filter_sav);

      signalcomp[i]->plif_ecg_filter_sav = NULL;

      update_scr = 1;
    }
  }

  if(update_scr)
  {
    setup_viewbuf();
  }
}


void UI_Mainwindow::remove_all_fir_filters()
{
  int i,
      update_scr=0;

  if(!files_open)  return;

  for(i=0; i<signalcomps; i++)
  {
    if(signalcomp[i]->fir_filter)
    {
      free_fir_filter(signalcomp[i]->fir_filter);

      signalcomp[i]->fir_filter = NULL;

      update_scr = 1;
    }
  }

  if(update_scr)
  {
    setup_viewbuf();
  }
}


void UI_Mainwindow::remove_all_spike_filters()
{
  int i,
      update_scr=0;

  if(!files_open)  return;

  for(i=0; i<signalcomps; i++)
  {
    if(signalcomp[i]->spike_filter)
    {
      free_spike_filter(signalcomp[i]->spike_filter);

      signalcomp[i]->spike_filter = NULL;

      update_scr = 1;
    }
  }

  if(update_scr)
  {
    setup_viewbuf();
  }
}


void UI_Mainwindow::remove_all_signals()
{
  int i;

  stop_video_generic(0);

  stop_playback();

  session_start_video = 0;

  for(i=0; i<MAXSPECTRUMDOCKS; i++)
  {
    spectrumdock[i]->clear();
    spectrumdock[i]->dock->hide();
  }

  for(i=0; i<MAXSPECTRUMDIALOGS; i++)
  {
    if(spectrumdialog[i] != NULL)
    {
      delete spectrumdialog[i];

      spectrumdialog[i] = NULL;
    }
  }

  for(i=0; i<MAXCDSADOCKS; i++)
  {
    if(cdsa_dock[i] != NULL)
    {
      delete cdsa_dock[i];

      cdsa_dock[i] = NULL;
    }
  }

  for(i=0; i<MAXAVERAGECURVEDIALOGS; i++)
  {
    if(averagecurvedialog[i] != NULL)
    {
      delete averagecurvedialog[i];

      averagecurvedialog[i] = NULL;
    }
  }

  for(i=0; i<MAXZSCOREDIALOGS; i++)
  {
    if(zscoredialog[i] != NULL)
    {
      delete zscoredialog[i];

      zscoredialog[i] = NULL;
    }
  }

  maincurve->crosshair_1.active = 0;
  maincurve->crosshair_2.active = 0;
  maincurve->crosshair_1.moving = 0;
  maincurve->crosshair_2.moving = 0;

  remove_all_filters();

  remove_all_spike_filters();

  remove_all_plif_ecg_filters();

  for(i=0; i<signalcomps; i++)
  {
    free(signalcomp[i]);

    signalcomp[i] = NULL;
  }

  signalcomps = 0;

  if(viewbuf!=NULL)
  {
    free(viewbuf);
    viewbuf = NULL;
  }

  slidertoolbar->setEnabled(false);
  positionslider->blockSignals(true);

  setup_viewbuf();
}


void UI_Mainwindow::close_file_action_func(QAction *action)
{
  int i, j, k, p,
      file_n,
      inst_num=0;

  char f_path[MAX_PATH_LENGTH];

  if(files_open < 2)
  {
    close_all_files();

    return;
  }

  toolbar_stats.active = 0;

  nav_toolbar_label->setText("");

  strlcpy(f_path, action->text().toLocal8Bit().data(), MAX_PATH_LENGTH);

  for(file_n=0; file_n<files_open; file_n++)
  {
    if(!strcmp(f_path, edfheaderlist[file_n]->filename))
    {
      break;
    }
  }

  if(file_n == files_open)  return;

  delete action;

  stop_video_generic(0);

  stop_playback();

  for(i=0; i<MAXHYPNOGRAMDOCKS; i++)
  {
    inst_num = edfheaderlist[file_n]->hypnogram_idx[i];

    if(inst_num > 0)
    {
      if(hypnogram_dock[inst_num - 1] != NULL)
      {
        delete hypnogram_dock[inst_num - 1];

        hypnogram_dock[inst_num - 1] = NULL;
      }

      edfheaderlist[file_n]->hypnogram_idx[i] = 0;
    }
  }

  for(i=0; i<MAXHRVDOCKS; i++)
  {
    inst_num = edfheaderlist[file_n]->hrv_dock[i];

    if(inst_num > 0)
    {
      if(hrv_dock[inst_num - 1] != NULL)
      {
        delete hrv_dock[inst_num - 1];

        hrv_dock[inst_num - 1] = NULL;
      }

      edfheaderlist[file_n]->hrv_dock[i] = 0;
    }
  }

  for(j=0; j<signalcomps; )
  {
    if(signalcomp[j]->edfhdr == edfheaderlist[file_n])
    {
      for(i=0; i<MAXSPECTRUMDOCKS; i++)
      {
        if(spectrumdock[i]->signalcomp == signalcomp[j])
        {
          spectrumdock[i]->clear();
          spectrumdock[i]->dock->hide();
        }
      }

      for(i=0; i<MAXSPECTRUMDIALOGS; i++)
      {
        p = signalcomp[j]->spectr_dialog[i];

        if(p != 0)
        {
          delete spectrumdialog[p - 1];

          spectrumdialog[p - 1] = NULL;
        }
      }

      for(i=0; i<MAXCDSADOCKS; i++)
      {
        p = signalcomp[j]->cdsa_idx[i];

        if(p != 0)
        {
          delete cdsa_dock[p - 1];

          cdsa_dock[p - 1] = NULL;
        }
      }

      for(i=0; i<MAXAVERAGECURVEDIALOGS; i++)
      {
        p = signalcomp[j]->avg_dialog[i];

        if(p != 0)
        {
          delete averagecurvedialog[p - 1];

          averagecurvedialog[p - 1] = NULL;
        }
      }

      for(i=0; i<MAXZSCOREDIALOGS; i++)
      {
        p = signalcomp[j]->zscoredialog[i];

        if(p != 0)
        {
          delete zscoredialog[p - 1];

          zscoredialog[p - 1] = NULL;
        }
      }

      if(signalcomp[j]->hascursor2)
      {
        maincurve->crosshair_2.active = 0;
        maincurve->crosshair_2.moving = 0;
      }

      if(signalcomp[j]->hascursor1)
      {
        maincurve->crosshair_1.active = 0;
        maincurve->crosshair_2.active = 0;
        maincurve->crosshair_1.moving = 0;
        maincurve->crosshair_2.moving = 0;

        for(i=0; i<signalcomps; i++)
        {
          signalcomp[i]->hascursor2 = 0;
        }
      }

      if(signalcomp[j]->hasruler)
      {
        maincurve->ruler_active = 0;
        maincurve->ruler_moving = 0;
      }

      for(k=0; k<signalcomp[j]->filter_cnt; k++)
      {
        free(signalcomp[j]->filter[k]);
      }

      signalcomp[j]->filter_cnt = 0;

      if(signalcomp[j]->plif_ecg_filter)
      {
        plif_free_subtract_filter(signalcomp[j]->plif_ecg_filter);

        signalcomp[j]->plif_ecg_filter = NULL;
      }

      if(signalcomp[j]->plif_ecg_filter_sav)
      {
        plif_free_subtract_filter(signalcomp[j]->plif_ecg_filter_sav);

        signalcomp[j]->plif_ecg_filter_sav = NULL;
      }

      if(signalcomp[j]->spike_filter)
      {
        free_spike_filter(signalcomp[j]->spike_filter);

        signalcomp[j]->spike_filter = NULL;
      }

      for(k=0; k<signalcomp[j]->ravg_filter_cnt; k++)
      {
        free_ravg_filter(signalcomp[j]->ravg_filter[k]);
      }

      signalcomp[j]->ravg_filter_cnt = 0;

      if(signalcomp[j]->fir_filter != NULL)
      {
        free_fir_filter(signalcomp[j]->fir_filter);

        signalcomp[j]->fir_filter = NULL;
      }

      if(signalcomp[j]->ecg_filter != NULL)
      {
        free_ecg_filter(signalcomp[j]->ecg_filter);

        signalcomp[j]->ecg_filter = NULL;

        strlcpy(signalcomp[j]->signallabel, signalcomp[j]->signallabel_bu, 512);
        signalcomp[j]->signallabellen = signalcomp[j]->signallabellen_bu;
        strlcpy(signalcomp[j]->physdimension, signalcomp[j]->physdimension_bu, 512);
      }

      for(k=0; k<signalcomp[j]->fidfilter_cnt; k++)
      {
        free(signalcomp[j]->fidfilter[k]);
        fid_run_free(signalcomp[j]->fid_run[k]);
        fid_run_freebuf(signalcomp[j]->fidbuf[k]);
        fid_run_freebuf(signalcomp[j]->fidbuf2[k]);
      }

      signalcomp[j]->fidfilter_cnt = 0;

      signalcomp[j]->uid = 0LL;

      free(signalcomp[j]);

      signalcomp[j] = NULL;

      for(i=j; i<signalcomps - 1; i++)
      {
        signalcomp[i] = signalcomp[i + 1];
      }

      signalcomps--;
    }
    else
    {
      j++;
    }
  }

  edfplus_annotation_empty_list(&edfheaderlist[file_n]->annot_list);
  fclose(edfheaderlist[file_n]->file_hdl);
  free(edfheaderlist[file_n]->edfparam);
  free(edfheaderlist[file_n]);

  if(annotations_dock[file_n] != NULL)
  {
    annotations_dock[file_n]->docklist->close();
    delete annotations_dock[file_n];
    annotations_dock[file_n] = NULL;
  }

  if((file_n == sel_viewtime) && (files_open > 1))
  {
    if(file_n > 0)
    {
      sel_viewtime = 0;
    }
    else
    {
      sel_viewtime = 1;
    }

    sel_viewtime_act[sel_viewtime]->setChecked(true);

    setMainwindowTitle(edfheaderlist[sel_viewtime]);
  }

  delete sel_viewtime_act[file_n];

  for(i=file_n; i<files_open - 1; i++)
  {
    edfheaderlist[i] = edfheaderlist[i + 1];

    annotations_dock[i] = annotations_dock[i + 1];

    sel_viewtime_act[i] = sel_viewtime_act[i + 1];
  }

  files_open--;

  setup_viewbuf();

  if(!files_open)
  {
    save_session_act->setEnabled(false);
  }
}


void UI_Mainwindow::close_all_files()
{
  int i, j, button_nr=0, inst_num=0;

  if(annotations_edited)
  {
    QMessageBox messagewindow;
    messagewindow.setText("There are unsaved annotations,\n are you sure you want to close this file?");
    messagewindow.setIcon(QMessageBox::Question);
    messagewindow.setStandardButtons(QMessageBox::Cancel | QMessageBox::Discard);
    messagewindow.setDefaultButton(QMessageBox::Cancel);
    button_nr = messagewindow.exec();
  }

  if(button_nr == QMessageBox::Cancel)
  {
    return;
  }

  toolbar_stats.active = 0;
  nav_toolbar_label->setText("");
  live_stream_active = 0;
  live_stream_timer->stop();
  toolsmenu->setEnabled(true);
  timemenu->setEnabled(true);
//  windowmenu->setEnabled(true);
  windowmenu->actions().at(0)->setEnabled(true);  // Annotations window
  windowmenu->actions().at(1)->setEnabled(true);  // Annotation editor
  former_page_Act->setEnabled(true);
  shift_page_left_Act->setEnabled(true);
  shift_page_right_Act->setEnabled(true);
  next_page_Act->setEnabled(true);
  shift_page_up_Act->setEnabled(true);
  shift_page_down_Act->setEnabled(true);
  printmenu->setEnabled(true);
  recent_filesmenu->setEnabled(true);
  recent_session_menu->setEnabled(true);
  playback_file_Act->setEnabled(true);

  save_session_act->setEnabled(false);

  annotations_edited = 0;

  session_start_video = 0;

  remove_all_signals();

  while(files_open)
  {
    files_open--;

    for(i=0; i<MAXHYPNOGRAMDOCKS; i++)
    {
      inst_num = edfheaderlist[files_open]->hypnogram_idx[i];

      if(inst_num > 0)
      {
        if(hypnogram_dock[inst_num - 1] != NULL)
        {
          delete hypnogram_dock[inst_num - 1];

          hypnogram_dock[inst_num - 1] = NULL;
        }

        edfheaderlist[files_open]->hypnogram_idx[i] = 0;
      }
    }

    for(i=0; i<MAXHRVDOCKS; i++)
    {
      inst_num = edfheaderlist[files_open]->hrv_dock[i];

      if(inst_num > 0)
      {
        if(hrv_dock[inst_num - 1] != NULL)
        {
          delete hrv_dock[inst_num - 1];

          hrv_dock[inst_num - 1] = NULL;
        }

        edfheaderlist[files_open]->hrv_dock[i] = 0;
      }
    }

    if(edfheaderlist[files_open]->file_hdl != NULL)
    {
      fclose(edfheaderlist[files_open]->file_hdl);
    }
    edfplus_annotation_empty_list(&edfheaderlist[files_open]->annot_list);
    free(edfheaderlist[files_open]->edfparam);
    free(edfheaderlist[files_open]);

    if(annotations_dock[files_open] != NULL)
    {
      annotations_dock[files_open]->docklist->close();
      delete annotations_dock[files_open];
      annotations_dock[files_open] = NULL;
    }

    delete sel_viewtime_act[files_open];
  }

  if(annotationlist_backup != NULL)
  {
    edfplus_annotation_empty_list(annotationlist_backup);

    free(annotationlist_backup);

    annotationlist_backup = NULL;
  }

  sel_viewtime = 0;

  pagetime = 10 * TIME_DIMENSION;

  timescale_doubler = 10;

  amplitude_doubler = 10;

  viewtime_string[0] = 0;

  pagetime_string[0] = 0;

  zoomhistory->idx = 0;
  zoomhistory->history_size_tail = 0;
  zoomhistory->history_size_head = 0;
  for(i=0; i<MAXZOOMHISTORY; i++)
  {
    zoomhistory->pagetime[i] = 10 * TIME_DIMENSION;
    for(j=0; j<MAXFILES; j++)
    {
      zoomhistory->viewtime[i][j] = 0;
    }
    for(j=0; j<MAXSIGNALS; j++)
    {
      zoomhistory->voltpercm[i][j] = 70;
      zoomhistory->screen_offset[i][j] = 0.0;
      zoomhistory->sensitivity[i][j] = 0.0475;
    }
  }

  delete annotationEditDock;
  annotationEditDock = NULL;

  save_act->setEnabled(false);

  annotations_edited = 0;

  dig_min_max_overflow = 0;

  dig_min_max_overflow_warning_showed = 0;

  setWindowTitle(PROGRAM_NAME);

  close_filemenu->clear();

  if(!exit_in_progress)
  {
    setup_viewbuf();
  }
}


void UI_Mainwindow::show_about_dialog()
{
  UI_Aboutwindow aboutwindow(this);
}


void UI_Mainwindow::page_3cmsec()
{
  double mm;

  if(auto_dpi)
  {
    mm = maincurve->widthMM();

    pagetime = mm * 333333.3333;
  }
  else
  {
    pagetime = (long long)((((double)maincurve->width()) / (1.0 / x_pixelsizefactor) / 3.0) * TIME_DIMENSION);
  }

  setup_viewbuf();
}


void UI_Mainwindow::page_25mmsec()
{
  double mm;

  if(auto_dpi)
  {
    mm = maincurve->widthMM();

    pagetime = mm * 400000.0;
  }
  else
  {
    pagetime = (long long)((((double)maincurve->width()) / (1.0 / x_pixelsizefactor) / 2.5) * TIME_DIMENSION);
  }

  setup_viewbuf();
}


void UI_Mainwindow::page_50mmsec()
{
  double mm;

  if(auto_dpi)
  {
    mm = maincurve->widthMM();

    pagetime = mm * 200000.0;
  }
  else
  {
    pagetime = (long long)((((double)maincurve->width()) / (1.0 / x_pixelsizefactor) / 5.0) * TIME_DIMENSION);
  }

  setup_viewbuf();
}


void UI_Mainwindow::set_page_div2()
{
  int i;

//   long long l_tmp, trshld=100LL;

  if((viewtime_sync==VIEWTIME_SYNCED_OFFSET)||(viewtime_sync==VIEWTIME_SYNCED_ABSOLUT)||(viewtime_sync==VIEWTIME_USER_DEF_SYNCED))
  {
    for(i=0; i<files_open; i++)
    {
      if(timescale_doubler == 50)
      {
        edfheaderlist[i]->viewtime += (pagetime * 0.3);
      }
      else
      {
        edfheaderlist[i]->viewtime += (pagetime / 4);
      }

//       if(viewtime_sync!=VIEWTIME_USER_DEF_SYNCED)
//       {
//         l_tmp = edfheaderlist[i]->viewtime % TIME_DIMENSION;
//
//         if(l_tmp < trshld)
//         {
//           edfheaderlist[i]->viewtime -= l_tmp;
//         }
//
//         if(l_tmp > (TIME_DIMENSION - trshld))
//         {
//           edfheaderlist[i]->viewtime += (TIME_DIMENSION - l_tmp);
//         }
//       }
    }
  }

  if(viewtime_sync==VIEWTIME_UNSYNCED)
  {
    if(timescale_doubler == 50)
    {
      edfheaderlist[sel_viewtime]->viewtime += (pagetime * 0.3);
    }
    else
    {
      edfheaderlist[sel_viewtime]->viewtime += (pagetime / 4);
    }

//     l_tmp = edfheaderlist[sel_viewtime]->viewtime % TIME_DIMENSION;
//
//     if(l_tmp < trshld)
//     {
//       edfheaderlist[sel_viewtime]->viewtime -= l_tmp;
//     }
//
//     if(l_tmp > (TIME_DIMENSION - trshld))
//     {
//       edfheaderlist[sel_viewtime]->viewtime += (TIME_DIMENSION - l_tmp);
//     }
  }

  if(timescale_doubler == 10)
  {
    timescale_doubler = 50;

    pagetime /= 2;
  }
  else
  {
    if(timescale_doubler == 50)
    {
      timescale_doubler = 20;

      pagetime /= 2.5;
    }
    else
    {
      timescale_doubler = 10;

      pagetime /= 2;
    }
  }

  setup_viewbuf();
}


void UI_Mainwindow::set_page_mult2()
{
  int i;

//   long long l_tmp, trshld=100LL;

  if((viewtime_sync==VIEWTIME_SYNCED_OFFSET)||(viewtime_sync==VIEWTIME_SYNCED_ABSOLUT)||(viewtime_sync==VIEWTIME_USER_DEF_SYNCED))
  {
    for(i=0; i<files_open; i++)
    {
      if(timescale_doubler == 20)
      {
        edfheaderlist[i]->viewtime -= (pagetime * 0.75);
      }
      else
      {
        edfheaderlist[i]->viewtime -= (pagetime / 2);
      }

//       if(viewtime_sync!=VIEWTIME_USER_DEF_SYNCED)
//       {
//         l_tmp = edfheaderlist[i]->viewtime % TIME_DIMENSION;
//
//         if(l_tmp < trshld)
//         {
//           edfheaderlist[i]->viewtime -= l_tmp;
//         }
//
//         if(l_tmp > (TIME_DIMENSION - trshld))
//         {
//           edfheaderlist[i]->viewtime += (TIME_DIMENSION - l_tmp);
//         }
//       }
    }
  }

  if(viewtime_sync==VIEWTIME_UNSYNCED)
  {
    if(timescale_doubler == 20)
    {
      edfheaderlist[sel_viewtime]->viewtime -= (pagetime * 0.75);
    }
    else
    {
      edfheaderlist[sel_viewtime]->viewtime -= (pagetime / 2);
    }

//     l_tmp = edfheaderlist[sel_viewtime]->viewtime % TIME_DIMENSION;
//
//     if(l_tmp < trshld)
//     {
//       edfheaderlist[sel_viewtime]->viewtime -= l_tmp;
//     }
//
//     if(l_tmp > (TIME_DIMENSION - trshld))
//     {
//       edfheaderlist[sel_viewtime]->viewtime += (TIME_DIMENSION - l_tmp);
//     }
  }

  if(timescale_doubler == 10)
  {
    timescale_doubler = 20;

    pagetime *= 2;
  }
  else
  {
    if(timescale_doubler == 20)
    {
      timescale_doubler = 50;

      pagetime *= 2.5;
    }
    else
    {
      timescale_doubler = 10;

      pagetime *= 2;
    }
  }

  setup_viewbuf();
}


void UI_Mainwindow::set_display_time(QAction *action)
{
  if(action==page_10u)
  {
    pagetime = TIME_DIMENSION / 100000;

    timescale_doubler = 10;
  }
  if(action==page_20u)
  {
    pagetime = TIME_DIMENSION / 50000;

    timescale_doubler = 20;
  }
  if(action==page_50u)
  {
    pagetime = TIME_DIMENSION / 20000;

    timescale_doubler = 50;
  }
  if(action==page_100u)
  {
    pagetime = TIME_DIMENSION / 10000;

    timescale_doubler = 10;
  }
  if(action==page_200u)
  {
    pagetime = TIME_DIMENSION / 5000;

    timescale_doubler = 20;
  }
  if(action==page_500u)
  {
    pagetime = TIME_DIMENSION / 2000;

    timescale_doubler = 50;
  }
  if(action==page_1m)
  {
    pagetime = TIME_DIMENSION / 1000;

    timescale_doubler = 10;
  }
  if(action==page_2m)
  {
    pagetime = TIME_DIMENSION / 500;

    timescale_doubler = 20;
  }
  if(action==page_5m)
  {
    pagetime = TIME_DIMENSION / 200;

    timescale_doubler = 50;
  }
  if(action==page_10m)
  {
    pagetime = TIME_DIMENSION / 100;

    timescale_doubler = 10;
  }
  if(action==page_20m)
  {
    pagetime = TIME_DIMENSION / 50;

    timescale_doubler = 20;
  }
  if(action==page_50m)
  {
    pagetime = TIME_DIMENSION / 20;

    timescale_doubler = 50;
  }
  if(action==page_100m)
  {
    pagetime = TIME_DIMENSION / 10;

    timescale_doubler = 10;
  }
  if(action==page_200m)
  {
    pagetime = TIME_DIMENSION / 5;

    timescale_doubler = 20;
  }
  if(action==page_500m)
  {
    pagetime = TIME_DIMENSION / 2;

    timescale_doubler = 50;
  }
  if(action==page_1)
  {
    pagetime = TIME_DIMENSION;

    timescale_doubler = 10;
  }
  if(action==page_2)
  {
    pagetime = TIME_DIMENSION * 2;

    timescale_doubler = 20;
  }
  if(action==page_5)
  {
    pagetime = TIME_DIMENSION * 5;

    timescale_doubler = 50;
  }
  if(action==page_10)
  {
    pagetime = TIME_DIMENSION * 10;

    timescale_doubler = 10;
  }
  if(action==page_15)    pagetime = TIME_DIMENSION * 15;
  if(action==page_20)
  {
    pagetime = TIME_DIMENSION * 20;

    timescale_doubler = 20;
  }
  if(action==page_30)    pagetime = TIME_DIMENSION * 30;
  if(action==page_60)    pagetime = TIME_DIMENSION * 60;
  if(action==page_300)   pagetime = TIME_DIMENSION * 300;
  if(action==page_1200)  pagetime = TIME_DIMENSION * 1200;
  if(action==page_3600)  pagetime = TIME_DIMENSION * 3600;
  if(action==page_8h)    pagetime = TIME_DIMENSION * 3600 * 8;
  if(action==page_12h)   pagetime = TIME_DIMENSION * 3600 * 12;
  if(action==page_24h)   pagetime = TIME_DIMENSION * 3600 * 24;
  if(action==page_48h)   pagetime = TIME_DIMENSION * 3600 * 48;

  setup_viewbuf();
}


void UI_Mainwindow::set_user_defined_display_time()
{
  if(!files_open)  return;

  UI_Userdefined_timepage_Dialog set_displaytime_dialog(this);
}


void UI_Mainwindow::set_user_defined_amplitude()
{
  if(!files_open)  return;

  if(!signalcomps)  return;

  UI_Userdefined_amplitude_Dialog set_amplitude_dialog(this);
}


void UI_Mainwindow::set_display_time_whole_rec()
{
  int i;

  long long vtime=0LL;

  if(!files_open)  return;

  if(viewtime_sync==VIEWTIME_SYNCED_OFFSET)
  {
    for(i=0; i<files_open; i++)
    {
      edfheaderlist[i]->viewtime = 0;
    }
  }

  if(viewtime_sync==VIEWTIME_UNSYNCED)
  {
    edfheaderlist[sel_viewtime]->viewtime = 0;
  }

  if(viewtime_sync==VIEWTIME_SYNCED_ABSOLUT)
  {
    edfheaderlist[sel_viewtime]->viewtime = 0;

    for(i=0; i<files_open; i++)
    {
      if(i!=sel_viewtime)
      {
        edfheaderlist[i]->viewtime = ((edfheaderlist[sel_viewtime]->utc_starttime - edfheaderlist[i]->utc_starttime) * TIME_DIMENSION) + edfheaderlist[sel_viewtime]->starttime_offset - - edfheaderlist[i]->starttime_offset;
      }
    }
  }

  if(viewtime_sync==VIEWTIME_USER_DEF_SYNCED)
  {
    for(i=0; i<files_open; i++)
    {
      if(i!=sel_viewtime)
      {
        edfheaderlist[i]->viewtime -= edfheaderlist[sel_viewtime]->viewtime;
      }
    }

    edfheaderlist[sel_viewtime]->viewtime = 0;
  }

  for(i=0; i<files_open; i++)
  {
    if(edfheaderlist[i]->viewtime > vtime)
    {
      vtime = edfheaderlist[i]->viewtime;
    }
  }

  for(i=0; i<files_open; i++)
  {
    edfheaderlist[i]->viewtime -= vtime;
  }

  pagetime = 0LL;

  for(i=0; i<files_open; i++)
  {
    if(((edfheaderlist[i]->datarecords * edfheaderlist[i]->long_data_record_duration) - edfheaderlist[i]->viewtime) > pagetime)
    {
      pagetime = (edfheaderlist[i]->datarecords * edfheaderlist[i]->long_data_record_duration) - edfheaderlist[i]->viewtime;
    }
  }

  setup_viewbuf();
}


void UI_Mainwindow::fit_signals_to_pane(int n)
{
  int i,
      pane_size;

  if(!signalcomps)  return;

  pane_size = maincurve->height() / (signalcomps + 1);

  for(i=0; i<signalcomps; i++)
  {
    if((n >= 0) && (n != i))
    {
      continue;
    }

    if(signalcomp[i]->max_dig_value!=signalcomp[i]->min_dig_value)
    {
      signalcomp[i]->sensitivity = (double)pane_size / (double)(signalcomp[i]->max_dig_value - signalcomp[i]->min_dig_value);
    }
    else
    {
      signalcomp[i]->sensitivity = pane_size;
    }

    signalcomp[i]->voltpercm =
     signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[0]].bitvalue
     / (signalcomp[i]->sensitivity * y_pixelsizefactor);

    signalcomp[i]->screen_offset = ((signalcomp[i]->max_dig_value + signalcomp[i]->min_dig_value) / 2.0) * signalcomp[i]->sensitivity * signalcomp[i]->polarity;
  }

  maincurve->drawCurve_stage_1();
}


void UI_Mainwindow::fit_signals_dc_offset(int n)
{
  int i;

  if(!signalcomps)  return;

  for(i=0; i<signalcomps; i++)
  {
    if((n >= 0) && (n != i))
    {
      continue;
    }

    signalcomp[i]->screen_offset = ((signalcomp[i]->max_dig_value + signalcomp[i]->min_dig_value) / 2.0) * signalcomp[i]->sensitivity * signalcomp[i]->polarity;
  }

  maincurve->drawCurve_stage_1();
}


void UI_Mainwindow::set_amplitude(QAction *action)
{
  int i;

  double value=100.0, original_value, value2=100.0;


  if(action==amp_00001)
  {
    value = 0.0001;
    amplitude_doubler = 10;
  }
  if(action==amp_00002)
  {
    value = 0.0002;
    amplitude_doubler = 20;
  }
  if(action==amp_00005)
  {
    value = 0.0005;
    amplitude_doubler = 50;
  }
  if(action==amp_0001)
  {
    value = 0.001;
    amplitude_doubler = 10;
  }
  if(action==amp_0002)
  {
    value = 0.002;
    amplitude_doubler = 20;
  }
  if(action==amp_0005)
  {
    value = 0.005;
    amplitude_doubler = 50;
  }
  if(action==amp_001)
  {
    value = 0.01;
    amplitude_doubler = 10;
  }
  if(action==amp_002)
  {
    value = 0.02;
    amplitude_doubler = 20;
  }
  if(action==amp_005)
  {
    value = 0.05;
    amplitude_doubler = 50;
  }
  if(action==amp_01)
  {
    value = 0.1;
    amplitude_doubler = 10;
  }
  if(action==amp_02)
  {
    value = 0.2;
    amplitude_doubler = 20;
  }
  if(action==amp_05)
  {
    value = 0.5;
    amplitude_doubler = 50;
  }
  if(action==amp_1)
  {
    value = 1.0;
    amplitude_doubler = 10;
  }
  if(action==amp_2)
  {
    value = 2.0;
    amplitude_doubler = 20;
  }
  if(action==amp_5)
  {
    value = 5.0;
    amplitude_doubler = 50;
  }
  if(action==amp_10)
  {
    value = 10.0;
    amplitude_doubler = 10;
  }
  if(action==amp_20)
  {
    value = 20.0;
    amplitude_doubler = 20;
  }
  if(action==amp_50)
  {
    value = 50.0;
    amplitude_doubler = 50;
  }
  if(action==amp_100)
  {
    value = 100.0;
    amplitude_doubler = 10;
  }
  if(action==amp_200)
  {
    value = 200.0;
    amplitude_doubler = 20;
  }
  if(action==amp_500)
  {
    value = 500.0;
    amplitude_doubler = 50;
  }
  if(action==amp_1000)
  {
    value = 1000.0;
    amplitude_doubler = 10;
  }
  if(action==amp_2000)
  {
    value = 2000.0;
    amplitude_doubler = 20;
  }
  if(action==amp_5000)
  {
    value = 5000.0;
    amplitude_doubler = 50;
  }
  if(action==amp_10000)
  {
    value = 10000.0;
    amplitude_doubler = 10;
  }
  if(action==amp_20000)
  {
    value = 20000.0;
    amplitude_doubler = 20;
  }
  if(action==amp_50000)
  {
    value = 50000.0;
    amplitude_doubler = 50;
  }

  for(i=0; i<signalcomps; i++)
  {
    value2 = value;

    if(signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[0]].bitvalue < 0.0)
    {
      value2 *= -1.0;
    }

    signalcomp[i]->sensitivity = (signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[0]].bitvalue / value2) / y_pixelsizefactor;

    original_value = signalcomp[i]->voltpercm;

    signalcomp[i]->voltpercm = value2;

    signalcomp[i]->screen_offset *= (original_value / value2);
  }

  maincurve->drawCurve_stage_1();
}


void UI_Mainwindow::set_amplitude_mult2()
{
  int i;

  for(i=0; i<signalcomps; i++)
  {
    if(signalcomp[i]->voltpercm < 0)
    {
      if(signalcomp[i]->voltpercm < -5000000.0)
      {
        return;
      }
    }
    else
    {
      if(signalcomp[i]->voltpercm > 5000000.0)
      {
        return;
      }
    }
  }

  for(i=0; i<signalcomps; i++)
  {
    if(amplitude_doubler != 20)
    {
      signalcomp[i]->voltpercm *= 2;

      signalcomp[i]->screen_offset /= 2;
    }
    else
    {
      signalcomp[i]->voltpercm *= 2.5;

      signalcomp[i]->screen_offset /= 2.5;
    }

    signalcomp[i]->sensitivity = (signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[0]].bitvalue / signalcomp[i]->voltpercm) / y_pixelsizefactor;
  }

  if(amplitude_doubler == 10)
  {
    amplitude_doubler = 20;
  }
  else
  {
    if(amplitude_doubler == 20)
    {
      amplitude_doubler = 50;
    }
    else
    {
      amplitude_doubler = 10;
    }
  }

  maincurve->drawCurve_stage_1();
}


void UI_Mainwindow::set_amplitude_div2()
{
  int i;

  for(i=0; i<signalcomps; i++)
  {
    if(signalcomp[i]->voltpercm < 0)
    {
      if(signalcomp[i]->voltpercm > -0.000001)
      {
        return;
      }
    }
    else
    {
      if(signalcomp[i]->voltpercm < 0.000001)
      {
        return;
      }
    }
  }

  for(i=0; i<signalcomps; i++)
  {
    if(amplitude_doubler != 50)
    {
      signalcomp[i]->voltpercm /= 2;

      signalcomp[i]->screen_offset *= 2;
    }
    else
    {
      signalcomp[i]->voltpercm /= 2.5;

      signalcomp[i]->screen_offset *= 2.5;
    }

    signalcomp[i]->sensitivity = (signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[0]].bitvalue / signalcomp[i]->voltpercm) / y_pixelsizefactor;
  }

  if(amplitude_doubler == 10)
  {
    amplitude_doubler = 50;
  }
  else
  {
    if(amplitude_doubler == 50)
    {
      amplitude_doubler = 20;
    }
    else
    {
      amplitude_doubler = 10;
    }
  }

  maincurve->drawCurve_stage_1();
}


void UI_Mainwindow::load_predefined_mtg(QAction *action)
{
  int i;

  if(!files_open)
  {
    return;
  }

  for(i=0; i < MAXPREDEFINEDMONTAGES; i++)
  {
    if(action==load_predefined_mtg_act[i])
    {
      if(predefined_mtg_path[i][0] != 0)
      {
        strlcpy(montagepath, &predefined_mtg_path[i][0], MAX_PATH_LENGTH);

        UI_LoadMontagewindow load_mtg(this, montagepath);

        return;
      }
    }
  }
}


void UI_Mainwindow::export_to_ascii()
{
  if(!files_open)
  {
    QMessageBox::critical(this, "Error", "You have to open a file first.");
    return;
  }

  UI_AsciiExportwindow exportdialog(this);
}


void UI_Mainwindow::export_ecg_rr_interval_to_ascii()
{
  UI_ECGExport ECG_export(this);
}


void UI_Mainwindow::qrs_detector()
{
  int signal_nr=-2;

  char str[2048]={""};

  double sf=1;

  if((!files_open) || live_stream_active || (!signalcomps))  return;

  if(signalcomps > 1)
  {
    UI_SignalChooser signal_chooser(this, 4, &signal_nr);
  }
  else
  {
    signal_nr = 0;
  }

  if((signal_nr < 0) || (signal_nr >= signalcomps))  return;

  sf = signalcomp[signal_nr]->edfhdr->edfparam[signalcomp[signal_nr]->edfsignal[0]].sf_f;
  if(sf < 199.999)
  {
    snprintf(str, 2048, "Sample rate of selected signal is %.1f Hz."
                        "The QRS detector needs a samplerate of 200 Hz or higher.", sf);
    QMessageBox::critical(this, "Error", str);
    return;
  }

  strlcpy(str, signalcomp[signal_nr]->edfhdr->edfparam[signalcomp[signal_nr]->edfsignal[0]].physdimension, 2048);
  trim_spaces(str);
  if((strcmp(str, "uV")) && (strcmp(str, "ECG uV")) && (strcmp(str, "EEG uV")) &&
     (strcmp(str, "mV")) && (strcmp(str, "ECG mV")) && (strcmp(str, "EEG mV")) &&
     (strcmp(str, "V")) && (strcmp(str, "ECG V")) && (strcmp(str, "EEG V")))
  {
    snprintf(str, 2048, "Unknown unit (physical dimension): \"%s\", expected uV or mV or V",
             signalcomp[signal_nr]->edfhdr->edfparam[signalcomp[signal_nr]->edfsignal[0]].physdimension);
    QMessageBox::critical(this, "Error", str);
    return;
  }

  if(signalcomp[signal_nr]->zratio_filter != NULL)
  {
    QMessageBox::critical(this, "Error", "Z-ratio is active for this signal!");
    return;
  }

  UI_QRS_detector ui_qrs_det(this, signalcomp[signal_nr]);

  enable_hrv_stats_toolbar(ecg_qrs_rpeak_descr, &signalcomp[signal_nr]->edfhdr->annot_list, signalcomp[signal_nr]);
}


void UI_Mainwindow::export_filtered_signals()
{
  UI_ExportFilteredSignalsWindow filt_signalswdw(this);
}


void UI_Mainwindow::export_annotations()
{
  if(!files_open)
  {
    QMessageBox::critical(this, "Error", "You have to open a file first.");
    return;
  }

  UI_ExportAnnotationswindow exportAnnotsDialog(this);
}


void UI_Mainwindow::import_annotations()
{
  UI_ImportAnnotationswindow importAnnotsDialog(this);
}


void UI_Mainwindow::check_edf_compatibility()
{
  if(!files_open)
  {
    QMessageBox::critical(this, "Error", "You have to open the file first.");
    return;
  }

  UI_EDFCompatwindow edfcompatdialog(this);
}


void UI_Mainwindow::unify_resolution()
{
  if(!files_open)
  {
    QMessageBox::critical(this, "Error", "You have to open a file first.");
    return;
  }

  UI_unify_resolution unify_resolution_dialog(this);
}


void UI_Mainwindow::print_to_img_640x480()
{
  maincurve->print_to_image(640, 480);
}


void UI_Mainwindow::print_to_img_800x600()
{
  maincurve->print_to_image(800, 600);
}


void UI_Mainwindow::print_to_img_1024x768()
{
  maincurve->print_to_image(1024, 768);
}


void UI_Mainwindow::print_to_img_1280x1024()
{
  maincurve->print_to_image(1280, 1024);
}


void UI_Mainwindow::print_to_img_1600x1200()
{
  maincurve->print_to_image(1600, 1200);
}


long long UI_Mainwindow::get_long_time(char *str)
{
  int i, len, hasdot=0, dotposition=0, neg=0;

  long long value=0, radix;

  if(str[0] == '+')
  {
    str++;
  }
  else if(str[0] == '-')
    {
      neg = 1;
      str++;
    }

  len = strlen(str);

  for(i=0; i<len; i++)
  {
    if(str[i]=='.')
    {
      hasdot = 1;
      dotposition = i;
      break;
    }
  }

  if(hasdot)
  {
    radix = TIME_DIMENSION;

    for(i=dotposition-1; i>=0; i--)
    {
        value += ((long long)(str[i] - 48)) * radix;
        radix *= 10;
    }

    radix = TIME_DIMENSION / 10;

    for(i=dotposition+1; i<len; i++)
    {
        value += ((long long)(str[i] - 48)) * radix;
        radix /= 10;
    }
  }
  else
  {
    radix = TIME_DIMENSION;

    for(i=len-1; i>=0; i--)
    {
        value += ((long long)(str[i] - 48)) * radix;
        radix *= 10;
    }
  }

  if(neg)  value = -value;

  return value;
}


void UI_Mainwindow::recent_file_action_func(QAction *action)
{
  int i, idx=-1;

  idx = action->data().toInt();

  if(idx == MAX_RECENTFILES)
  {
    recent_filesmenu->clear();

    for(i=0; i<MAX_RECENTFILES; i++)
    {
      recent_file_path[i][0] = 0;
    }

    return;
  }

  if(idx < MAX_RECENTFILES)
  {
    strlcpy(path, &recent_file_path[idx][0], MAX_PATH_LENGTH);

    cmdlineargument = 1;

    open_new_file();
  }
}


void UI_Mainwindow::recent_session_action_func(QAction *action)
{
  int i, button_nr=0, err, present=0, position=0, idx=-1;

  char session_path[MAX_PATH_LENGTH]="";

  QAction *act=NULL;

  idx = action->data().toInt();

  if(idx == MAX_RECENTFILES)
  {
    recent_session_menu->clear();

    for(i=0; i<MAX_RECENTFILES; i++)
    {
      recent_session_path[i][0] = 0;
    }

    return;
  }

  if(annotations_edited)
  {
    QMessageBox messagewindow;
    messagewindow.setText("There are unsaved annotations,\n are you sure you want to close this file?");
    messagewindow.setIcon(QMessageBox::Question);
    messagewindow.setStandardButtons(QMessageBox::Cancel | QMessageBox::Discard);
    messagewindow.setDefaultButton(QMessageBox::Cancel);
    button_nr = messagewindow.exec();
  }
  else if(files_open)
    {
      QMessageBox messagewindow;
      messagewindow.setText("This will close all files, continue?");
      messagewindow.setIcon(QMessageBox::Question);
      messagewindow.setStandardButtons(QMessageBox::Cancel | QMessageBox::Close);
      messagewindow.setDefaultButton(QMessageBox::Cancel);
      button_nr = messagewindow.exec();
    }

  if(button_nr == QMessageBox::Cancel)
  {
    return;
  }

  annotations_edited = 0;

  close_all_files();

  strlcpy(session_path, &recent_session_path[idx][0], MAX_PATH_LENGTH);
  if(!strcmp(session_path, ""))
  {
    return;
  }

  get_directory_from_path(recent_sessiondir, session_path, MAX_PATH_LENGTH);

  for(i=0, present=0; i<MAX_RECENTFILES; i++)
  {
//    printf("mainwindow.cpp: load_session(): recent_session_path[i]: ->%s<-\nsession_path: ->%s<-\n", recent_session_path[i], session_path);  //FIXME

    if(!strcmp(&recent_session_path[i][0], session_path))
    {
      present = 1;

      position = i;

      break;
    }
  }

  if(present)
  {
    for(i=position; i>0; i--)
    {
      strlcpy(&recent_session_path[i][0], &recent_session_path[i-1][0], MAX_PATH_LENGTH);
    }
  }
  else
  {
    for(i=MAX_RECENTFILES-1; i>0; i--)
    {
      strlcpy(&recent_session_path[i][0], &recent_session_path[i-1][0], MAX_PATH_LENGTH);
    }
  }

  strlcpy(&recent_session_path[0][0], session_path, MAX_PATH_LENGTH);

  recent_session_menu->clear();

  for(i=0; i<MAX_RECENTFILES; i++)
  {
    if(recent_session_path[i][0] == 0)
    {
      break;
    }
    act = new QAction(QString::fromLocal8Bit(&recent_session_path[i][0]), recent_session_menu);
    act->setData(QVariant(i));
    recent_session_menu->addAction(act);
  }
  recent_session_menu->addSeparator();
  act = new QAction("Clear list", recent_session_menu);
  act->setData(QVariant(MAX_RECENTFILES));
  recent_session_menu->addAction(act);

  err = read_session_file(session_path);

  if(err)
  {
    printf("load_session() returned error: %i\n", err);
  }
}


void UI_Mainwindow::save_montage()
{
  if(!files_open)  return;

  if(!signalcomps)  return;

  UI_SaveMontagewindow save_mtg(this);
}


void UI_Mainwindow::load_montage()
{
  if(!files_open)  return;

  UI_LoadMontagewindow load_mtg(this);
}


void UI_Mainwindow::view_montage()
{
  UI_ViewMontagewindow view_mtg(this);
}


void UI_Mainwindow::show_this_montage()
{
  UI_ShowActualMontagewindow show_mtg(this);
}


void UI_Mainwindow::print_to_edf()
{
  print_screen_to_edf(this);
}


void UI_Mainwindow::print_to_bdf()
{
  print_screen_to_bdf(this);
}


void UI_Mainwindow::set_dc_offset_to_zero(int n)
{
  int i;

  if(!signalcomps)
  {
    return;
  }

  for(i=0; i<signalcomps; i++)
  {
    if((n >= 0) && (n != i))
    {
      continue;
    }

    signalcomp[i]->screen_offset = 0.0;
  }

  maincurve->drawCurve_stage_1();
}


void UI_Mainwindow::show_help()
{
#ifdef Q_OS_LINUX
  QDesktopServices::openUrl(QUrl("file:///usr/share/doc/edfbrowser/manual.html"));
#endif

#ifdef Q_OS_WIN32
  char p_path[MAX_PATH_LENGTH];

  strlcpy(p_path, "file:///", MAX_PATH_LENGTH);
  strlcat(p_path, specialFolder(CSIDL_PROGRAM_FILES).toLocal8Bit().data(), MAX_PATH_LENGTH);
  strlcat(p_path, "\\EDFbrowser\\manual.html", MAX_PATH_LENGTH);
  QDesktopServices::openUrl(QUrl(p_path));
#endif
}


void UI_Mainwindow::show_kb_shortcuts()
{
  QMessageBox messagewindow(QMessageBox::NoIcon,
   "Keyboard shortcuts",
   "PgDn\t\tnext page\n"
   "PgUp\t\tformer page\n"
   "Right Arrow\tshift right one tenth of pagetime\n"
   "Left Arrow\t\tshift left one tenth of pagetime\n"
   "Plus\t\tincrease amplitude\n"
   "Minus\t\tdecrease amplitude\n"
   "Up Arrow\t\tshift traces up\n"
   "Down Arrow\tshift traces down\n"
   "Ctrl-Home\t\tgo to start of file\n"
   "Ctrl-End\t\tgo to end of file\n"
   "Ctrl++\t\tzoom in horizontally\n"
   "Ctrl+-\t\tzoom out horizontally\n"
   "F1 - F8\t\tload predefined montage\n"
   "Alt+Shift+C\t\tcrosshair\n"
   "(repeat for second crosshair)\n"
   "Esc\t\tremove crosshairs or floating ruler\n"

   "\nafter zooming in by drawing a rectangle:\n"
   "Backspace\t\tzoom back\n"
   "Insert\t\tzoom in\n"
#ifdef Q_OS_WIN32
   "\nCtrl+O\t\tOpen a file\n"
   "Ctrl+Shift+O\tOpen a stream\n"
   "Ctrl+F4\tClose all files\n"
   "Alt+F4\tExit program\n"
#else
   "\nCtrl+O\t\tOpen a file\n"
   "Ctrl+Shift+O\tOpen a stream\n"
   "Ctrl+W\t\tClose all files\n"
   "Ctrl+Q\t\tExit program\n"
#endif
   "\nMousewheel\tshift left or right\n"
   "Ctrl+Mousewheel\tzoom in or out horizontally\n"
   "Keep middle mousebutton pressed to drag horizontally\n"

   "\nCtrl+Space\t\tToggle Playback or Pause\n"
   "Ctrl+Shift+V\tToggle Playback with video\n"

   "\nAnnotation editor user buttons: Ctrl +1, Ctrl + 2, etc.\n",
   QMessageBox::Close
  );

  messagewindow.exec();
}


#ifdef Q_OS_WIN32

QString UI_Mainwindow::specialFolder(int type)
{
    QString result;

    QLibrary library(QLatin1String("shell32"));
    QT_WA( {
        typedef BOOL (WINAPI*GetSpecialFolderPath)(HWND, LPTSTR, int, BOOL);
        GetSpecialFolderPath SHGetSpecialFolderPath = (GetSpecialFolderPath)library.resolve("SHGetSpecialFolderPathW");
        if (SHGetSpecialFolderPath) {
            TCHAR path_w[MAX_PATH];
            SHGetSpecialFolderPath(0, path_w, type, false);
            result = QString::fromUtf16((ushort*)path_w);
        }
    } , {
        typedef BOOL (WINAPI*GetSpecialFolderPath)(HWND, char*, int, BOOL);
        GetSpecialFolderPath SHGetSpecialFolderPath = (GetSpecialFolderPath)library.resolve("SHGetSpecialFolderPathA");
        if (SHGetSpecialFolderPath) {
            char path_w[MAX_PATH];
            SHGetSpecialFolderPath(0, path_w, type, false);
            result = QString::fromLocal8Bit(path_w);
        }
    } );

    if (result.isEmpty()) {
         result = QLatin1String("C:\\temp");
    }

    return result;
}

#endif


int UI_Mainwindow::file_is_opened(const char *o_path)
{
  int i;


  for(i=0; i<files_open; i++)
  {
    if(!strcmp(edfheaderlist[i]->filename, o_path))
    {
      return 1;
    }
  }

  return 0;
}


void UI_Mainwindow::organize_signals()
{
  UI_SignalChooser signalchooser(this, 3);
}


void UI_Mainwindow::edfplus_remove_duplicate_annotations()
{
  int i, dup_cnt=0, cnt;

  char str[1024];

  if(!files_open)
  {
    QMessageBox::critical(this, "Error", "You have to open a file first.");
    return;
  }

  QProgressDialog progress("Checking for duplicates...", "Abort", 0, 10, this);
  progress.setWindowModality(Qt::WindowModal);
  progress.setMinimumDuration(200);

  for(i=0; i<files_open; i++)
  {
    cnt = edfplus_annotation_remove_duplicates(&edfheaderlist[i]->annot_list);

    dup_cnt += cnt;
  }

  progress.reset();

  if(dup_cnt)
  {
    for(i=0; i<files_open; i++)
    {
      if(annotations_dock[i] != NULL)
      {
        annotations_dock[i]->updateList(0);
      }
    }

    annotations_edited = 1;

    save_act->setEnabled(true);
  }

  snprintf(str, 256, "Removed %i duplicates from the annotationlist(s)", dup_cnt);
  QMessageBox::information(this, "Ready", str);
}


struct signalcompblock * UI_Mainwindow::create_signalcomp_copy(struct signalcompblock *original_signalcomp)
{
  int i;

  char spec_str[256],
       *filter_spec,
       *err;

  struct signalcompblock *newsignalcomp;

  if(signalcomps >= MAXSIGNALS)
  {
    return NULL;
  }

  if(original_signalcomp->ecg_filter != NULL)
  {
    return NULL;
  }

  if(original_signalcomp->zratio_filter != NULL)
  {
    return NULL;
  }

  newsignalcomp = (struct signalcompblock *)calloc(1, sizeof(struct signalcompblock));
  if(newsignalcomp == NULL)
  {
    QMessageBox::critical(this, "Error", "malloc() error");
    return NULL;
  }

  *newsignalcomp = *original_signalcomp;

  if(newsignalcomp->spike_filter)
  {
    newsignalcomp->spike_filter = create_spike_filter_copy(original_signalcomp->spike_filter);
    if(newsignalcomp->spike_filter == NULL)
    {
      QMessageBox::critical(this, "Error", "malloc() error");
      free(signalcomp);
      return NULL;
    }
  }

  if(newsignalcomp->plif_ecg_filter)
  {
    newsignalcomp->plif_ecg_filter = plif_subtract_filter_create_copy(original_signalcomp->plif_ecg_filter);
    if(newsignalcomp->plif_ecg_filter == NULL)
    {
      QMessageBox::critical(this, "Error", "malloc() error");
      free(signalcomp);
      return NULL;
    }
  }

  newsignalcomp->plif_ecg_filter_sav = NULL;

  for(i=0; i<newsignalcomp->filter_cnt; i++)
  {
    newsignalcomp->filter[i] = create_filter_copy(original_signalcomp->filter[i]);
    if(newsignalcomp->filter[i] == NULL)
    {
      QMessageBox::critical(this, "Error", "malloc() error");
      free(signalcomp);
      return NULL;
    }
  }

  for(i=0; i<newsignalcomp->ravg_filter_cnt; i++)
  {
    newsignalcomp->ravg_filter[i] = create_ravg_filter_copy(original_signalcomp->ravg_filter[i]);
    if(newsignalcomp->ravg_filter[i] == NULL)
    {
      QMessageBox::critical(this, "Error", "malloc() error");
      free(signalcomp);
      return NULL;
    }
  }

  if(original_signalcomp->fir_filter != NULL)
  {
    newsignalcomp->fir_filter = create_fir_filter_copy(original_signalcomp->fir_filter);
    if(newsignalcomp->fir_filter == NULL)
    {
      QMessageBox::critical(this, "Error", "malloc() error");
      free(signalcomp);
      return NULL;
    }
  }

  for(i=0; i<newsignalcomp->fidfilter_cnt; i++)
  {
    if(newsignalcomp->fidfilter_type[i] == 0)
    {
      if(newsignalcomp->fidfilter_model[i] == 0)
      {
        snprintf(spec_str, 256, "HpBu%i/%f", newsignalcomp->fidfilter_order[i], newsignalcomp->fidfilter_freq[i]);
      }

      if(newsignalcomp->fidfilter_model[i] == 1)
      {
        snprintf(spec_str, 256, "HpCh%i/%f/%f", newsignalcomp->fidfilter_order[i], newsignalcomp->fidfilter_ripple[i], newsignalcomp->fidfilter_freq[i]);
      }

      if(newsignalcomp->fidfilter_model[i] == 2)
      {
        snprintf(spec_str, 256, "HpBe%i/%f", newsignalcomp->fidfilter_order[i], newsignalcomp->fidfilter_freq[i]);
      }
    }

    if(newsignalcomp->fidfilter_type[i] == 1)
    {
      if(newsignalcomp->fidfilter_model[i] == 0)
      {
        snprintf(spec_str, 256, "LpBu%i/%f", newsignalcomp->fidfilter_order[i], newsignalcomp->fidfilter_freq[i]);
      }

      if(newsignalcomp->fidfilter_model[i] == 1)
      {
        snprintf(spec_str, 256, "LpCh%i/%f/%f", newsignalcomp->fidfilter_order[i], newsignalcomp->fidfilter_ripple[i], newsignalcomp->fidfilter_freq[i]);
      }

      if(newsignalcomp->fidfilter_model[i] == 2)
      {
        snprintf(spec_str, 256, "LpBe%i/%f", newsignalcomp->fidfilter_order[i], newsignalcomp->fidfilter_freq[i]);
      }
    }

    if(newsignalcomp->fidfilter_type[i] == 2)
    {
      if(newsignalcomp->fidfilter_model[i] == 0)
      {
        snprintf(spec_str, 256, "BsRe/%i/%f", newsignalcomp->fidfilter_order[i], newsignalcomp->fidfilter_freq[i]);
      }
    }

    if(newsignalcomp->fidfilter_type[i] == 3)
    {
      if(newsignalcomp->fidfilter_model[i] == 0)
      {
        snprintf(spec_str, 256, "BpBu%i/%f-%f", newsignalcomp->fidfilter_order[i], newsignalcomp->fidfilter_freq[i], newsignalcomp->fidfilter_freq2[i]);
      }

      if(newsignalcomp->fidfilter_model[i] == 1)
      {
        snprintf(spec_str, 256, "BpCh%i/%f/%f-%f", newsignalcomp->fidfilter_order[i], newsignalcomp->fidfilter_ripple[i], newsignalcomp->fidfilter_freq[i], newsignalcomp->fidfilter_freq2[i]);
      }

      if(newsignalcomp->fidfilter_model[i] == 2)
      {
        snprintf(spec_str, 256, "BpBe%i/%f-%f", newsignalcomp->fidfilter_order[i], newsignalcomp->fidfilter_freq[i], newsignalcomp->fidfilter_freq2[i]);
      }
    }

    if(newsignalcomp->fidfilter_type[i] == 4)
    {
      if(newsignalcomp->fidfilter_model[i] == 0)
      {
        snprintf(spec_str, 256, "BsBu%i/%f-%f", newsignalcomp->fidfilter_order[i], newsignalcomp->fidfilter_freq[i], newsignalcomp->fidfilter_freq2[i]);
      }

      if(newsignalcomp->fidfilter_model[i] == 1)
      {
        snprintf(spec_str, 256, "BsCh%i/%f/%f-%f", newsignalcomp->fidfilter_order[i], newsignalcomp->fidfilter_ripple[i], newsignalcomp->fidfilter_freq[i], newsignalcomp->fidfilter_freq2[i]);
      }

      if(newsignalcomp->fidfilter_model[i] == 2)
      {
        snprintf(spec_str, 256, "BsBe%i/%f-%f", newsignalcomp->fidfilter_order[i], newsignalcomp->fidfilter_freq[i], newsignalcomp->fidfilter_freq2[i]);
      }
    }

    filter_spec = spec_str;

    err = fid_parse(newsignalcomp->edfhdr->edfparam[newsignalcomp->edfsignal[0]].sf_f,
                    &filter_spec,
                    &newsignalcomp->fidfilter[i]);

    if(err != NULL)
    {
      QMessageBox::critical(this, "Error", err);
      free(err);
      free(signalcomp);
      return NULL;
    }

    newsignalcomp->fid_run[i] = fid_run_new(newsignalcomp->fidfilter[i], &newsignalcomp->fidfuncp[i]);

    newsignalcomp->fidbuf[i] = fid_run_newbuf(newsignalcomp->fid_run[i]);
    newsignalcomp->fidbuf2[i] = fid_run_newbuf(newsignalcomp->fid_run[i]);

//    memcpy(newsignalcomp->fidbuf[i], original_signalcomp->fidbuf[i], fid_run_bufsize(original_signalcomp->fid_run[i]));
//    memcpy(newsignalcomp->fidbuf2[i], original_signalcomp->fidbuf2[i], fid_run_bufsize(original_signalcomp->fid_run[i]));
  }

  signalcomp[signalcomps] = newsignalcomp;

  signalcomps++;

  return newsignalcomp;
}


void UI_Mainwindow::annot_dock_updated(void)
{
  emit annot_docklist_changed();
}


int UI_Mainwindow::get_filenum(struct edfhdrblock *ptr)
{
  int i;

  for(i=0; i<files_open; i++)
  {
    if(ptr == edfheaderlist[i])
    {
      return i;
    }
  }

  return -1;
}


void UI_Mainwindow::enable_hrv_stats_toolbar(const char *annotation, struct annotation_list *annot_list, struct signalcompblock *sigcomp)
{
  char annot_descr[MAX_ANNOTATION_LEN]="";

  if(annotation == NULL)  return;

  strlcpy(annot_descr, annotation, MAX_ANNOTATION_LEN);

  if(sigcomp != NULL)
  {
    if(use_signallabel_in_annot_descr)
    {
      strlcat(annot_descr, " ", MAX_ANNOTATION_LEN);
      if(strlen(sigcomp->alias))
      {
        strlcat(annot_descr, sigcomp->alias, MAX_ANNOTATION_LEN);
      }
      else
      {
        strlcat(annot_descr, sigcomp->signallabel, MAX_ANNOTATION_LEN);
      }
    }
  }

  strlcpy(toolbar_stats.annot_label, annot_descr, MAX_ANNOTATION_LEN);

  toolbar_stats.annot_list = annot_list;

  toolbar_stats.sz = 0;

  toolbar_stats.active = 1;
}


void UI_Mainwindow::get_unique_annotations(struct edfhdrblock *hdr)
{
  int i, j, listsz;

  struct annotationblock *annot;

  if(hdr == NULL)  return;

  hdr->unique_annotations_list[0][0] = 0;

  listsz = edfplus_annotation_size(&hdr->annot_list);

  for(i=0; i<listsz; i++)
  {
    annot = edfplus_annotation_get_item(&hdr->annot_list, i);

    for(j=0; j<MAX_UNIQUE_ANNOTATIONS; j++)
    {
      if(hdr->unique_annotations_list[j][0] == 0)
      {
        strlcpy(hdr->unique_annotations_list[j], annot->description, MAX_ANNOTATION_LEN);

        if(j < (MAX_UNIQUE_ANNOTATIONS - 1))
        {
          hdr->unique_annotations_list[j+1][0] = 0;
        }

        break;
      }

      if(!strcmp(annot->description, hdr->unique_annotations_list[j]))  break;
    }
  }
}


void UI_Mainwindow::create_annot_by_rect_draw()
{
  int i;

  if((!annot_editor_active) || (annotationEditDock == NULL))
  {
    return;
  }

  for(i=0; i<MAX_ANNOTEDIT_SIDE_MENU_ANNOTS; i++)
  {
    if(strlen(annot_by_rect_draw_description[i]))
    {
      break;
    }
  }
  if(i == MAX_ANNOTEDIT_SIDE_MENU_ANNOTS)
  {
    return;
  }

  annotationEditDock->process_annot_by_rect_draw();
}


void UI_Mainwindow::dig_min_max_overflow_timer_func()
{
  if(dig_min_max_overflow && (!dig_min_max_overflow_warning_showed))
  {
    dig_min_max_overflow_warning_showed = 1;

    QMessageBox::warning(this, "Warning",
                        "Detected some samples that have a value higher than the maximum specified in the EDF header and/or samples that have a value lower than the minimum specified in the EDF header.\n"
                        "Please check your EDF file using: Tools -> Check EDF/BDF compatibility\n"
                        "You can continue browsing this EDF file but all samples outside the maximum - minimum range will be clipped."
    );
  }
}


void UI_Mainwindow::remove_signalcomp(int signal_nr)
{
  int i, j, p;

  if((signal_nr < 0) || (signal_nr >= signalcomps))
  {
    return;
  }

  for(i=0; i<MAXSPECTRUMDOCKS; i++)
  {
    if(spectrumdock[i]->signalcomp == signalcomp[signal_nr])
    {
      spectrumdock[i]->clear();
      spectrumdock[i]->dock->hide();
    }
  }

  for(i=0; i<MAXSPECTRUMDIALOGS; i++)
  {
    p = signalcomp[signal_nr]->spectr_dialog[i];

    if(p != 0)
    {
      delete spectrumdialog[p - 1];

      spectrumdialog[p - 1] = NULL;
    }
  }

  for(i=0; i<MAXCDSADOCKS; i++)
  {
    p = signalcomp[signal_nr]->cdsa_idx[i];

    if(p != 0)
    {
      delete cdsa_dock[p - 1];

      cdsa_dock[p - 1] = NULL;
    }
  }

  for(i=0; i<MAXAVERAGECURVEDIALOGS; i++)
  {
    p = signalcomp[signal_nr]->avg_dialog[i];

    if(p != 0)
    {
      delete averagecurvedialog[p - 1];

      averagecurvedialog[p - 1] = NULL;
    }
  }

  for(i=0; i<MAXZSCOREDIALOGS; i++)
  {
    p = signalcomp[signal_nr]->zscoredialog[i];

    if(p != 0)
    {
      delete zscoredialog[p - 1];

      zscoredialog[p - 1] = NULL;
    }
  }

  if(signalcomp[signal_nr]->hascursor2)
  {
    maincurve->crosshair_2.active = 0;
    maincurve->crosshair_2.moving = 0;
  }

  if(signalcomp[signal_nr]->hascursor1)
  {
    maincurve->crosshair_1.active = 0;
    maincurve->crosshair_2.active = 0;
    maincurve->crosshair_1.moving = 0;
    maincurve->crosshair_2.moving = 0;

    for(i=0; i<signalcomps; i++)
    {
      signalcomp[i]->hascursor2 = 0;
    }
  }

  if(signalcomp[signal_nr]->hasruler)
  {
    maincurve->ruler_active = 0;
    maincurve->ruler_moving = 0;
  }

  for(j=0; j<signalcomp[signal_nr]->filter_cnt; j++)
  {
    free(signalcomp[signal_nr]->filter[j]);
  }

  signalcomp[signal_nr]->filter_cnt = 0;

  if(signalcomp[signal_nr]->plif_ecg_filter)
  {
    plif_free_subtract_filter(signalcomp[signal_nr]->plif_ecg_filter);

    signalcomp[signal_nr]->plif_ecg_filter = NULL;
  }

  if(signalcomp[signal_nr]->plif_ecg_filter_sav)
  {
    plif_free_subtract_filter(signalcomp[signal_nr]->plif_ecg_filter_sav);

    signalcomp[signal_nr]->plif_ecg_filter_sav = NULL;
  }

  if(signalcomp[signal_nr]->spike_filter)
  {
    free_spike_filter(signalcomp[signal_nr]->spike_filter);

    signalcomp[signal_nr]->spike_filter = NULL;
  }

  for(j=0; j<signalcomp[signal_nr]->ravg_filter_cnt; j++)
  {
    free_ravg_filter(signalcomp[signal_nr]->ravg_filter[j]);
  }

  signalcomp[signal_nr]->ravg_filter_cnt = 0;

  if(signalcomp[signal_nr]->fir_filter != NULL)
  {
    free_fir_filter(signalcomp[signal_nr]->fir_filter);

    signalcomp[signal_nr]->fir_filter = NULL;
  }

  if(signalcomp[signal_nr]->ecg_filter != NULL)
  {
    free_ecg_filter(signalcomp[signal_nr]->ecg_filter);

    signalcomp[signal_nr]->ecg_filter = NULL;

    strlcpy(signalcomp[signal_nr]->signallabel, signalcomp[signal_nr]->signallabel_bu, 512);
    signalcomp[signal_nr]->signallabellen = signalcomp[signal_nr]->signallabellen_bu;
    strlcpy(signalcomp[signal_nr]->physdimension, signalcomp[signal_nr]->physdimension_bu, 32);
  }

  if(signalcomp[signal_nr]->zratio_filter != NULL)
  {
    free_zratio_filter(signalcomp[signal_nr]->zratio_filter);

    signalcomp[signal_nr]->zratio_filter = NULL;

    strlcpy(signalcomp[signal_nr]->signallabel, signalcomp[signal_nr]->signallabel_bu, 512);
    signalcomp[signal_nr]->signallabellen = signalcomp[signal_nr]->signallabellen_bu;
    strlcpy(signalcomp[signal_nr]->physdimension, signalcomp[signal_nr]->physdimension_bu, 9);
  }

  for(j=0; j<signalcomp[signal_nr]->fidfilter_cnt; j++)
  {
    free(signalcomp[signal_nr]->fidfilter[j]);
    fid_run_free(signalcomp[signal_nr]->fid_run[j]);
    fid_run_freebuf(signalcomp[signal_nr]->fidbuf[j]);
    fid_run_freebuf(signalcomp[signal_nr]->fidbuf2[j]);
  }

  signalcomp[signal_nr]->fidfilter_cnt = 0;

  free(signalcomp[signal_nr]);

  signalcomp[signal_nr] = NULL;

  for(i=signal_nr; i<signalcomps - 1; i++)
  {
    signalcomp[i] = signalcomp[i + 1];
  }

  signalcomps--;

  if(!signalcomps)
  {
    free(viewbuf);
    viewbuf = NULL;

    slidertoolbar->setEnabled(false);
    positionslider->blockSignals(true);
  }
}


int UI_Mainwindow::get_signalcomp_number(const char *label)
{
  int i;

  char str1[512]="",
       str2[512]="";

  strlcpy(str1, label, 512);
  strip_types_from_label(str1);
  trim_spaces(str1);

  for(i=0; i<signalcomps; i++)
  {
    strlcpy(str2, signalcomp[i]->signallabel, 512);
    trim_spaces(str2);
    if(!strcmp(str2, str1))
    {
      return i;
    }
  }

  return -1;
}

/* inv: 0 - not inverted
 * inv: 1 - inverted
 * inv: 2 - toggle
 *
 * n: default: -1: apply to all signals
 * n: >= 0: apply one signal only
 */
void UI_Mainwindow::signalcomp_invert(int inv, int n)
{
  int i;

  for(i=0; i<signalcomps; i++)
  {
    if((n >= 0) && (n != i))  continue;

    if((inv == 0) && (signalcomp[i]->polarity == -1))  /* not inverted */
    {
      signalcomp[i]->polarity *= -1;

      signalcomp[i]->screen_offset *= -1;
    }
    else if((inv == 1) && (signalcomp[i]->polarity == 1))  /* inverted */
      {
        signalcomp[i]->polarity *= -1;

        signalcomp[i]->screen_offset *= -1;
      }
      else if(inv == 2)  /* toggle */
      {
        signalcomp[i]->polarity *= -1;

        signalcomp[i]->screen_offset *= -1;
      }
  }

  setup_viewbuf();
}


void UI_Mainwindow::desktop_resized(int)
{
  QFontMetrics fm(*myfont);

  font_pixel_height = fm.ascent();

#if QT_VERSION >= 0x050B00
  font_pixel_width = (fm.horizontalAdvance("ABCDEFGHIJKLMNOPQRSTUVWXYZ") / 26.0) + 0.5;
#else
  font_pixel_width = (fm.width("ABCDEFGHIJKLMNOPQRSTUVWXYZ") / 26.0) + 0.5;
#endif
//  printf("font_pixel_height: %i    font_pixel_width: %i\n", font_pixel_height, font_pixel_width);

  h_scaling = fm.height() / 18.0;

#if QT_VERSION >= 0x050B00
  w_scaling = fm.horizontalAdvance("ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789") / 260.0;
#else
  w_scaling = fm.width("ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789") / 260.0;
#endif

  if(w_scaling > 4.0)  w_scaling = 4.0;
  if(w_scaling < 1.0)  w_scaling = 1.0;

  if(h_scaling > 4.0)  h_scaling = 4.0;
  if(h_scaling < 1.0)  h_scaling = 1.0;

//  printf("w_scaling: %f    h_scaling: %f\n", w_scaling, h_scaling);

  dpix = maincurve->physicalDpiX();

  dpiy = maincurve->physicalDpiY();

//  printf("dpix: %i    dpiy: %i\n", dpix, dpiy);

  if(auto_dpi)
  {
    y_pixelsizefactor = 2.54 / dpiy;

    x_pixelsizefactor = 2.54 / dpix;
  }

  maincurve->update();
}


void UI_Mainwindow::screen_changed(QScreen *)
{
  desktop_resized(0);
}












