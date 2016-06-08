#ifndef QLEDMATRIX_H
#define QLEDMATRIX_H

#include <QImage>
#include <QPaintDevice>
#include <QUdpSocket>



class QLEDMatrix : public QObject
{
  Q_OBJECT

  int m_updateTimer;
  uchar m_row;

  QRgb m_realBuff[48][96];

  int m_updates;

  QImage* m_buff;
  QUdpSocket *m_sock;

  QByteArray m_sendBuff[6];
public:
  QLEDMatrix();
  ~QLEDMatrix();

  QPaintDevice *paintDevice() const;

  void timerEvent(QTimerEvent *ev);

  QImage *image() { return m_buff; }

public slots:
  void update();
};

#endif // QLEDMATRIX_H
