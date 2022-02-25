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


#include "mainwindow.h"


int UI_Mainwindow::read_project_file(const char *path_pro)
{
  int i;

  char result[XML_STRBUFLEN],
       scratchpad[2048]="",
       str[128]="",
       str2[512]="",
       *err_ptr=NULL,
       *filter_spec_ptr=NULL,
       spec_str[256]="";

  double frequency=1.0,
         frequency2=2.0,
         ripple=1.0,
         velocity=1.0,
         dthreshold,
         fir_vars[1000];


  struct xml_handle *xml_hdl=NULL;

  struct signalcompblock *newsignalcomp=NULL;

  struct edfhdrblock *edf_hdr=NULL;

  if(path_pro == NULL) return -999;

  xml_hdl = xml_get_handle(path_pro);
  if(xml_hdl==NULL)
  {
    snprintf(scratchpad, 2048, "Can not open project file:\n%s", path_pro);
    QMessageBox messagewindow(QMessageBox::Critical, "Error", QString::fromLocal8Bit(scratchpad));
    messagewindow.exec();
    return -1;
  }

  if(strcmp(xml_hdl->elementname[xml_hdl->level], PROGRAM_NAME "_project"))
  {
    return project_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
  }

  if(xml_goto_nth_element_inside(xml_hdl, "edf_files", 0))
  {
    return project_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
  }

  for(i=0; i<MAXFILES; i++)
  {
    if(xml_goto_nth_element_inside(xml_hdl, "file", i))
    {
      break;
    }

    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      return project_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
    }

    cmdlineargument = 0;

    rc_file_open_err = 0;

    rc_file_open_requested = 1;

    strlcpy(path, result, MAX_PATH_LENGTH);

    open_new_file();

    rc_file_open_requested = 0;

    if(rc_file_open_err)
    {
      printf("read_project_file(): error: %i\n", rc_file_open_err);

      return project_format_error(__FILE__, __LINE__, newsignalcomp, xml_hdl);
    }

    xml_go_up(xml_hdl);
  }






  xml_close(xml_hdl);

  return 0;
}




int UI_Mainwindow::project_format_error(const char *file_name, int line_number, struct signalcompblock *sigcomp, struct xml_handle *hdl)
{
  char str[2048]="";

  snprintf(str, 2048, "There seems to be an error in this project file.\nFile: %s\nline: %i", file_name, line_number);
  QMessageBox messagewindow(QMessageBox::Critical, "Error", str);
  messagewindow.exec();
  free(sigcomp);
  xml_close(hdl);
  return 0;
}




