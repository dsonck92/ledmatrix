#include "qledmatrix.h"

#include <QTimerEvent>

#include <iostream>

QLEDMatrix::QLEDMatrix()
{
  m_buff = new QImage((uchar*)&m_realBuff,96,48,QImage::Format_RGB32);

  m_updateTimer = -1;
  m_row = 0;
  m_updates = 0;

  m_sock = new QUdpSocket(this);

  m_sock->connectToHost("minimatrix",1337);
  m_sock->waitForConnected();

  m_buff->fill(Qt::black);
}

QLEDMatrix::~QLEDMatrix()
{
  delete m_buff;
}

QPaintDevice *QLEDMatrix::paintDevice() const
{
  return m_buff;
}

void QLEDMatrix::timerEvent(QTimerEvent *ev)
{
  if(m_updateTimer == ev->timerId())
    {
      QByteArray buff;
      if(m_row > 5)
        {
          m_updates --;

          m_row = 0;
          if(m_updates<=0)
            {
            buff.append(0x01);
            killTimer(m_updateTimer);
            m_updateTimer = -1;
            m_updates = 0;
            std::cout << "LEDMATRIX: Send swap" << std::endl;
            }
          else
            return;
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
              m_row++;
            }while(buff == m_sendBuff[m_row-1] && m_row <= 5);
          std::cout << "LEDMATRIX: write line" << std::endl;
          m_sendBuff[m_row-1] = buff;
        }

      if(buff.size())
        m_sock->write(buff);
    }
}

void QLEDMatrix::update()
{
//  if(m_updates < 0)
    m_updates++;
  if(m_updateTimer < 0)
    {
      m_updateTimer = startTimer(20);
    }

  std::cout << "LEDMATRIX: updates: " << m_updates << std::endl;
}

