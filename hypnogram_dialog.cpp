/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2020 - 2022 Teunis van Beelen
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



#include "hypnogram_dialog.h"





UI_hypnogram_window::UI_hypnogram_window(QWidget *w_parent, struct edfhdrblock *e_hdr, int i_num)
{
  mainwindow = (UI_Mainwindow *)w_parent;

  instance_num = i_num;

  edfhdr = e_hdr;

  myobjectDialog = new QDialog;

  QHBoxLayout *hlayout_tmp=NULL;

  myobjectDialog->setMinimumSize(400 * mainwindow->w_scaling, 400 * mainwindow->h_scaling);
  myobjectDialog->setWindowTitle("Hypnogram");
  myobjectDialog->setModal(true);
  myobjectDialog->setAttribute(Qt::WA_DeleteOnClose, true);
  myobjectDialog->setSizeGripEnabled(true);

  stage_label = new QLabel;
  stage_label->setText("Stage");
  stage_label->setToolTip("The labels as how they will be displayed in the hypnogram");

  annot_label = new QLabel;
  annot_label->setText("Annotation");
  annot_label->setToolTip("The annotations that will be mapped to the labels in the left column");

  stage1_edit = new QLineEdit;
  stage1_edit->setToolTip("The label as how it will be displayed in the hypnogram");
  stage2_edit = new QLineEdit;
  stage2_edit->setToolTip("The label as how it will be displayed in the hypnogram");
  stage3_edit = new QLineEdit;
  stage3_edit->setToolTip("The label as how it will be displayed in the hypnogram");
  stage4_edit = new QLineEdit;
  stage4_edit->setToolTip("The label as how it will be displayed in the hypnogram");
  stage5_edit = new QLineEdit;
  stage5_edit->setToolTip("The label as how it will be displayed in the hypnogram");
  stage6_edit = new QLineEdit;
  stage6_edit->setToolTip("The label as how it will be displayed in the hypnogram");

  annot1_edit = new QLineEdit;
  annot1_edit->setToolTip("The annotation that will be mapped to the label in the left column");
  annot2_edit = new QLineEdit;
  annot2_edit->setToolTip("The annotation that will be mapped to the label in the left column");
  annot3_edit = new QLineEdit;
  annot3_edit->setToolTip("The annotation that will be mapped to the label in the left column");
  annot4_edit = new QLineEdit;
  annot4_edit->setToolTip("The annotation that will be mapped to the label in the left column");
  annot5_edit = new QLineEdit;
  annot5_edit->setToolTip("The annotation that will be mapped to the label in the left column");
  annot6_edit = new QLineEdit;
  annot6_edit->setToolTip("The annotation that will be mapped to the label in the left column");

  close_button = new QPushButton;
  close_button->setText("Close");

  default_button = new QPushButton;
  default_button->setText("Default");

  start_button = new QPushButton;
  start_button->setText("Start");

  stage1_edit->setText(mainwindow->hypnogram_stage_name[0]);
  stage2_edit->setText(mainwindow->hypnogram_stage_name[1]);
  stage3_edit->setText(mainwindow->hypnogram_stage_name[2]);
  stage4_edit->setText(mainwindow->hypnogram_stage_name[3]);
  stage5_edit->setText(mainwindow->hypnogram_stage_name[4]);
  stage6_edit->setText(mainwindow->hypnogram_stage_name[5]);

  annot1_edit->setText(mainwindow->hypnogram_annot_name[0]);
  annot2_edit->setText(mainwindow->hypnogram_annot_name[1]);
  annot3_edit->setText(mainwindow->hypnogram_annot_name[2]);
  annot4_edit->setText(mainwindow->hypnogram_annot_name[3]);
  annot5_edit->setText(mainwindow->hypnogram_annot_name[4]);
  annot6_edit->setText(mainwindow->hypnogram_annot_name[5]);

  QVBoxLayout *vlayout2 = new QVBoxLayout;
  vlayout2->addWidget(stage_label);
  vlayout2->addWidget(stage1_edit);
  vlayout2->addWidget(stage2_edit);
  vlayout2->addWidget(stage3_edit);
  vlayout2->addWidget(stage4_edit);
  vlayout2->addWidget(stage5_edit);
  vlayout2->addWidget(stage6_edit);

  QVBoxLayout *vlayout3 = new QVBoxLayout;
  vlayout3->addWidget(annot_label);
  vlayout3->addWidget(annot1_edit);
  vlayout3->addWidget(annot2_edit);
  vlayout3->addWidget(annot3_edit);
  vlayout3->addWidget(annot4_edit);
  vlayout3->addWidget(annot5_edit);
  vlayout3->addWidget(annot6_edit);

  QHBoxLayout *hlayout2 = new QHBoxLayout;
  hlayout2->addLayout(vlayout2);
  hlayout2->addStretch(100);
  hlayout2->addLayout(vlayout3);
  hlayout2->addStretch(1000);

  QFormLayout *flayout1 = new QFormLayout;
  flayout1->setSpacing(20);

  flayout1->addRow(" ", (QWidget *)NULL);

  use_epoch_len_checkbox = new QCheckBox;
  use_epoch_len_checkbox->setTristate(false);
  use_epoch_len_checkbox->setToolTip("If enabled, it will draw according to the duration of the annotation.\n"
                                     "If disabled, it will draw from onset to the next onset.");
  if(mainwindow->hypnogram_use_epoch_len)
  {
    use_epoch_len_checkbox->setCheckState(Qt::Checked);
  }
  else
  {
    use_epoch_len_checkbox->setCheckState(Qt::Unchecked);
  }
  hlayout_tmp = new QHBoxLayout;
  hlayout_tmp->setAlignment(Qt::AlignCenter);
  hlayout_tmp->addWidget(use_epoch_len_checkbox);
  hlayout_tmp->addStretch(1000);
  flayout1->addRow("Use annotations' duration for epoch length", hlayout_tmp);
  flayout1->labelForField(hlayout_tmp)->setToolTip("If enabled, it will draw according to the duration of the annotation.\n"
                                                   "If disabled, it will draw from onset to the next onset.");
  QObject::connect(use_epoch_len_checkbox, SIGNAL(stateChanged(int)), this, SLOT(use_epoch_len_checkbox_changed(int)));

  QHBoxLayout *hlayout1 = new QHBoxLayout;
  hlayout1->addWidget(close_button);
  hlayout1->addStretch(1000);
  hlayout1->addWidget(default_button);
  hlayout1->addStretch(1000);
  hlayout1->addWidget(start_button);

  QVBoxLayout *vlayout1 = new QVBoxLayout;
  vlayout1->addLayout(hlayout2);
  vlayout1->addLayout(flayout1);
  vlayout1->addStretch(1000);
  vlayout1->addSpacing(20);
  vlayout1->addLayout(hlayout1);

  myobjectDialog->setLayout(vlayout1);

  QObject::connect(close_button,   SIGNAL(clicked()), myobjectDialog, SLOT(close()));
  QObject::connect(default_button, SIGNAL(clicked()),           this, SLOT(default_button_clicked()));
  QObject::connect(start_button,   SIGNAL(clicked()),           this, SLOT(start_button_clicked()));

  myobjectDialog->exec();
}


void UI_hypnogram_window::use_epoch_len_checkbox_changed(int state)
{
  if(state==Qt::Checked)
  {
    mainwindow->hypnogram_use_epoch_len = 1;
  }
  else
  {
    mainwindow->hypnogram_use_epoch_len = 0;
  }
}


void UI_hypnogram_window::default_button_clicked()
{
  stage1_edit->setText("W");
  stage2_edit->setText("R");
  stage3_edit->setText("N1");
  stage4_edit->setText("N2");
  stage5_edit->setText("N3");
  stage6_edit->setText("N4");

  annot1_edit->setText("W");
  annot2_edit->setText("R");
  annot3_edit->setText("N1");
  annot4_edit->setText("N2");
  annot5_edit->setText("N3");
  annot6_edit->setText("N4");
}


void UI_hypnogram_window::start_button_clicked()
{
  int i;

  struct hypnogram_dock_param_struct dock_param;

  memset(&dock_param, 0, sizeof(struct hypnogram_dock_param_struct));

  dock_param.instance_num = instance_num;

  dock_param.edfhdr = edfhdr;

  dock_param.mainwindow = mainwindow;

  strlcpy(dock_param.stage_name[0], stage1_edit->text().toLatin1().data(), 32);
  strlcpy(dock_param.stage_name[1], stage2_edit->text().toLatin1().data(), 32);
  strlcpy(dock_param.stage_name[2], stage3_edit->text().toLatin1().data(), 32);
  strlcpy(dock_param.stage_name[3], stage4_edit->text().toLatin1().data(), 32);
  strlcpy(dock_param.stage_name[4], stage5_edit->text().toLatin1().data(), 32);
  strlcpy(dock_param.stage_name[5], stage6_edit->text().toLatin1().data(), 32);

  for(i=0; i<6; i++)
  {
    trim_spaces(dock_param.stage_name[i]);

    strlcpy(mainwindow->hypnogram_stage_name[i], dock_param.stage_name[i], 32);
  }

  strlcpy(dock_param.annot_name[0], annot1_edit->text().toLatin1().data(), 32);
  strlcpy(dock_param.annot_name[1], annot2_edit->text().toLatin1().data(), 32);
  strlcpy(dock_param.annot_name[2], annot3_edit->text().toLatin1().data(), 32);
  strlcpy(dock_param.annot_name[3], annot4_edit->text().toLatin1().data(), 32);
  strlcpy(dock_param.annot_name[4], annot5_edit->text().toLatin1().data(), 32);
  strlcpy(dock_param.annot_name[5], annot6_edit->text().toLatin1().data(), 32);

  for(i=0; i<6; i++)
  {
    trim_spaces(dock_param.annot_name[i]);

    strlcpy(mainwindow->hypnogram_annot_name[i], dock_param.annot_name[i], 32);
  }

  mainwindow->hypnogram_dock[instance_num] = new UI_hypnogram_dock(mainwindow, dock_param);

  mainwindow->addToolBar(Qt::BottomToolBarArea, mainwindow->hypnogram_dock[instance_num]->hypnogram_dock);

  mainwindow->insertToolBarBreak(mainwindow->hypnogram_dock[instance_num]->hypnogram_dock);

  edfhdr->hypnogram_dock[instance_num] = instance_num + 1;

  QObject::connect(mainwindow, SIGNAL(annot_docklist_changed()), mainwindow->hypnogram_dock[instance_num], SLOT(update_curve()));

  myobjectDialog->close();
}





























