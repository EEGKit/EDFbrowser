/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2010 - 2022 Teunis van Beelen
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



#include "edf_helper.h"


#define EDF_LABEL_TYPE_NUM  (7)


static const char edf_label_types[EDF_LABEL_TYPE_NUM][32]=
{
  "EEG             ",
  "ECG             ",
  "EOG             ",
  "ERG             ",
  "EMG             ",
  "MEG             ",
  "MCG             "
};


int edfplus_annotation_get_tal_timestamp_digit_cnt(struct edfhdrblock *hdr)
{
  int timestamp_digits;

  char scratchpad[256];

  long long time;


  if(hdr==NULL)
  {
    return -1;
  }

  time = (hdr->datarecords * hdr->long_data_record_duration) / TIME_DIMENSION;

#ifdef _WIN32
  timestamp_digits = __mingw_snprintf(scratchpad, 256, "%lli", time);
#else
  timestamp_digits = snprintf(scratchpad, 256, "%lli", time);
#endif

  return timestamp_digits;
}


int edfplus_annotation_get_tal_timestamp_decimal_cnt(struct edfhdrblock *hdr)
{
  int i, j,
      timestamp_decimals;


  if(hdr==NULL)
  {
    return -1;
  }

  j = 10;

  for(timestamp_decimals=7; timestamp_decimals>0; timestamp_decimals--)
  {
    if(hdr->long_data_record_duration % j)
    {
      break;
    }

    j *= 10;
  }

  if((hdr->edfplus==1)||(hdr->bdfplus==1))
  {
    j = 10;

    for(i=7; i>0; i--)
    {
      if(hdr->starttime_offset % j)
      {
        break;
      }

      j *= 10;
    }

    if(i > timestamp_decimals)
    {
      timestamp_decimals = i;
    }
  }

  return timestamp_decimals;
}


int strip_types_from_label(char *label)
{
  int i, type, len;

  len = strlen(label);
  if(len<5)
  {
    return 0;
  }

  for(type=0; type<EDF_LABEL_TYPE_NUM; type++)
  {
    if(!strncmp(label, edf_label_types[type], 4))
    {
      break;
    }
  }
  if(type == EDF_LABEL_TYPE_NUM)
  {
    return 0;
  }

  if(label[4] == ' ')
  {
    return 0;
  }

  for(i=0; i<(len-4); i++)
  {
    label[i] = label[i+4];
  }

  for(; i<len; i++)
  {
    label[i] = ' ';
  }

  return (type + 1);
}


int utc_to_edf_startdate(long long utc_time, char *dest)
{
  struct date_time_struct dt;

  utc_to_date_time(utc_time, &dt);

  if((dt.year >= 1985) && (dt.year <= 2084))
  {
    snprintf(dest, 12, "%02i-%.3s-%04i", dt.day, dt.month_str, dt.year);

    return 0;
  }
  else
  {
    return -1;
  }
}


int to_edf_startdate(int day, int month, int year, char *dest)
{
  char month_str[4]="";

  if((year < 1985) || (year > 2084) ||
     (month < 1)   || (month > 12)  ||
     (day < 1)     || (day > 31))
  {
    return -1;
  }

  switch(month)
  {
    case  1 : strlcpy(month_str, "JAN", 4);
              break;
    case  2 : strlcpy(month_str, "FEB", 4);
              break;
    case  3 : strlcpy(month_str, "MAR", 4);
              break;
    case  4 : strlcpy(month_str, "APR", 4);
              break;
    case  5 : strlcpy(month_str, "MAY", 4);
              break;
    case  6 : strlcpy(month_str, "JUN", 4);
              break;
    case  7 : strlcpy(month_str, "JUL", 4);
              break;
    case  8 : strlcpy(month_str, "AUG", 4);
              break;
    case  9 : strlcpy(month_str, "SEP", 4);
              break;
    case 10 : strlcpy(month_str, "OCT", 4);
              break;
    case 11 : strlcpy(month_str, "NOV", 4);
              break;
    case 12 : strlcpy(month_str, "DEC", 4);
              break;
    default : strlcpy(month_str, "ERR", 4);
              break;
  }

  snprintf(dest, 12, "%02i-%.3s-%04i", day, month_str, year);

  return 0;
}









