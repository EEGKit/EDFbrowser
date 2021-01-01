/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2012 - 2021 Teunis van Beelen
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





#ifndef filteredblockread_INCLUDED
#define filteredblockread_INCLUDED



#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "global.h"
#include "filter.h"
#include "spike_filter.h"
#include "ravg_filter.h"

#include "third_party/fidlib/fidlib.h"



class FilteredBlockReadClass
{
private:
  FILE *inputfile;

  struct edfhdrblock *hdr;

  struct signalcompblock *signalcomp;

  int datarecord_cnt,
      samples_per_datrec,
      skip_filters,
      smpl_mode;

  long long total_samples,
            samples_in_file;

  char *readbuf;

  double *processed_samples_buf,
         bitvalue;


public:
  double * init_signalcomp(struct signalcompblock *, int, int, int mode=0);
  int process_signalcomp(int);
  long long samples_in_buf(void);
  FilteredBlockReadClass();
  ~FilteredBlockReadClass();
};

#endif





