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





#ifndef CHECK_EDF_FILE1_H
#define CHECK_EDF_FILE1_H


#include "qt_headers.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "utc_date_time.h"
#include "utils.h"



class EDFfileCheck
{

public:

  struct edfhdrblock * check_edf_file(FILE *, char *, int, int, int);

private:

  struct edfhdrblock *edfhdr;

  int is_integer_number(char *);
  int is_number(char *);
  long long get_long_duration(char *);

};



#endif // CHECK_EDF_FILE1_H


