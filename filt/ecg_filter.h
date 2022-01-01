/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2011 - 2022 Teunis van Beelen
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







#ifndef ecg_filter_INCLUDED
#define ecg_filter_INCLUDED



#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "pt_qrs.h"


#ifdef __cplusplus
extern "C" {
#endif




struct ecg_filter_settings
{
  struct pt_qrs_settings *pt_qrs,
                         *pt_qrs_bu;

  int qrs_smpl_offset_old;

  int smpl_n;
  double sf;
  double bpm;
  double bitvalue;

  int smpl_n_bu;
  double bpm_bu;
  int bu_filled;

  double *stat_buf;
  long long *stat_smpl_buf;
  int stat_buf_idx;
  int sample_cntr;
};


struct ecg_filter_settings * create_ecg_filter(double, double, double);
double run_ecg_filter(double, struct ecg_filter_settings *);
void free_ecg_filter(struct ecg_filter_settings *);
void ecg_filter_save_buf(struct ecg_filter_settings *);
void ecg_filter_restore_buf(struct ecg_filter_settings *);
void reset_ecg_filter(struct ecg_filter_settings *);
int ecg_filter_get_beat_cnt(struct ecg_filter_settings *);
long long * ecg_filter_get_onset_beatlist(struct ecg_filter_settings *);
double * ecg_filter_get_interval_beatlist(struct ecg_filter_settings *);


#ifdef __cplusplus
} /* extern "C" */
#endif


#endif





