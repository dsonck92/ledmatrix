#include "ledmatrix.h"

#include <QPainter>
#include <QTimerEvent>
#include <QFontDialog>
#include <QThread>
#include <QTime>
#include <math.h>

LEDMatrix::LEDMatrix(QWidget *parent) :
  QWidget(parent)
{
  setupUi(this);



  m_buff = QImage((uchar*)&m_realBuff,96,48,QImage::Format_RGB32);

  m_updTimer = startTimer(20);
  m_row = 0;

  m_sock = new QUdpSocket(this);

  m_sock->connectToHost("minimatrix",1337);
  m_sock->waitForConnected();

  QPainter p(&m_buff);

  p.setBrush(Qt::black);
  p.drawRect(0,0,96,48);

  x=0;

  updated = true;

  bool ok;
//  myFont = QFontDialog::getFont(&ok);

  myFont.setStyleStrategy(QFont::NoAntialias);
  myFont.setPixelSize(8);

  server = new LEDServer(&matrix,this);
}

void LEDMatrix::paintEvent(QPaintEvent *)
{
  QPainter p(this);
  p.scale(4,4);

  p.drawImage(0,0,*matrix.image());
//  p.setPen(Qt::blue);
//  p.drawRect(0,m_row*8,96,7);
}

void LEDMatrix::timerEvent(QTimerEvent *ev)
{
  if(m_updTimer == ev->timerId() && updated>0)
    {

      QByteArray buff;
      if(m_row > 5)
        {

          buff.append(0x01);

          qreal msecs = QTime::currentTime().msecsSinceStartOfDay();

          m_row = 0;

          updated = 1;

          QPainter p(matrix.paintDevice());
          p.setRenderHint(QPainter::Antialiasing);
          p.setBrush(QColor::fromRgbF(0,0,0,0.75));
          p.drawRect(0,0,96,48);

          QSize sz(32,32);
          QPoint center(sz.width()/2,sz.height()/2);

          p.setPen(QPen(Qt::white,0));
//          p.drawEllipse(QPointF(m_buff.height()/2,m_buff.height()/2),m_buff.height()/2,m_buff.height()/2);

          for(int I = 0;I<60;I++)
            {
              qreal f = 1-0.1*(I%5==0) - 0.1*(I%15==0);
              if(I%5==0)
                {
                  if(I%15 == 0)
                    p.setPen(QPen(Qt::white,0));
                    else
                    p.setPen(QPen(Qt::white,0.2));
                  p.drawLine(center+QPoint(sin(I/60.0*M_PI*2)*sz.width()/2,-cos(I/60.0*M_PI*2)*sz.height()/2),center+QPoint(sin(I/60.0*M_PI*2)*sz.width()/2,-cos(I/60.0*M_PI*2)*sz.height()/2)*f);
                }
            }

          msecs /= 1000.0;
          p.setPen(QPen(Qt::white,0.2));

          msecs /= 60.0;

          p.drawLine(center,center+QPoint(sin(msecs*M_PI*2)*sz.width()/2,-cos(msecs*M_PI*2)*sz.height()/2));
          msecs /= 60.0;
          p.setPen(QPen(Qt::white,0));
          p.drawLine(center,center+QPoint(sin(msecs*M_PI*2)*sz.width()/2*0.7,-cos(msecs*M_PI*2)*sz.height()/2*.7));
          msecs /= 12.0;
          p.drawLine(center,center+QPoint(sin(msecs*M_PI*2)*sz.width()/2*.5,-cos(msecs*M_PI*2)*sz.height()/2*.5));

          matrix.update();
          QThread::msleep(100);
        }
      else
        {
          do
            {
              buff.clear();
              buff.append(0x10);
              buff.append(m_row);
              for(int y=0;y<8;y++)
                for(int x=0;x<96;x++)
                  {
                    buff.append(qGray(m_realBuff[y+m_row*8][x]));
                  }
              m_row ++;
            }while(buff == send_buff[m_row-1] && m_row <= 5);
            send_buff[m_row-1] = buff;
        }

      m_sock->write(buff);

      update();
    }
}


