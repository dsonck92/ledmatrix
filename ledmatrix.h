#ifndef LEDMATRIX_H
#define LEDMATRIX_H

#include "ledserver.h"
#include "qledmatrix.h"
#include "ui_ledmatrix.h"

#include <QUdpSocket>

class LEDMatrix : public QWidget, private Ui::LEDMatrix
{
  Q_OBJECT

  int m_updTimer;
  uchar m_row;

  QRgb m_realBuff[48][96];

  QLEDMatrix matrix;
  LEDServer *server;

  bool updated;
  bool toggle;

  char x;

  QImage m_buff;
  QUdpSocket *m_sock;

  QByteArray send_buff[6];
  QFont myFont;
public:
  explicit LEDMatrix(QWidget *parent = 0);

  void paintEvent(QPaintEvent *);
  void timerEvent(QTimerEvent *ev);
};

#endif // LEDMATRIX_H
