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


#include "filteredblockread.h"


//#define FBR_DEBUG

#ifdef FBR_DEBUG

#include "edflib.h"

int edf_hdl=-1, edf_chan=0, *edf_ibuf=NULL;

struct edf_hdr_struct edfhdr;

#endif


double * FilteredBlockReadClass::init_signalcomp(struct signalcompblock *i_signalcomp, int datarecord_or_smpls_cnt, int skip_f, int mode)
{
  skip_filters = skip_f;

  if(i_signalcomp == NULL)
  {
    datarecord_cnt = -1;
    total_samples = -1LL;

    return NULL;
  }
  signalcomp = i_signalcomp;

  hdr = signalcomp->edfhdr;
  if(hdr == NULL)
  {
    datarecord_cnt = -1;
    total_samples = -1LL;

    return NULL;
  }

  inputfile = hdr->file_hdl;
  if(inputfile == NULL)
  {
    datarecord_cnt = -1;
    total_samples = -1LL;

    return NULL;
  }

  smpl_mode = mode;

  samples_per_datrec = hdr->edfparam[signalcomp->edfsignal[0]].smp_per_record;

  samples_in_file = hdr->edfparam[signalcomp->edfsignal[0]].smpls;

  if(smpl_mode)
  {
    total_samples = datarecord_or_smpls_cnt;

    datarecord_cnt = total_samples / samples_per_datrec;

    if(total_samples % samples_per_datrec)  datarecord_cnt++;

    if((datarecord_cnt > hdr->datarecords) || (total_samples < 1))
    {
      datarecord_cnt = -1;
      total_samples = -1LL;

      return NULL;
    }
  }
  else
  {
    datarecord_cnt = datarecord_or_smpls_cnt;

    if((datarecord_cnt > hdr->datarecords) || (datarecord_cnt < 1))
    {
      datarecord_cnt = -1;
      total_samples = -1LL;

      return NULL;
    }

    total_samples = (long long)samples_per_datrec * (long long)datarecord_cnt;
  }
#ifdef FBR_DEBUG
  printf("init_signalcomp(): samples_per_datrec: %i\n", samples_per_datrec);

  printf("init_signalcomp(): total_samples: %lli\n", total_samples);

  printf("init_signalcomp(): datarecord_cnt: %i\n", datarecord_cnt);
#endif
  if(processed_samples_buf != NULL)
  {
    free(processed_samples_buf);
  }
  processed_samples_buf = (double *)malloc(total_samples * sizeof(double));
  if(processed_samples_buf == NULL)
  {
    datarecord_cnt = -1;
    total_samples = -1LL;

    return NULL;
  }

  if(readbuf != NULL)
  {
    free(readbuf);
  }
  readbuf = (char *)malloc(hdr->recordsize  * (datarecord_cnt + 1));
  if(readbuf == NULL)
  {
    datarecord_cnt = -1;
    total_samples = -1LL;

    free(processed_samples_buf);
    processed_samples_buf = NULL;
    return NULL;
  }

  bitvalue = signalcomp->edfhdr->edfparam[signalcomp->edfsignal[0]].bitvalue;
#ifdef FBR_DEBUG
  edf_hdl = -1;

  if(smpl_mode)
  {
    if(!edfopen_file_readonly(signalcomp->edfhdr->filename, &edfhdr, 0))
    {
      edf_hdl = edfhdr.handle;

      printf("init_signalcomp(): datarecord_duration: %lli    smp_in_datarecord: %i    sf: %f Hz   total samples in file: %lli\n",
             edfhdr.datarecord_duration, edfhdr.signalparam[0].smp_in_datarecord,
             (edfhdr.signalparam[0].smp_in_datarecord * EDFLIB_TIME_DIMENSION) / (double)edfhdr.datarecord_duration,
              edfhdr.signalparam[0].smp_in_file);

      edf_ibuf = (int *)malloc(total_samples * sizeof(int));
      if(edf_ibuf == NULL)
      {
        edfclose_file(edf_hdl);
        edf_hdl = -1;

        printf("malloc error\n");
      }
    }
  }
#endif
  return processed_samples_buf;
}


long long FilteredBlockReadClass::samples_in_buf(void)
{
  return total_samples;
}


FilteredBlockReadClass::FilteredBlockReadClass()
{
  readbuf = NULL;
  processed_samples_buf = NULL;

  datarecord_cnt = -1;
  total_samples = -1LL;

  inputfile = NULL;
  hdr = NULL;
  signalcomp = NULL;
}


FilteredBlockReadClass::~FilteredBlockReadClass()
{
#ifdef FBR_DEBUG
  if(edf_hdl >= 0)
  {
    edfclose_file(edf_hdl);
  }

  free(edf_ibuf);
#endif
  free(processed_samples_buf);

  free(readbuf);
}


int FilteredBlockReadClass::process_signalcomp(int datarecord_or_sample_start)
{
  int j, k, datarecord_start, extra_datrec=0;

  long long s, s_end, sample_start, s_off=0;

  double dig_value=0.0,
         f_tmp=0.0;

  union {
          unsigned int one;
          signed int one_signed;
          unsigned short two[2];
          signed short two_signed[2];
          unsigned char four[4];
        } var;
#ifdef FBR_DEBUG
  printf("process_signalcomp(): datarecord_or_sample_start: %i\n", datarecord_or_sample_start);
#endif
  if((total_samples < 1) || (datarecord_cnt < 1))
  {
    return -1;
  }

  if(smpl_mode)
  {
    sample_start = datarecord_or_sample_start;

    if((sample_start < 0) || (sample_start >= samples_in_file))
    {
      return -2;
    }

    if(total_samples > (samples_in_file - sample_start))
    {
      return -3;
    }

    datarecord_start = sample_start / (long long)samples_per_datrec;

    s_off = sample_start % (long long)samples_per_datrec;
    if(s_off)
    {
      if(((long long)samples_per_datrec - s_off) < total_samples)
      {
        extra_datrec = 1;
      }
    }
  }
  else
  {
    datarecord_start = datarecord_or_sample_start;

    if((datarecord_start < 0) || (datarecord_start >= hdr->datarecords))
    {
      return -2;
    }

    if(datarecord_cnt > (hdr->datarecords - datarecord_start))
    {
      return -3;
    }

    sample_start = (long long)datarecord_cnt * (long long)samples_per_datrec;
  }
#ifdef FBR_DEBUG
  printf("process_signalcomp(): s_off: %lli\n", s_off);

  printf("process_signalcomp(): hdr->hdrsize: %i\n", hdr->hdrsize);

  printf("process_signalcomp(): datarecord_start: %i\n", datarecord_start);

  printf("process_signalcomp(): hdr->recordsize: %i\n", hdr->recordsize);

  printf("process_signalcomp(): fseek: %lli\n", ((long long)hdr->hdrsize) + (((long long)datarecord_start) * ((long long) hdr->recordsize)));
#endif
  if(fseeko(inputfile, ((long long)hdr->hdrsize) + (((long long)datarecord_start) * ((long long) hdr->recordsize)), SEEK_SET) == -1LL)
  {
    return -4;
  }
#ifdef FBR_DEBUG
  printf("process_signalcomp(): datarecord_cnt: %i\n", datarecord_cnt);

  printf("process_signalcomp(): extra_datrec: %i\n\n", extra_datrec);
#endif
  if(fread(readbuf, hdr->recordsize * (datarecord_cnt + extra_datrec), 1, inputfile) != 1)
  {
    return -5;
  }

  if((readbuf == NULL) || (processed_samples_buf == NULL))
  {
    return -6;
  }
#ifdef FBR_DEBUG
  if(smpl_mode)
  {
    if(edf_hdl >= 0)
    {
      if(edfseek(edf_hdl, 0, sample_start, EDFSEEK_SET) != sample_start)
      {
        printf("error: edfseek(%lli)\n", sample_start);
      }

      if(edfread_digital_samples(edf_hdl, 0, total_samples, edf_ibuf) != total_samples)
      {
        printf("error: edfread_digital_samples(%lli)\n", total_samples);
      }
    }
  }
#endif
  s_end = total_samples + s_off;

  for(s=s_off; s<s_end; s++)
  {
    dig_value = 0.0;

    for(j=0; j<signalcomp->num_of_signals; j++)
    {
      if(signalcomp->edfhdr->bdf)
      {
        var.two[0] = *((unsigned short *)(
          readbuf
          + (signalcomp->edfhdr->recordsize * (s / samples_per_datrec))
          + signalcomp->edfhdr->edfparam[signalcomp->edfsignal[j]].buf_offset
          + ((s % samples_per_datrec) * 3LL)));

        var.four[2] = *((unsigned char *)(
          readbuf
          + (signalcomp->edfhdr->recordsize * (s / samples_per_datrec))
          + signalcomp->edfhdr->edfparam[signalcomp->edfsignal[j]].buf_offset
          + ((s % samples_per_datrec) * 3LL)
          + 2LL));

        if(var.four[2]&0x80)
        {
          var.four[3] = 0xff;
        }
        else
        {
          var.four[3] = 0x00;
        }

        f_tmp = var.one_signed;
      }

      if(signalcomp->edfhdr->edf)
      {
        f_tmp = *(((short *)(
          readbuf
          + (signalcomp->edfhdr->recordsize * (s / samples_per_datrec))
          + signalcomp->edfhdr->edfparam[signalcomp->edfsignal[j]].buf_offset))
          + (s % samples_per_datrec));
      }

      f_tmp += signalcomp->edfhdr->edfparam[signalcomp->edfsignal[j]].offset;
      f_tmp *= signalcomp->factor[j];

      dig_value += f_tmp;
    }
#ifdef FBR_DEBUG
//     if(((dig_value - edf_ibuf[s - s_off]) > 1) || ((dig_value - edf_ibuf[s - s_off]) < -1 ))
//     {
//       printf("dig_value: %f   edf_ibuf[%lli]: %i\n", dig_value, s - s_off, edf_ibuf[s - s_off]);
//     }
#endif
    if(!skip_filters)
    {
      if(signalcomp->spike_filter)
      {
        dig_value = run_spike_filter(dig_value, signalcomp->spike_filter);
      }

      for(k=0; k<signalcomp->filter_cnt; k++)
      {
        dig_value = first_order_filter(dig_value, signalcomp->filter[k]);
      }

      for(k=0; k<signalcomp->ravg_filter_cnt; k++)
      {
        dig_value = run_ravg_filter(dig_value, signalcomp->ravg_filter[k]);
      }

      for(k=0; k<signalcomp->fidfilter_cnt; k++)
      {
        dig_value = signalcomp->fidfuncp[k](signalcomp->fidbuf[k], dig_value);
      }

      if(signalcomp->fir_filter)
      {
        dig_value = run_fir_filter(dig_value, signalcomp->fir_filter);
      }

      if(signalcomp->plif_ecg_filter)
      {
        dig_value = plif_run_subtract_filter(dig_value, signalcomp->plif_ecg_filter);
      }

      if(signalcomp->ecg_filter)
      {
        dig_value = run_ecg_filter(dig_value, signalcomp->ecg_filter);
      }

      if(signalcomp->zratio_filter)
      {
        dig_value = run_zratio_filter(dig_value, signalcomp->zratio_filter);
      }

      dig_value *= signalcomp->polarity;
    }

    processed_samples_buf[s - s_off] = (dig_value * bitvalue);
  }

  return 0;
}















