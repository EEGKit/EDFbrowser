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


#include "viewcurve.h"



void ViewCurve::drawCurve_stage_2(QPainter *painter, int w_width, int w_height, int print_linewidth)
{
  int i, j, k, x_pix=0, x_tmp,
      signalcomps,
      baseline,
      m_pagetime=10,
      vert_ruler_offset,
      vertical_distance,
      marker_x,
      marker_x2,
      annot_list_sz=0,
      font_pixel_height=20,
      font_pixel_width=12;

  char *viewbuf=NULL,
       string[600]="",
       str2[32]="",
       str3[128]="",
       str4[1024]="",
       *chp=NULL;

  long long time_ppixel,
            ll_elapsed_time,
            l_time,
            l_tmp,
            l_tmp2=0;

  double dtmp=0;

  struct signalcompblock **signalcomp=NULL;

  struct annotation_list *annot_list=NULL;

  struct annotationblock *annot=NULL;

  struct date_time_struct date_time_str;

  if(mainwindow->exit_in_progress)
  {
    return;
  }

  signalcomps = mainwindow->signalcomps;
  signalcomp = mainwindow->signalcomp;
  viewbuf = mainwindow->viewbuf;

  font_pixel_height = mainwindow->font_pixel_height;
  font_pixel_width = mainwindow->font_pixel_width;

  vertical_distance = 1;

  painter_pixelsizefactor = 1.0 / mainwindow->y_pixelsizefactor;

  if(!w_width||!w_height)
  {
    w = width();
    h = height();

    painter->setFont(*mainwindow->myfont);

    printing = 0;
  }
  else
  {
    w = w_width;
    h = w_height;

    printfont->setPixelSize((int)((double)w / 104.0));

    painter->setFont(*printfont);

    printsize_x_factor = ((double)w_width) / ((double)width());
    printsize_y_factor = ((double)w_height) / ((double)height());

    painter_pixelsizefactor *= printsize_y_factor;

    printing = 1;
  }

  for(i=0; i<signalcomps; i++)
  {
    signalcomp[i]->sample_pixel_ratio = (double)signalcomp[i]->samples_on_screen / (double)w;

    signalcomp[i]->draw_rectangle_sum_y = 0;

    signalcomp[i]->draw_rectangle_sum_cnt = 0;
  }

  vertical_distance = h / (signalcomps + 1);

  painter->fillRect(0, 0, w, h, backgroundcolor);

  if(mainwindow->show_annot_markers)
  {
    for(i=0; i<mainwindow->files_open; i++)
    {
      annot_list = &mainwindow->edfheaderlist[i]->annot_list;

      annot_list_sz = edfplus_annotation_size(annot_list);

      for(j=0; j<annot_list_sz; j++)
      {
        annot = edfplus_annotation_get_item(annot_list, j);

        if(annot->long_duration)
        {
          l_tmp = annot->onset - mainwindow->edfheaderlist[i]->starttime_offset;

          if(((l_tmp + annot->long_duration) > (mainwindow->edfheaderlist[i]->viewtime - TIME_DIMENSION)) && (!annot->hided))
          {
            if(l_tmp > (long long)(mainwindow->edfheaderlist[i]->viewtime + mainwindow->pagetime))
            {
              break;
            }

            l_tmp -= mainwindow->edfheaderlist[i]->viewtime;

            marker_x = (int)((((double)w) / mainwindow->pagetime) * l_tmp);

            marker_x2 = (int)((((double)w) / mainwindow->pagetime) * annot->long_duration);

            if(marker_x < 0)
            {
              marker_x2 += marker_x;

              marker_x = 0;
            }

            if((marker_x + marker_x2) > w)
            {
              x_tmp = marker_x + marker_x2 - w;

              marker_x2 -= x_tmp;
            }

            if((marker_x < w) && (marker_x2 > 0))
            {
              chp = NULL;

              if((signalcomps > 1) && (mainwindow->channel_linked_annotations))
              {
                strlcpy(str4, annot->description, MAX_ANNOTATION_LEN);
                chp = strstr(str4, "@@");
                if(chp != NULL)
                {
                  if(strlen(chp) > 2)
                  {
                    chp += 2;
                    if(mainwindow->strip_label_types)
                    {
                      strip_types_from_label(chp);
                    }
                    trim_spaces(chp);

                    for(k=0; k<signalcomps; k++)
                    {
                      if(!strcmp(chp, signalcomp[k]->signallabel))
                      {
                        baseline = (vertical_distance * (k + 1)) + signalcomp[k]->screen_offset;

                        if(annot->selected_in_dock)
                        {
                          painter->fillRect(marker_x, baseline - (vertical_distance / 2), marker_x2, vertical_distance, annot_duration_color_selected);
                        }
                        else
                        {
                          painter->fillRect(marker_x, baseline - (vertical_distance / 2), marker_x2, vertical_distance, annot_duration_color);
                        }

                        break;
                      }
                    }
                    if(k == signalcomps)
                    {
                      chp = NULL;
                    }
                  }
                  else
                  {
                    chp = NULL;
                  }
                }
              }

              if(chp == NULL)
              {
                if(annot->selected_in_dock)
                {
                  if(mainwindow->annotations_duration_background_type == 0)
                  {
                    painter->fillRect(marker_x, 0, marker_x2, h, annot_duration_color_selected);
                  }
                  else
                  {
                    painter->fillRect(marker_x, h - 92 + ((j % 3) * 30), marker_x2, 32, annot_duration_color_selected);
                  }
                }
                else
                {
                  if(mainwindow->annotations_duration_background_type == 0)
                  {
                    painter->fillRect(marker_x, 0, marker_x2, h, annot_duration_color);
                  }
                  else
                  {
                    painter->fillRect(marker_x, h - 92 + ((j % 3) * 30), marker_x2, 32, annot_duration_color);
                  }
                }
              }
            }
          }
        }
      }
    }
  }

  m_pagetime = (int)(mainwindow->pagetime / TIME_DIMENSION);

  time_ppixel = mainwindow->pagetime / w;

  if(mainwindow->files_open&&mainwindow->signalcomps)
  {
    ll_elapsed_time = mainwindow->edfheaderlist[mainwindow->sel_viewtime]->viewtime;
  }
  else
  {
    ll_elapsed_time = 0;
  }

  ruler_pen->setWidth(print_linewidth);

  if(printing)
  {
    ruler_pen->setColor(Qt::black);
    painter->setPen(*ruler_pen);
    painter->drawLine(0, 0, 0, h-1);
    painter->drawLine(0, 0, w-1, 0);
    painter->drawLine(w-1, h-1, w-1, 0);
    painter->drawLine(w-1, h-1, 0, h-1);
  }

  if(m_pagetime<=20)
  {
    ruler_pen->setColor(small_ruler_color);
    painter->setPen(*ruler_pen);

    for(x_pix=0; x_pix<w; x_pix++)
    {
      if((ll_elapsed_time / (TIME_DIMENSION / 10))!=((ll_elapsed_time + time_ppixel) / (TIME_DIMENSION / 10)))
      {
        if(printing)
        {
          painter->drawLine(x_pix, 0, x_pix, 4 * printsize_y_factor);
        }
        else
        {
          painter->drawLine(x_pix, 0, x_pix, 4 * h_scaling);
        }
      }

      if(((ll_elapsed_time / TIME_DIMENSION)!=((ll_elapsed_time + time_ppixel) / TIME_DIMENSION)) ||
        ((ll_elapsed_time < time_ppixel) && (ll_elapsed_time > -time_ppixel)))
      {
        if(x_pix)
        {
          ruler_pen->setColor(big_ruler_color);
          painter->setPen(*ruler_pen);
          painter->drawLine(x_pix, 0, x_pix, h);
          if(ll_elapsed_time < 0LL)
          {
            snprintf(str4, 1024, "%i", ((int)((ll_elapsed_time - time_ppixel) / TIME_DIMENSION)) % 60);
          }
          else
          {
            snprintf(str4, 1024, "%i", ((int)((ll_elapsed_time + time_ppixel) / TIME_DIMENSION)) % 60);
          }
          painter->drawText(x_pix + 2, 20 * h_scaling, str4);
          ruler_pen->setColor(small_ruler_color);
          painter->setPen(*ruler_pen);
        }
        if(printing)
        {
          painter->drawLine(x_pix, 0, x_pix, 8 * printsize_y_factor);
        }
        else
        {
          painter->drawLine(x_pix, 0, x_pix, 8 * h_scaling);
        }
      }

      ll_elapsed_time += time_ppixel;
    }
  }

  if((m_pagetime>20)&&(m_pagetime<100))
  {
    ruler_pen->setColor(small_ruler_color);
    painter->setPen(*ruler_pen);

    for(x_pix=0; x_pix<w; x_pix++)
    {
      if((ll_elapsed_time / (TIME_DIMENSION / 5))!=((ll_elapsed_time + time_ppixel) / (TIME_DIMENSION / 5)))
      {
        if(printing)
        {
          painter->drawLine(x_pix, 0, x_pix, 4 * printsize_y_factor);
        }
        else
        {
          painter->drawLine(x_pix, 0, x_pix, 4 * h_scaling);
        }
      }

      if(((ll_elapsed_time / TIME_DIMENSION)!=((ll_elapsed_time + time_ppixel) / TIME_DIMENSION)) ||
        ((ll_elapsed_time < time_ppixel) && (ll_elapsed_time > -time_ppixel)))
      {
        if(x_pix)
        {
          ruler_pen->setColor(big_ruler_color);
          painter->setPen(*ruler_pen);
          painter->drawLine(x_pix, 0, x_pix, h);
          if(ll_elapsed_time < 0LL)
          {
            if(!(((int)((ll_elapsed_time - time_ppixel) / TIME_DIMENSION)) % 5))
            {
              snprintf(str4, 1024, "%i", ((int)((ll_elapsed_time - time_ppixel) / TIME_DIMENSION)) % 60);

              painter->drawText(x_pix + 2, 20 * h_scaling, str4);
            }
          }
          else
          {
            if(!(((int)((ll_elapsed_time + time_ppixel) / TIME_DIMENSION)) % 5))
            {
              snprintf(str4, 1024, "%i", ((int)((ll_elapsed_time + time_ppixel) / TIME_DIMENSION)) % 60);

              painter->drawText(x_pix + 2, 20 * h_scaling, str4);
            }
          }
          ruler_pen->setColor(small_ruler_color);
          painter->setPen(*ruler_pen);
        }
        if(printing)
        {
          painter->drawLine(x_pix, 0, x_pix, 8 * printsize_y_factor);
        }
        else
        {
          painter->drawLine(x_pix, 0, x_pix, 8 * h_scaling);
        }
      }

      ll_elapsed_time += time_ppixel;
    }
  }

  if((m_pagetime>=100)&&(m_pagetime<1000))
  {
    ruler_pen->setColor(small_ruler_color);
    painter->setPen(*ruler_pen);

    for(x_pix=0; x_pix<w; x_pix++)
    {
      if((ll_elapsed_time / 33333333LL)!=((ll_elapsed_time + time_ppixel) / 33333333LL))
      {
        if(printing)
        {
          painter->drawLine(x_pix, 0, x_pix, 4 * printsize_y_factor);
        }
        else
        {
          painter->drawLine(x_pix, 0, x_pix, 4 * h_scaling);
        }
      }

      if(((ll_elapsed_time / (TIME_DIMENSION * 10))!=((ll_elapsed_time + time_ppixel) / (TIME_DIMENSION * 10))) ||
        ((ll_elapsed_time < time_ppixel) && (ll_elapsed_time > -time_ppixel)))
      {
        if(x_pix)
        {
          ruler_pen->setColor(big_ruler_color);
          painter->setPen(*ruler_pen);
          painter->drawLine(x_pix, 0, x_pix, h);
          if(ll_elapsed_time < 0LL)
          {
            if(!(((int)((ll_elapsed_time - time_ppixel) / TIME_DIMENSION)) % 10))
            {
              snprintf(str4, 1024, "%i", ((int)((ll_elapsed_time - time_ppixel) / TIME_DIMENSION)) % 60);

              painter->drawText(x_pix + 2, 20 * h_scaling, str4);
            }
          }
          else
          {
            if(!(((int)((ll_elapsed_time + time_ppixel) / TIME_DIMENSION)) % 10))
            {
              snprintf(str4, 1024, "%i", ((int)((ll_elapsed_time + time_ppixel) / TIME_DIMENSION)) % 60);

              painter->drawText(x_pix + 2, 20 * h_scaling, str4);
            }
          }
          ruler_pen->setColor(small_ruler_color);
          painter->setPen(*ruler_pen);
        }
        if(printing)
        {
          painter->drawLine(x_pix, 0, x_pix, 8 * printsize_y_factor);
        }
        else
        {
          painter->drawLine(x_pix, 0, x_pix, 8 * h_scaling);
        }
      }

      ll_elapsed_time += time_ppixel;
    }
  }

  if((m_pagetime>=1000)&&(m_pagetime<5000))
  {
    ruler_pen->setColor(small_ruler_color);
    painter->setPen(*ruler_pen);

    for(x_pix=0; x_pix<w; x_pix++)
    {
      if((ll_elapsed_time / (TIME_DIMENSION * 10))!=((ll_elapsed_time + time_ppixel) / (TIME_DIMENSION * 10)))
      {
        if(printing)
        {
          painter->drawLine(x_pix, 0, x_pix, 4 * printsize_y_factor);
        }
        else
        {
          painter->drawLine(x_pix, 0, x_pix, 4 * h_scaling);
        }
      }

      if(((ll_elapsed_time / (TIME_DIMENSION * 60))!=((ll_elapsed_time + time_ppixel) / (TIME_DIMENSION * 60)))  ||
        ((ll_elapsed_time < time_ppixel) && (ll_elapsed_time > -time_ppixel)))
      {
        if(x_pix)
        {
          ruler_pen->setColor(big_ruler_color);
          painter->setPen(*ruler_pen);
          painter->drawLine(x_pix, 0, x_pix, h);
          if(ll_elapsed_time < 0LL)
          {
//            printf("ll_elapsed_time: %lli    time_ppixel: %lli    result:%i\n", ll_elapsed_time, time_ppixel, ((int)((ll_elapsed_time - time_ppixel) / TIME_DIMENSION)) % 60);

            snprintf(str4, 1024, "%i", ((int)((ll_elapsed_time - time_ppixel) / (TIME_DIMENSION * 60))) % 60);
          }
          else
          {
//            printf("ll_elapsed_time: %lli    time_ppixel: %lli    result:%i\n", ll_elapsed_time, time_ppixel, ((int)((ll_elapsed_time + time_ppixel) / TIME_DIMENSION)) % 60);

            snprintf(str4, 1024, "%i", ((int)((ll_elapsed_time + time_ppixel) / (TIME_DIMENSION * 60))) % 60);
          }
          painter->drawText(x_pix + 2, 20 * h_scaling, str4);
          ruler_pen->setColor(small_ruler_color);
          painter->setPen(*ruler_pen);
        }
        if(printing)
        {
          painter->drawLine(x_pix, 0, x_pix, 8 * printsize_y_factor);
        }
        else
        {
          painter->drawLine(x_pix, 0, x_pix, 8 * h_scaling);
        }
      }

      ll_elapsed_time += time_ppixel;
    }
  }

  if((m_pagetime>=5000)&&(m_pagetime<173000))
  {
    ruler_pen->setColor(small_ruler_color);
    painter->setPen(*ruler_pen);

    for(x_pix=0; x_pix<w; x_pix++)
    {
      if((ll_elapsed_time / (TIME_DIMENSION * 600))!=((ll_elapsed_time + time_ppixel) / (TIME_DIMENSION * 600)))
      {
        if(printing)
        {
          painter->drawLine(x_pix, 0, x_pix, 4 * printsize_y_factor);
        }
        else
        {
          painter->drawLine(x_pix, 0, x_pix, 4 * h_scaling);
        }
      }

      if(((ll_elapsed_time / (TIME_DIMENSION * 3600))!=((ll_elapsed_time + time_ppixel) / (TIME_DIMENSION * 3600))) ||
        ((ll_elapsed_time < time_ppixel) && (ll_elapsed_time > -time_ppixel)))
      {
        if(x_pix)
        {
          ruler_pen->setColor(big_ruler_color);
          painter->setPen(*ruler_pen);
          painter->drawLine(x_pix, 0, x_pix, h);
          if(ll_elapsed_time < 0LL)
          {
//            printf("ll_elapsed_time: %lli    time_ppixel: %lli    result:%i\n", ll_elapsed_time, time_ppixel, ((int)((ll_elapsed_time - time_ppixel) / TIME_DIMENSION)) % 3600);

            snprintf(str4, 1024, "%i", ((int)((ll_elapsed_time - time_ppixel) / (TIME_DIMENSION * 3600))) % 24);
          }
          else
          {
//            printf("ll_elapsed_time: %lli    time_ppixel: %lli    result:%i\n", ll_elapsed_time, time_ppixel, ((int)((ll_elapsed_time + time_ppixel) / TIME_DIMENSION)) % 3600);

            snprintf(str4, 1024, "%i", ((int)((ll_elapsed_time + time_ppixel) / (TIME_DIMENSION * 3600))) % 24);
          }
          painter->drawText(x_pix + 2, 20 * h_scaling, str4);
          ruler_pen->setColor(small_ruler_color);
          painter->setPen(*ruler_pen);
        }
        if(printing)
        {
          painter->drawLine(x_pix, 0, x_pix, 8 * printsize_y_factor);
        }
        else
        {
          painter->drawLine(x_pix, 0, x_pix, 8 * h_scaling);
        }
      }

      ll_elapsed_time += time_ppixel;
    }
  }

  if((m_pagetime>=173000)&&(m_pagetime<3000000))
  {
    ruler_pen->setColor(small_ruler_color);
    painter->setPen(*ruler_pen);

    for(x_pix=0; x_pix<w; x_pix++)
    {
      if((ll_elapsed_time / (TIME_DIMENSION * 3600))!=((ll_elapsed_time + time_ppixel) / (TIME_DIMENSION * 3600)))
      {
        if(printing)
        {
          painter->drawLine(x_pix, 0, x_pix, 4 * printsize_y_factor);
        }
        else
        {
          painter->drawLine(x_pix, 0, x_pix, 4 * h_scaling);
        }
      }

      if(((ll_elapsed_time / (TIME_DIMENSION * 86400))!=((ll_elapsed_time + time_ppixel) / (TIME_DIMENSION * 86400))) ||
         ((ll_elapsed_time < time_ppixel) && (ll_elapsed_time > -time_ppixel)))
     {
        if(x_pix)
        {
          ruler_pen->setColor(big_ruler_color);
          painter->setPen(*ruler_pen);
          painter->drawLine(x_pix, 0, x_pix, h);
          ruler_pen->setColor(small_ruler_color);
          painter->setPen(*ruler_pen);
        }
        if(printing)
        {
          painter->drawLine(x_pix, 0, x_pix, 8 * printsize_y_factor);
        }
        else
        {
          painter->drawLine(x_pix, 0, x_pix, 8 * h_scaling);
        }
      }

      ll_elapsed_time += time_ppixel;
    }
  }

  if(mainwindow->show_baselines)
  {
    vertical_distance = h / (signalcomps + 1);

    painter->setPen(baseline_color);

    for(i=0; i<signalcomps; i++)
    {
      baseline = vertical_distance * (i + 1);

      painter->drawLine(0, baseline, w, baseline);

      dtmp = fabs(signalcomp[i]->voltpercm);

      if(dtmp < 0.001)
      {
        strlcpy(str2, "%+.5f ", 32);
      }
      else if(dtmp < 0.01)
        {
          strlcpy(str2, "%+.4f ", 32);
        }
        else if(dtmp < 0.1)
          {
            strlcpy(str2, "%+.3f ", 32);
          }
          else if(dtmp < 1.0)
                {
                  strlcpy(str2, "%+.2f ", 32);
                }
                else if(dtmp < 10.0)
                    {
                      strlcpy(str2, "%+.1f ", 32);
                    }
                    else
                    {
                      strlcpy(str2, "%+.0f ", 32);
                    }

      strlcat(str2, signalcomp[i]->physdimension, 32);

      for(j=1; j<18; j++)
      {
        vert_ruler_offset = j * painter_pixelsizefactor;

        if(signalcomps!=1)
        {
          if(vert_ruler_offset>((vertical_distance / 2)) - 8)
          {
            break;
          }
        }

        if(printing)
        {
          if((baseline + vert_ruler_offset)>(h - (15 * printsize_y_factor)))
          {
            break;
          }

          if((baseline - vert_ruler_offset)<(15 * printsize_y_factor))
          {
            break;
          }
        }
        else
        {
          if((baseline + vert_ruler_offset)>(h - 15))
          {
            break;
          }

          if((baseline - vert_ruler_offset)<15)
          {
            break;
          }
        }

        painter->drawLine(0, baseline - vert_ruler_offset, w, baseline - vert_ruler_offset);

#pragma GCC diagnostic ignored "-Wformat-nonliteral"

        if(printing)
        {
          snprintf(string, 128, str2,
            ((signalcomp[i]->voltpercm * j) + ((signalcomp[i]->screen_offset * signalcomp[i]->voltpercm) / (painter_pixelsizefactor / printsize_y_factor))) * (double)signalcomp[i]->polarity);
        }
        else
        {
          snprintf(string, 128, str2,
            ((signalcomp[i]->voltpercm * j) + ((signalcomp[i]->screen_offset * signalcomp[i]->voltpercm) / painter_pixelsizefactor)) * (double)signalcomp[i]->polarity);
        }

        painter->drawText(5 * printsize_x_factor, baseline - vert_ruler_offset - (4 * printsize_y_factor), string);

        painter->drawLine(0, baseline + vert_ruler_offset, w, baseline + vert_ruler_offset);

        if(printing)
        {
          snprintf(string, 128, str2,
            (((signalcomp[i]->screen_offset * signalcomp[i]->voltpercm) / (painter_pixelsizefactor / printsize_y_factor)) - (signalcomp[i]->voltpercm * j)) * (double)signalcomp[i]->polarity);
        }
        else
        {
          snprintf(string, 128, str2,
            (((signalcomp[i]->screen_offset * signalcomp[i]->voltpercm) / painter_pixelsizefactor) - (signalcomp[i]->voltpercm * j)) * (double)signalcomp[i]->polarity);
        }

#pragma GCC diagnostic warning "-Wformat-nonliteral"

        painter->drawText(5 * printsize_x_factor, baseline + vert_ruler_offset - (4 * printsize_y_factor), string);
      }
    }
  }

  if(mainwindow->show_annot_markers || mainwindow->toolbar_stats.active)
  {
    annot_marker_pen->setColor(annot_marker_color);

    annot_marker_pen->setWidth(print_linewidth);

    painter->setPen(*annot_marker_pen);

    if(!annot_marker_moving)
    {
      active_markers->count = 0;
    }

    for(i=0; i<mainwindow->files_open; i++)
    {
      annot_list = &mainwindow->edfheaderlist[i]->annot_list;

      annot_list_sz = edfplus_annotation_size(annot_list);

      if((mainwindow->toolbar_stats.active) && (mainwindow->toolbar_stats.annot_list == annot_list))
      {
        mainwindow->toolbar_stats.sz = 0;
      }

      for(j=0; j<annot_list_sz; j++)
      {
        annot = edfplus_annotation_get_item(annot_list, j);

        l_tmp = annot->onset - mainwindow->edfheaderlist[i]->starttime_offset;

        if(l_tmp > (long long)(mainwindow->edfheaderlist[i]->viewtime + mainwindow->pagetime))
        {
          break;
        }

        if((l_tmp > (mainwindow->edfheaderlist[i]->viewtime - TIME_DIMENSION)) && (!annot->hided))
        {
          if(mainwindow->show_annot_markers)
          {
            if(annot->selected_in_dock)
            {
              annot_marker_pen->setColor(annot_marker_selected_color);

              painter->setPen(*annot_marker_pen);
            }

            l_tmp -= mainwindow->edfheaderlist[i]->viewtime;

            marker_x = (int)((((double)w) / mainwindow->pagetime) * l_tmp);

            chp = NULL;

            if((signalcomps > 1) && (mainwindow->channel_linked_annotations))
            {
              strlcpy(str4, annot->description, MAX_ANNOTATION_LEN);
              chp = strstr(str4, "@@");
              if(chp != NULL)
              {
                if(strlen(chp) > 2)
                {
                  chp += 2;
                  if(mainwindow->strip_label_types)
                  {
                    strip_types_from_label(chp);
                  }
                  trim_spaces(chp);

                  for(k=0; k<signalcomps; k++)
                  {
                    if(!strcmp(chp, signalcomp[k]->signallabel))
                    {
                      baseline = (vertical_distance * (k + 1)) + signalcomp[k]->screen_offset;

                      painter->drawLine(marker_x, baseline + (vertical_distance / 2), marker_x, baseline - (vertical_distance / 2));

                      break;
                    }
                  }
                  if(k == signalcomps)
                  {
                    chp = NULL;
                  }
                }
                else
                {
                  chp = NULL;
                }
              }
            }

            if(chp == NULL)
            {
              painter->drawLine(marker_x, 0, marker_x, h);
            }

            l_tmp = annot->onset - mainwindow->edfheaderlist[i]->starttime_offset;

            if(mainwindow->annotations_onset_relative)
            {
              if(l_tmp < 0LL)
              {
                snprintf(string, (MAX_ANNOTATION_LEN + 32) / 2, "-%i:%02i:%02i.%04i",
                        (int)(((-(l_tmp)) / TIME_DIMENSION)/ 3600LL),
                        (int)((((-(l_tmp)) / TIME_DIMENSION) % 3600LL) / 60LL),
                        (int)(((-(l_tmp)) / TIME_DIMENSION) % 60LL),
                        (int)((((-(l_tmp)) % TIME_DIMENSION) / 1000LL)));
              }
              else
              {
                snprintf(string, (MAX_ANNOTATION_LEN + 32) / 2, "%i:%02i:%02i.%04i",
                        (int)((l_tmp / TIME_DIMENSION)/ 3600LL),
                        (int)(((l_tmp / TIME_DIMENSION) % 3600LL) / 60LL),
                        (int)((l_tmp / TIME_DIMENSION) % 60LL),
                        (int)(((l_tmp % TIME_DIMENSION) / 1000LL)));
              }
            }
            else
            {
              if(mainwindow->viewtime_indicator_type == VIEWTIME_INDICATOR_TYPE_DATE_REAL_RELATIVE)
              {
                utc_to_date_time((annot->onset / TIME_DIMENSION) + mainwindow->edfheaderlist[i]->utc_starttime, &date_time_str);

                snprintf(string, 32, "%2i-%s ", date_time_str.day, date_time_str.month_str);
              }
              else
              {
                string[0] = 0;
              }

              snprintf(string + strlen(string), MAX_ANNOTATION_LEN + 32, "%i:%02i:%02i.%04i",
                      (int)((((annot->onset + mainwindow->edfheaderlist[i]->l_starttime) / TIME_DIMENSION)/ 3600) % 24),
                      (int)((((annot->onset + mainwindow->edfheaderlist[i]->l_starttime) / TIME_DIMENSION) % 3600) / 60),
                      (int)(((annot->onset + mainwindow->edfheaderlist[i]->l_starttime) / TIME_DIMENSION) % 60),
                      (int)(((annot->onset + mainwindow->edfheaderlist[i]->l_starttime) % TIME_DIMENSION) / 1000LL));
            }

            remove_trailing_zeros(string);

            if((annot->duration[0]) && (mainwindow->annotations_show_duration))
            {
              strlcat(string, " Duration: ", 600);

              strlcat(string, annot->duration, 600);

              remove_trailing_zeros(string);

              strlcat(string, " sec", 600);
            }

            if(printing)
            {
              painter->drawText(marker_x + (5  * printsize_x_factor), h - (25  * printsize_y_factor), string);
            }
            else
            {
              painter->drawText(marker_x + 5, (h - 5) - ((j % 3) * font_pixel_height * 2), string);
            }

            strlcpy(string, annot->description, 80);

            utf8_set_char_len(string, 20);

            if(printing)
            {
              painter->drawText(marker_x + (5  * printsize_x_factor), h - (40  * printsize_y_factor), QString::fromUtf8(string));
            }
            else
            {
              painter->drawText(marker_x + 5, ((h - 5) - font_pixel_height) - ((j % 3) * font_pixel_height * 2), QString::fromUtf8(string));
            }

            if(!annot_marker_moving)
            {
              if(active_markers->count<MAX_ACTIVE_ANNOT_MARKERS)
              {
                annot->x_pos = marker_x;

                active_markers->list[active_markers->count] = annot;

                active_markers->count++;
              }
            }

            if(annot->selected_in_dock)
            {
              annot_marker_pen->setColor(annot_marker_color);  /* set it back to the normal (not selected) color */

              painter->setPen(*annot_marker_pen);
            }
          }

          if((mainwindow->toolbar_stats.active) && (mainwindow->toolbar_stats.annot_list == annot_list))
          {
            if((!strcmp(annot->description, mainwindow->toolbar_stats.annot_label)) && (mainwindow->toolbar_stats.sz < STATISTICS_IVAL_LIST_SZ))
            {
              if(mainwindow->toolbar_stats.sz > 0)
              {
                mainwindow->toolbar_stats.ival[mainwindow->toolbar_stats.sz - 1] = ((double)(annot->onset - l_tmp2)) / (double)TIME_DIMENSION;
              }

              l_tmp2 = annot->onset;

              mainwindow->toolbar_stats.sz++;
            }
          }
        }
      }
    }

    if(mainwindow->toolbar_stats.sz)  mainwindow->toolbar_stats.sz--;
  }

  struct ecg_hr_statistics_struct hr_stats;

  if((mainwindow->toolbar_stats.active) && (mainwindow->toolbar_stats.sz > 2))
  {
    if(ecg_get_hr_statistics(mainwindow->toolbar_stats.ival, mainwindow->toolbar_stats.sz, &hr_stats))
    {
      mainwindow->nav_toolbar_label->setText("Error");
    }
    else
    {
      snprintf(str4, 1024, "  "
              "Beats: %i  "
              "Mean RR: %.1fms  "
              "SDNN RR: %.1fms  "
              "RMSSD RR: %.1fms  "
              "Mean HR: %.1fbpm  "
              "SDNN HR: %.1fbpm  "
              "NN20: %i  "
              "pNN20: %.1f%%  "
              "NN50:  %i  "
              "pNN50: %.1f%%  ",
              hr_stats.beat_cnt,
              hr_stats.mean_rr,
              hr_stats.sdnn_rr,
              hr_stats.rmssd_rr,
              hr_stats.mean_hr,
              hr_stats.sdnn_hr,
              hr_stats.NN20,
              hr_stats.pNN20,
              hr_stats.NN50,
              hr_stats.pNN50);

      mainwindow->nav_toolbar_label->setText(str4);
    }
  }
  else
  {
    mainwindow->nav_toolbar_label->setText("");
  }

  if((viewbuf==NULL)||(graphicBuf==NULL)||(screensamples==NULL))
  {
    if(!printing)
    {
      painter->setPen(text_color);

      painter->drawText(8, h - 4, mainwindow->viewtime_string);
      painter->drawText(w - strlen(mainwindow->pagetime_string) * font_pixel_width, h - 4, mainwindow->pagetime_string);

      if(!mainwindow->files_open)
      {
        painter->drawText(w / 2, h / 2, "NO FILE OPENED");
      }
      else if(!signalcomps)
        {
          if(mainwindow->files_open == 1)
          {
            snprintf(str4, 1024, "A FILE IS OPENED BUT NO SIGNALS ARE ADDED TO THE SCREEN");
          }
          else
          {
            snprintf(str4, 1024, "%i FILES ARE OPENED BUT NO SIGNALS ARE ADDED TO THE SCREEN", mainwindow->files_open);
          }

          painter->drawText(w * 0.4, h / 2, str4);
        }
    }

    return;
  }

  if(mainwindow->clip_to_pane)
  {
    painter->setClipping(true);

    vertical_distance = h / (signalcomps + 1);
  }

  for(i=0; i<signalcomps; i++)
  {
    if(mainwindow->clip_to_pane)
    {
      painter->setClipRect(0, vertical_distance * i + (vertical_distance / 2), w, vertical_distance);
    }

    if(printing)
    {
      signal_pen->setColor((Qt::GlobalColor)signalcomp[i]->color);
      signal_pen->setWidth(print_linewidth);
      painter->setPen(*signal_pen);
    }
    else
    {
      painter->setPen((Qt::GlobalColor)signalcomp[i]->color);
    }

    for(j=0; j<screensamples[i]; j++)
    {
      painter->drawLine(graphicBuf[j].graphicLine[i].x1,
                        graphicBuf[j].graphicLine[i].y1,
                        graphicBuf[j].graphicLine[i].x2,
                        graphicBuf[j].graphicLine[i].y2);

      if(draw_zoom_rectangle && use_move_events)
      {
        if((graphicBuf[j].graphicLine[i].x1 >= mouse_press_coordinate_x) &&
           (graphicBuf[j].graphicLine[i].x1 <= mouse_x))
        {
          signalcomp[i]->draw_rectangle_sum_y += graphicBuf[j].graphicLine[i].y1;

          signalcomp[i]->draw_rectangle_sum_cnt++;
        }
      }
    }
  }

  painter->setClipping(false);

  for(i=0; i<signalcomps; i++)
  {
    baseline = h / (signalcomps + 1);
    baseline *= (i + 1);

    painter->setPen((Qt::GlobalColor)signalcomp[i]->color);

    if(signalcomp[i]->hascursor1)
    {
      if(printing)
      {
        painter->setPen((Qt::GlobalColor)crosshair_1.color);
        painter->drawLine(0, crosshair_1.y_value, w, crosshair_1.y_value);
        painter->drawLine((int)((double)crosshair_1.x_position * printsize_x_factor), 0, (int)((double)crosshair_1.x_position * printsize_x_factor), h);
        snprintf(string, 128, "%+f %s",
                        crosshair_1.value,
                        signalcomp[i]->physdimension);
        painter->drawText((int)(((double)crosshair_1.x_position + 5.0) * printsize_x_factor), (int)(((double)crosshair_1.y_position - 40.0) * printsize_y_factor), string);
        snprintf(string, 128, "%2i:%02i:%02i.%04i",
                        (int)(((crosshair_1.time / TIME_DIMENSION)/ 3600LL) % 24LL),
                        (int)(((crosshair_1.time / TIME_DIMENSION) % 3600LL) / 60LL),
                        (int)((crosshair_1.time / TIME_DIMENSION) % 60LL),
                        (int)((crosshair_1.time % TIME_DIMENSION) / 1000LL));
        painter->drawText((int)(((double)crosshair_1.x_position + 5.0) * printsize_x_factor), (int)(((double)crosshair_1.y_position - 25.0) * printsize_y_factor), string);
        if(signalcomp[i]->alias[0] != 0)
        {
          painter->drawText((int)(((double)crosshair_1.x_position + 5.0) * printsize_x_factor), (int)(((double)crosshair_1.y_position - 10.0) * printsize_y_factor), signalcomp[i]->alias);
        }
        else
        {
          painter->drawText((int)(((double)crosshair_1.x_position + 5.0) * printsize_x_factor), (int)(((double)crosshair_1.y_position - 10.0) * printsize_y_factor), signalcomp[i]->signallabel);
        }
        painter->setPen((Qt::GlobalColor)signalcomp[i]->color);
      }
      else
      {
        painter->setPen((Qt::GlobalColor)crosshair_1.color);
        if(crosshair_1.has_hor_line)
        {
          painter->drawLine(0, crosshair_1.y_value, w, crosshair_1.y_value);
        }
        if(crosshair_1.dot_sz)
        {
          painter->setBrush(QBrush((Qt::GlobalColor)crosshair_1.color));
          painter->drawEllipse(crosshair_1.x_position - crosshair_1.dot_sz, crosshair_1.y_value - crosshair_1.dot_sz, crosshair_1.dot_sz * 2 + 1, crosshair_1.dot_sz * 2 + 1);
        }
        painter->drawLine(crosshair_1.x_position, 0, crosshair_1.x_position, h);
        snprintf(string, 128, "%+f %s",
                        crosshair_1.value,
                        signalcomp[i]->physdimension);
        painter->drawText(crosshair_1.x_position + 5, crosshair_1.y_position - (10 + (font_pixel_height * 2)), string);
        snprintf(string, 128, "%2i:%02i:%02i.%04i",
                        (int)(((crosshair_1.time / TIME_DIMENSION)/ 3600LL) % 24LL),
                        (int)(((crosshair_1.time / TIME_DIMENSION) % 3600LL) / 60LL),
                        (int)((crosshair_1.time / TIME_DIMENSION) % 60LL),
                        (int)((crosshair_1.time % TIME_DIMENSION) / 1000LL));
        if(crosshair_1.time_relative >= TIME_DIMENSION)
        {
          snprintf(string + strlen(string), 32, " (%i:%02i:%02i.%04i)",
                  (int)((crosshair_1.time_relative / TIME_DIMENSION)/ 3600LL),
                  (int)(((crosshair_1.time_relative / TIME_DIMENSION) % 3600LL) / 60LL),
                  (int)((crosshair_1.time_relative / TIME_DIMENSION) % 60LL),
                  (int)((crosshair_1.time_relative % TIME_DIMENSION) / 1000LL));
        }
        else
        {
          convert_to_metric_suffix(str3, (double)crosshair_1.time_relative / TIME_DIMENSION, 3, 128);

          snprintf(string + strlen(string), 32, " (%.26sS)", str3);
        }

        painter->drawText(crosshair_1.x_position + 5, crosshair_1.y_position - (10 + font_pixel_height), string);
        if(signalcomp[i]->alias[0] != 0)
        {
          painter->drawText(crosshair_1.x_position + 5, crosshair_1.y_position - 10, signalcomp[i]->alias);
        }
        else
        {
          painter->drawText(crosshair_1.x_position + 5, crosshair_1.y_position - 10, signalcomp[i]->signallabel);
        }
        painter->setPen((Qt::GlobalColor)signalcomp[i]->color);

        if(mainwindow->auto_update_annot_onset && (!crosshair_1.moving) && (mainwindow->annotationEditDock != NULL))
        {
          mainwindow->annotationEditDock->annotEditSetOnset(crosshair_1.time_relative);
        }
      }
    }

    if(signalcomp[i]->hascursor2)
    {
      if(printing)
      {
        painter->setPen((Qt::GlobalColor)crosshair_2.color);
        painter->drawLine(0, crosshair_2.y_value, w, crosshair_2.y_value);
        painter->drawLine((int)((double)crosshair_2.x_position * printsize_x_factor), 0, (int)((double)crosshair_2.x_position * printsize_x_factor), h);
        snprintf(string, 128, "%+f %s",
                        crosshair_2.value,
                        signalcomp[i]->physdimension);
        painter->drawText((int)(((double)crosshair_2.x_position + 5.0) * printsize_x_factor), (int)(((double)crosshair_2.y_position - 70.0) * printsize_y_factor), string);
        snprintf(string, 128, "%2i:%02i:%02i.%04i",
                        (int)(((crosshair_2.time / TIME_DIMENSION)/ 3600LL) % 24LL),
                        (int)(((crosshair_2.time / TIME_DIMENSION) % 3600LL) / 60LL),
                        (int)((crosshair_2.time / TIME_DIMENSION) % 60LL),
                        (int)((crosshair_2.time % TIME_DIMENSION) / 1000LL));
        painter->drawText((int)(((double)crosshair_2.x_position + 5.0) * printsize_x_factor), (int)(((double)crosshair_2.y_position - 55.0) * printsize_y_factor), string);
        snprintf(string, 128, "delta %+f %s",
                        crosshair_2.value - crosshair_1.value,
                        signalcomp[i]->physdimension);
        painter->drawText((int)(((double)crosshair_2.x_position + 5.0) * printsize_x_factor), (int)(((double)crosshair_2.y_position - 40.0) * printsize_y_factor), string);
        l_time = crosshair_2.time - crosshair_1.time;
        if(l_time<0) l_time = -l_time;
        snprintf(string, 128, "delta %i:%02i:%02i.%04i",
                        (int)((l_time / TIME_DIMENSION)/ 3600LL),
                        (int)(((l_time / TIME_DIMENSION) % 3600LL) / 60LL),
                        (int)((l_time / TIME_DIMENSION) % 60LL),
                        (int)((l_time % TIME_DIMENSION) / 1000LL));
        painter->drawText((int)(((double)crosshair_2.x_position + 5.0) * printsize_x_factor), (int)(((double)crosshair_2.y_position - 25.0) * printsize_y_factor), string);
        if(signalcomp[i]->alias[0] != 0)
        {
          painter->drawText((int)(((double)crosshair_2.x_position + 5.0) * printsize_x_factor), (int)(((double)crosshair_2.y_position - 10.0) * printsize_y_factor), signalcomp[i]->alias);
        }
        else
        {
          painter->drawText((int)(((double)crosshair_2.x_position + 5.0) * printsize_x_factor), (int)(((double)crosshair_2.y_position - 10.0) * printsize_y_factor), signalcomp[i]->signallabel);
        }
        painter->setPen((Qt::GlobalColor)signalcomp[i]->color);
      }
      else
      {
        painter->setPen((Qt::GlobalColor)crosshair_2.color);
        if(crosshair_2.has_hor_line)
        {
          painter->drawLine(0, crosshair_2.y_value, w, crosshair_2.y_value);
        }
        if(crosshair_2.dot_sz)
        {
          painter->setBrush(QBrush((Qt::GlobalColor)crosshair_2.color));
          painter->drawEllipse(crosshair_2.x_position - crosshair_2.dot_sz, crosshair_2.y_value - crosshair_2.dot_sz, crosshair_2.dot_sz * 2 + 1, crosshair_2.dot_sz * 2 + 1);
        }
        painter->drawLine(crosshair_2.x_position, 0, crosshair_2.x_position, h);
        snprintf(string, 128, "%+f %s",
                        crosshair_2.value,
                        signalcomp[i]->physdimension);
        painter->drawText(crosshair_2.x_position + 5, crosshair_2.y_position - (10 + (font_pixel_height * 4)), string);
        snprintf(string, 128, "%2i:%02i:%02i.%04i",
                        (int)(((crosshair_2.time / TIME_DIMENSION)/ 3600LL) % 24LL),
                        (int)(((crosshair_2.time / TIME_DIMENSION) % 3600LL) / 60LL),
                        (int)((crosshair_2.time / TIME_DIMENSION) % 60LL),
                        (int)((crosshair_2.time % TIME_DIMENSION) / 1000LL));
        if(crosshair_2.time_relative >= TIME_DIMENSION)
        {
          snprintf(string + strlen(string), 32, " (%i:%02i:%02i.%04i)",
                  (int)((crosshair_2.time_relative / TIME_DIMENSION)/ 3600LL),
                  (int)(((crosshair_2.time_relative / TIME_DIMENSION) % 3600LL) / 60LL),
                  (int)((crosshair_2.time_relative / TIME_DIMENSION) % 60LL),
                  (int)((crosshair_2.time_relative % TIME_DIMENSION) / 1000LL));
        }
        else
        {
          convert_to_metric_suffix(str3, (double)crosshair_2.time_relative / TIME_DIMENSION, 3, 128);

          snprintf(string + strlen(string), 32, " (%.26sS)", str3);
        }

        painter->drawText(crosshair_2.x_position + 5, crosshair_2.y_position - (10 + (font_pixel_height * 3)), string);
        snprintf(string, 128, "delta %+f %s",
                        crosshair_2.value - crosshair_1.value,
                        signalcomp[i]->physdimension);
        painter->drawText(crosshair_2.x_position + 5, crosshair_2.y_position - (10 + (font_pixel_height * 2)), string);
        l_time = crosshair_2.time - crosshair_1.time;
        if(l_time<0) l_time = -l_time;
        if(l_time >= TIME_DIMENSION)
        {
          snprintf(string, 128, "delta %i:%02i:%02i.%04i",
                          (int)((l_time / TIME_DIMENSION)/ 3600LL),
                          (int)(((l_time / TIME_DIMENSION) % 3600LL) / 60LL),
                          (int)((l_time / TIME_DIMENSION) % 60LL),
                          (int)((l_time % TIME_DIMENSION) / 1000LL));
        }
        else
        {
          convert_to_metric_suffix(str3, (double)l_time / TIME_DIMENSION, 3, 128);

          snprintf(string, 32, "delta %.24sS", str3);
        }
        painter->drawText(crosshair_2.x_position + 5, crosshair_2.y_position - (10 + font_pixel_height), string);
        if(signalcomp[i]->alias[0] != 0)
        {
          painter->drawText(crosshair_2.x_position + 5, crosshair_2.y_position - 10, signalcomp[i]->alias);
        }
        else
        {
          painter->drawText(crosshair_2.x_position + 5, crosshair_2.y_position - 10, signalcomp[i]->signallabel);
        }
        painter->setPen((Qt::GlobalColor)signalcomp[i]->color);
      }
    }

    if(signalcomp[i]->hasoffsettracking)
    {
      snprintf(string, 128, "offset: %f %s",
        -signalcomp[i]->screen_offset * mainwindow->y_pixelsizefactor * signalcomp[i]->voltpercm,
        signalcomp[i]->physdimension);
      painter->fillRect(132 * w_scaling, baseline - (2 * h_scaling), 190 * w_scaling, -15 * h_scaling, backgroundcolor);
      painter->setPen((Qt::GlobalColor)signalcomp[i]->color);
      painter->drawText(135 * w_scaling, baseline - (5 * h_scaling), string);
    }

    if(signalcomp[i]->hasgaintracking)
    {
      snprintf(string, 128, "amplitude: %f %s/cm",
        signalcomp[i]->voltpercm,
        signalcomp[i]->physdimension);
      painter->fillRect(132 * w_scaling, baseline - (2 * h_scaling), 190 * w_scaling, -15 * h_scaling, backgroundcolor);
      painter->setPen((Qt::GlobalColor)signalcomp[i]->color);
      painter->drawText(135 * w_scaling, baseline - (5 * h_scaling), string);
    }
  }

  vertical_distance = h / (signalcomps + 1);

  for(i=0; i<signalcomps; i++)
  {
    baseline = vertical_distance * (i + 1);

    if(signalcomp[i]->alias[0] != 0)
    {
      painter->fillRect(2 * w_scaling, baseline - (2 * h_scaling), strlen(signalcomp[i]->alias) * 8 * w_scaling, -15 * h_scaling, backgroundcolor);
    }
    else
    {
      painter->fillRect(2 * w_scaling, baseline - (2 * h_scaling), strlen(signalcomp[i]->signallabel) * 8 * w_scaling, -15 * h_scaling, backgroundcolor);
    }

    painter->setPen((Qt::GlobalColor)signalcomp[i]->color);

    if(signalcomp[i]->alias[0] != 0)
    {
      painter->drawText(5 * w_scaling, baseline - (5 * h_scaling), signalcomp[i]->alias);
    }
    else
    {
      painter->drawText(5 * w_scaling, baseline - (5 * h_scaling), signalcomp[i]->signallabel);
    }

    if(signalcomp[i]->hasruler)
    {
      floating_ruler(painter, ruler_x_position, ruler_y_position, signalcomp[i], print_linewidth);
    }
  }

  if(draw_zoom_rectangle)
  {
    special_pen->setColor(mouse_rect_color);

    painter->setPen(*special_pen);

    painter->drawLine(mouse_press_coordinate_x, mouse_press_coordinate_y, mouse_press_coordinate_x, mouse_y);
    painter->drawLine(mouse_press_coordinate_x, mouse_press_coordinate_y, mouse_x, mouse_press_coordinate_y);
    painter->drawLine(mouse_x, mouse_y, mouse_press_coordinate_x, mouse_y);
    painter->drawLine(mouse_x, mouse_y, mouse_x, mouse_press_coordinate_y);
  }

  painter->setPen(text_color);

  if(printing)
  {
    painter->drawText((int)(8.0 * printsize_y_factor), h - (int)(4.0 * printsize_y_factor), mainwindow->viewtime_string);
    painter->drawText(w - (int)(60.0 * printsize_y_factor), h - (int)(4.0 * printsize_y_factor), mainwindow->pagetime_string);
  }
  else
  {
//    painter->fillRect(5, h - (4 + font_pixel_height), strlen(mainwindow->viewtime_string) * font_pixel_width, (4 + font_pixel_height), backgroundcolor);
    painter->drawText(8, h - 4, mainwindow->viewtime_string);
//    painter->fillRect(w - 63, h - (4 + font_pixel_height), 60, (4 + font_pixel_height), backgroundcolor);
    painter->drawText(w - strlen(mainwindow->pagetime_string) * font_pixel_width, h - 4, mainwindow->pagetime_string);
  }
}


inline void ViewCurve::floating_ruler(QPainter *painter, int x_pos, int y_pos, struct signalcompblock *signalcomp, int print_linewidth)
{
  int i, j, v_use_exp=0, h_use_exp=0;

  double d_tmp,
         d_tmp2,
         h_size,
         w_size,
         pixels_per_second;

  char str_hz[7][16]={" 9", "10", "11", "12", "13", "14", "15"};

  char str_uv[5][16]={"200", "150", "100", "50", "0"};

  char str_s[4][16]={"1.00", "0.75", "0.50", "0.25"};


  if(w < 400)
  {
    return;
  }

  if(floating_ruler_use_var_width)
  {
    if((mainwindow->pagetime / (TIME_DIMENSION / 10LL)) < 2LL)
    {
      h_use_exp = 1;
    }
  }
  else if((mainwindow->pagetime / TIME_DIMENSION) < 2LL)
    {
      h_use_exp = 1;
    }

  if((mainwindow->pagetime / TIME_DIMENSION) > 60LL)
  {
    h_use_exp = 1;
  }

  if((signalcomp->voltpercm < 1.0) && (signalcomp->voltpercm > -1.0))
  {
    v_use_exp = 1;
  }

  if((signalcomp->voltpercm > 2000.0) || (signalcomp->voltpercm < -2000.0))
  {
    v_use_exp = 1;
  }

  x_pos *= printsize_x_factor;
  y_pos *= printsize_y_factor;

  pixels_per_second = ((double)w) / (((double)mainwindow->pagetime) / ((double)TIME_DIMENSION));

  h_size = 4.0 * painter_pixelsizefactor;

  if(floating_ruler_use_var_width)
  {
    w_size = pixels_per_second;

    while(w_size > (w / 5))
    {
      w_size /= 2;
    }

    while(w_size < (w / 20))
    {
      w_size *= 2;
    }
  }
  else
  {
    w_size = (double)w / 10.0;
  }

  d_tmp = h_size / 7.0;

  d_tmp2 = h_size / 14.0;

  if(float_ruler_more)
  {
    for(i=0; i<7; i++)
    {
      if(h_use_exp)
      {
        snprintf(str_hz[i], 15, "%.1e",  (pixels_per_second / w_size) * (2.0 + i));
      }
      else
      {
        if(floating_ruler_use_var_width)
        {
          snprintf(str_hz[i], 15, "%.0f",  (pixels_per_second / w_size) * (2.0 + i));
        }
        else
        {
          snprintf(str_hz[i], 15, "%.1f",  (pixels_per_second / w_size) * (2.0 + i));
        }
      }

      str_hz[i][15] = 0;
    }

    if(((int)signalcomp->voltpercm) < 20)
    {
      for(i=0; i<5; i++)
      {
        if(v_use_exp)
        {
          snprintf(str_uv[i], 15, "%.1e", signalcomp->voltpercm * i * (double)(signalcomp->polarity * -1));
        }
        else
        {
          snprintf(str_uv[i], 15, "%.1f", signalcomp->voltpercm * i * (double)(signalcomp->polarity * -1));
        }

        str_uv[i][15] = 0;
      }
    }
    else
    {
      for(i=0; i<5; i++)
      {
        if(v_use_exp)
        {
          snprintf(str_uv[i], 15, "%.0e", signalcomp->voltpercm * i * (double)(signalcomp->polarity * -1));
        }
        else
        {
          snprintf(str_uv[i], 15, "%i", (int)(signalcomp->voltpercm * i * (double)(signalcomp->polarity * -1)));
        }

        str_uv[i][15] = 0;
      }
    }
  }
  else
  {
    for(i=0; i<7; i++)
    {
      if(h_use_exp)
      {
        snprintf(str_hz[i], 15, "%.1e",  (pixels_per_second / w_size) * (9.0 + i));
      }
      else
      {
        if(floating_ruler_use_var_width)
        {
          snprintf(str_hz[i], 15, "%.0f",  (pixels_per_second / w_size) * (9.0 + i));
        }
        else
        {
          snprintf(str_hz[i], 15, "%.1f",  (pixels_per_second / w_size) * (9.0 + i));
        }
      }

      str_hz[i][15] = 0;
    }

    if(((int)signalcomp->voltpercm) < 20)
    {
      for(i=0; i<5; i++)
      {
        if(v_use_exp)
        {
          snprintf(str_uv[i], 15, "%.1e", signalcomp->voltpercm * (4 - i) * (double)signalcomp->polarity);
        }
        else
        {
          snprintf(str_uv[i], 15, "%.1f", signalcomp->voltpercm * (4 - i) * (double)signalcomp->polarity);
        }

        str_uv[i][15] = 0;
      }
    }
    else
    {
      for(i=0; i<5; i++)
      {
        if(v_use_exp)
        {
          snprintf(str_uv[i], 15, "%.0e", signalcomp->voltpercm * (4 - i) * signalcomp->polarity);
        }
        else
        {
          snprintf(str_uv[i], 15, "%i", (int)(signalcomp->voltpercm * (4 - i)) * signalcomp->polarity);
        }

        str_uv[i][15] = 0;
      }
    }
  }

  for(i=0; i<4; i++)
  {
    if(h_use_exp)
    {
      if(floating_ruler_use_var_width)
      {
        snprintf(str_s[i], 15, "%.1e",  ((w_size / 4.0) / pixels_per_second) * (4 - i));
      }
      else
      {
        snprintf(str_s[i], 15, "%.1e",  ((w / 40.0) / pixels_per_second) * (4 - i));
      }
    }
    else
    {
      if(floating_ruler_use_var_width)
      {
        snprintf(str_s[i], 15, "%.2f",  ((w_size / 4.0) / pixels_per_second) * (4 - i));
      }
      else
      {
        snprintf(str_s[i], 15, "%.2f",  ((w / 40.0) / pixels_per_second) * (4 - i));
      }
    }

    str_s[i][15] = 0;
  }

  painter->setPen((Qt::GlobalColor)floating_ruler_color);

  painter->drawText(x_pos, y_pos - (h / 80.0), "Hz");

  for(i=0; i<7; i++)
  {
    painter->drawText(x_pos - ((w / 60.0) * w_scaling), y_pos + (h / 204.8) + d_tmp2 + (d_tmp * i) + (h_scaling * 3.0 - 3.0), str_hz[i]);
  }

  for(i=0; i<8; i++)
  {
    painter->drawLine(x_pos, y_pos + (d_tmp * i), x_pos + w_size, y_pos + (d_tmp * i));
  }
  painter->drawLine(x_pos + w_size, y_pos, x_pos + w_size, y_pos + h_size);
  painter->drawLine(x_pos, y_pos, x_pos, y_pos + h_size);

  if(float_ruler_more)
  {
    for(j=2; j<9; j++)
    {
      d_tmp2 = w_size / j;

      for(i=1; i<j; i++)
      {
        painter->drawLine(x_pos + (d_tmp2 * i), y_pos + (d_tmp * (j - 2)), x_pos + (d_tmp2 * i), y_pos + (d_tmp * (j - 1)));
      }
    }
  }
  else
  {
    for(j=9; j<16; j++)
    {
      d_tmp2 = w_size / j;

      for(i=1; i<j; i++)
      {
        painter->drawLine(x_pos + (d_tmp2 * i), y_pos + (d_tmp * (j - 9)), x_pos + (d_tmp2 * i), y_pos + (d_tmp * (j - 8)));
      }
    }
  }

  painter->drawText(x_pos + (3.0 * printsize_x_factor), y_pos + h_size + (25.0 * printsize_y_factor), "more...");

  x_pos += (w_size + ((double)w / 18.0));

  d_tmp = h_size / 4.0;

  painter->drawText(x_pos, y_pos - ((double)h / 80.0), signalcomp->physdimension);

  for(i=0; i<5; i++)
  {
    painter->drawText(x_pos - ((w / 80.0) * w_scaling), y_pos + ((double)h / 204.8) + (d_tmp * i) + (h_scaling * 3.0 - 3.0), str_uv[i]);

    painter->drawLine(x_pos + ((double)w / 160.0), y_pos + (d_tmp * i), x_pos + ((double)w / 45.7), y_pos + (d_tmp * i));
  }

  painter->drawLine(x_pos + ((double)w / 45.7), y_pos, x_pos + ((double)w / 45.7), y_pos + h_size);

  d_tmp = h_size / 20.0;

  for(i=1; i<20; i++)
  {
    painter->drawLine(x_pos + ((double)w / 71.1), y_pos + (d_tmp * i), x_pos + ((double)w / 45.7), y_pos + (d_tmp * i));
  }

  x_pos += ((double)w / 10.0);

  painter->drawLine(x_pos, y_pos, x_pos + (w_size / 2.0), y_pos + h_size);
  painter->drawLine(x_pos + w_size, y_pos, x_pos + (w_size / 2.0), y_pos + h_size);

  d_tmp = h_size / 4.0;
  d_tmp2 = w_size / 8.0;

  for(i=0; i<4; i++)
  {
    painter->drawLine(x_pos + (d_tmp2 * i), y_pos + (d_tmp * i), x_pos + w_size - (d_tmp2 * i), y_pos + (d_tmp * i));
  }

  for(i=0; i<4; i++)
  {
    painter->drawText(x_pos - ((w / 50.0) * w_scaling) + (d_tmp2 * i), y_pos + ((double)h / 204.8) + (d_tmp * i), str_s[i]);
  }

  painter->drawText(x_pos + ((double)w / 125.0) + w_size, y_pos + ((double)h / 204.8), "Sec");

  special_pen->setColor((Qt::GlobalColor)floating_ruler_color);

  special_pen->setWidth(print_linewidth);

  painter->setPen(*special_pen);

  d_tmp = h_size / 20.0;
  d_tmp2 = w_size / 40.0;

  for(i=1; i<20; i++)
  {
    painter->drawLine(x_pos + (d_tmp2 * i), y_pos + (d_tmp * i), x_pos + w_size - (d_tmp2 * i), y_pos + (d_tmp * i));
  }
}














