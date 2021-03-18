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



#include "run_qrs_detector.h"



UI_QRS_detector::UI_QRS_detector(QWidget *w_parent, struct signalcompblock *signalcomp_)
{
  int i,
      sense=1,
      total_datrecs=0,
      beat_cnt=0,
      filenum=-1,
      instance_num=0;

  char str[32]="",
       rpeak_descr[MAX_ANNOTATION_LEN]="";

  double *processed_samples_buf=NULL;

  long long l_time=0LL,
            l_time2=0LL,
            smpls_left=0,
            *beat_onset_list=NULL;

  struct annotationblock annotation;

  FilteredBlockReadClass fbr;

  QProgressDialog progress("Processing file...", "Abort", 0, 1);
  progress.setWindowModality(Qt::WindowModal);
  progress.setMinimumDuration(200);
  progress.reset();

  if(signalcomp_ == NULL)  return;

  if(w_parent == NULL)  return;

  mainwindow = (UI_Mainwindow *)w_parent;

  signalcomp = signalcomp_;

  filenum = mainwindow->get_filenum(signalcomp->edfhdr);
  if(filenum < 0)
  {
    QMessageBox::critical(mainwindow, "Error", "Internal error: filenum < 0", QMessageBox::Close);
    return;
  }

  sf = signalcomp->edfhdr->edfparam[signalcomp->edfsignal[0]].sf_f;
  if(sf < 199.999)  return;

  strlcpy(str, signalcomp->edfhdr->edfparam[signalcomp->edfsignal[0]].physdimension, 32);
  trim_spaces(str);
  if((!strcmp(str, "uV")) || (!strcmp(str, "ECG uV")) || (!strcmp(str, "EEG uV")))
  {
    sense = 1;
  }
  else if((!strcmp(str, "mV")) || (!strcmp(str, "ECG mV")) || (!strcmp(str, "EEG mV")))
    {
      sense = 1000;
    }
    else if((!strcmp(str, "V")) || (!strcmp(str, "ECG V")) || (!strcmp(str, "EEG V")))
      {
        sense = 1000000;
      }
      else
      {
        return;
      }

  if(signalcomp->edfhdr->datarecords < 1)  return;

  ecg_filter_bu = signalcomp->ecg_filter;

  signalcomp->ecg_filter = create_ecg_filter(sf, signalcomp->edfhdr->edfparam[signalcomp->edfsignal[0]].bitvalue, sense);
  if(signalcomp->ecg_filter == NULL)
  {
    goto OUT_EXIT_RETURN;
  }

  total_datrecs = signalcomp->edfhdr->datarecords;

  processed_samples_buf = fbr.init_signalcomp(signalcomp, 1, 0, 0, NULL);
  if(processed_samples_buf == NULL)
  {
    goto OUT_EXIT_RETURN;
  }

  progress.setRange(0, total_datrecs);
  progress.setValue(0);

  for(i=0; i<total_datrecs; i++)
  {
    if(!(i % 100))
    {
      progress.setValue(i);

      qApp->processEvents();

      if(progress.wasCanceled() == true)
      {
        goto OUT_EXIT_RETURN;
      }
    }

    if(fbr.process_signalcomp(i))
    {
      goto OUT_EXIT_RETURN;
    }
  }

  progress.reset();

  beat_cnt = ecg_filter_get_beat_cnt(signalcomp->ecg_filter);

  beat_onset_list = ecg_filter_get_onset_beatlist(signalcomp->ecg_filter);

  if(beat_cnt < 1)
  {
    goto OUT_EXIT_RETURN;
  }

  if(mainwindow->annotationlist_backup==NULL)
  {
    mainwindow->annotationlist_backup = edfplus_annotation_create_list_copy(&mainwindow->edfheaderlist[filenum]->annot_list);
  }

  strlcpy(rpeak_descr, mainwindow->ecg_qrs_rpeak_descr, MAX_ANNOTATION_LEN);
  if(mainwindow->use_signallabel_in_annot_descr)
  {
    strlcat(rpeak_descr, " ", MAX_ANNOTATION_LEN);
    if(strlen(signalcomp->alias))
    {
      strlcat(rpeak_descr, signalcomp->alias, MAX_ANNOTATION_LEN);
    }
    else
    {
      strlcat(rpeak_descr, signalcomp->signallabel, MAX_ANNOTATION_LEN);
    }
  }

  memset(&annotation, 0, sizeof(struct annotationblock));
  strlcpy(annotation.description, rpeak_descr, MAX_ANNOTATION_LEN);

  for(i=0; i<beat_cnt; i++)
  {
    l_time2 = beat_onset_list[i] / signalcomp->edfhdr->edfparam[signalcomp->edfsignal[0]].smp_per_record;

    smpls_left = beat_onset_list[i] % signalcomp->edfhdr->edfparam[signalcomp->edfsignal[0]].smp_per_record;

    l_time = (l_time2 * signalcomp->edfhdr->long_data_record_duration);

    l_time += ((smpls_left * signalcomp->edfhdr->long_data_record_duration) / signalcomp->edfhdr->edfparam[signalcomp->edfsignal[0]].smp_per_record);

    annotation.onset = l_time;
    annotation.edfhdr = mainwindow->edfheaderlist[filenum];
    edfplus_annotation_add_item(&mainwindow->edfheaderlist[filenum]->annot_list, annotation);
  }

  if(mainwindow->annotations_dock[filenum] == NULL)
  {
    mainwindow->annotations_dock[filenum] = new UI_Annotationswindow(signalcomp->edfhdr, mainwindow);

    mainwindow->addDockWidget(Qt::RightDockWidgetArea, mainwindow->annotations_dock[filenum]->docklist, Qt::Vertical);

    if(edfplus_annotation_size(&mainwindow->edfheaderlist[filenum]->annot_list) < 1)
    {
      mainwindow->annotations_dock[filenum]->docklist->hide();
    }
  }

  if(edfplus_annotation_size(&mainwindow->edfheaderlist[filenum]->annot_list) > 0)
  {
    mainwindow->annotations_dock[filenum]->docklist->show();

    mainwindow->annotations_edited = 1;

    mainwindow->annotations_dock[filenum]->updateList();

    mainwindow->save_act->setEnabled(true);
  }

  struct hrv_dock_param_struct dock_param;

  memset(&dock_param, 0, sizeof(struct hrv_dock_param_struct));

  for(instance_num=0; instance_num<MAXHRVDOCKS; instance_num++)
  {
    if(mainwindow->hrv_dock[instance_num] == NULL)
    {
      break;
    }
  }

  if(instance_num == MAXHRVDOCKS)
  {
    goto OUT_EXIT_RETURN;
  }

  dock_param.instance_num = instance_num;

  dock_param.edfhdr = signalcomp->edfhdr;

  dock_param.mainwindow = mainwindow;

  strlcpy(dock_param.annot_name, rpeak_descr, 64);

  mainwindow->hrv_dock[instance_num] = new UI_hrv_dock(mainwindow, dock_param);

  mainwindow->addToolBar(Qt::BottomToolBarArea, mainwindow->hrv_dock[instance_num]->hrv_dock);

  mainwindow->insertToolBarBreak(mainwindow->hrv_dock[instance_num]->hrv_dock);

  signalcomp->edfhdr->hrv_dock[instance_num] = instance_num + 1;

  QObject::connect(mainwindow, SIGNAL(annot_docklist_changed()), mainwindow->hrv_dock[instance_num], SLOT(update_curve()));

OUT_EXIT_RETURN:

  progress.reset();

  free_ecg_filter(signalcomp->ecg_filter);

  signalcomp->ecg_filter = ecg_filter_bu;

  mainwindow->setup_viewbuf();
}



















