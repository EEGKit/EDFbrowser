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


#ifndef EDFBROWSER_GLOBAL_H
#define EDFBROWSER_GLOBAL_H

#if !defined(__GNUC__)
#if defined(__APPLE__) || defined(__MACH__) || defined(__APPLE_CC__)
/* nothing */
#else
#error "You need the GNU C compiler!"
#endif
#endif

#if defined(__LP64__) || defined(__MINGW64__)
#define THIS_APP_BITS_W   "64-bit"
#else
#define THIS_APP_BITS_W   "32-bit"
#endif

#include <stdio.h>

#if defined(__APPLE__) || defined(__MACH__) || defined(__APPLE_CC__) || defined(__HAIKU__)

#define fopeno fopen

#else

#define fseeko fseeko64
#define ftello ftello64
#define fopeno fopen64

#endif

#define QT_NO_DEPRECATED_WARNINGS

#define PROGRAM_NAME                "EDFbrowser"
#define PROGRAM_VERSION                   "1.88"
#define PROGRAM_BETA_SUFFIX                   ""
#define MINIMUM_QT4_VERSION             0x040701
#define MINIMUM_QT5_VERSION             0x050901
#define MAXFILES                            (32)
#define MAXSIGNALS                         (640)
#define MAXFILTERS                          (16)
#define TIME_DIMENSION                (10000000LL)
#define MAX_ANNOTATION_LEN                 (512)
#define MAX_UNIQUE_ANNOTATIONS              (64)
#define VIEWTIME_SYNCED_OFFSET               (0)
#define VIEWTIME_SYNCED_ABSOLUT              (1)
#define VIEWTIME_UNSYNCED                    (2)
#define VIEWTIME_USER_DEF_SYNCED             (3)
#define MAX_PATH_LENGTH                   (1024)
#define MAX_RECENTFILES                     (32)
#define MAX_ACTIVE_ANNOT_MARKERS            (64)
#define MAXSPECTRUMDIALOGS                  (32)
#define MAXSPECTRUMDOCKS                     (8)
#define MAXCDSADOCKS                        (64)
#define MAXHYPNOGRAMDOCKS                   (64)
#define MAXHRVDOCKS                         (64)
#define MAXPREDEFINEDMONTAGES               (12)
#define MAXAVERAGECURVEDIALOGS              (32)
#define MAXZSCOREDIALOGS                    (32)
#define MAXZOOMHISTORY                      (16)
#define MAX_ANNOTEDIT_SIDE_MENU_ANNOTS       (8)
/* 200 bpm x 60 min. x 24 hours x 2 days */
#define STATISTICS_IVAL_LIST_SZ         (576000)


#define ANNOT_ID_NK_TRIGGER       (0)
#define ANNOT_ID_BS_TRIGGER       (1)

#define VIDEO_STATUS_STOPPED      (0)
#define VIDEO_STATUS_STARTUP_1    (1)
#define VIDEO_STATUS_STARTUP_2    (2)
#define VIDEO_STATUS_STARTUP_3    (3)
#define VIDEO_STATUS_STARTUP_4    (4)
#define VIDEO_STATUS_STARTUP_5    (5)
#define VIDEO_STATUS_STARTUP_6    (6)
#define VIDEO_STATUS_PLAYING     (16)
#define VIDEO_STATUS_PAUSED      (17)
#define VIDEO_STATUS_ENDED       (18)

#define RC_CMD_LIST          (0)
#define RC_CMD_IDN           (1)
#define RC_CMD_QUIT          (2)
#define RC_CMD_RST           (3)
#define RC_CMD_CLS           (4)
#define RC_CMD_FAULT         (5)
#define RC_CMD_OPC           (6)
#define RC_CMD_FILE          (7)
#define RC_CMD_OPEN          (8)
#define RC_CMD_CLOSE         (9)
#define RC_CMD_ALL          (10)
#define RC_CMD_MONTAGE      (11)
#define RC_CMD_LOAD         (12)
#define RC_CMD_SIGNAL       (13)
#define RC_CMD_ADD          (14)
#define RC_CMD_LABEL        (15)
#define RC_CMD_AMPLITUDE    (16)
#define RC_CMD_FIT          (17)
#define RC_CMD_OFFSET       (18)
#define RC_CMD_ADJUST       (19)
#define RC_CMD_ZERO         (20)
#define RC_CMD_INVERT       (21)
#define RC_CMD_REMOVE       (22)
#define RC_CMD_TIMESCALE    (23)
#define RC_CMD_VIEWTIME     (24)

#define RC_CMD_LIST_SZ      (25)
#define RC_CMD_QUERY       (128)
#define RC_ERR_QUEUE_SZ     (16)


#include "filter.h"
#include "third_party/fidlib/fidlib.h"
#include "ravg_filter.h"
#include "spike_filter.h"
#include "ecg_filter.h"
#include "z_ratio_filter.h"
#include "edf_annot_list.h"
#include "plif_ecg_subtract_filter.h"
#include "fir_filter.h"


struct edfparamblock{
        char   label[17];
        char   transducer[81];
        char   physdimension[9];
        double phys_min;
        double phys_max;
        int    dig_min;
        int    dig_max;
        char   prefilter[81];
        int    smp_per_record;
        char   reserved[33];
        double offset;
        int    buf_offset;
        double bitvalue;
        int    annotation;
        int    sf_int;
        double sf_f;
        long long smpls;
      };

struct edfhdrblock{
        FILE      *file_hdl;
        char      version[32];
        char      filename[MAX_PATH_LENGTH];
        char      patient[81];
        char      recording[81];
        char      plus_patientcode[81];
        char      plus_gender[16];
        char      plus_birthdate[16];
        char      plus_patient_name[81];
        char      plus_patient_additional[81];
        char      plus_startdate[16];
        char      plus_admincode[81];
        char      plus_technician[81];
        char      plus_equipment[81];
        char      plus_recording_additional[81];
        int       plus_patientcode_offset;
        int       plus_gender_offset;
        int       plus_birthdate_offset;
        int       plus_patient_name_offset;
        int       plus_patient_additional_offset;
        int       plus_startdate_offset;
        int       plus_admincode_offset;
        int       plus_technician_offset;
        int       plus_equipment_offset;
        int       plus_recording_additional_offset;
        long long l_starttime;
        long long utc_starttime;
        char      reserved[45];
        int       hdrsize;
        int       edfsignals;
        long long datarecords;
        int       recordsize;
        int       annot_ch[256];
        int       nr_annot_chns;
        int       edf;
        int       edfplus;
        int       bdf;
        int       bdfplus;
        int       discontinuous;
        int       genuine_nk;
        int       nk_triggers_read;
        int       genuine_biosemi;
        double    data_record_duration;
        unsigned long long long_data_record_duration;
        long long viewtime;
        long long starttime_offset;
        long long prefiltertime;
        int       annots_not_read;
        int       recording_len_sec;
        struct    edfparamblock *edfparam;
        struct    annotation_list annot_list;
        char      unique_annotations_list[MAX_UNIQUE_ANNOTATIONS][MAX_ANNOTATION_LEN];
        int       hypnogram_dock[MAXHYPNOGRAMDOCKS];
        int       hrv_dock[MAXHRVDOCKS];
      };

struct signalcompblock{
        unsigned long long uid;
        struct edfhdrblock *edfhdr;
        int num_of_signals;
        unsigned long long viewbufsize;
        unsigned long long viewbufoffset;
        unsigned long long records_in_viewbuf;
        unsigned long long samples_in_viewbuf;
        unsigned long long samples_on_screen;
        unsigned long long sample_start;
        unsigned long long sample_stop;
        int timeoffset;
        int sample_timeoffset;
        int pixels_shift;
        double sample_timeoffset_part;
        double sample_pixel_ratio;
        int edfsignal[MAXSIGNALS];
        double factor[MAXSIGNALS];
        int polarity;
        double sensitivity[MAXSIGNALS];
        int oldvalue;
        long long oldsmplnr;
        long long file_duration;
        char signallabel[512];
        int signallabel_type_stripped;
        char alias[17];
        int signallabellen;
        char signallabel_bu[512];
        int signallabellen_bu;
        int hascursor1;
        int hascursor2;
        int hasoffsettracking;
        int hasgaintracking;
        int hasruler;
        double screen_offset;
        double voltpercm;
        char physdimension[9];
        char physdimension_bu[9];
        int color;
        int filter_cnt;
        int samples_in_prefilterbuf;
        long long prefilter_starttime;
        int prefilter_reset_sample;
        double filterpreset_a[MAXFILTERS];
        double filterpreset_b[MAXFILTERS];
        struct filter_settings *filter[MAXFILTERS];
        int max_dig_value;
        int min_dig_value;
        int fidfilter_cnt;
        int fidfilter_type[MAXFILTERS];
        double fidfilter_freq[MAXFILTERS];
        double fidfilter_freq2[MAXFILTERS];
        double fidfilter_ripple[MAXFILTERS];
        int fidfilter_order[MAXFILTERS];
        int fidfilter_model[MAXFILTERS];
        FidFilter *fidfilter[MAXFILTERS];
        FidRun *fid_run[MAXFILTERS];
        FidFunc *fidfuncp[MAXFILTERS];
        void *fidbuf[MAXFILTERS];
        void *fidbuf2[MAXFILTERS];
        int fidfilter_setup[MAXFILTERS];
        int stat_cnt;
        int stat_zero_crossing_cnt;
        double stat_sum;
        double stat_sum_sqr;
        double stat_sum_rectified;
        int draw_rectangle_sum_y;
        int draw_rectangle_sum_cnt;
        int ravg_filter_cnt;
        int ravg_filter_type[MAXFILTERS];
        int ravg_filter_size[MAXFILTERS];
        int ravg_filter_setup[MAXFILTERS];
        struct ravg_filter_settings *ravg_filter[MAXFILTERS];
        struct ecg_filter_settings *ecg_filter;
        struct fir_filter_settings *fir_filter;
        double spike_filter_velocity;
        int spike_filter_holdoff;
        struct spike_filter_settings *spike_filter;
        int plif_ecg_subtract_filter_plf;
        struct plif_subtract_filter_settings *plif_ecg_filter;
        struct plif_subtract_filter_settings *plif_ecg_filter_sav;
        struct zratio_filter_settings *zratio_filter;
        double zratio_crossoverfreq;
        int spectr_dialog[MAXSPECTRUMDIALOGS];
        int cdsa_dock[MAXCDSADOCKS];
        int avg_dialog[MAXAVERAGECURVEDIALOGS];
        int zscoredialog[MAXZSCOREDIALOGS];
        int annot_created_by_rect_draw_active;
        long long annot_created_by_rect_draw_onset;
        long long annot_created_by_rect_draw_duration;
      };

struct zoomhistoryblock{
        int pntr;
        int history_size_tail;
        int history_size_front;
        long long viewtime[MAXZOOMHISTORY][MAXFILES];
        long long pagetime[MAXZOOMHISTORY];
        double voltpercm[MAXZOOMHISTORY][MAXSIGNALS];
        double sensitivity[MAXZOOMHISTORY][MAXSIGNALS][MAXSIGNALS];
        double screen_offset[MAXZOOMHISTORY][MAXSIGNALS];
       };

struct active_markersblock{
        struct edfhdrblock *edf_hdr;
        struct annotationblock *list[MAX_ACTIVE_ANNOT_MARKERS];
        int count;
        int selected;
       };

struct graphicLineStruct{
        int x1;
        int y1;
        int x2;
        int y2;
        };

struct graphicBufStruct{
        struct graphicLineStruct graphicLine[MAXSIGNALS];
        };

struct crossHairStruct{
        int color;
        struct edfhdrblock *edf_hdr;
        int active;
        int moving;
        int position;
        int x_position;
        int y_position;
        int y_value;
        double value;
        long long time;
        long long time_relative;
        int has_hor_line;
        int dot_sz;
       };

struct video_player_struct{
        int status;
        int cntdwn_timer;
        int poll_timer;
        long long utc_starttime;
        int starttime_diff;
        int stop_det_counter;
        int fpos;
        int speed;
       };

struct annot_filter_struct{
       int tmin;
       int tmax;
       int invert;
       int hide_other;
       int hide_in_list_only;
       };

#endif



