/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2010 - 2021 Teunis van Beelen
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



#include "header_editor.h"



UI_headerEditorWindow::UI_headerEditorWindow(QWidget *w_parent)
{
  mainwindow = (UI_Mainwindow *)w_parent;

  myobjectDialog = new QDialog;
  myobjectDialog->setMinimumSize(670 * mainwindow->w_scaling, 670 * mainwindow->h_scaling);
  myobjectDialog->setWindowTitle("EDF header editor");
  myobjectDialog->setSizeGripEnabled(true);
  myobjectDialog->setModal(true);
  myobjectDialog->setAttribute(Qt::WA_DeleteOnClose, true);

  edf = 0;
  bdf = 0;
  edfplus = 0;
  bdfplus = 0;
  edfsignals = 0;

  file = NULL;

  hdr = NULL;

/************************************* TAB 1 *********************************/

  fileNameLabel = new QLabel;

  startTimeDateLabel = new QLabel;
  startTimeDateLabel->setText("Startdate/time");
  startTimeDateLabel->setVisible(false);

  startTimeDate = new QDateTimeEdit;
  startTimeDate->setDisplayFormat("dd.MM.yyyy hh:mm:ss");
  startTimeDate->setMinimumDate(QDate(1985, 1, 1));
  startTimeDate->setMaximumDate(QDate(2084, 12, 31));
  startTimeDate->setVisible(false);

  label1 = new QLabel;
  label1->setText("Subject");
  label1->setVisible(false);

  lineEdit1 = new QLineEdit;
  lineEdit1->setMaxLength(80);
  lineEdit1->setVisible(false);

  label2 = new QLabel;
  label2->setText("Recording");
  label2->setVisible(false);

  lineEdit2 = new QLineEdit;
  lineEdit2->setMaxLength(80);
  lineEdit2->setVisible(false);

  label3 = new QLabel;
  label3->setText("Subject code");
  label3->setVisible(false);

  lineEdit3 = new QLineEdit;
  lineEdit3->setMaxLength(80);
  lineEdit3->setVisible(false);

  label4 = new QLabel;
  label4->setText("Subject name");
  label4->setVisible(false);

  lineEdit4 = new QLineEdit;
  lineEdit4->setMaxLength(80);
  lineEdit4->setVisible(false);

  label11 = new QLabel;
  label11->setText("Gender");
  label11->setVisible(false);

  comboBox1 = new QComboBox;
  comboBox1->addItem("unknown");
  comboBox1->addItem("male");
  comboBox1->addItem("female");
  comboBox1->setVisible(false);

  charsleft1Label = new QLabel;
  charsleft1Label->setVisible(false);

  label12 = new QLabel;
  label12->setText("Birthdate");
  label12->setVisible(false);

  dateEdit1 = new QDateEdit;
  dateEdit1->setDisplayFormat("d MMM yyyy");
  dateEdit1->setVisible(false);

  checkBox1 = new QCheckBox("No birthdate");
  checkBox1->setTristate(false);
  checkBox1->setCheckState(Qt::Unchecked);
  checkBox1->setVisible(false);

  label5 = new QLabel;
  label5->setText("Additional info");
  label5->setVisible(false);

  lineEdit5 = new QLineEdit;
  lineEdit5->setMaxLength(80);
  lineEdit5->setVisible(false);

  label6 = new QLabel;
  label6->setText("Admin. code");
  label6->setVisible(false);

  lineEdit6 = new QLineEdit;
  lineEdit6->setMaxLength(80);
  lineEdit6->setVisible(false);

  label7 = new QLabel;
  label7->setText("Technician");
  label7->setVisible(false);

  lineEdit7 = new QLineEdit;
  lineEdit7->setMaxLength(80);
  lineEdit7->setVisible(false);

  label8 = new QLabel;
  label8->setText("Device");
  label8->setVisible(false);

  lineEdit8 = new QLineEdit;
  lineEdit8->setMaxLength(80);
  lineEdit8->setVisible(false);

  label9 = new QLabel;
  label9->setText("Additional info");
  label9->setVisible(false);

  lineEdit9 = new QLineEdit;
  lineEdit9->setMaxLength(80);
  lineEdit9->setVisible(false);

  charsleft2Label = new QLabel;
  charsleft2Label->setVisible(false);

  label13 = new QLabel;
  label13->setText("Number of datarecords");
  label13->setVisible(false);

  lineEdit10 = new QLineEdit;
  lineEdit10->setEnabled(false);
  lineEdit10->setVisible(false);

  label14 = new QLabel;
  label14->setText("Datarecord duration");
  label14->setVisible(false);

  lineEdit11 = new QLineEdit;
  lineEdit11->setEnabled(false);
  lineEdit11->setVisible(false);

  label15 = new QLabel;
  label15->setText("Number of signals");
  label15->setVisible(false);

  lineEdit12 = new QLineEdit;
  lineEdit12->setEnabled(false);
  lineEdit12->setVisible(false);

  label16 = new QLabel;
  label16->setText("Reserved");
  label16->setVisible(false);

  lineEdit13 = new QLineEdit;
  lineEdit13->setEnabled(false);
  lineEdit13->setVisible(false);

  label17 = new QLabel;
  label17->setText("Version");
  label17->setVisible(false);

  lineEdit14 = new QLineEdit;
  lineEdit14->setEnabled(false);
  lineEdit14->setVisible(false);

/************************************* TAB 2 *********************************/

  signallist = new QTableWidget;
  signallist->setSelectionMode(QAbstractItemView::NoSelection);
  signallist->setEditTriggers(QAbstractItemView::NoEditTriggers);
  signallist->setColumnCount(9);
  signallist->setSelectionMode(QAbstractItemView::NoSelection);
  QStringList horizontallabels;
  horizontallabels += "Label";
  horizontallabels += "Physical dimension";
  horizontallabels += "Prefilter";
  horizontallabels += "Transducer";
  horizontallabels += "Physical maximum";
  horizontallabels += "Physical minimum";
  horizontallabels += "Digital maximum";
  horizontallabels += "Digital minimum";
  horizontallabels += "Samples per datarecord";
  signallist->setHorizontalHeaderLabels(horizontallabels);
  signallist->resizeColumnsToContents();

  pushButton1 = new QPushButton;
  pushButton1->setText("Close");

  pushButton2 = new QPushButton;
  pushButton2->setText("Save");
  pushButton2->setToolTip("By clicking on \"Save\", the file will be checked for\n"
                          "errors and, if found, it will try to correct them.");

  pushButton3 = new QPushButton;
  pushButton3->setText("Select file");

  helpButton = new QPushButton;
  helpButton->setText("Help");

  QHBoxLayout *hlayout2 = new QHBoxLayout;
  hlayout2->addWidget(dateEdit1);
  hlayout2->addSpacing(20);
  hlayout2->addWidget(checkBox1);
  hlayout2->addStretch(1000);

  QHBoxLayout *hlayout3 = new QHBoxLayout;
  hlayout3->addWidget(startTimeDate);
  hlayout3->addStretch(1000);

  QFormLayout *flayout = new QFormLayout;
  flayout->addRow(startTimeDateLabel, hlayout3);
  flayout->addRow(label1, lineEdit1);
  flayout->addRow(label2, lineEdit2);
  flayout->addRow(label3, lineEdit3);
  flayout->addRow(label4, lineEdit4);
  flayout->addRow(label11, comboBox1);
  flayout->addRow(label12, hlayout2);
  flayout->addRow(label5, lineEdit5);
  flayout->addRow(NULL, charsleft1Label);
  flayout->addRow(" ", (QWidget *)NULL);
  flayout->addRow(label6, lineEdit6);
  flayout->addRow(label7, lineEdit7);
  flayout->addRow(label8, lineEdit8);
  flayout->addRow(label9, lineEdit9);
  flayout->addRow(NULL, charsleft2Label);
  flayout->addRow(" ", (QWidget *)NULL);
  flayout->addRow(label17, lineEdit14);
  flayout->addRow(label13, lineEdit10);
  flayout->addRow(label14, lineEdit11);
  flayout->addRow(label15, lineEdit12);
  flayout->addRow(label16, lineEdit13);

  QVBoxLayout *vlayout2 = new QVBoxLayout;
  vlayout2->addWidget(fileNameLabel);
  vlayout2->addSpacing(20);
  vlayout2->addLayout(flayout);
  vlayout2->addStretch(1000);

  tab1 = new QWidget;
  tab1->setLayout(vlayout2);

  tabholder = new QTabWidget;
  tabholder->addTab(tab1, "Header");
  tabholder->addTab(signallist, "Signals");

  QHBoxLayout *hlayout1 = new QHBoxLayout;
  hlayout1->addWidget(pushButton3);
  hlayout1->addStretch(1000);
  hlayout1->addWidget(pushButton2);
  hlayout1->addStretch(1000);
  hlayout1->addWidget(helpButton);
  hlayout1->addStretch(1000);
  hlayout1->addWidget(pushButton1);

  QVBoxLayout *vlayout1 = new QVBoxLayout;
  vlayout1->addWidget(tabholder, 1000);
  vlayout1->addSpacing(20);
  vlayout1->addLayout(hlayout1);

  myobjectDialog->setLayout(vlayout1);

  QMessageBox::warning(myobjectDialog, "Warning", "Always make a backup copy of your file before using this tool");

  connect(pushButton1,    SIGNAL(clicked()),            myobjectDialog, SLOT(close()));
  connect(pushButton2,    SIGNAL(clicked()),            this,           SLOT(save_hdr()));
  connect(pushButton3,    SIGNAL(clicked()),            this,           SLOT(open_file()));
  connect(helpButton,     SIGNAL(clicked()),            this,           SLOT(helpbuttonpressed()));

  myobjectDialog->exec();
}


UI_headerEditorWindow::~UI_headerEditorWindow()
{
  if(file != NULL)
  {
    fclose(file);
  }

  if(hdr != NULL)
  {
    free(hdr);
  }
}


void UI_headerEditorWindow::open_file()
{
  long long filesize;


  if(file != NULL)
  {
    fclose(file);
    file = NULL;
  }

  edf = 0;
  bdf = 0;
  edfplus = 0;
  bdfplus = 0;
  edfsignals = 0;

  disconnect(lineEdit3,   SIGNAL(textEdited(const QString &)), this, SLOT(calculate_chars_left_name(const QString &)));
  disconnect(lineEdit4,   SIGNAL(textEdited(const QString &)), this, SLOT(calculate_chars_left_name(const QString &)));
  disconnect(lineEdit5,   SIGNAL(textEdited(const QString &)), this, SLOT(calculate_chars_left_name(const QString &)));
  disconnect(checkBox1,   SIGNAL(stateChanged(int)),           this, SLOT(calculate_chars_left_name(int)));
  disconnect(lineEdit6,   SIGNAL(textEdited(const QString &)), this, SLOT(calculate_chars_left_recording(const QString &)));
  disconnect(lineEdit7,   SIGNAL(textEdited(const QString &)), this, SLOT(calculate_chars_left_recording(const QString &)));
  disconnect(lineEdit8,   SIGNAL(textEdited(const QString &)), this, SLOT(calculate_chars_left_recording(const QString &)));
  disconnect(lineEdit9,   SIGNAL(textEdited(const QString &)), this, SLOT(calculate_chars_left_recording(const QString &)));

  lineEdit3->setMaxLength(80);
  lineEdit4->setMaxLength(80);
  lineEdit5->setMaxLength(80);
  lineEdit6->setMaxLength(80);
  lineEdit7->setMaxLength(80);
  lineEdit8->setMaxLength(80);
  lineEdit9->setMaxLength(80);

  lineEdit1->clear();
  lineEdit2->clear();
  lineEdit3->clear();
  lineEdit4->clear();
  lineEdit5->clear();
  lineEdit6->clear();
  lineEdit7->clear();
  lineEdit8->clear();
  lineEdit9->clear();
  lineEdit10->clear();
  lineEdit11->clear();
  lineEdit12->clear();
  lineEdit13->clear();
  lineEdit14->clear();

  signallist->setRowCount(0);

  fileNameLabel->clear();

  free(hdr);
  hdr = (char *)calloc(1, 256 * 3);
  if(hdr==NULL)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "A memory allocation error occurred. (hdr)");
    messagewindow.exec();
    return;
  }

  strlcpy(path, QFileDialog::getOpenFileName(0, "Open file", QString::fromLocal8Bit(mainwindow->recent_opendir), "EDF/BDF files (*.edf *.EDF *.bdf *.BDF)").toLocal8Bit().data(), MAX_PATH_LENGTH);

  if(!strcmp(path, ""))
  {
    return;
  }

  get_directory_from_path(mainwindow->recent_opendir, path, MAX_PATH_LENGTH);

  if(mainwindow->file_is_opened(path))
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Selected file is in use.");
    messagewindow.exec();
    return;
  }

  file = fopeno(path, "r+b");
  if(file==NULL)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Can not open file.");
    messagewindow.exec();
    return;
  }

  rewind(file);

  if(fread(hdr, 256, 1, file) != 1)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Can not read from file.");
    messagewindow.exec();
    fclose(file);
    file = NULL;
    return;
  }

  hdr[256] = 0;

  if(!(strncmp(hdr, "0       ", 8)))  edf = 1;

  if((!(strncmp(hdr + 1, "BIOSEMI", 7))) && (((unsigned char *)hdr)[0] == 255))
  {
    bdf = 1;
  }

  if((!edf) && (!bdf))
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "File is not a valid EDF or BDF file.\n"
                                                              "Invalid version.");
    messagewindow.exec();
    fclose(file);
    file = NULL;
    return;
  }

  edfsignals = antoi(hdr + 252, 4);

  if(edfsignals < 1)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Invalid number of signals in header (less than 1)");
    messagewindow.exec();
    fclose(file);
    file = NULL;
    return;
  }

  fseeko(file, 0LL, SEEK_END);
  filesize = ftello(file);
  if(filesize < (((edfsignals + 1LL) * 256LL) + edfsignals))
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Filesize is too small.\n"
                                                              "Can not fix this file.");
    messagewindow.exec();
    fclose(file);
    file = NULL;
    return;
  }

  if((!(strncmp(hdr + 192, "EDF+C", 5))) || (!(strncmp(hdr + 192, "EDF+D", 5))))
  {
    edfplus = 1;
  }

  if((!(strncmp(hdr + 192, "BDF+C", 5))) || (!(strncmp(hdr + 192, "BDF+D", 5))))
  {
    bdfplus = 1;
  }

  read_header();
}


void UI_headerEditorWindow::read_header()
{
  int i, j, p, r;

  char scratchpad[256],
       str[256];


  if(file == NULL)
  {
    return;
  }

  fileNameLabel->setText(QString::fromLocal8Bit(path));

  rewind(file);

  free(hdr);
  hdr = (char *)calloc(1, 256 * (edfsignals + 3));
  if(hdr==NULL)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "A memory allocation error occurred. (hdr)");
    messagewindow.exec();
    fclose(file);
    file = NULL;
    return;
  }

  if(fread(hdr, 256 * (edfsignals + 1), 1, file) != 1)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Can not read from file.");
    messagewindow.exec();
    fclose(file);
    file = NULL;
    return;
  }

  hdr[256 * (edfsignals + 1)] = 0;

  startTimeDate->setVisible(true);
  startTimeDateLabel->setVisible(true);
  startTimeDate->setMinimumDate(QDate(1985, 1, 1));
  startTimeDate->setMaximumDate(QDate(2084, 12, 31));
  startTimeDate->setDate(QDate(1985, 1, 1));
  startTimeDate->setTime(QTime(0, 0, 0));

  memcpy(scratchpad, hdr + 168, 8);
  scratchpad[2] = 0;
  scratchpad[5] = 0;
  scratchpad[8] = 0;
  r = atoi(scratchpad + 6);
  if(r < 85)
  {
    r += 2000;
  }
  else
  {
    r += 1900;
  }
  startTimeDate->setDate(QDate(r, atoi(scratchpad + 3), atoi(scratchpad)));
  startTimeDate->setMinimumDate(QDate(1985, 1, 1));
  startTimeDate->setMaximumDate(QDate(2084, 12, 31));

  memcpy(scratchpad, hdr + 176, 8);
  scratchpad[2] = 0;
  scratchpad[5] = 0;
  scratchpad[8] = 0;
  startTimeDate->setTime(QTime(atoi(scratchpad), atoi(scratchpad + 3), atoi(scratchpad + 6)));

  if(edfplus || bdfplus)
  {
    label1->setVisible(false);
    label2->setVisible(false);

    lineEdit1->setVisible(false);
    lineEdit2->setVisible(false);

    label3->setVisible(true);
    label4->setVisible(true);
    label5->setVisible(true);
    label6->setVisible(true);
    label7->setVisible(true);
    label8->setVisible(true);
    label9->setVisible(true);
    label11->setVisible(true);
    label12->setVisible(true);
    charsleft1Label->setVisible(true);
    charsleft2Label->setVisible(true);

    lineEdit3->setVisible(true);
    lineEdit4->setVisible(true);
    lineEdit5->setVisible(true);
    lineEdit6->setVisible(true);
    lineEdit7->setVisible(true);
    lineEdit8->setVisible(true);
    lineEdit9->setVisible(true);

    comboBox1->setVisible(true);

    checkBox1->setVisible(true);

    dateEdit1->setVisible(true);

    strncpy(scratchpad, hdr + 8, 80);
    scratchpad[80] = 0;
    strlcat(scratchpad, "    ", 256);

    p = 0;

    for(i=0; i<80; i++)
    {
      if(scratchpad[i] == ' ')  break;
    }
    if(strncmp(scratchpad, "X ", 2))
    {
      scratchpad[i] = 0;
      for(j=p; j<i; j++)
      {
        if(scratchpad[j] == '_')  scratchpad[j] = ' ';
      }
      strlcpy(str, scratchpad, 256);
      remove_trailing_spaces(str);
      lineEdit3->setText(str);
    }
    else
    {
      lineEdit3->clear();
    }

    p = ++i;

    for(; i<80; i++)
    {
      if(scratchpad[i] == ' ')  break;
    }
    comboBox1->setCurrentIndex(0);
    if(!(strncmp(scratchpad + p, "M ", 2)))
    {
      comboBox1->setCurrentIndex(1);
    }
    if(!(strncmp(scratchpad + p, "F ", 2)))
    {
      comboBox1->setCurrentIndex(2);
    }

    p = ++i;

    for(; i<80; i++)
    {
      if(scratchpad[i] == ' ')  break;
    }
    dateEdit1->setDate(QDate(1800, 1, 1));
    if(strncmp(scratchpad + p, "X ", 2))
    {
      scratchpad[i] = 0;

      r = 0;
      if(!strncmp(scratchpad + p + 3, "JAN", 3))  r = 1;
        else if(!strncmp(scratchpad + p + 3, "FEB", 3))  r = 2;
          else if(!strncmp(scratchpad + p + 3, "MAR", 3))  r = 3;
            else if(!strncmp(scratchpad + p + 3, "APR", 3))  r = 4;
              else if(!strncmp(scratchpad + p + 3, "MAY", 3))  r = 5;
                else if(!strncmp(scratchpad + p + 3, "JUN", 3))  r = 6;
                  else if(!strncmp(scratchpad + p + 3, "JUL", 3))  r = 7;
                    else if(!strncmp(scratchpad + p + 3, "AUG", 3))  r = 8;
                      else if(!strncmp(scratchpad + p + 3, "SEP", 3))  r = 9;
                        else if(!strncmp(scratchpad + p + 3, "OCT", 3))  r = 10;
                          else if(!strncmp(scratchpad + p + 3, "NOV", 3))  r = 11;
                            else if(!strncmp(scratchpad + p + 3, "DEC", 3))  r = 12;

      if(r)
      {
        dateEdit1->setDate(QDate(atoi(scratchpad + p + 7), r, atoi(scratchpad + p)));

        checkBox1->setCheckState(Qt::Unchecked);
      }

      p += 10;
    }
    else
    {
      checkBox1->setCheckState(Qt::Checked);
    }

    p = ++i;

    for(; i<80; i++)
    {
      if(scratchpad[i] == ' ')  break;
    }
    if(strncmp(scratchpad + p, "X ", 2))
    {
      scratchpad[i] = 0;
      for(j=p; j<i; j++)
      {
        if(scratchpad[j] == '_')  scratchpad[j] = ' ';
      }
      strlcpy(str, scratchpad + p, 256);
      remove_trailing_spaces(str);
      lineEdit4->setText(str);
    }
    else
    {
      lineEdit4->clear();
    }

    p = ++i;

    scratchpad[80] = 0;
    strlcpy(str, scratchpad + p, 256);
    remove_trailing_spaces(str);
    lineEdit5->setText(str);

    strncpy(scratchpad, hdr + 88, 80);
    scratchpad[80] = 0;
    strlcat(scratchpad, "    ", 256);

    p = 10;

    for(i=10; i<80; i++)
    {
      if(scratchpad[i] == ' ')  break;
    }
    if(strncmp(scratchpad + p, "X ", 2))
    {
      scratchpad[i] = 0;

      has_startdate = 1;

      scratchpad[12] = 0;
      scratchpad[16] = 0;
      scratchpad[21] = 0;

      r = 0;
      if(!strncmp(scratchpad + 13, "JAN", 3))  r = 1;
        else if(!strncmp(scratchpad + 13, "FEB", 3))  r = 2;
          else if(!strncmp(scratchpad + 13, "MAR", 3))  r = 3;
            else if(!strncmp(scratchpad + 13, "APR", 3))  r = 4;
              else if(!strncmp(scratchpad + 13, "MAY", 3))  r = 5;
                else if(!strncmp(scratchpad + 13, "JUN", 3))  r = 6;
                  else if(!strncmp(scratchpad + 13, "JUL", 3))  r = 7;
                    else if(!strncmp(scratchpad + 13, "AUG", 3))  r = 8;
                      else if(!strncmp(scratchpad + 13, "SEP", 3))  r = 9;
                        else if(!strncmp(scratchpad + 13, "OCT", 3))  r = 10;
                          else if(!strncmp(scratchpad + 13, "NOV", 3))  r = 11;
                            else if(!strncmp(scratchpad + 13, "DEC", 3))  r = 12;

      if(r)
      {
        startTimeDate->setDate(QDate(atoi(scratchpad + 17), r, atoi(scratchpad + 10)));

        startTimeDate->setMinimumDate(QDate(1985, 1, 1));
        startTimeDate->setMaximumDate(QDate(2084, 12, 31));
      }
    }
    else
    {
      has_startdate = 0;
    }

    p = ++i;

    for(; i<80; i++)
    {
      if(scratchpad[i] == ' ')  break;
    }
    if(strncmp(scratchpad + p, "X ", 2))
    {
      scratchpad[i] = 0;
      for(j=p; j<i; j++)
      {
        if(scratchpad[j] == '_')  scratchpad[j] = ' ';
      }
      strlcpy(str, scratchpad + p, 256);
      remove_trailing_spaces(str);
      lineEdit6->setText(str);
    }
    else
    {
      lineEdit6->clear();
    }

    p = ++i;

    for(; i<80; i++)
    {
      if(scratchpad[i] == ' ')  break;
    }
    if(strncmp(scratchpad + p, "X ", 2))
    {
      scratchpad[i] = 0;
      for(j=p; j<i; j++)
      {
        if(scratchpad[j] == '_')  scratchpad[j] = ' ';
      }
      strlcpy(str, scratchpad + p, 256);
      remove_trailing_spaces(str);
      lineEdit7->setText(str);
    }
    else
    {
      lineEdit7->clear();
    }

    p = ++i;

    for(; i<80; i++)
    {
      if(scratchpad[i] == ' ')  break;
    }
    if(strncmp(scratchpad + p, "X ", 2))
    {
      scratchpad[i] = 0;
      for(j=p; j<i; j++)
      {
        if(scratchpad[j] == '_')  scratchpad[j] = ' ';
      }
      strlcpy(str, scratchpad + p, 256);
      remove_trailing_spaces(str);
      lineEdit8->setText(str);
    }
    else
    {
      lineEdit8->clear();
    }

    p = ++i;

    scratchpad[80] = 0;
    strlcpy(str, scratchpad + p, 256);
    remove_trailing_spaces(str);
    lineEdit9->setText(str);

    calculate_chars_left_name("");
    calculate_chars_left_recording("");

    connect(lineEdit3,   SIGNAL(textEdited(const QString &)), this, SLOT(calculate_chars_left_name(const QString &)));
    connect(lineEdit4,   SIGNAL(textEdited(const QString &)), this, SLOT(calculate_chars_left_name(const QString &)));
    connect(lineEdit5,   SIGNAL(textEdited(const QString &)), this, SLOT(calculate_chars_left_name(const QString &)));
    connect(checkBox1,   SIGNAL(stateChanged(int)),           this, SLOT(calculate_chars_left_name(int)));
    connect(lineEdit6,   SIGNAL(textEdited(const QString &)), this, SLOT(calculate_chars_left_recording(const QString &)));
    connect(lineEdit7,   SIGNAL(textEdited(const QString &)), this, SLOT(calculate_chars_left_recording(const QString &)));
    connect(lineEdit8,   SIGNAL(textEdited(const QString &)), this, SLOT(calculate_chars_left_recording(const QString &)));
    connect(lineEdit9,   SIGNAL(textEdited(const QString &)), this, SLOT(calculate_chars_left_recording(const QString &)));
  }
  else  // if(edfplus || bdfplus)
  {
    label3->setVisible(false);
    label4->setVisible(false);
    label5->setVisible(false);
    label6->setVisible(false);
    label7->setVisible(false);
    label8->setVisible(false);
    label9->setVisible(false);
    label11->setVisible(false);
    label12->setVisible(false);
    charsleft1Label->setVisible(false);
    charsleft2Label->setVisible(false);

    lineEdit3->setVisible(false);
    lineEdit4->setVisible(false);
    lineEdit5->setVisible(false);
    lineEdit6->setVisible(false);
    lineEdit7->setVisible(false);
    lineEdit8->setVisible(false);
    lineEdit9->setVisible(false);

    comboBox1->setVisible(false);

    checkBox1->setVisible(false);

    dateEdit1->setVisible(false);

    label1->setVisible(true);
    label2->setVisible(true);

    lineEdit1->setVisible(true);
    lineEdit2->setVisible(true);

    memcpy(scratchpad, hdr + 8, 80);
    scratchpad[80] = 0;
    remove_trailing_spaces(scratchpad);
    lineEdit1->setText(scratchpad);

    memcpy(scratchpad, hdr + 88, 80);
    scratchpad[80] = 0;
    remove_trailing_spaces(scratchpad);
    lineEdit2->setText(scratchpad);
  }

  datrecs = antoi(hdr + 236, 8);

  signallist->setRowCount(edfsignals);

  recordsize = 0;

  label13->setVisible(true);
  memcpy(scratchpad, hdr + 236, 8);  /* number of datarecords */
  scratchpad[8] = 0;
  remove_trailing_spaces(scratchpad);
  lineEdit10->setVisible(true);
  lineEdit10->setText(scratchpad);

  label14->setVisible(true);
  memcpy(scratchpad, hdr + 244, 8);  /* duration of a datarecord */
  scratchpad[8] = 0;
  remove_trailing_spaces(scratchpad);
  lineEdit11->setVisible(true);
  lineEdit11->setText(scratchpad);

  label15->setVisible(true);
  memcpy(scratchpad, hdr + 252, 4);  /* number of signals */
  scratchpad[4] = 0;
  remove_trailing_spaces(scratchpad);
  lineEdit12->setVisible(true);
  lineEdit12->setText(scratchpad);

  label16->setVisible(true);
  memcpy(scratchpad, hdr + 192, 44);  /* reserved */
  scratchpad[44] = 0;
  remove_trailing_spaces(scratchpad);
  lineEdit13->setVisible(true);
  lineEdit13->setText(scratchpad);

  label17->setVisible(true);
  memcpy(scratchpad, hdr, 8);  /* version */
  scratchpad[8] = 0;
  remove_trailing_spaces(scratchpad);
  sanitize_ascii(scratchpad);
  lineEdit14->setVisible(true);
  lineEdit14->setText(scratchpad);

  for(i=0; i<edfsignals; i++)
  {
    memcpy(scratchpad, hdr + 256 + (i * 16), 16);
    scratchpad[16] = 0;
    remove_trailing_spaces(scratchpad);
    signallist->setCellWidget(i, 0, new QLineEdit(scratchpad));
    ((QLineEdit *)(signallist->cellWidget(i, 0)))->setMaxLength(16);
    ((QLineEdit *)(signallist->cellWidget(i, 0)))->setText(scratchpad);

    memcpy(scratchpad, hdr + 256 + (edfsignals * 96) + (i * 8), 8);
    scratchpad[8] = 0;
    remove_trailing_spaces(scratchpad);
    signallist->setCellWidget(i, 1, new QLineEdit(scratchpad));
    ((QLineEdit *)(signallist->cellWidget(i, 1)))->setMaxLength(8);
    ((QLineEdit *)(signallist->cellWidget(i, 1)))->setText(scratchpad);

    memcpy(scratchpad, hdr + 256 + (edfsignals * 136) + (i * 80), 80);
    scratchpad[80] = 0;
    remove_trailing_spaces(scratchpad);
    signallist->setCellWidget(i, 2, new QLineEdit(scratchpad));
    ((QLineEdit *)(signallist->cellWidget(i, 2)))->setMaxLength(80);
    ((QLineEdit *)(signallist->cellWidget(i, 2)))->setText(scratchpad);

    memcpy(scratchpad, hdr + 256 + (edfsignals * 16) + (i * 80), 80);
    scratchpad[80] = 0;
    remove_trailing_spaces(scratchpad);
    signallist->setCellWidget(i, 3, new QLineEdit(scratchpad));
    ((QLineEdit *)(signallist->cellWidget(i, 3)))->setMaxLength(80);
    ((QLineEdit *)(signallist->cellWidget(i, 3)))->setText(scratchpad);

    memcpy(scratchpad, hdr + 256 + (edfsignals * 112) + (i * 8), 8);
    scratchpad[8] = 0;
    remove_trailing_spaces(scratchpad);
    signallist->setCellWidget(i, 4, new QLineEdit(scratchpad));
    ((QLineEdit *)(signallist->cellWidget(i, 4)))->setMaxLength(80);
    ((QLineEdit *)(signallist->cellWidget(i, 4)))->setText(scratchpad);
    ((QLineEdit *)(signallist->cellWidget(i, 4)))->setEnabled(false);

    memcpy(scratchpad, hdr + 256 + (edfsignals * 104) + (i * 8), 8);
    scratchpad[8] = 0;
    remove_trailing_spaces(scratchpad);
    signallist->setCellWidget(i, 5, new QLineEdit(scratchpad));
    ((QLineEdit *)(signallist->cellWidget(i, 5)))->setMaxLength(80);
    ((QLineEdit *)(signallist->cellWidget(i, 5)))->setText(scratchpad);
    ((QLineEdit *)(signallist->cellWidget(i, 5)))->setEnabled(false);

    memcpy(scratchpad, hdr + 256 + (edfsignals * 128) + (i * 8), 8);
    scratchpad[8] = 0;
    remove_trailing_spaces(scratchpad);
    signallist->setCellWidget(i, 6, new QLineEdit(scratchpad));
    ((QLineEdit *)(signallist->cellWidget(i, 6)))->setMaxLength(80);
    ((QLineEdit *)(signallist->cellWidget(i, 6)))->setText(scratchpad);
    ((QLineEdit *)(signallist->cellWidget(i, 6)))->setEnabled(false);

    memcpy(scratchpad, hdr + 256 + (edfsignals * 120) + (i * 8), 8);
    scratchpad[8] = 0;
    remove_trailing_spaces(scratchpad);
    signallist->setCellWidget(i, 7, new QLineEdit(scratchpad));
    ((QLineEdit *)(signallist->cellWidget(i, 7)))->setMaxLength(80);
    ((QLineEdit *)(signallist->cellWidget(i, 7)))->setText(scratchpad);
    ((QLineEdit *)(signallist->cellWidget(i, 7)))->setEnabled(false);

    memcpy(scratchpad, hdr + 256 + (edfsignals * 216) + (i * 8), 8);
    scratchpad[8] = 0;
    remove_trailing_spaces(scratchpad);
    signallist->setCellWidget(i, 8, new QLineEdit(scratchpad));
    ((QLineEdit *)(signallist->cellWidget(i, 8)))->setMaxLength(80);
    ((QLineEdit *)(signallist->cellWidget(i, 8)))->setText(scratchpad);
    ((QLineEdit *)(signallist->cellWidget(i, 8)))->setEnabled(false);

    memcpy(scratchpad, hdr + 256 + (edfsignals * 216) + (i * 8), 8);
    scratchpad[8] = 0;
    if(atoi(scratchpad) < 1)
    {
      snprintf(str, 256, "Samples per record of signal %i is less than 1.\n"
                   "Can not fix this file.", i + 1);
      QMessageBox messagewindow(QMessageBox::Critical, "Error", str);
      messagewindow.exec();
      fclose(file);
      file = NULL;
      return;
    }
    if(bdf)
    {
      recordsize += (atoi(scratchpad) * 3);
    }
    else
    {
      recordsize += (atoi(scratchpad) * 2);
    }

    if(edfplus)
    {
      if(!(strncmp(((QLineEdit *)(signallist->cellWidget(i, 0)))->text().toLatin1().data(), "EDF Annotations", 15)))
      {
        ((QLineEdit *)(signallist->cellWidget(i, 0)))->setEnabled(false);
        ((QLineEdit *)(signallist->cellWidget(i, 1)))->setEnabled(false);
        ((QLineEdit *)(signallist->cellWidget(i, 2)))->setEnabled(false);
        ((QLineEdit *)(signallist->cellWidget(i, 3)))->setEnabled(false);
      }
    }

    if(bdfplus)
    {
      if(!(strncmp(((QLineEdit *)(signallist->cellWidget(i, 0)))->text().toLatin1().data(), "BDF Annotations", 15)))
      {
        ((QLineEdit *)(signallist->cellWidget(i, 0)))->setEnabled(false);
        ((QLineEdit *)(signallist->cellWidget(i, 1)))->setEnabled(false);
        ((QLineEdit *)(signallist->cellWidget(i, 2)))->setEnabled(false);
        ((QLineEdit *)(signallist->cellWidget(i, 3)))->setEnabled(false);
      }
    }
  }

  signallist->resizeColumnsToContents();

  if(recordsize < 1)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Recordsize is less than 1.\n"
                                                              "Can not fix this file.");
    messagewindow.exec();
    fclose(file);
    file = NULL;
    return;
  }
}


void UI_headerEditorWindow::save_hdr()
{
  int i, j, p, len, hassign, digmin=0, digmax=0, dig_ok, dots, commas;

  char scratchpad[256],
       scratchpad2[256],
       str[256];

  long long filesize,
            l_tmp=0LL;


  if(file == NULL)
  {
    return;
  }

  snprintf(scratchpad, 256, "%02i.%02i.%02i%02i.%02i.%02i",
          startTimeDate->date().day(),
          startTimeDate->date().month(),
          startTimeDate->date().year() % 100,
          startTimeDate->time().hour(),
          startTimeDate->time().minute(),
          startTimeDate->time().second());
  fseeko(file, 168LL, SEEK_SET);
  fprintf(file, "%s", scratchpad);

  if(edfplus || bdfplus)
  {
    if((calculate_chars_left_name("") < 0) || (calculate_chars_left_recording("") < 0))
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "Too many characters.");
      messagewindow.exec();
      return;
    }

    str[0] = 0;

    len = strlen(lineEdit3->text().toLatin1().data());  // patient code
    if(len)
    {
      strlcpy(scratchpad, lineEdit3->text().toLatin1().data(), 256);
      latin1_to_ascii(scratchpad, len);
      remove_trailing_spaces(scratchpad);
      for(i=0; i<len; i++)
      {
        if(scratchpad[i] == ' ')  scratchpad[i] = '_';
      }
      if(!strlen(scratchpad))
      {
        strlcpy(scratchpad, "X", 256);
      }
      strlcat(scratchpad, " ", 256);
      strlcat(str, scratchpad, 256);
    }
    else
    {
      strlcat(str, "X ", 256);
    }

    if(comboBox1->currentIndex() == 0)  // gender
    {
      strlcat(str, "X ", 256);
    }
    if(comboBox1->currentIndex() == 1)
    {
      strlcat(str, "M ", 256);
    }
    if(comboBox1->currentIndex() == 2)
    {
      strlcat(str, "F ", 256);
    }

    if(checkBox1->checkState() == Qt::Checked)  // birthdate
    {
      strlcat(str, "X ", 256);
    }
    else
    {
      i = dateEdit1->date().month();

      switch(i)
      {
        case  1 : snprintf(scratchpad, 256, "%02i-JAN-%04i ", dateEdit1->date().day(), dateEdit1->date().year());
                  break;
        case  2 : snprintf(scratchpad, 256, "%02i-FEB-%04i ", dateEdit1->date().day(), dateEdit1->date().year());
                  break;
        case  3 : snprintf(scratchpad, 256, "%02i-MAR-%04i ", dateEdit1->date().day(), dateEdit1->date().year());
                  break;
        case  4 : snprintf(scratchpad, 256, "%02i-APR-%04i ", dateEdit1->date().day(), dateEdit1->date().year());
                  break;
        case  5 : snprintf(scratchpad, 256, "%02i-MAY-%04i ", dateEdit1->date().day(), dateEdit1->date().year());
                  break;
        case  6 : snprintf(scratchpad, 256, "%02i-JUN-%04i ", dateEdit1->date().day(), dateEdit1->date().year());
                  break;
        case  7 : snprintf(scratchpad, 256, "%02i-JUL-%04i ", dateEdit1->date().day(), dateEdit1->date().year());
                  break;
        case  8 : snprintf(scratchpad, 256, "%02i-AUG-%04i ", dateEdit1->date().day(), dateEdit1->date().year());
                  break;
        case  9 : snprintf(scratchpad, 256, "%02i-SEP-%04i ", dateEdit1->date().day(), dateEdit1->date().year());
                  break;
        case 10 : snprintf(scratchpad, 256, "%02i-OCT-%04i ", dateEdit1->date().day(), dateEdit1->date().year());
                  break;
        case 11 : snprintf(scratchpad, 256, "%02i-NOV-%04i ", dateEdit1->date().day(), dateEdit1->date().year());
                  break;
        case 12 : snprintf(scratchpad, 256, "%02i-DEC-%04i ", dateEdit1->date().day(), dateEdit1->date().year());
                  break;
      }

      strlcat(str, scratchpad, 256);
    }

    len = strlen(lineEdit4->text().toLatin1().data());  // patient name
    if(len)
    {
      strlcpy(scratchpad, lineEdit4->text().toLatin1().data(), 256);
      latin1_to_ascii(scratchpad, len);
      remove_trailing_spaces(scratchpad);
      for(i=0; i<len; i++)
      {
        if(scratchpad[i] == ' ')  scratchpad[i] = '_';
      }
      if(!strlen(scratchpad))
      {
        strlcpy(scratchpad, "X", 256);
      }
      strlcat(scratchpad, " ", 256);
      strlcat(str, scratchpad, 256);
    }
    else
    {
      strlcat(str, "X ", 256);
    }

    len = strlen(lineEdit5->text().toLatin1().data());  // additional info
    if(len)
    {
      strlcpy(scratchpad, lineEdit5->text().toLatin1().data(), 256);
      latin1_to_ascii(scratchpad, len);
      remove_trailing_spaces(scratchpad);
      strlcat(str, scratchpad, 256);
    }

    for(i=0; i<8; i++)
    {
      strlcat(str, "          ", 256);
    }

    str[80] = 0;

    fseeko(file, 8LL, SEEK_SET);
    fprintf(file, "%s", str);

    if(has_startdate)
    {
      strlcpy(str, "Startdate ", 256);

      i = startTimeDate->date().month();

      switch(i)
      {
        case  1 : snprintf(scratchpad, 256, "%02i-JAN-%04i ", startTimeDate->date().day(), startTimeDate->date().year());
                  break;
        case  2 : snprintf(scratchpad, 256, "%02i-FEB-%04i ", startTimeDate->date().day(), startTimeDate->date().year());
                  break;
        case  3 : snprintf(scratchpad, 256, "%02i-MAR-%04i ", startTimeDate->date().day(), startTimeDate->date().year());
                  break;
        case  4 : snprintf(scratchpad, 256, "%02i-APR-%04i ", startTimeDate->date().day(), startTimeDate->date().year());
                  break;
        case  5 : snprintf(scratchpad, 256, "%02i-MAY-%04i ", startTimeDate->date().day(), startTimeDate->date().year());
                  break;
        case  6 : snprintf(scratchpad, 256, "%02i-JUN-%04i ", startTimeDate->date().day(), startTimeDate->date().year());
                  break;
        case  7 : snprintf(scratchpad, 256, "%02i-JUL-%04i ", startTimeDate->date().day(), startTimeDate->date().year());
                  break;
        case  8 : snprintf(scratchpad, 256, "%02i-AUG-%04i ", startTimeDate->date().day(), startTimeDate->date().year());
                  break;
        case  9 : snprintf(scratchpad, 256, "%02i-SEP-%04i ", startTimeDate->date().day(), startTimeDate->date().year());
                  break;
        case 10 : snprintf(scratchpad, 256, "%02i-OCT-%04i ", startTimeDate->date().day(), startTimeDate->date().year());
                  break;
        case 11 : snprintf(scratchpad, 256, "%02i-NOV-%04i ", startTimeDate->date().day(), startTimeDate->date().year());
                  break;
        case 12 : snprintf(scratchpad, 256, "%02i-DEC-%04i ", startTimeDate->date().day(), startTimeDate->date().year());
                  break;
      }

      strlcat(str, scratchpad, 256);
    }
    else
    {
      strlcpy(str, "Startdate X ", 256);
    }

    len = strlen(lineEdit6->text().toLatin1().data());  // administration code
    if(len)
    {
      strlcpy(scratchpad, lineEdit6->text().toLatin1().data(), 256);
      latin1_to_ascii(scratchpad, len);
      remove_trailing_spaces(scratchpad);
      for(i=0; i<len; i++)
      {
        if(scratchpad[i] == ' ')  scratchpad[i] = '_';
      }
      if(!strlen(scratchpad))
      {
        strlcpy(scratchpad, "X", 256);
      }
      strlcat(scratchpad, " ", 256);
      strlcat(str, scratchpad, 256);
    }
    else
    {
      strlcat(str, "X ", 256);
    }

    len = strlen(lineEdit7->text().toLatin1().data());  // technician
    if(len)
    {
      strlcpy(scratchpad, lineEdit7->text().toLatin1().data(), 256);
      latin1_to_ascii(scratchpad, len);
      remove_trailing_spaces(scratchpad);
      for(i=0; i<len; i++)
      {
        if(scratchpad[i] == ' ')  scratchpad[i] = '_';
      }
      if(!strlen(scratchpad))
      {
        strlcpy(scratchpad, "X", 256);
      }
      strlcat(scratchpad, " ", 256);
      strlcat(str, scratchpad, 256);
    }
    else
    {
      strlcat(str, "X ", 256);
    }

    len = strlen(lineEdit8->text().toLatin1().data());  // device
    if(len)
    {
      strlcpy(scratchpad, lineEdit8->text().toLatin1().data(), 256);
      latin1_to_ascii(scratchpad, len);
      remove_trailing_spaces(scratchpad);
      for(i=0; i<len; i++)
      {
        if(scratchpad[i] == ' ')  scratchpad[i] = '_';
      }
      if(!strlen(scratchpad))
      {
        strlcpy(scratchpad, "X", 256);
      }
      strlcat(scratchpad, " ", 256);
      strlcat(str, scratchpad, 256);
    }
    else
    {
      strlcat(str, "X ", 256);
    }

    len = strlen(lineEdit9->text().toLatin1().data());  // additional info
    if(len)
    {
      strlcpy(scratchpad, lineEdit9->text().toLatin1().data(), 256);
      latin1_to_ascii(scratchpad, len);
      remove_trailing_spaces(scratchpad);
      strlcat(str, scratchpad, 256);
    }

    for(i=0; i<8; i++)
    {
      strlcat(str, "          ", 256);
    }

    str[80] = 0;

    fseeko(file, 88LL, SEEK_SET);
    fprintf(file, "%s", str);
  }
  else
  {
    strlcpy(scratchpad, lineEdit1->text().toLatin1().data(), 256);
    for(j=strlen(scratchpad); j<80; j++)
    {
      scratchpad[j] = ' ';
    }
    latin1_to_ascii(scratchpad, 80);
    scratchpad[80] = 0;
    fseeko(file, 8LL, SEEK_SET);
    fprintf(file, "%s", scratchpad);

    strlcpy(scratchpad, lineEdit2->text().toLatin1().data(), 256);
    for(j=strlen(scratchpad); j<80; j++)
    {
      scratchpad[j] = ' ';
    }
    latin1_to_ascii(scratchpad, 80);
    scratchpad[80] = 0;
    fseeko(file, 88LL, SEEK_SET);
    fprintf(file, "%s", scratchpad);
  }

  fseeko(file, 170LL, SEEK_SET);
  fputc('.', file);
  fseeko(file, 173LL, SEEK_SET);
  fputc('.', file);
  fseeko(file, 178LL, SEEK_SET);
  fputc('.', file);
  fseeko(file, 181LL, SEEK_SET);
  fputc('.', file);

  if(edfplus || bdfplus)           // reserved
  {
    fseeko(file, 197LL, SEEK_SET);
    for(j=0; j<39; j++)
    {
      fputc(' ', file);
    }
  }
  else
  {
    fseeko(file, 192LL, SEEK_SET);
    for(j=0; j<44; j++)
    {
      fputc(' ', file);
    }
  }

  fseeko(file, 244LL, SEEK_SET);
  fread(scratchpad, 8, 1, file);
  scratchpad[8] = 0;
  if(!strcmp(scratchpad, "0       "))
  {
    fseeko(file, 244LL, SEEK_SET);
    fputc('1', file);              /* fix for broken EDF+ files from Physionet */
  }

  for(i=0; i<edfsignals; i++)
  {
    if(((QLineEdit *)(signallist->cellWidget(i, 0)))->isEnabled() == false)  // EDF/BDF Annotations signal
    {
      fseeko(file, (long long)(256 + (edfsignals * 96) + (i * 8)), SEEK_SET);  // physical dimension
      fprintf(file, "        ");

      for(j=0; j<80; j++)
      {
        scratchpad[j] = ' ';
      }
      scratchpad[80] = 0;
      fseeko(file, (long long)(256 + (edfsignals * 136) + (i * 80)), SEEK_SET);  // prefiltering
      fprintf(file, "%s", scratchpad);

      for(j=0; j<80; j++)
      {
        scratchpad[j] = ' ';
      }
      scratchpad[80] = 0;
      fseeko(file, (long long)(256 + (edfsignals * 16) + (i * 80)), SEEK_SET);  // transducer type
      fprintf(file, "%s", scratchpad);
      fseeko(file, (long long)(256 + (edfsignals * 120) + (i * 8)), SEEK_SET);  // digital minimum
      if(edfplus)
      {
        fprintf(file, "-32768  ");
      }
      else
      {
        if(bdfplus)
        {
          fprintf(file, "-8388608");
        }
      }
      fseeko(file, (long long)(256 + (edfsignals * 128) + (i * 8)), SEEK_SET);  // digital maximum
      if(edfplus)
      {
        fprintf(file, "32767   ");
      }
      else
      {
        if(bdfplus)
        {
          fprintf(file, "8388607 ");
        }
      }
      fseeko(file, (long long)(256 + (edfsignals * 104) + (i * 8)), SEEK_SET);  // physical minimum
      fprintf(file, "-1      ");
      fseeko(file, (long long)(256 + (edfsignals * 112) + (i * 8)), SEEK_SET);  // physical maximum
      fprintf(file, "1       ");
    }
    else
    {
      strlcpy(scratchpad, ((QLineEdit *)(signallist->cellWidget(i, 0)))->text().toLatin1().data(), 256);
      for(j=strlen(scratchpad); j<16; j++)
      {
        scratchpad[j] = ' ';
      }
      latin1_to_ascii(scratchpad, 16);
      scratchpad[16] = 0;
      fseeko(file, (long long)(256 + (i * 16)), SEEK_SET);
      fprintf(file, "%s", scratchpad);

      strlcpy(scratchpad, ((QLineEdit *)(signallist->cellWidget(i, 1)))->text().toLatin1().data(), 256);
      for(j=strlen(scratchpad); j<8; j++)
      {
        scratchpad[j] = ' ';
      }
      latin1_to_ascii(scratchpad, 8);
      scratchpad[8] = 0;
      fseeko(file, (long long)(256 + (edfsignals * 96) + (i * 8)), SEEK_SET);
      fprintf(file, "%s", scratchpad);

      strlcpy(scratchpad, ((QLineEdit *)(signallist->cellWidget(i, 2)))->text().toLatin1().data(), 256);
      for(j=strlen(scratchpad); j<80; j++)
      {
        scratchpad[j] = ' ';
      }
      latin1_to_ascii(scratchpad, 80);
      scratchpad[80] = 0;
      fseeko(file, (long long)(256 + (edfsignals * 136) + (i * 80)), SEEK_SET);
      fprintf(file, "%s", scratchpad);

      strlcpy(scratchpad, ((QLineEdit *)(signallist->cellWidget(i, 3)))->text().toLatin1().data(), 256);
      for(j=strlen(scratchpad); j<80; j++)
      {
        scratchpad[j] = ' ';
      }
      latin1_to_ascii(scratchpad, 80);
      scratchpad[80] = 0;
      fseeko(file, (long long)(256 + (edfsignals * 16) + (i * 80)), SEEK_SET);
      fprintf(file, "%s", scratchpad);

      strlcpy(scratchpad, hdr + 256 + (edfsignals * 120) + (i * 8), 9);  // digital minimum
      hassign = 0;
      p = 0;
      dig_ok = 0;
      if((scratchpad[0] == '+') || (scratchpad[0] == '-'))
      {
        hassign = 1;
        p++;
      }
      for(; p<8; p++)
      {
        if((scratchpad[p] < '0') || (scratchpad[p] > '9'))
        {
          break;
        }
      }
      if(p != hassign)
      {
        for(; p<8; p++)
        {
          scratchpad[p] = ' ';
        }
        scratchpad[8] = 0;
        fseeko(file, (long long)(256 + (edfsignals * 120) + (i * 8)), SEEK_SET);
        fprintf(file, "%s", scratchpad);
        digmin = atoi(scratchpad);
        dig_ok++;
      }

      strlcpy(scratchpad2, hdr + 256 + (edfsignals * 128) + (i * 8), 9);  // digital maximum
      hassign = 0;
      p = 0;
      if((scratchpad2[0] == '+') || (scratchpad2[0] == '-'))
      {
        hassign = 1;
        p++;
      }
      for(; p<8; p++)
      {
        if((scratchpad2[p] < '0') || (scratchpad2[p] > '9'))
        {
          break;
        }
      }
      if(p != hassign)
      {
        for(; p<8; p++)
        {
          scratchpad2[p] = ' ';
        }
        scratchpad2[8] = 0;
        fseeko(file, (long long)(256 + (edfsignals * 128) + (i * 8)), SEEK_SET);
        fprintf(file, "%s", scratchpad2);
        digmax = atoi(scratchpad2);
        dig_ok++;
      }
      if(dig_ok == 2)
      {
        if(digmax < digmin)
        {
          dig_ok++;

          fseeko(file, (long long)(256 + (edfsignals * 120) + (i * 8)), SEEK_SET);
          fprintf(file, "%s", scratchpad2);

          fseeko(file, (long long)(256 + (edfsignals * 128) + (i * 8)), SEEK_SET);
          fprintf(file, "%s", scratchpad);
        }
        if(digmax == digmin)
        {
          if(edf)
          {
            if(digmax < 32767)
            {
              digmax++;
              snprintf(scratchpad, 256, "%i", digmax);
              strlcat(scratchpad, "        ", 256);
              scratchpad[8] = 0;
              fseeko(file, (long long)(256 + (edfsignals * 128) + (i * 8)), SEEK_SET);
              fprintf(file, "%s", scratchpad);
            }
            else
            {
              digmin--;
              snprintf(scratchpad, 256, "%i", digmin);
              strlcat(scratchpad, "        ", 256);
              scratchpad[8] = 0;
              fseeko(file, (long long)(256 + (edfsignals * 120) + (i * 8)), SEEK_SET);
              fprintf(file, "%s", scratchpad);
            }
          }
          if(bdf)
          {
            if(digmax < 8388607)
            {
              digmax++;
              snprintf(scratchpad, 256, "%i", digmax);
              strlcat(scratchpad, "        ", 256);
              scratchpad[8] = 0;
              fseeko(file, (long long)(256 + (edfsignals * 128) + (i * 8)), SEEK_SET);
              fprintf(file, "%s", scratchpad);
            }
            else
            {
              digmin--;
              snprintf(scratchpad, 256, "%i", digmin);
              strlcat(scratchpad, "        ", 256);
              scratchpad[8] = 0;
              fseeko(file, (long long)(256 + (edfsignals * 120) + (i * 8)), SEEK_SET);
              fprintf(file, "%s", scratchpad);
            }
          }
        }
      }

      strlcpy(scratchpad, hdr + 256 + (edfsignals * 104) + (i * 8), 9);  // physical minimum
      for(p=7; p>0; p--)
      {
        if((scratchpad[p] < '0') || (scratchpad[p] > '9'))
        {
          scratchpad[p] = ' ';
        }
        else
        {
          break;
        }
      }
      scratchpad[8] = 0;

      for(p=0, dots=0, commas=0; p<8; p++)
      {
        if(scratchpad[p] == ',')  commas++;
        if(scratchpad[p] == '.')  dots++;
      }

      if((commas == 1) && (dots == 0))
      {
        for(p=0; p<8; p++)
        {
          if(scratchpad[p] == ',')
          {
            scratchpad[p] = '.';
            break;
          }
        }
      }

      fseeko(file, (long long)(256 + (edfsignals * 104) + (i * 8)), SEEK_SET);
      fprintf(file, "%s", scratchpad);

      strncpy(scratchpad2, hdr + 256 + (edfsignals * 112) + (i * 8), 8);  // physical maximum
      for(p=7; p>0; p--)
      {
        if((scratchpad2[p] < '0') || (scratchpad2[p] > '9'))
        {
          scratchpad2[p] = ' ';
        }
        else
        {
          break;
        }
      }
      scratchpad2[8] = 0;

      for(p=0, dots=0, commas=0; p<8; p++)
      {
        if(scratchpad2[p] == ',')  commas++;
        if(scratchpad2[p] == '.')  dots++;
      }

      if((commas == 1) && (dots == 0))
      {
        for(p=0; p<8; p++)
        {
          if(scratchpad2[p] == ',')
          {
            scratchpad2[p] = '.';
            break;
          }
        }
      }

      fseeko(file, (long long)(256 + (edfsignals * 112) + (i * 8)), SEEK_SET);
      fprintf(file, "%s", scratchpad2);

      if(dig_ok == 3)
      {
        fseeko(file, (long long)(256 + (edfsignals * 104) + (i * 8)), SEEK_SET);
        fprintf(file, "%s", scratchpad2);

        fseeko(file, (long long)(256 + (edfsignals * 112) + (i * 8)), SEEK_SET);
        fprintf(file, "%s", scratchpad);
      }
    }

    fseeko(file, (long long)(256 + (edfsignals * 224) + (i * 32)), SEEK_SET);  // reserved
    for(j=0; j<32; j++)
    {
      fputc(' ', file);
    }
  }

  fseeko(file, 236LL, SEEK_SET);
  fprintf(file, "%-8i", datrecs);  // datarecords

  fseeko(file, 0LL, SEEK_END);
  filesize = ftello(file);
  if(filesize != (((long long)recordsize * (long long)datrecs)) + ((edfsignals * 256) + 256))
  {
    l_tmp = filesize - ((edfsignals * 256) + 256);
    datrecs = l_tmp / (long long)recordsize;

    if(datrecs > 0)
    {
      if(datrecs > 99999999)
      {
        datrecs = 99999999;
      }

      fseeko(file, 236LL, SEEK_SET);
      fprintf(file, "%-8i", datrecs);

      l_tmp = ((edfsignals * 256) + 256);
      l_tmp += (long long)datrecs * (long long)recordsize;

      if(l_tmp != filesize)
      {
        fclose(file);

        if(QFile::resize(QString::fromLocal8Bit(path), l_tmp) == false)
        {
          QMessageBox messagewindow(QMessageBox::Critical, "Error", "Can not resize file.");
          messagewindow.exec();
        }

        file = fopeno(path, "r+b");
        if(file==NULL)
        {
          QMessageBox messagewindow(QMessageBox::Critical, "Error", "Can not open file.");
          messagewindow.exec();
          return;
        }
      }
    }
  }

  fflush(file);

  read_header();

  QMessageBox messagewindow(QMessageBox::Information, "Ready", "File has been saved.");
  messagewindow.setIconPixmap(QPixmap(":/images/ok.png"));
  messagewindow.exec();
}


void UI_headerEditorWindow::calculate_chars_left_name(int)
{
  calculate_chars_left_name("");
}


int UI_headerEditorWindow::calculate_chars_left_name(const QString &)
{
  int n = 62;

  int len;

  int cursorposition[3];

  char str[128];

  if(checkBox1->checkState() == Qt::Checked)  // birthdate
  {
    n += 10;
  }

  cursorposition[0] = lineEdit3->cursorPosition();
  cursorposition[1] = lineEdit4->cursorPosition();
  cursorposition[2] = lineEdit5->cursorPosition();

  len = strlen(lineEdit3->text().toLatin1().data());
  if(len>1)
  {
    n -= (len - 1);
  }

  len = strlen(lineEdit4->text().toLatin1().data());
  if(len>1)
  {
    n -= (len - 1);
  }

  len = strlen(lineEdit5->text().toLatin1().data());
  if(len>1)
  {
    n -= (len - 1);
  }

  if(n<1)
  {
    lineEdit3->setMaxLength(strlen(lineEdit3->text().toLatin1().data()));
    lineEdit4->setMaxLength(strlen(lineEdit4->text().toLatin1().data()));
    lineEdit5->setMaxLength(strlen(lineEdit5->text().toLatin1().data()));
  }
  else
  {
    lineEdit3->setMaxLength(80);
    lineEdit4->setMaxLength(80);
    lineEdit5->setMaxLength(80);
  }

  lineEdit3->setCursorPosition(cursorposition[0]);
  lineEdit4->setCursorPosition(cursorposition[1]);
  lineEdit5->setCursorPosition(cursorposition[2]);

  snprintf(str, 128, "Characters left:  %i", n);

  charsleft1Label->setText(str);
  if(n < 0)
  {
    charsleft1Label->setStyleSheet("color: red;");
  }
  else
  {
    charsleft1Label->setStyleSheet("color: black;");
  }

  return n;
}


int UI_headerEditorWindow::calculate_chars_left_recording(const QString &)
{
  int n = 52;

  int len;

  int cursorposition[4];

  char str[128];


  cursorposition[0] = lineEdit6->cursorPosition();
  cursorposition[1] = lineEdit7->cursorPosition();
  cursorposition[2] = lineEdit8->cursorPosition();
  cursorposition[3] = lineEdit9->cursorPosition();

  len = strlen(lineEdit6->text().toLatin1().data());
  if(len>1)
  {
    n -= (len - 1);
  }

  len = strlen(lineEdit7->text().toLatin1().data());
  if(len>1)
  {
    n -= (len - 1);
  }

  len = strlen(lineEdit8->text().toLatin1().data());
  if(len>1)
  {
    n -= (len - 1);
  }

  len = strlen(lineEdit9->text().toLatin1().data());
  if(len>0)
  {
    n -= len;
  }

  if(n<1)
  {
    lineEdit6->setMaxLength(strlen(lineEdit6->text().toLatin1().data()));
    lineEdit7->setMaxLength(strlen(lineEdit7->text().toLatin1().data()));
    lineEdit8->setMaxLength(strlen(lineEdit8->text().toLatin1().data()));
    lineEdit9->setMaxLength(strlen(lineEdit9->text().toLatin1().data()));
  }
  else
  {
    lineEdit6->setMaxLength(80);
    lineEdit7->setMaxLength(80);
    lineEdit8->setMaxLength(80);
    lineEdit9->setMaxLength(80);
  }

  lineEdit6->setCursorPosition(cursorposition[0]);
  lineEdit7->setCursorPosition(cursorposition[1]);
  lineEdit8->setCursorPosition(cursorposition[2]);
  lineEdit9->setCursorPosition(cursorposition[3]);

  snprintf(str, 128, "Characters left:  %i", n);

  charsleft2Label->setText(str);
  if(n < 0)
  {
    charsleft2Label->setStyleSheet("color: red;");
  }
  else
  {
    charsleft2Label->setStyleSheet("color: black;");
  }

  return n;
}


void UI_headerEditorWindow::helpbuttonpressed()
{
#ifdef Q_OS_LINUX
  QDesktopServices::openUrl(QUrl("file:///usr/share/doc/edfbrowser/manual.html#Header_editor"));
#endif

#ifdef Q_OS_WIN32
  char p_path[MAX_PATH_LENGTH];

  strlcpy(p_path, "file:///", MAX_PATH_LENGTH);
  strlcat(p_path, mainwindow->specialFolder(CSIDL_PROGRAM_FILES).toLocal8Bit().data(), MAX_PATH_LENGTH);
  strlcat(p_path, "\\EDFbrowser\\manual.html#Header_editor", MAX_PATH_LENGTH);
  QDesktopServices::openUrl(QUrl(p_path));
#endif
}





















