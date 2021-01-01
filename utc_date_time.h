/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2008 - 2021 Teunis van Beelen
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



#ifndef UTC_DATE_TIME_INCLUDED
#define UTC_DATE_TIME_INCLUDED


#include <string.h>
#include "utils.h"



#ifdef __cplusplus
extern "C" {
#endif




struct date_time_struct{
         int year;
         int month;
         int day;
         int hour;
         int minute;
         int second;
         char month_str[4];
       };


void utc_to_date_time(long long, struct date_time_struct *);

void date_time_to_utc(long long *, struct date_time_struct);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif


