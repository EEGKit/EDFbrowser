/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2007 - 2020 Teunis van Beelen
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



#include "options_dialog.h"




UI_OptionsDialog::UI_OptionsDialog(QWidget *w_parent)
{
  int i, screen_height=600;

  QGridLayout *gr;

  mainwindow = (UI_Mainwindow *)w_parent;

  double fsz = mainwindow->fontsize_factor;

  optionsdialog = new QDialog(w_parent);

#if QT_VERSION > 0x050C00
  screen_height = QGuiApplication::primaryScreen()->geometry().height();
#else
  screen_height = QApplication::desktop()->screenGeometry().height():
#endif

  if((400 + (600 * fsz)) < (screen_height - 200))
  {
    optionsdialog->setMinimumSize(170 + (410 * fsz), 400 + (600 * fsz));

    optionsdialog->setMaximumWidth(170 + (410 * fsz));
  }
  else
  {
    optionsdialog->setMinimumSize(170 + (410 * fsz), screen_height - 200);

    optionsdialog->setMaximumWidth(170 + (410 * fsz));
  }

  scrollarea = new QScrollArea;
  scrollarea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

  frame = new QFrame;
  optionsdialog->setWindowTitle("Settings");
  optionsdialog->setModal(true);
  optionsdialog->setAttribute(Qt::WA_DeleteOnClose, true);

  tabholder = new QTabWidget;

  CloseButton = new QPushButton;
  CloseButton->setText("Close");

/////////////////////////////////////// tab 1  Colors ///////////////////////////////////////////////////////////////////////

  tab1 = new QWidget;

  gr = new QGridLayout(tab1);
  gr->setVerticalSpacing(15);
  gr->setHorizontalSpacing(15);

  label1 = new QLabel(tab1);
  label1->setText("Background color");
  gr->addWidget(label1, 0, 0);

  BgColorButton = new SpecialButton(tab1);
  BgColorButton->setColor(mainwindow->maincurve->backgroundcolor);
  gr->addWidget(BgColorButton, 0, 1);

  label2 = new QLabel(tab1);
  label2->setText("Small ruler color");
  gr->addWidget(label2, 1, 0);

  SrColorButton = new SpecialButton(tab1);
  SrColorButton->setColor(mainwindow->maincurve->small_ruler_color);
  gr->addWidget(SrColorButton, 1, 1);

  label3 = new QLabel(tab1);
  label3->setText("Big ruler color");
  gr->addWidget(label3, 2, 0);

  BrColorButton = new SpecialButton(tab1);
  BrColorButton->setColor(mainwindow->maincurve->big_ruler_color);
  gr->addWidget(BrColorButton, 2, 1);

  label4 = new QLabel(tab1);
  label4->setText("Mouse rectangle color");
  gr->addWidget(label4, 3, 0);

  MrColorButton = new SpecialButton(tab1);
  MrColorButton->setColor(mainwindow->maincurve->mouse_rect_color);
  gr->addWidget(MrColorButton, 3, 1);

  label5 = new QLabel(tab1);
  label5->setText("Text color");
  gr->addWidget(label5, 4, 0);

  TxtColorButton = new SpecialButton(tab1);
  TxtColorButton->setColor(mainwindow->maincurve->text_color);
  gr->addWidget(TxtColorButton, 4, 1);

  label6 = new QLabel(tab1);
  label6->setText("Signals color");
  gr->addWidget(label6, 5, 0);

  SigColorButton = new SpecialButton(tab1);
  SigColorButton->setColor((Qt::GlobalColor)mainwindow->maincurve->signal_color);
  gr->addWidget(SigColorButton, 5, 1);

  label16 = new QLabel(tab1);
  label16->setText("Vary signal colors");
  label16->setToolTip("When adding signals to the screen, vary the traces' color");
  gr->addWidget(label16, 6, 0);

  checkbox16 = new QCheckBox(tab1);
  checkbox16->setTristate(false);
  checkbox16->setToolTip("When adding signals to the screen, vary the traces' color");
  if(mainwindow->use_diverse_signal_colors)
  {
    checkbox16->setCheckState(Qt::Checked);
  }
  else
  {
    checkbox16->setCheckState(Qt::Unchecked);
  }
  gr->addWidget(checkbox16, 6, 1);

  label7 = new QLabel(tab1);
  label7->setText("Baseline color");
  gr->addWidget(label7, 7, 0);

  checkbox3 = new QCheckBox(tab1);
  checkbox3->setTristate(false);
  if(mainwindow->show_baselines)
  {
    checkbox3->setCheckState(Qt::Checked);
  }
  else
  {
    checkbox3->setCheckState(Qt::Unchecked);
  }
  gr->addWidget(checkbox3, 7, 1);

  BaseColorButton = new SpecialButton(tab1);
  BaseColorButton->setColor(mainwindow->maincurve->baseline_color);
  gr->addWidget(BaseColorButton, 7, 2);

  label8 = new QLabel(tab1);
  label8->setText("First Crosshair color");
  gr->addWidget(label8, 8, 0);

  Crh1ColorButton = new SpecialButton(tab1);
  Crh1ColorButton->setColor((Qt::GlobalColor)mainwindow->maincurve->crosshair_1.color);
  gr->addWidget(Crh1ColorButton, 8, 1);

  label9 = new QLabel(tab1);
  label9->setText("Second Crosshair color");
  gr->addWidget(label9, 9, 0);

  Crh2ColorButton = new SpecialButton(tab1);
  Crh2ColorButton->setColor((Qt::GlobalColor)mainwindow->maincurve->crosshair_2.color);
  gr->addWidget(Crh2ColorButton, 9, 1);

  label10 = new QLabel(tab1);
  label10->setText("Floating ruler color");
  gr->addWidget(label10, 10, 0);

  FrColorButton = new SpecialButton(tab1);
  FrColorButton->setColor((Qt::GlobalColor)mainwindow->maincurve->floating_ruler_color);
  gr->addWidget(FrColorButton, 10, 1);

  label12 = new QLabel(tab1);
  label12->setText("Annotation marker");
  gr->addWidget(label12, 11, 0);

  checkbox2 = new QCheckBox(tab1);
  checkbox2->setTristate(false);
  if(mainwindow->show_annot_markers)
  {
    checkbox2->setCheckState(Qt::Checked);
  }
  else
  {
    checkbox2->setCheckState(Qt::Unchecked);
  }
  gr->addWidget(checkbox2, 11, 1);

  AnnotMkrButton = new SpecialButton(tab1);
  AnnotMkrButton->setColor(mainwindow->maincurve->annot_marker_color);
  gr->addWidget(AnnotMkrButton, 11, 2);

  label12_2 = new QLabel(tab1);
  label12_2->setText("Show duration at marker");
  gr->addWidget(label12_2, 12, 0);

  checkbox2_1 = new QCheckBox(tab1);
  checkbox2_1->setTristate(false);
  if(mainwindow->annotations_show_duration)
  {
    checkbox2_1->setCheckState(Qt::Checked);
  }
  else
  {
    checkbox2_1->setCheckState(Qt::Unchecked);
  }
  gr->addWidget(checkbox2_1, 12, 1);

  label12_1 = new QLabel(tab1);
  label12_1->setText("Annotation duration background");
  label12_1->setToolTip("The second color is used to indicate if it's selected");
  gr->addWidget(label12_1, 13, 0);

  AnnotDurationButton = new SpecialButton(tab1);
  AnnotDurationButton->setColor(mainwindow->maincurve->annot_duration_color);
  AnnotDurationButton->setToolTip("The second color is used to indicate if it's selected");
  gr->addWidget(AnnotDurationButton, 13, 1);

  AnnotDurationSelectedButton = new SpecialButton(tab1);
  AnnotDurationSelectedButton->setColor(mainwindow->maincurve->annot_duration_color_selected);
  AnnotDurationSelectedButton->setToolTip("The second color is used to indicate if it's selected");
  gr->addWidget(AnnotDurationSelectedButton, 13, 2);

  label12_3 = new QLabel(tab1);
  label12_3->setText("Show only at screen bottom");
  label12_3->setToolTip("Show the colored background only at the bottom of the screen");
  gr->addWidget(label12_3, 14, 0);

  checkbox2_2 = new QCheckBox(tab1);
  checkbox2_2->setTristate(false);
  if(mainwindow->annotations_duration_background_type)
  {
    checkbox2_2->setCheckState(Qt::Checked);
  }
  else
  {
    checkbox2_2->setCheckState(Qt::Unchecked);
  }
  checkbox2_2->setToolTip("Show the colored background only at the bottom of the screen");
  gr->addWidget(checkbox2_2, 14, 1);

  label14 = new QLabel(tab1);
  label14->setText("Annotations: filter list only");
  gr->addWidget(label14, 15, 0);

  checkbox5 = new QCheckBox(tab1);
  checkbox5->setTristate(false);
  checkbox5->setToolTip("Annotation filter affects the annotationlist only, not the annotation markers in the signal window");
  if(mainwindow->annot_filter->hide_in_list_only)
  {
    checkbox5->setCheckState(Qt::Checked);
  }
  else
  {
    checkbox5->setCheckState(Qt::Unchecked);
  }
  gr->addWidget(checkbox5, 15, 1);

  label11 = new QLabel(tab1);
  label11->setText("Print in grayscale");
  gr->addWidget(label11, 16, 0);

  checkbox1 = new QCheckBox(tab1);
  checkbox1->setTristate(false);
  if(mainwindow->maincurve->blackwhite_printing)
  {
    checkbox1->setCheckState(Qt::Checked);
  }
  else
  {
    checkbox1->setCheckState(Qt::Unchecked);
  }
  gr->addWidget(checkbox1, 16, 1);

  label13 = new QLabel(tab1);
  label13->setText("Clip signals to pane");
  gr->addWidget(label13, 17, 0);

  checkbox4 = new QCheckBox(tab1);
  checkbox4->setTristate(false);
  if(mainwindow->clip_to_pane)
  {
    checkbox4->setCheckState(Qt::Checked);
  }
  else
  {
    checkbox4->setCheckState(Qt::Unchecked);
  }
  gr->addWidget(checkbox4, 17, 1);

  gr->addWidget(new QLabel(""), 18, 0);

  colorSchema_Dark_Button = new QPushButton(tab1);
  colorSchema_Dark_Button->setText("\"Dark\"");
  colorSchema_Dark_Button->setToolTip("Set color schema \"Dark\"");
  gr->addWidget(colorSchema_Dark_Button, 19, 1);

  colorSchema_NK_Button = new QPushButton(tab1);
  colorSchema_NK_Button->setText("\"NK\"");
  colorSchema_NK_Button->setToolTip("Set color schema \"NK\"");
  gr->addWidget(colorSchema_NK_Button, 20, 1);

  colorSchema_Blue_on_Gray_Button = new QPushButton(tab1);
  colorSchema_Blue_on_Gray_Button->setText("\"Blue on gray\"");
  colorSchema_Blue_on_Gray_Button->setToolTip("Set color schema \"Blue on gray\"");
  gr->addWidget(colorSchema_Blue_on_Gray_Button, 21, 1);

  gr->addWidget(new QLabel(""), 22, 1);

  saveColorSchemaButton = new QPushButton(tab1);
  saveColorSchemaButton->setText("Save");
  saveColorSchemaButton->setToolTip("Save this color schema to a file");
  gr->addWidget(saveColorSchemaButton, 23, 1);

  loadColorSchemaButton = new QPushButton(tab1);
  loadColorSchemaButton->setText("Load");
  loadColorSchemaButton->setToolTip("Load a color schema from a file");
  gr->addWidget(loadColorSchemaButton, 24, 1);

  gr->addWidget(new QLabel(""), 25, 0);

  QObject::connect(BgColorButton,           SIGNAL(clicked(SpecialButton *)), this, SLOT(BgColorButtonClicked(SpecialButton *)));
  QObject::connect(SrColorButton,           SIGNAL(clicked(SpecialButton *)), this, SLOT(SrColorButtonClicked(SpecialButton *)));
  QObject::connect(BrColorButton,           SIGNAL(clicked(SpecialButton *)), this, SLOT(BrColorButtonClicked(SpecialButton *)));
  QObject::connect(MrColorButton,           SIGNAL(clicked(SpecialButton *)), this, SLOT(MrColorButtonClicked(SpecialButton *)));
  QObject::connect(TxtColorButton,          SIGNAL(clicked(SpecialButton *)), this, SLOT(TxtColorButtonClicked(SpecialButton *)));
  QObject::connect(SigColorButton,          SIGNAL(clicked(SpecialButton *)), this, SLOT(SigColorButtonClicked(SpecialButton *)));
  QObject::connect(BaseColorButton,         SIGNAL(clicked(SpecialButton *)), this, SLOT(BaseColorButtonClicked(SpecialButton *)));
  QObject::connect(Crh1ColorButton,         SIGNAL(clicked(SpecialButton *)), this, SLOT(Crh1ColorButtonClicked(SpecialButton *)));
  QObject::connect(Crh2ColorButton,         SIGNAL(clicked(SpecialButton *)), this, SLOT(Crh2ColorButtonClicked(SpecialButton *)));
  QObject::connect(FrColorButton,           SIGNAL(clicked(SpecialButton *)), this, SLOT(FrColorButtonClicked(SpecialButton *)));
  QObject::connect(AnnotMkrButton,          SIGNAL(clicked(SpecialButton *)), this, SLOT(AnnotMkrButtonClicked(SpecialButton *)));
  QObject::connect(AnnotDurationButton,     SIGNAL(clicked(SpecialButton *)), this, SLOT(AnnotDurationButtonClicked(SpecialButton *)));
  QObject::connect(AnnotDurationSelectedButton,     SIGNAL(clicked(SpecialButton *)), this, SLOT(AnnotDurationSelectedButtonClicked(SpecialButton *)));
  QObject::connect(checkbox1,               SIGNAL(stateChanged(int)),        this, SLOT(checkbox1Clicked(int)));
  QObject::connect(checkbox2,               SIGNAL(stateChanged(int)),        this, SLOT(checkbox2Clicked(int)));
  QObject::connect(checkbox2_1,             SIGNAL(stateChanged(int)),        this, SLOT(checkbox2_1Clicked(int)));
  QObject::connect(checkbox2_2,             SIGNAL(stateChanged(int)),        this, SLOT(checkbox2_2Clicked(int)));
  QObject::connect(checkbox3,               SIGNAL(stateChanged(int)),        this, SLOT(checkbox3Clicked(int)));
  QObject::connect(checkbox4,               SIGNAL(stateChanged(int)),        this, SLOT(checkbox4Clicked(int)));
  QObject::connect(checkbox5,               SIGNAL(stateChanged(int)),        this, SLOT(checkbox5Clicked(int)));
  QObject::connect(checkbox16,              SIGNAL(stateChanged(int)),        this, SLOT(checkbox16Clicked(int)));
  QObject::connect(saveColorSchemaButton,   SIGNAL(clicked()),                this, SLOT(saveColorSchemaButtonClicked()));
  QObject::connect(loadColorSchemaButton,   SIGNAL(clicked()),                this, SLOT(loadColorSchemaButtonClicked()));
  QObject::connect(colorSchema_Blue_on_Gray_Button, SIGNAL(clicked()),        this, SLOT(loadColorSchema_blue_gray()));
  QObject::connect(colorSchema_NK_Button,   SIGNAL(clicked()),                this, SLOT(loadColorSchema_NK()));
  QObject::connect(colorSchema_Dark_Button, SIGNAL(clicked()),                this, SLOT(loadColorSchema_Dark()));

  tabholder->addTab(tab1, "Colors");

/////////////////////////////////////// tab 2   Calibration ///////////////////////////////////////////////////////////////////////

  tab2 = new QWidget;

  checkbox2_1 = new QCheckBox("Manually override automatic DPI settings", tab2);
  checkbox2_1->setGeometry(85, 520 + (40 * fsz), 200 * fsz, 20 * fsz);
  checkbox2_1->setTristate(false);
  if(mainwindow->auto_dpi)
  {
    checkbox2_1->setCheckState(Qt::Unchecked);
  }
  else
  {
    checkbox2_1->setCheckState(Qt::Checked);
  }

  slabel2_1 = new SpecialButton(tab2);
  slabel2_1->setGeometry(30, 20, 5 + (5 * fsz), 445);
  slabel2_1->setColor(Qt::black);

  slabel2_3 = new SpecialButton(tab2);
  slabel2_3->setGeometry(30, 460 + (40 * fsz), 355, 5 + (5 * fsz));
  slabel2_3->setColor(Qt::black);

  label2_2 = new QLabel(tab2);
  label2_2->setGeometry(85, 50, 200 * fsz, 50 * fsz);
  label2_2->setText("Measure the length of the black\nrectangles and enter the values.");

  spinbox2_1 = new QSpinBox(tab2);
  spinbox2_1->setGeometry(85, 240, 60 + (40 * fsz), 25 * fsz);
  spinbox2_1->setSuffix(" mm");
  spinbox2_1->setMinimum(10);
  spinbox2_1->setMaximum(500);
  spinbox2_1->setValue((int)(4450.0 / (1.0 / mainwindow->pixelsizefactor)));

  spinbox2_2 = new QSpinBox(tab2);
  spinbox2_2->setGeometry(170, 450, 60 + (40 * fsz), 25 * fsz);
  spinbox2_2->setSuffix(" mm");
  spinbox2_2->setMinimum(10);
  spinbox2_2->setMaximum(500);
  spinbox2_2->setValue((int)(3550.0 / (1.0 / mainwindow->x_pixelsizefactor)));

  ApplyButton = new QPushButton(tab2);
  ApplyButton->setGeometry(85, 500 + (100 * fsz), 60 + (40 * fsz), 25 * fsz);
  ApplyButton->setText("Apply");

  if(checkbox2_1->checkState() == Qt::Unchecked)
  {
    spinbox2_1->setEnabled(false);
    spinbox2_2->setEnabled(false);
    ApplyButton->setEnabled(false);
  }

  QObject::connect(ApplyButton, SIGNAL(clicked()),         this, SLOT(ApplyButtonClicked()));
  QObject::connect(checkbox2_1, SIGNAL(stateChanged(int)), this, SLOT(calibrate_checkbox_stateChanged(int)));

  tabholder->addTab(tab2, "Calibration");

/////////////////////////////////////// tab 3  Powerspectrum ///////////////////////////////////////////////////////////////////////

  tab3 = new QWidget;

  label1_3 = new QLabel(tab3);
  label1_3->setGeometry(20, 20, 230 * fsz, 10 + (12 * fsz));
  label1_3->setText("Frequency regions of the colorbars:");

  colorBarTable = new QTableWidget(tab3);
  colorBarTable->setGeometry(20, 40 + (12 * fsz), 100 + (270 * fsz), 400);
  colorBarTable->setSelectionMode(QAbstractItemView::NoSelection);
  colorBarTable->setColumnCount(4);
  colorBarTable->setColumnWidth(0, 20);
  colorBarTable->setColumnWidth(1, 10 + (100 * fsz));
  colorBarTable->setColumnWidth(2, 80);
  colorBarTable->setColumnWidth(3, 100 * fsz);
  colorBarTable->setRowCount(MAXSPECTRUMMARKERS);
  for(i=0; i < MAXSPECTRUMMARKERS; i++)
  {
    colorBarTable->setRowHeight(i, 10 + (12 * fsz));

    colorBarTable->setCellWidget(i, 0, new QCheckBox);
   ((QCheckBox *)(colorBarTable->cellWidget(i, 0)))->setTristate(false);
    if(i < mainwindow->spectrum_colorbar->items)
    {
      ((QCheckBox *)(colorBarTable->cellWidget(i, 0)))->setCheckState(Qt::Checked);
    }
    else
    {
      ((QCheckBox *)(colorBarTable->cellWidget(i, 0)))->setCheckState(Qt::Unchecked);
    }
    QObject::connect(colorBarTable->cellWidget(i, 0), SIGNAL(stateChanged(int)), this, SLOT(checkBoxChanged(int)));

    colorBarTable->setCellWidget(i, 1, new QDoubleSpinBox);
    ((QDoubleSpinBox *)(colorBarTable->cellWidget(i, 1)))->setDecimals(3);
    ((QDoubleSpinBox *)(colorBarTable->cellWidget(i, 1)))->setSuffix(" Hz");
    ((QDoubleSpinBox *)(colorBarTable->cellWidget(i, 1)))->setRange(0.001, 100000.0);
    ((QDoubleSpinBox *)(colorBarTable->cellWidget(i, 1)))->setValue(mainwindow->spectrum_colorbar->freq[i]);
    QObject::connect((QDoubleSpinBox *)(colorBarTable->cellWidget(i, 1)), SIGNAL(valueChanged(double)), this, SLOT(spinBoxValueChanged(double)));

    colorBarTable->setCellWidget(i, 2, new SpecialButton);
    ((SpecialButton *)(colorBarTable->cellWidget(i, 2)))->setGlobalColor(mainwindow->spectrum_colorbar->color[i]);
    QObject::connect((SpecialButton *)(colorBarTable->cellWidget(i, 2)), SIGNAL(clicked(SpecialButton *)), this, SLOT(colorBarButtonClicked(SpecialButton *)));

    colorBarTable->setCellWidget(i, 3, new QLineEdit);
    ((QLineEdit *)(colorBarTable->cellWidget(i, 3)))->setText(mainwindow->spectrum_colorbar->label[i]);
    ((QLineEdit *)(colorBarTable->cellWidget(i, 3)))->setMaxLength(16);
    QObject::connect((QLineEdit *)(colorBarTable->cellWidget(i, 3)), SIGNAL(textEdited(const QString  &)), this, SLOT(labelEdited(const QString  &)));
  }

  QStringList horizontallabels;
  horizontallabels += "";
  horizontallabels += "Frequency";
  horizontallabels += "Color";
  horizontallabels += "Label";
  colorBarTable->setHorizontalHeaderLabels(horizontallabels);

  label2_3 = new QLabel(tab3);
  label2_3->setGeometry(20, 460 + (12 * fsz), 220 * fsz, 10 + (12 * fsz));
  label2_3->setText("Height of colorbars are relative to the");

  radiobutton1 = new QRadioButton(tab3);
  radiobutton1->setGeometry(20, 480 + (24 * fsz), 200 * fsz, 10 + (12 * fsz));
  radiobutton1->setText("sum");
  if(mainwindow->spectrum_colorbar->method == 0)
  {
    radiobutton1->setChecked(true);  // sum
  }

  radiobutton2 = new QRadioButton(tab3);
  radiobutton2->setGeometry(20, 500 + (36 * fsz), 200 * fsz, 10 + (12 * fsz));
  radiobutton2->setText("peak");
  if(mainwindow->spectrum_colorbar->method == 1)
  {
    radiobutton2->setChecked(true);  // peak
  }

  radiobutton3 = new QRadioButton(tab3);
  radiobutton3->setGeometry(20, 520 + (48 * fsz), 200 * fsz, 10 + (12 * fsz));
  radiobutton3->setText("average");
  if(mainwindow->spectrum_colorbar->method == 2)
  {
    radiobutton3->setChecked(true);  // average
  }

  label3_3 = new QLabel(tab3);
  label3_3->setGeometry(20, 540 + (60 * fsz), 220 * fsz, 10 + (12 * fsz));
  label3_3->setText("of the power in the colorbar region.");

  label3_4 = new QLabel(tab3);
  label3_4->setGeometry(20, 580 + (72 * fsz), 120 * fsz, 10 + (12 * fsz));
  label3_4->setText("FFT blocksize:");

  spinbox3_1 = new QSpinBox(tab3);
  spinbox3_1->setGeometry(40 + (120 * fsz), 580 + (72 * fsz), 140 * fsz, 10 + (12 * fsz));
  spinbox3_1->setSuffix(" samples");
  spinbox3_1->setMinimum(10);
  spinbox3_1->setMaximum(1000);
  spinbox3_1->setSingleStep(2);
  spinbox3_1->setValue(mainwindow->maxdftblocksize);

  label3_5 = new QLabel(tab3);
  label3_5->setGeometry(20, 620 + (84 * fsz), 120 * fsz, 10 + (12 * fsz));
  label3_5->setText("Colorbar sensitivity:");

  dspinbox3_2 = new QDoubleSpinBox(tab3);
  dspinbox3_2->setGeometry(40 + (120 * fsz), 620 + (84 * fsz), 140 * fsz, 10 + (12 * fsz));
  dspinbox3_2->setMinimum(0.0001);
  dspinbox3_2->setMaximum(100000.0);
  dspinbox3_2->setValue(mainwindow->spectrum_colorbar->max_colorbar_value);

  checkbox3_1 = new QCheckBox("Auto", tab3);
  checkbox3_1->setGeometry(60 + (260 * fsz), 620 + (84 * fsz), 100 * fsz, 10 + (12 * fsz));
  checkbox3_1->setTristate(false);
  if(mainwindow->spectrum_colorbar->auto_adjust)
  {
    checkbox3_1->setCheckState(Qt::Checked);

    dspinbox3_2->setEnabled(false);
  }
  else
  {
    checkbox3_1->setCheckState(Qt::Unchecked);
  }

  ApplyButton2 = new QPushButton(tab3);
  ApplyButton2->setGeometry(20, 700 + (96 * fsz), 120 * fsz, 13 + (12 * fsz));
  ApplyButton2->setText("Apply");
  ApplyButton2->setEnabled(false);

  DefaultButton2 = new QPushButton(tab3);
  DefaultButton2->setGeometry(60 + (120 * fsz), 700 + (96 * fsz), 120 * fsz, 13 + (12 * fsz));
  DefaultButton2->setText("Restore default");

  QObject::connect(radiobutton1,   SIGNAL(toggled(bool)),        this, SLOT(radioButtonToggled(bool)));
  QObject::connect(radiobutton2,   SIGNAL(toggled(bool)),        this, SLOT(radioButtonToggled(bool)));
  QObject::connect(radiobutton3,   SIGNAL(toggled(bool)),        this, SLOT(radioButtonToggled(bool)));
  QObject::connect(spinbox3_1,     SIGNAL(valueChanged(int)),    this, SLOT(spinBox3_1ValueChanged(int)));
  QObject::connect(dspinbox3_2,    SIGNAL(valueChanged(double)), this, SLOT(dspinBox3_2ValueChanged(double)));
  QObject::connect(ApplyButton2,   SIGNAL(clicked()),            this, SLOT(ApplyButton2Clicked()));
  QObject::connect(DefaultButton2, SIGNAL(clicked()),            this, SLOT(DefaultButton2Clicked()));
  QObject::connect(checkbox3_1,    SIGNAL(stateChanged(int)),    this, SLOT(checkbox3_1Clicked(int)));

  colorBarTable->resizeColumnsToContents();

  tabholder->addTab(tab3, "Power Spectrum");

/////////////////////////////////////// tab 4  Other ///////////////////////////////////////////////////////////////////////

  tab4 = new QWidget;

  gr = new QGridLayout(tab4);
  gr->setVerticalSpacing(15);
  gr->setHorizontalSpacing(15);

  label4_1 = new QLabel(tab4);
  label4_1->setText("Reload last used montage");
  gr->addWidget(label4_1, 0, 0);

  checkbox4_1 = new QCheckBox(tab4);
  checkbox4_1->setTristate(false);
  if(mainwindow->auto_reload_mtg)
  {
    checkbox4_1->setCheckState(Qt::Checked);
  }
  else
  {
    checkbox4_1->setCheckState(Qt::Unchecked);
  }
  gr->addWidget(checkbox4_1, 0, 1);

  QObject::connect(checkbox4_1, SIGNAL(stateChanged(int)), this, SLOT(checkbox4_1Clicked(int)));

  label4_7 = new QLabel(tab4);
  label4_7->setText("Do not read annotations, Biosemi Status signal\n"
                    "or Nihon Kohden triggers when filesize\n"
                    "is more than:");
  gr->addWidget(label4_7, 1, 0);

  spinbox4_3 = new QSpinBox(tab4);
  spinbox4_3->setSuffix(" MB");
  spinbox4_3->setMinimum(100);
  spinbox4_3->setMaximum(100000);
  spinbox4_3->setSingleStep(1);
  spinbox4_3->setValue((int)(mainwindow->maxfilesize_to_readin_annotations / 1048576LL));
  gr->addWidget(spinbox4_3, 1, 1);

  QObject::connect(spinbox4_3, SIGNAL(valueChanged(int)), this, SLOT(spinBox4_3ValueChanged(int)));

  label4_2 = new QLabel(tab4);
  label4_2->setText("Read Biosemi Status signal");
  gr->addWidget(label4_2, 2, 0);

  checkbox4_2 = new QCheckBox(tab4);
  checkbox4_2->setTristate(false);
  if(mainwindow->read_biosemi_status_signal)
  {
    checkbox4_2->setCheckState(Qt::Checked);
  }
  else
  {
    checkbox4_2->setCheckState(Qt::Unchecked);
  }
  gr->addWidget(checkbox4_2, 2, 1);

  QObject::connect(checkbox4_2, SIGNAL(stateChanged(int)), this, SLOT(checkbox4_2Clicked(int)));

  label4_6 = new QLabel(tab4);
  label4_6->setText("Read Nihon Kohden Trigger/Marker signal");
  gr->addWidget(label4_6, 3, 0);

  checkbox4_3 = new QCheckBox(tab4);
  checkbox4_3->setTristate(false);
  if(mainwindow->read_nk_trigger_signal)
  {
    checkbox4_3->setCheckState(Qt::Checked);
  }
  else
  {
    checkbox4_3->setCheckState(Qt::Unchecked);
  }
  gr->addWidget(checkbox4_3, 3, 1);

  QObject::connect(checkbox4_3, SIGNAL(stateChanged(int)), this, SLOT(checkbox4_3Clicked(int)));

  label4_3 = new QLabel(tab4);
  label4_3->setText("livestream update interval");
  gr->addWidget(label4_3, 4, 0);

  spinbox4_1 = new QSpinBox(tab4);
  spinbox4_1->setSuffix(" mSec");
  spinbox4_1->setMinimum(100);
  spinbox4_1->setMaximum(3000);
  spinbox4_1->setSingleStep(1);
  spinbox4_1->setValue(mainwindow->live_stream_update_interval);
  gr->addWidget(spinbox4_1, 4, 1);

  QObject::connect(spinbox4_1, SIGNAL(valueChanged(int)), this, SLOT(spinBox4_1ValueChanged(int)));

  label4_4 = new QLabel(tab4);
  label4_4->setText("Powerline Frequency");
  gr->addWidget(label4_4, 5, 0);

  combobox4_1 = new QComboBox(tab4);
  combobox4_1->addItem("50 Hz");
  combobox4_1->addItem("60 Hz");
  if(mainwindow->powerlinefreq == 50)
  {
    combobox4_1->setCurrentIndex(0);
  }
  if(mainwindow->powerlinefreq == 60)
  {
    combobox4_1->setCurrentIndex(1);
  }
  gr->addWidget(combobox4_1, 5, 1);

  QObject::connect(combobox4_1, SIGNAL(currentIndexChanged(int)), this, SLOT(combobox4_1IndexChanged(int)));

  label4_4 = new QLabel(tab4);
  label4_4->setText("Mousewheel stepsize\n"
                    "(0 is no scroll)");
  gr->addWidget(label4_4, 6, 0);

  spinbox4_2 = new QSpinBox(tab4);
  spinbox4_2->setPrefix("Timescale / ");
  spinbox4_2->setMinimum(0);
  spinbox4_2->setMaximum(100);
  spinbox4_2->setSingleStep(1);
  spinbox4_2->setValue(mainwindow->mousewheelsens);
  gr->addWidget(spinbox4_2, 6, 1);

  QObject::connect(spinbox4_2, SIGNAL(valueChanged(int)), this, SLOT(spinBox4_2ValueChanged(int)));

  label4_5 = new QLabel(tab4);
  label4_5->setText("Use Multi-Threading");
  gr->addWidget(label4_5, 7, 0);

  checkbox4_4 = new QCheckBox(tab4);
  checkbox4_4->setTristate(false);
  if(mainwindow->use_threads)
  {
    checkbox4_4->setCheckState(Qt::Checked);
  }
  else
  {
    checkbox4_4->setCheckState(Qt::Unchecked);
  }
  gr->addWidget(checkbox4_4, 7, 1);

  QObject::connect(checkbox4_4, SIGNAL(stateChanged(int)), this, SLOT(checkbox4_4Clicked(int)));

  label4_8 = new QLabel(tab4);
  label4_8->setText("Check for updates during startup");
  gr->addWidget(label4_8, 8, 0);

  checkbox4_5 = new QCheckBox(tab4);
  checkbox4_5->setTristate(false);
  if(mainwindow->check_for_updates)
  {
    checkbox4_5->setCheckState(Qt::Checked);
  }
  else
  {
    checkbox4_5->setCheckState(Qt::Unchecked);
  }
  gr->addWidget(checkbox4_5, 8, 1);

  QObject::connect(checkbox4_5, SIGNAL(stateChanged(int)), this, SLOT(checkbox4_5Clicked(int)));

  label4_9 = new QLabel(tab4);
  label4_9->setText("Viewtime / fileposition indicator");
  gr->addWidget(label4_9, 9, 0);

  combobox4_2 = new QComboBox(tab4);
  combobox4_2->addItem("relative");
  combobox4_2->addItem("real (relative)");
  combobox4_2->addItem("date real (relative)");
  combobox4_2->setCurrentIndex(mainwindow->viewtime_indicator_type);
  gr->addWidget(combobox4_2, 9, 1);

  QObject::connect(combobox4_2, SIGNAL(currentIndexChanged(int)), this, SLOT(combobox4_2IndexChanged(int)));

  label4_10 = new QLabel(tab4);
  label4_10->setText("Window title content");
  gr->addWidget(label4_10, 10, 0);

  combobox4_3 = new QComboBox(tab4);
  combobox4_3->addItem("Subject info");
  combobox4_3->addItem("Filename");
  combobox4_3->addItem("Filename with full path");
  combobox4_3->setCurrentIndex(mainwindow->mainwindow_title_type);
  gr->addWidget(combobox4_3, 10, 1);

  QObject::connect(combobox4_3, SIGNAL(currentIndexChanged(int)), this, SLOT(combobox4_3IndexChanged(int)));

  label4_11 = new QLabel(tab4);
  label4_11->setText("Default amplitude");
  gr->addWidget(label4_11, 11, 0);

  dspinbox4_4 = new QDoubleSpinBox(tab4);
  dspinbox4_4->setMinimum(0.001);
  dspinbox4_4->setMaximum(10000000);
  dspinbox4_4->setSuffix(" /cm");
  dspinbox4_4->setValue(mainwindow->default_amplitude);
  gr->addWidget(dspinbox4_4, 11, 1);

  QObject::connect(dspinbox4_4, SIGNAL(valueChanged(double)), this, SLOT(dspinbox4_4ValueChanged(double)));

  label4_12 = new QLabel(tab4);
  label4_12->setText("Use linear interpolation for plotting");
  label4_12->setToolTip("Enabling this option will avoid the \"stairstep\" effect and will make the signal look smoother.");
  gr->addWidget(label4_12, 12, 0);

  checkbox4_6 = new QCheckBox(tab4);
  checkbox4_6->setTristate(false);
  checkbox4_6->setToolTip("Enabling this option will avoid the \"stairstep\" effect and will make the signal look smoother.");
  if(mainwindow->linear_interpol)
  {
    checkbox4_6->setCheckState(Qt::Checked);
  }
  else
  {
    checkbox4_6->setCheckState(Qt::Unchecked);
  }
  gr->addWidget(checkbox4_6, 12, 1);

  QObject::connect(checkbox4_6, SIGNAL(stateChanged(int)), this, SLOT(checkbox4_6Clicked(int)));

  label4_13 = new QLabel(tab4);
  label4_13->setText("Auto update annotation-editor onsettime");
  label4_13->setToolTip("Enabling this option will automatically update the onsettime field of the annotation-editor\n"
                        "when scrolling and a cross-hair is active.");
  gr->addWidget(label4_13, 13, 0);

  checkbox4_7 = new QCheckBox(tab4);
  checkbox4_7->setTristate(false);
  checkbox4_7->setToolTip("Enabling this option will automatically update the onsettime field of the annotation-editor\n"
                          "when scrolling and a cross-hair is active.");
  if(mainwindow->auto_update_annot_onset)
  {
    checkbox4_7->setCheckState(Qt::Checked);
  }
  else
  {
    checkbox4_7->setCheckState(Qt::Unchecked);
  }
  gr->addWidget(checkbox4_7, 13, 1);

  QObject::connect(checkbox4_7, SIGNAL(stateChanged(int)), this, SLOT(checkbox4_7Clicked(int)));

  label4_14 = new QLabel(tab4);
  label4_14->setText("R-wave description string");
  gr->addWidget(label4_14, 14, 0);

  lineedit4_1 = new QLineEdit(tab4);
  lineedit4_1->setMaxLength(31);
  lineedit4_1->setText(mainwindow->ecg_qrs_rpeak_descr);
  gr->addWidget(lineedit4_1, 14, 1);

  label4_15 = new QLabel(tab4);
  label4_15->setText("Fontsize factor");
  label4_15->setToolTip("Changing the fontsize factor will require a restart.");
  gr->addWidget(label4_15, 15, 0);

  dspinbox4_5 = new QDoubleSpinBox(tab4);
  dspinbox4_5->setMinimum(1.0);
  dspinbox4_5->setMaximum(3.0);
  dspinbox4_5->setDecimals(1);
  dspinbox4_5->setSingleStep(0.1);
  dspinbox4_5->setValue(mainwindow->fontsize_factor);
  dspinbox4_5->setToolTip("Changing the fontsize factor will require a restart.");
  dspinbox4_5->setEnabled(false);
  gr->addWidget(dspinbox4_5, 15, 1);

  gr->addItem(new QSpacerItem(25, 25), 16, 0, -1, -1);
  gr->setRowStretch(16, 1000);

  QObject::connect(dspinbox4_5, SIGNAL(valueChanged(double)), this, SLOT(dspinbox4_5ValueChanged(double)));

  tabholder->addTab(tab4, "Other");

  QObject::connect(lineedit4_1, SIGNAL(textEdited(const QString)), this, SLOT(lineedit4_1_changed(const QString)));

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  horLayout = new QHBoxLayout;
  horLayout->addSpacing(1000);
  horLayout->addWidget(CloseButton);
  horLayout->addSpacing(100);

  mainLayout = new QVBoxLayout;
  mainLayout->addWidget(tabholder);
  mainLayout->addLayout(horLayout);

  frame->setLayout(mainLayout);

  scrollarea->setWidget(frame);

  scrolllayout = new QGridLayout;
  scrolllayout->addWidget(scrollarea, 0, 0);

  optionsdialog->setLayout(scrolllayout);

  tabholder->setMinimumWidth(100 + (410 * fsz));
  tabholder->setMaximumWidth(100 + (410 * fsz));

  frame->setMinimumSize(160 + (410 * fsz), 870);
  frame->setMaximumWidth(160 + (410 * fsz));

  QObject::connect(CloseButton, SIGNAL(clicked()), optionsdialog, SLOT(close()));

  optionsdialog->exec();
}


void UI_OptionsDialog::spinBox4_3ValueChanged(int filesize)
{
  mainwindow->maxfilesize_to_readin_annotations = (long long)filesize * 1048576LL;
}


void UI_OptionsDialog::spinBox4_2ValueChanged(int stepsize)
{
  mainwindow->mousewheelsens = stepsize;
}


void UI_OptionsDialog::combobox4_1IndexChanged(int index)
{
  if(index == 0)
  {
    mainwindow->powerlinefreq = 50;
  }

  if(index == 1)
  {
    mainwindow->powerlinefreq = 60;
  }
}


void UI_OptionsDialog::combobox4_2IndexChanged(int index)
{
  mainwindow->viewtime_indicator_type = index;

  mainwindow->setup_viewbuf();
}


void UI_OptionsDialog::combobox4_3IndexChanged(int index)
{
  mainwindow->mainwindow_title_type = index;

  mainwindow->setMainwindowTitle(mainwindow->edfheaderlist[mainwindow->sel_viewtime]);
}


void UI_OptionsDialog::spinBox4_1ValueChanged(int interval)
{
  mainwindow->live_stream_update_interval = interval;
}


void UI_OptionsDialog::calibrate_checkbox_stateChanged(int state)
{
  if(state == Qt::Checked)
  {
    spinbox2_1->setEnabled(true);
    spinbox2_2->setEnabled(true);
    ApplyButton->setEnabled(true);
    mainwindow->auto_dpi = 0;
  }
  else
  {
    spinbox2_1->setEnabled(false);
    spinbox2_2->setEnabled(false);
    ApplyButton->setEnabled(false);
    mainwindow->auto_dpi = 1;
    mainwindow->pixelsizefactor = 1.0 / ((double)mainwindow->dpiy / 2.54);
    mainwindow->x_pixelsizefactor = 1.0 / ((double)mainwindow->dpix / 2.54);

    mainwindow->maincurve->drawCurve_stage_1();
  }
}


void UI_OptionsDialog::ApplyButtonClicked()
{
  int i, j;


  mainwindow->pixelsizefactor = 1.0 / (4450.0 / spinbox2_1->value());

  mainwindow->x_pixelsizefactor = 1.0 / (3550.0 / spinbox2_2->value());

  for(i=0; i<mainwindow->signalcomps; i++)
  {
    for(j=0; j<mainwindow->signalcomp[i]->num_of_signals; j++)
    {
      mainwindow->signalcomp[i]->sensitivity[j] =
       mainwindow->signalcomp[i]->edfhdr->edfparam[mainwindow->signalcomp[i]->edfsignal[j]].bitvalue
       / ((double)mainwindow->signalcomp[i]->voltpercm
       * mainwindow->pixelsizefactor);
    }
  }

  mainwindow->maincurve->drawCurve_stage_1();
}


void UI_OptionsDialog::labelEdited(const QString  &)
{
  ApplyButton2->setEnabled(true);
}


void UI_OptionsDialog::spinBox3_1ValueChanged(int)
{
  ApplyButton2->setEnabled(true);
}


void UI_OptionsDialog::dspinBox3_2ValueChanged(double)
{
  ApplyButton2->setEnabled(true);
}


void UI_OptionsDialog::radioButtonToggled(bool)
{
  ApplyButton2->setEnabled(true);
}


void UI_OptionsDialog::spinBoxValueChanged(double)
{
  ApplyButton2->setEnabled(true);
}


void UI_OptionsDialog::ApplyButton2Clicked()
{
  int i, row;

  char str[256];

  for(row = 1; row < MAXSPECTRUMMARKERS; row++)
  {
    if(((QCheckBox *)(colorBarTable->cellWidget(row, 0)))->checkState() == Qt::Checked)
    {
      if(((QDoubleSpinBox *)(colorBarTable->cellWidget(row - 1, 1)))->value() >= ((QDoubleSpinBox *)(colorBarTable->cellWidget(row, 1)))->value())
      {
        snprintf(str, 256, "Row %i must have a higher frequency than row %i", row + 1, row);

        QMessageBox messagewindow(QMessageBox::Critical, "Error", str);
        messagewindow.exec();

        return;
      }
    }
    else
    {
      break;
    }
  }

  for(row = 0; row < MAXSPECTRUMMARKERS; row++)
  {
    if(((QCheckBox *)(colorBarTable->cellWidget(row, 0)))->checkState() == Qt::Checked)
    {
      mainwindow->spectrum_colorbar->freq[row] = ((QDoubleSpinBox *)(colorBarTable->cellWidget(row, 1)))->value();
      mainwindow->spectrum_colorbar->color[row] = ((SpecialButton *)(colorBarTable->cellWidget(row, 2)))->globalColor();
      strncpy(mainwindow->spectrum_colorbar->label[row], ((QLineEdit *)(colorBarTable->cellWidget(row, 3)))->text().toLatin1().data(), 16);
      mainwindow->spectrum_colorbar->label[row][16] = 0;
    }
    else
    {
      break;
    }
  }

  mainwindow->spectrum_colorbar->items = row;

  for(; row < MAXSPECTRUMMARKERS; row++)
  {
    mainwindow->spectrum_colorbar->freq[row] = ((QDoubleSpinBox *)(colorBarTable->cellWidget(row, 1)))->value();
    mainwindow->spectrum_colorbar->color[row] = ((SpecialButton *)(colorBarTable->cellWidget(row, 2)))->globalColor();
  }

  if(radiobutton1->isChecked()) // sum
  {
    mainwindow->spectrum_colorbar->method = 0;
  }
  else
    if(radiobutton2->isChecked()) // peak
    {
      mainwindow->spectrum_colorbar->method = 1;
    }
    else
      if(radiobutton3->isChecked()) // average
      {
        mainwindow->spectrum_colorbar->method = 2;
      }

  mainwindow->maxdftblocksize = spinbox3_1->value();
  if(mainwindow->maxdftblocksize & 1)
  {
    mainwindow->maxdftblocksize--;
  }

  mainwindow->spectrum_colorbar->max_colorbar_value = dspinbox3_2->value();

  if(checkbox3_1->checkState() == Qt::Checked)
  {
    mainwindow->spectrum_colorbar->auto_adjust = 1;
  }
  else
  {
    mainwindow->spectrum_colorbar->auto_adjust = 0;
  }

  ApplyButton2->setEnabled(false);

  for(i=0; i<MAXSPECTRUMDOCKS; i++)
  {
    if(mainwindow->spectrumdock[i]->dock->isVisible())
    {
      mainwindow->spectrumdock[i]->rescan();
    }
  }
}


void UI_OptionsDialog::checkBoxChanged(int state)
{
  int i,
      row,
      lastrow=0;

  if(state == Qt::Checked)
  {
    for(row = MAXSPECTRUMMARKERS - 1; row >= 0; row--)
    {
      if(((QCheckBox *)(colorBarTable->cellWidget(row, 0)))->checkState() == Qt::Checked)
      {
        lastrow = row;

        if(row)
        {
          for(i=row-1; i>=0; i--)
          {
            ((QCheckBox *)(colorBarTable->cellWidget(i, 0)))->setCheckState(Qt::Checked);
          }
        }

        break;
      }
    }
  }
  else
  {
    for(row = 0; row < MAXSPECTRUMMARKERS; row++)
    {
      if(((QCheckBox *)(colorBarTable->cellWidget(row, 0)))->checkState() == Qt::Unchecked)
      {
        lastrow = row - 1;

        for(; row < MAXSPECTRUMMARKERS; row++)
        {
          ((QCheckBox *)(colorBarTable->cellWidget(row, 0)))->setCheckState(Qt::Unchecked);
        }

        break;
      }
    }
  }

  for(row=0; row < lastrow; row++)
  {
    if(((QDoubleSpinBox *)(colorBarTable->cellWidget(row, 1)))->value() >= ((QDoubleSpinBox *)(colorBarTable->cellWidget(row + 1, 1)))->value())
    {
      ((QDoubleSpinBox *)(colorBarTable->cellWidget(row + 1, 1)))->setValue(((QDoubleSpinBox *)(colorBarTable->cellWidget(row, 1)))->value() + 1.0);
    }
  }

  ApplyButton2->setEnabled(true);
}


void UI_OptionsDialog::DefaultButton2Clicked()
{
  int i;

  for(i=0; i<5; i++)
  {
    ((QCheckBox *)(colorBarTable->cellWidget(i, 0)))->setCheckState(Qt::Checked);
    ((QLineEdit *)(colorBarTable->cellWidget(i, 3)))->clear();
  }

  ((QDoubleSpinBox *)(colorBarTable->cellWidget(0, 1)))->setValue(4.0);
  ((SpecialButton *)(colorBarTable->cellWidget(0, 2)))->setGlobalColor(Qt::darkRed);

  ((QDoubleSpinBox *)(colorBarTable->cellWidget(1, 1)))->setValue(8.0);
  ((SpecialButton *)(colorBarTable->cellWidget(1, 2)))->setGlobalColor(Qt::darkGreen);

  ((QDoubleSpinBox *)(colorBarTable->cellWidget(2, 1)))->setValue(12.0);
  ((SpecialButton *)(colorBarTable->cellWidget(2, 2)))->setGlobalColor(Qt::darkBlue);

  ((QDoubleSpinBox *)(colorBarTable->cellWidget(3, 1)))->setValue(30.0);
  ((SpecialButton *)(colorBarTable->cellWidget(3, 2)))->setGlobalColor(Qt::darkCyan);

  ((QDoubleSpinBox *)(colorBarTable->cellWidget(4, 1)))->setValue(100.0);
  ((SpecialButton *)(colorBarTable->cellWidget(4, 2)))->setGlobalColor(Qt::darkMagenta);

  for(i=5; i<MAXSPECTRUMMARKERS; i++)
  {
    ((QCheckBox *)(colorBarTable->cellWidget(i, 0)))->setCheckState(Qt::Unchecked);
    ((QDoubleSpinBox *)(colorBarTable->cellWidget(i, 1)))->setValue(1.0);
    ((SpecialButton *)(colorBarTable->cellWidget(i, 2)))->setGlobalColor(Qt::white);
    ((QLineEdit *)(colorBarTable->cellWidget(i, 3)))->clear();
  }

  spinbox3_1->setValue(1000);

  radiobutton1->setChecked(true);

  ApplyButton2->setEnabled(true);
}


void UI_OptionsDialog::colorBarButtonClicked(SpecialButton *button)
{
  int color;

  UI_ColorMenuDialog colormenudialog(&color, mainwindow);

  if(color < 0)  return;

  button->setGlobalColor(color);

  ApplyButton2->setEnabled(true);
}


void UI_OptionsDialog::checkbox1Clicked(int state)
{
  if(state==Qt::Checked)
  {
    mainwindow->maincurve->blackwhite_printing = 1;
  }

  if(state==Qt::Unchecked)
  {
    mainwindow->maincurve->blackwhite_printing = 0;
  }
}


void UI_OptionsDialog::checkbox2Clicked(int state)
{
  if(state==Qt::Checked)
  {
    mainwindow->show_annot_markers = 1;
  }

  if(state==Qt::Unchecked)
  {
    mainwindow->show_annot_markers = 0;
  }

  mainwindow->maincurve->update();
}


void UI_OptionsDialog::checkbox2_1Clicked(int state)
{
  if(state==Qt::Checked)
  {
    mainwindow->annotations_show_duration = 1;
  }

  if(state==Qt::Unchecked)
  {
    mainwindow->annotations_show_duration = 0;
  }

  mainwindow->maincurve->update();
}


void UI_OptionsDialog::checkbox2_2Clicked(int state)
{
  if(state==Qt::Checked)
  {
    mainwindow->annotations_duration_background_type = 1;
  }

  if(state==Qt::Unchecked)
  {
    mainwindow->annotations_duration_background_type = 0;
  }

  mainwindow->maincurve->update();
}


void UI_OptionsDialog::checkbox3Clicked(int state)
{
  if(state==Qt::Checked)
  {
    mainwindow->show_baselines = 1;
  }

  if(state==Qt::Unchecked)
  {
    mainwindow->show_baselines = 0;
  }

  mainwindow->maincurve->update();
}


void UI_OptionsDialog::checkbox4Clicked(int state)
{
  if(state==Qt::Checked)
  {
    mainwindow->clip_to_pane = 1;
  }

  if(state==Qt::Unchecked)
  {
    mainwindow->clip_to_pane = 0;
  }

  mainwindow->maincurve->update();
}


void UI_OptionsDialog::checkbox5Clicked(int state)
{
  if(state==Qt::Checked)
  {
    mainwindow->annot_filter->hide_in_list_only = 1;
  }

  if(state==Qt::Unchecked)
  {
    mainwindow->annot_filter->hide_in_list_only = 0;
  }
}


void UI_OptionsDialog::checkbox16Clicked(int state)
{
  if(state==Qt::Checked)
  {
    mainwindow->use_diverse_signal_colors = 1;
  }

  if(state==Qt::Unchecked)
  {
    mainwindow->use_diverse_signal_colors = 0;
  }
}


void UI_OptionsDialog::checkbox3_1Clicked(int state)
{
  if(state==Qt::Checked)
  {
    dspinbox3_2->setEnabled(false);

    mainwindow->spectrum_colorbar->auto_adjust = 1;
  }

  if(state==Qt::Unchecked)
  {
    dspinbox3_2->setEnabled(true);

    mainwindow->spectrum_colorbar->auto_adjust = 0;
  }

  ApplyButton2->setEnabled(true);
}


void UI_OptionsDialog::checkbox4_1Clicked(int state)
{
  if(state==Qt::Checked)
  {
    mainwindow->auto_reload_mtg = 1;
  }

  if(state==Qt::Unchecked)
  {
    mainwindow->auto_reload_mtg = 0;
  }
}


void UI_OptionsDialog::checkbox4_2Clicked(int state)
{
  if(state==Qt::Checked)
  {
    mainwindow->read_biosemi_status_signal = 1;
  }

  if(state==Qt::Unchecked)
  {
    mainwindow->read_biosemi_status_signal = 0;
  }
}


void UI_OptionsDialog::checkbox4_3Clicked(int state)
{
  if(state==Qt::Checked)
  {
    mainwindow->read_nk_trigger_signal = 1;
  }

  if(state==Qt::Unchecked)
  {
    mainwindow->read_nk_trigger_signal = 0;
  }
}


void UI_OptionsDialog::checkbox4_4Clicked(int state)
{
  if(state==Qt::Checked)
  {
    mainwindow->use_threads = 1;
  }

  if(state==Qt::Unchecked)
  {
    mainwindow->use_threads = 0;
  }
}


void UI_OptionsDialog::checkbox4_5Clicked(int state)
{
  if(state==Qt::Checked)
  {
    mainwindow->check_for_updates = 1;
  }

  if(state==Qt::Unchecked)
  {
    mainwindow->check_for_updates = 0;
  }
}


void UI_OptionsDialog::checkbox4_6Clicked(int state)
{
  if(state==Qt::Checked)
  {
    mainwindow->linear_interpol = 1;
  }

  if(state==Qt::Unchecked)
  {
    mainwindow->linear_interpol = 0;
  }

  mainwindow->setup_viewbuf();
}


void UI_OptionsDialog::checkbox4_7Clicked(int state)
{
  if(state==Qt::Checked)
  {
    mainwindow->auto_update_annot_onset = 1;
  }

  if(state==Qt::Unchecked)
  {
    mainwindow->auto_update_annot_onset = 0;
  }
}


void UI_OptionsDialog::BgColorButtonClicked(SpecialButton *)
{
  int i;

  QColor temp;

  QPalette palette;

  temp = QColorDialog::getColor(mainwindow->maincurve->backgroundcolor, tab1);

  if(temp.isValid())
  {
    mainwindow->maincurve->backgroundcolor = temp;

    BgColorButton->setColor(mainwindow->maincurve->backgroundcolor);

    palette.setColor(QPalette::Text, mainwindow->maincurve->text_color);
    palette.setColor(QPalette::Base, mainwindow->maincurve->backgroundcolor);

    for(i=0; i<mainwindow->files_open; i++)
    {
      if(mainwindow->annotations_dock[i])
      {
        mainwindow->annotations_dock[i]->list->setPalette(palette);
      }
    }

    mainwindow->maincurve->update();
  }
}



void UI_OptionsDialog::SrColorButtonClicked(SpecialButton *)
{
  QColor temp;

  temp = QColorDialog::getColor(mainwindow->maincurve->small_ruler_color, tab1);

  if(temp.isValid())
  {
    mainwindow->maincurve->small_ruler_color = temp;

    SrColorButton->setColor(mainwindow->maincurve->small_ruler_color);

    mainwindow->maincurve->update();
  }
}



void UI_OptionsDialog::BrColorButtonClicked(SpecialButton *)
{
  QColor temp;

  temp = QColorDialog::getColor(mainwindow->maincurve->big_ruler_color, tab1);

  if(temp.isValid())
  {
    mainwindow->maincurve->big_ruler_color = temp;

    BrColorButton->setColor(mainwindow->maincurve->big_ruler_color);

    mainwindow->maincurve->update();
  }
}



void UI_OptionsDialog::MrColorButtonClicked(SpecialButton *)
{
  QColor temp;

  temp = QColorDialog::getColor(mainwindow->maincurve->mouse_rect_color, tab1);

  if(temp.isValid())
  {
    mainwindow->maincurve->mouse_rect_color = temp;

    MrColorButton->setColor(mainwindow->maincurve->mouse_rect_color);

    mainwindow->maincurve->update();
  }
}



void UI_OptionsDialog::TxtColorButtonClicked(SpecialButton *)
{
  int i;

  QColor temp;

  QPalette palette;

  temp = QColorDialog::getColor(mainwindow->maincurve->text_color, tab1);

  if(temp.isValid())
  {
    mainwindow->maincurve->text_color = temp;

    TxtColorButton->setColor(mainwindow->maincurve->text_color);

    palette.setColor(QPalette::Text, mainwindow->maincurve->text_color);
    palette.setColor(QPalette::Base, mainwindow->maincurve->backgroundcolor);

    for(i=0; i<mainwindow->files_open; i++)
    {
      if(edfplus_annotation_size(&mainwindow->edfheaderlist[i]->annot_list) > 0)
      {
        mainwindow->annotations_dock[i]->list->setPalette(palette);
      }
    }

    mainwindow->maincurve->update();
  }
}



void UI_OptionsDialog::SigColorButtonClicked(SpecialButton *)
{
  int i, color;

  UI_ColorMenuDialog colormenudialog(&color, mainwindow);

  if(color < 0)  return;

  SigColorButton->setColor((Qt::GlobalColor)color);

  mainwindow->maincurve->signal_color = color;

  for(i=0; i<mainwindow->signalcomps; i++)
  {
    mainwindow->signalcomp[i]->color = color;
  }

  mainwindow->maincurve->update();
}



void UI_OptionsDialog::BaseColorButtonClicked(SpecialButton *)
{
  QColor temp;

  temp = QColorDialog::getColor(mainwindow->maincurve->baseline_color, tab1);

  if(temp.isValid())
  {
    mainwindow->maincurve->baseline_color = temp;

    BaseColorButton->setColor(mainwindow->maincurve->baseline_color);

    mainwindow->maincurve->update();
  }
}



void UI_OptionsDialog::Crh1ColorButtonClicked(SpecialButton *)
{
  int color;

  UI_ColorMenuDialog colormenudialog(&color, mainwindow);

  if(color < 0)  return;

  Crh1ColorButton->setColor((Qt::GlobalColor)color);

  mainwindow->maincurve->crosshair_1.color = color;

  mainwindow->maincurve->update();
}



void UI_OptionsDialog::Crh2ColorButtonClicked(SpecialButton *)
{
  int color;

  UI_ColorMenuDialog colormenudialog(&color, mainwindow);

  if(color < 0)  return;

  Crh2ColorButton->setColor((Qt::GlobalColor)color);

  mainwindow->maincurve->crosshair_2.color = color;

  mainwindow->maincurve->update();
}


void UI_OptionsDialog::FrColorButtonClicked(SpecialButton *)
{
  int color;

  UI_ColorMenuDialog colormenudialog(&color, mainwindow);

  if(color < 0)  return;

  FrColorButton->setColor((Qt::GlobalColor)color);

  mainwindow->maincurve->floating_ruler_color = color;

  mainwindow->maincurve->update();
}


void UI_OptionsDialog::AnnotMkrButtonClicked(SpecialButton *)
{
  QColor temp;

  temp = QColorDialog::getColor(mainwindow->maincurve->annot_marker_color, tab1);

  if(temp.isValid())
  {
    mainwindow->maincurve->annot_marker_color = temp;

    AnnotMkrButton->setColor(mainwindow->maincurve->annot_marker_color);

    mainwindow->maincurve->update();
  }
}


void UI_OptionsDialog::AnnotDurationButtonClicked(SpecialButton *)
{
  QColor temp;

  temp = QColorDialog::getColor(mainwindow->maincurve->annot_duration_color, tab1, "Select Color", QColorDialog::ShowAlphaChannel);

  if(temp.isValid())
  {
    mainwindow->maincurve->annot_duration_color = temp;

    AnnotDurationButton->setColor(mainwindow->maincurve->annot_duration_color);

    mainwindow->maincurve->update();
  }
}


void UI_OptionsDialog::AnnotDurationSelectedButtonClicked(SpecialButton *)
{
  QColor temp;

  temp = QColorDialog::getColor(mainwindow->maincurve->annot_duration_color_selected, tab1, "Select Color", QColorDialog::ShowAlphaChannel);

  if(temp.isValid())
  {
    mainwindow->maincurve->annot_duration_color_selected = temp;

    AnnotDurationSelectedButton->setColor(mainwindow->maincurve->annot_duration_color_selected);

    mainwindow->maincurve->update();
  }
}


void UI_OptionsDialog::dspinbox4_4ValueChanged(double val)
{
  mainwindow->default_amplitude = val;
}


void UI_OptionsDialog::dspinbox4_5ValueChanged(double val)
{
  mainwindow->fontsize_factor = val;
}


void UI_OptionsDialog::saveColorSchemaButtonClicked()
{
  char path[MAX_PATH_LENGTH];

  FILE *colorfile;

  strlcpy(path, mainwindow->recent_colordir, MAX_PATH_LENGTH);
  strlcat(path, "/my_colorschema.color", MAX_PATH_LENGTH);

  strlcpy(path, QFileDialog::getSaveFileName(0, "Save colorschema", QString::fromLocal8Bit(path), "Colorschema files (*.color *.COLOR)").toLocal8Bit().data(), MAX_PATH_LENGTH);

  if(!strcmp(path, ""))
  {
    return;
  }

  if(strlen(path) > 4)
  {
    if(strcmp(path + strlen(path) - 6, ".color"))
    {
      strlcat(path, ".color", MAX_PATH_LENGTH);
    }
  }

  get_directory_from_path(mainwindow->recent_colordir, path, MAX_PATH_LENGTH);

  colorfile = fopen(path, "wb");
  if(colorfile==NULL)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Can not open file for writing.");
    messagewindow.exec();

    return;
  }

  fprintf(colorfile, "<?xml version=\"1.0\"?>\n<" PROGRAM_NAME "_colorschema>\n");

  fprintf(colorfile, " <backgroundcolor>\n"
                  "  <red>%i</red>\n"
                  "  <green>%i</green>\n"
                  "  <blue>%i</blue>\n"
                  " </backgroundcolor>\n",
                  mainwindow->maincurve->backgroundcolor.red(),
                  mainwindow->maincurve->backgroundcolor.green(),
                  mainwindow->maincurve->backgroundcolor.blue());

  fprintf(colorfile, " <small_ruler_color>\n"
                  "  <red>%i</red>\n"
                  "  <green>%i</green>\n"
                  "  <blue>%i</blue>\n"
                  " </small_ruler_color>\n",
                  mainwindow->maincurve->small_ruler_color.red(),
                  mainwindow->maincurve->small_ruler_color.green(),
                  mainwindow->maincurve->small_ruler_color.blue());

  fprintf(colorfile, " <big_ruler_color>\n"
                  "  <red>%i</red>\n"
                  "  <green>%i</green>\n"
                  "  <blue>%i</blue>\n"
                  " </big_ruler_color>\n",
                  mainwindow->maincurve->big_ruler_color.red(),
                  mainwindow->maincurve->big_ruler_color.green(),
                  mainwindow->maincurve->big_ruler_color.blue());

  fprintf(colorfile, " <mouse_rect_color>\n"
                  "  <red>%i</red>\n"
                  "  <green>%i</green>\n"
                  "  <blue>%i</blue>\n"
                  " </mouse_rect_color>\n",
                  mainwindow->maincurve->mouse_rect_color.red(),
                  mainwindow->maincurve->mouse_rect_color.green(),
                  mainwindow->maincurve->mouse_rect_color.blue());

  fprintf(colorfile, " <text_color>\n"
                  "  <red>%i</red>\n"
                  "  <green>%i</green>\n"
                  "  <blue>%i</blue>\n"
                  " </text_color>\n",
                  mainwindow->maincurve->text_color.red(),
                  mainwindow->maincurve->text_color.green(),
                  mainwindow->maincurve->text_color.blue());

  fprintf(colorfile, " <baseline_color>\n"
                  "  <red>%i</red>\n"
                  "  <green>%i</green>\n"
                  "  <blue>%i</blue>\n"
                  " </baseline_color>\n",
                  mainwindow->maincurve->baseline_color.red(),
                  mainwindow->maincurve->baseline_color.green(),
                  mainwindow->maincurve->baseline_color.blue());

  fprintf(colorfile, " <annot_marker_color>\n"
                  "  <red>%i</red>\n"
                  "  <green>%i</green>\n"
                  "  <blue>%i</blue>\n"
                  " </annot_marker_color>\n",
                  mainwindow->maincurve->annot_marker_color.red(),
                  mainwindow->maincurve->annot_marker_color.green(),
                  mainwindow->maincurve->annot_marker_color.blue());

  fprintf(colorfile, " <annot_duration_color>\n"
                  "  <red>%i</red>\n"
                  "  <green>%i</green>\n"
                  "  <blue>%i</blue>\n"
                  "  <alpha>%i</alpha>\n"
                  " </annot_duration_color>\n",
                  mainwindow->maincurve->annot_duration_color.red(),
                  mainwindow->maincurve->annot_duration_color.green(),
                  mainwindow->maincurve->annot_duration_color.blue(),
                  mainwindow->maincurve->annot_duration_color.alpha());

  fprintf(colorfile, " <annot_duration_color_selected>\n"
                  "  <red>%i</red>\n"
                  "  <green>%i</green>\n"
                  "  <blue>%i</blue>\n"
                  "  <alpha>%i</alpha>\n"
                  " </annot_duration_color_selected>\n",
                  mainwindow->maincurve->annot_duration_color_selected.red(),
                  mainwindow->maincurve->annot_duration_color_selected.green(),
                  mainwindow->maincurve->annot_duration_color_selected.blue(),
                  mainwindow->maincurve->annot_duration_color_selected.alpha());

  fprintf(colorfile, " <signal_color>%i</signal_color>\n",
                  mainwindow->maincurve->signal_color);

  fprintf(colorfile, " <crosshair_1_color>%i</crosshair_1_color>\n",
                  mainwindow->maincurve->crosshair_1.color);

  fprintf(colorfile, " <crosshair_2_color>%i</crosshair_2_color>\n",
                  mainwindow->maincurve->crosshair_2.color);

  fprintf(colorfile, " <floating_ruler_color>%i</floating_ruler_color>\n",
                  mainwindow->maincurve->floating_ruler_color);

  fprintf(colorfile, " <blackwhite_printing>%i</blackwhite_printing>\n",
                  mainwindow->maincurve->blackwhite_printing);

  fprintf(colorfile, " <show_annot_markers>%i</show_annot_markers>\n",
                  mainwindow->show_annot_markers);

  fprintf(colorfile, " <show_baselines>%i</show_baselines>\n",
                  mainwindow->show_baselines);

  fprintf(colorfile, " <clip_to_pane>%i</clip_to_pane>\n",
                  mainwindow->clip_to_pane);




  fprintf(colorfile, "</" PROGRAM_NAME "_colorschema>\n");

  fclose(colorfile);
}


void UI_OptionsDialog::loadColorSchemaButtonClicked()
{
  char path[MAX_PATH_LENGTH],
       scratchpad[2048],
       result[XML_STRBUFLEN];

  struct xml_handle *xml_hdl;


  strlcpy(path, QFileDialog::getOpenFileName(0, "Load colorschema", QString::fromLocal8Bit(mainwindow->recent_colordir), "Montage files (*.color *.COLOR)").toLocal8Bit().data(), MAX_PATH_LENGTH);

  if(!strcmp(path, ""))
  {
    return;
  }

  get_directory_from_path(mainwindow->recent_colordir, path, MAX_PATH_LENGTH);

  xml_hdl = xml_get_handle(path);
  if(xml_hdl==NULL)
  {
    snprintf(scratchpad, 2048, "Can not open colorschema:\n%s", path);
    QMessageBox messagewindow(QMessageBox::Critical, "Error", QString::fromLocal8Bit(scratchpad));
    messagewindow.exec();
    return;
  }

  if(strcmp(xml_hdl->elementname[xml_hdl->level], PROGRAM_NAME "_colorschema"))
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this colorschema.");
    messagewindow.exec();
    xml_close(xml_hdl);
    return;
  }

  mainwindow->get_rgbcolor_settings(xml_hdl, "backgroundcolor", 0, &mainwindow->maincurve->backgroundcolor);

  mainwindow->get_rgbcolor_settings(xml_hdl, "small_ruler_color", 0, &mainwindow->maincurve->small_ruler_color);

  mainwindow->get_rgbcolor_settings(xml_hdl, "big_ruler_color", 0, &mainwindow->maincurve->big_ruler_color);

  mainwindow->get_rgbcolor_settings(xml_hdl, "mouse_rect_color", 0, &mainwindow->maincurve->mouse_rect_color);

  mainwindow->get_rgbcolor_settings(xml_hdl, "text_color", 0, &mainwindow->maincurve->text_color);

  mainwindow->get_rgbcolor_settings(xml_hdl, "baseline_color", 0, &mainwindow->maincurve->baseline_color);

  mainwindow->get_rgbcolor_settings(xml_hdl, "annot_marker_color", 0, &mainwindow->maincurve->annot_marker_color);

  mainwindow->get_rgbcolor_settings(xml_hdl, "annot_duration_color", 0, &mainwindow->maincurve->annot_duration_color);

  mainwindow->get_rgbcolor_settings(xml_hdl, "annot_duration_color_selected", 0, &mainwindow->maincurve->annot_duration_color_selected);

  if(xml_goto_nth_element_inside(xml_hdl, "signal_color", 0))
  {
    xml_close(xml_hdl);
    return;
  }
  if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
  {
    xml_close(xml_hdl);
    return;
  }
  mainwindow->maincurve->signal_color = atoi(result);

  xml_go_up(xml_hdl);

  if(xml_goto_nth_element_inside(xml_hdl, "floating_ruler_color", 0))
  {
    xml_close(xml_hdl);
    return;
  }
  if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
  {
    xml_close(xml_hdl);
    return;
  }
  mainwindow->maincurve->floating_ruler_color = atoi(result);

  xml_go_up(xml_hdl);

  if(xml_goto_nth_element_inside(xml_hdl, "blackwhite_printing", 0))
  {
    xml_close(xml_hdl);
    return;
  }
  if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
  {
    xml_close(xml_hdl);
    return;
  }
  mainwindow->maincurve->blackwhite_printing = atoi(result);

  xml_go_up(xml_hdl);

  if(xml_goto_nth_element_inside(xml_hdl, "show_annot_markers", 0))
  {
    xml_close(xml_hdl);
    return;
  }
  if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
  {
    xml_close(xml_hdl);
    return;
  }
  mainwindow->show_annot_markers = atoi(result);

  xml_go_up(xml_hdl);

  if(xml_goto_nth_element_inside(xml_hdl, "show_baselines", 0))
  {
    xml_close(xml_hdl);
    return;
  }
  if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
  {
    xml_close(xml_hdl);
    return;
  }
  mainwindow->show_baselines = atoi(result);

  xml_go_up(xml_hdl);

  if(xml_goto_nth_element_inside(xml_hdl, "clip_to_pane", 0))
  {
    xml_close(xml_hdl);
    return;
  }
  if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
  {
    xml_close(xml_hdl);
    return;
  }
  mainwindow->clip_to_pane = atoi(result);

  xml_go_up(xml_hdl);

  if(xml_goto_nth_element_inside(xml_hdl, "crosshair_1_color", 0))
  {
    xml_close(xml_hdl);
    return;
  }
  if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
  {
    xml_close(xml_hdl);
    return;
  }
  mainwindow->maincurve->crosshair_1.color = atoi(result);

  xml_go_up(xml_hdl);

  if(xml_goto_nth_element_inside(xml_hdl, "crosshair_2_color", 0))
  {
    xml_close(xml_hdl);
    return;
  }
  if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
  {
    xml_close(xml_hdl);
    return;
  }
  mainwindow->maincurve->crosshair_2.color = atoi(result);

  xml_close(xml_hdl);

  update_interface();
}


void UI_OptionsDialog::update_interface(void)
{
  int i;

  QPalette palette;


  BgColorButton->setColor(mainwindow->maincurve->backgroundcolor);

  SrColorButton->setColor(mainwindow->maincurve->small_ruler_color);

  BrColorButton->setColor(mainwindow->maincurve->big_ruler_color);

  MrColorButton->setColor(mainwindow->maincurve->mouse_rect_color);

  TxtColorButton->setColor(mainwindow->maincurve->text_color);

  SigColorButton->setColor((Qt::GlobalColor)mainwindow->maincurve->signal_color);

  if(mainwindow->show_baselines)
  {
    checkbox3->setCheckState(Qt::Checked);
  }
  else
  {
    checkbox3->setCheckState(Qt::Unchecked);
  }

  BaseColorButton->setColor(mainwindow->maincurve->baseline_color);

  Crh1ColorButton->setColor((Qt::GlobalColor)mainwindow->maincurve->crosshair_1.color);

  Crh2ColorButton->setColor((Qt::GlobalColor)mainwindow->maincurve->crosshair_2.color);

  FrColorButton->setColor((Qt::GlobalColor)mainwindow->maincurve->floating_ruler_color);

  if(mainwindow->show_annot_markers)
  {
    checkbox2->setCheckState(Qt::Checked);
  }
  else
  {
    checkbox2->setCheckState(Qt::Unchecked);
  }

  AnnotMkrButton->setColor(mainwindow->maincurve->annot_marker_color);

  AnnotDurationButton->setColor(mainwindow->maincurve->annot_duration_color);

  AnnotDurationSelectedButton->setColor(mainwindow->maincurve->annot_duration_color_selected);

  if(mainwindow->maincurve->blackwhite_printing)
  {
    checkbox1->setCheckState(Qt::Checked);
  }
  else
  {
    checkbox1->setCheckState(Qt::Unchecked);
  }

  if(mainwindow->clip_to_pane)
  {
    checkbox4->setCheckState(Qt::Checked);
  }
  else
  {
    checkbox4->setCheckState(Qt::Unchecked);
  }

  palette.setColor(QPalette::Text, mainwindow->maincurve->text_color);
  palette.setColor(QPalette::Base, mainwindow->maincurve->backgroundcolor);

  for(i=0; i<mainwindow->files_open; i++)
  {
    if(mainwindow->annotations_dock[i])
    {
      mainwindow->annotations_dock[i]->list->setPalette(palette);
    }
  }

  for(i=0; i<mainwindow->signalcomps; i++)
  {
    mainwindow->signalcomp[i]->color = mainwindow->maincurve->signal_color;
  }

  mainwindow->maincurve->update();
}


void UI_OptionsDialog::loadColorSchema_NK()
{
  mainwindow->maincurve->backgroundcolor.setRed(255);
  mainwindow->maincurve->backgroundcolor.setGreen(255);
  mainwindow->maincurve->backgroundcolor.setBlue(255);

  mainwindow->maincurve->small_ruler_color.setRed(0);
  mainwindow->maincurve->small_ruler_color.setGreen(0);
  mainwindow->maincurve->small_ruler_color.setBlue(0);

  mainwindow->maincurve->big_ruler_color.setRed(255);
  mainwindow->maincurve->big_ruler_color.setGreen(255);
  mainwindow->maincurve->big_ruler_color.setBlue(0);

  mainwindow->maincurve->mouse_rect_color.setRed(0);
  mainwindow->maincurve->mouse_rect_color.setGreen(0);
  mainwindow->maincurve->mouse_rect_color.setBlue(0);

  mainwindow->maincurve->text_color.setRed(0);
  mainwindow->maincurve->text_color.setGreen(0);
  mainwindow->maincurve->text_color.setBlue(0);

  mainwindow->maincurve->baseline_color.setRed(128);
  mainwindow->maincurve->baseline_color.setGreen(128);
  mainwindow->maincurve->baseline_color.setBlue(128);

  mainwindow->maincurve->annot_marker_color.setRed(0);
  mainwindow->maincurve->annot_marker_color.setGreen(0);
  mainwindow->maincurve->annot_marker_color.setBlue(0);

  mainwindow->maincurve->annot_duration_color.setRed(0);
  mainwindow->maincurve->annot_duration_color.setGreen(127);
  mainwindow->maincurve->annot_duration_color.setBlue(127);
  mainwindow->maincurve->annot_duration_color.setAlpha(32);

  mainwindow->maincurve->annot_duration_color_selected.setRed(127);
  mainwindow->maincurve->annot_duration_color_selected.setGreen(0);
  mainwindow->maincurve->annot_duration_color_selected.setBlue(127);
  mainwindow->maincurve->annot_duration_color_selected.setAlpha(32);

  mainwindow->maincurve->signal_color = 2;

  mainwindow->maincurve->floating_ruler_color = 7;

  mainwindow->maincurve->blackwhite_printing = 1;

  mainwindow->show_annot_markers = 1;

  mainwindow->show_baselines = 1;

  mainwindow->maincurve->crosshair_1.color = 7;

  mainwindow->maincurve->crosshair_2.color = 9;

  mainwindow->clip_to_pane = 0;

  update_interface();
}


void UI_OptionsDialog::loadColorSchema_Dark()
{
  mainwindow->maincurve->backgroundcolor.setRed(64);
  mainwindow->maincurve->backgroundcolor.setGreen(64);
  mainwindow->maincurve->backgroundcolor.setBlue(64);

  mainwindow->maincurve->small_ruler_color.setRed(255);
  mainwindow->maincurve->small_ruler_color.setGreen(255);
  mainwindow->maincurve->small_ruler_color.setBlue(255);

  mainwindow->maincurve->big_ruler_color.setRed(128);
  mainwindow->maincurve->big_ruler_color.setGreen(128);
  mainwindow->maincurve->big_ruler_color.setBlue(128);

  mainwindow->maincurve->mouse_rect_color.setRed(255);
  mainwindow->maincurve->mouse_rect_color.setGreen(255);
  mainwindow->maincurve->mouse_rect_color.setBlue(255);

  mainwindow->maincurve->text_color.setRed(255);
  mainwindow->maincurve->text_color.setGreen(255);
  mainwindow->maincurve->text_color.setBlue(255);

  mainwindow->maincurve->baseline_color.setRed(128);
  mainwindow->maincurve->baseline_color.setGreen(128);
  mainwindow->maincurve->baseline_color.setBlue(128);

  mainwindow->maincurve->annot_marker_color.setRed(255);
  mainwindow->maincurve->annot_marker_color.setGreen(255);
  mainwindow->maincurve->annot_marker_color.setBlue(255);

  mainwindow->maincurve->annot_duration_color.setRed(0);
  mainwindow->maincurve->annot_duration_color.setGreen(127);
  mainwindow->maincurve->annot_duration_color.setBlue(127);
  mainwindow->maincurve->annot_duration_color.setAlpha(32);

  mainwindow->maincurve->annot_duration_color_selected.setRed(127);
  mainwindow->maincurve->annot_duration_color_selected.setGreen(0);
  mainwindow->maincurve->annot_duration_color_selected.setBlue(127);
  mainwindow->maincurve->annot_duration_color_selected.setAlpha(32);

  mainwindow->maincurve->signal_color = 12;

  mainwindow->maincurve->floating_ruler_color = 10;

  mainwindow->maincurve->blackwhite_printing = 1;

  mainwindow->show_annot_markers = 1;

  mainwindow->show_baselines = 1;

  mainwindow->maincurve->crosshair_1.color = 7;

  mainwindow->maincurve->crosshair_2.color = 10;

  mainwindow->clip_to_pane = 0;

  update_interface();
}


void UI_OptionsDialog::loadColorSchema_blue_gray()
{
  int i;

  QPalette palette;

  mainwindow->maincurve->backgroundcolor = Qt::gray;
  BgColorButton->setColor(mainwindow->maincurve->backgroundcolor);

  mainwindow->maincurve->small_ruler_color = Qt::black;
  SrColorButton->setColor(mainwindow->maincurve->small_ruler_color);

  mainwindow->maincurve->big_ruler_color = Qt::darkGray;
  BrColorButton->setColor(mainwindow->maincurve->big_ruler_color);

  mainwindow->maincurve->mouse_rect_color = Qt::black;
  MrColorButton->setColor(mainwindow->maincurve->mouse_rect_color);

  mainwindow->maincurve->text_color = Qt::black;
  TxtColorButton->setColor(mainwindow->maincurve->text_color);

  mainwindow->maincurve->signal_color = Qt::blue;
  SigColorButton->setColor(Qt::blue);

  mainwindow->maincurve->baseline_color = Qt::darkGray;
  BaseColorButton->setColor(Qt::darkGray);
  mainwindow->show_baselines = 1;
  checkbox3->setCheckState(Qt::Checked);

  mainwindow->maincurve->crosshair_1.color = Qt::red;
  Crh1ColorButton->setColor(Qt::red);

  mainwindow->maincurve->crosshair_2.color = Qt::cyan;
  Crh2ColorButton->setColor(Qt::cyan);

  mainwindow->maincurve->floating_ruler_color = Qt::red;
  FrColorButton->setColor(Qt::red);

  mainwindow->maincurve->annot_marker_color = Qt::white;
  AnnotMkrButton->setColor(Qt::white);
  mainwindow->show_annot_markers = 1;
  checkbox2->setCheckState(Qt::Checked);

  mainwindow->maincurve->annot_duration_color.setRed(0);
  mainwindow->maincurve->annot_duration_color.setGreen(127);
  mainwindow->maincurve->annot_duration_color.setBlue(127);
  mainwindow->maincurve->annot_duration_color.setAlpha(32);
  AnnotDurationButton->setColor(mainwindow->maincurve->annot_duration_color);

  mainwindow->maincurve->annot_duration_color_selected.setRed(127);
  mainwindow->maincurve->annot_duration_color_selected.setGreen(0);
  mainwindow->maincurve->annot_duration_color_selected.setBlue(127);
  mainwindow->maincurve->annot_duration_color_selected.setAlpha(32);

  palette.setColor(QPalette::Text, mainwindow->maincurve->text_color);
  palette.setColor(QPalette::Base, mainwindow->maincurve->backgroundcolor);

  for(i=0; i<mainwindow->files_open; i++)
  {
    if(mainwindow->annotations_dock[i])
    {
      mainwindow->annotations_dock[i]->list->setPalette(palette);
    }
  }

  for(i=0; i<mainwindow->signalcomps; i++)
  {
    mainwindow->signalcomp[i]->color = mainwindow->maincurve->signal_color;
  }

  mainwindow->maincurve->blackwhite_printing = 1;

  checkbox1->setCheckState(Qt::Checked);

  checkbox4->setCheckState(Qt::Unchecked);

  mainwindow->maincurve->update();
}


void UI_OptionsDialog::lineedit4_1_changed(const QString qstr)
{
  int i, j, len, cp;

  char str[32];

  strlcpy(str, qstr.toLatin1().data(), 32);
  trim_spaces(str);
  len = strlen(str);

  cp = lineedit4_1->cursorPosition();

  for(i=0; i<len; i++)
  {
    if((str[i] < 32) || (str[i] > 126))
    {
      for(j=i; j<len; j++)
      {
        str[j] = str[j+1];
      }

      i--;
    }
  }

  lineedit4_1->setText(str);

  lineedit4_1->setCursorPosition(cp);

  if(strlen(mainwindow->ecg_qrs_rpeak_descr))
  {
    strlcpy(mainwindow->ecg_qrs_rpeak_descr, str, 32);
  }
  else
  {
    strlcpy(mainwindow->ecg_qrs_rpeak_descr, "R-wave", 32);
  }
}








