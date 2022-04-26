/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2022 Teunis van Beelen
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


#include "math_func.h"


struct math_func_settings * create_math_func(int func_f)
{
  struct math_func_settings *st;

  st = (struct math_func_settings *) calloc(1, sizeof(struct math_func_settings));
  if(st==NULL)  return NULL;

  st->func = func_f;

  return st;
}


void free_math_func(struct math_func_settings *st)
{
  free(st);
}


double run_math_func(double val, struct math_func_settings *st)
{
  if(st->func == MATH_FUNC_SQUARE)
  {
    if(val < 0)
    {
      return (-val * val);
    }
    else
    {
      return (val * val);
    }
  }
  else if(st->func == MATH_FUNC_SQRT)
    {
      if(val < 0)
        {
          return (-sqrt(-val));
        }
        else
        {
          return sqrt(val);
        }
    }
    else if(st->func == MATH_FUNC_NONE)
      {
        return val;
      }

  return 0;
}















