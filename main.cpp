
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <stdint.h>

#include <QApplication>
#include <QObject>
#include <QPixmap>
#include <QSplashScreen>
#include <QTimer>
#include <QPen>
#include <QColor>
#include <QFont>
#include <QEventLoop>
#include <QMessageBox>


#include "mainwindow.h"


int main(int argc, char *argv[])
{
#if defined(_MSC_VER) || defined(_MSC_FULL_VER) || defined(_MSC_BUILD)
#error "Wrong compiler or platform!"
#endif

#if CHAR_BIT != 8
#error "unsupported char size"
#endif

  union
  {
    char four[4];
    int one;
  } byte_order_test_var;

  /* avoid surprises! */
  if((sizeof(char)      != 1) ||
     (sizeof(short)     != 2) ||
     (sizeof(int)       != 4) ||
     (sizeof(long long) != 8) ||
     (sizeof(float)     != 4) ||
     (sizeof(double)    != 8))
  {
    fprintf(stderr, "Wrong compiler or platform!\n");
    return EXIT_FAILURE;
  }

  /* check endianness! */
  byte_order_test_var.one = 0x03020100;
  if((byte_order_test_var.four[0] != 0) ||
     (byte_order_test_var.four[1] != 1) ||
     (byte_order_test_var.four[2] != 2) ||
     (byte_order_test_var.four[3] != 3))
  {
    fprintf(stderr, "Wrong compiler or platform!\n");
    return EXIT_FAILURE;
  }

#if defined(__LP64__)
  if(sizeof(long) != 8)
  {
    fprintf(stderr, "Wrong compiler or platform!\n");
    return EXIT_FAILURE;
  }
#else
  if(sizeof(long) != 4)
  {
    fprintf(stderr, "Wrong compiler or platform!\n");
    return EXIT_FAILURE;
  }
#endif

#if defined(__MINGW64__)
  if(sizeof(long) != 4)
  {
    fprintf(stderr, "Wrong compiler or platform!\n");
    return EXIT_FAILURE;
  }
#endif

#if QT_VERSION >= 0x050600
  QCoreApplication::setAttribute(Qt::AA_DisableHighDpiScaling);
#endif
  QApplication app(argc, argv);

  QPixmap pixmap(":/images/splash.png");

  QPainter p(&pixmap);
  QFont sansFont("Noto Sans", 10);
  p.setFont(sansFont);
  p.setPen(Qt::black);
  if(!strcmp(PROGRAM_BETA_SUFFIX, ""))
  {
    p.drawText(250, 260, 300, 30, Qt::AlignLeft | Qt::TextSingleLine, "version " PROGRAM_VERSION "    " THIS_APP_BITS_W);
  }
  else
  {
    p.drawText(50, 240, 300, 30, Qt::AlignLeft | Qt::TextSingleLine, "version " PROGRAM_VERSION "  " THIS_APP_BITS_W);
    p.drawText(50, 260, 300, 30, Qt::AlignLeft | Qt::TextSingleLine, PROGRAM_BETA_SUFFIX);
  }

  QSplashScreen splash(pixmap, Qt::WindowStaysOnTopHint);

  QTimer t1;
  t1.setSingleShot(true);
#if QT_VERSION >= 0x050000
  t1.setTimerType(Qt::PreciseTimer);
#endif
  QObject::connect(&t1, SIGNAL(timeout()), &splash, SLOT(close()));

  if(QCoreApplication::arguments().size()<2)
  {
    splash.show();

    t1.start(3000);

    QEventLoop evlp;
    QTimer::singleShot(100, &evlp, SLOT(quit()));
    evlp.exec();
  }

  qApp->setStyleSheet("QMessageBox { messagebox-text-interaction-flags: 5; }");

  char str[512]="";

  UI_Mainwindow *MainWindow = new UI_Mainwindow;
  if(MainWindow == NULL)
  {
    splash.close();

    snprintf(str, 512, "Malloc error.\nFile: %s  line: %i", __FILE__, __LINE__);

    fprintf(stderr, "%s\n", str);

    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.setText(str);
    msgBox.exec();

    return EXIT_FAILURE;
  }

  int ret = app.exec();

  delete MainWindow;

  return ret;
}





