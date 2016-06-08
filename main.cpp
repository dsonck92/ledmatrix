#include "ledmatrix.h"
#include <QApplication>

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  LEDMatrix w;
  w.show();

  return a.exec();
}
