#include "ledserver.h"

#include <iostream>

#include <QPainter>
#include <QTimerEvent>

Source LEDServer::toSource(QHostAddress addr, quint16 port)
{
  Source src = addr.toIPv4Address();
  src <<= 16;
  src |= port;

  std::cout << "Source: " << src << std::endl;
  return src;
}

ClientData *LEDServer::findClient(Source src)
{
  for(ClientData * d : m_buffers)
    {
      if(d->src == src)
        return d;
    }

  return createClient(src);
}

ClientData *LEDServer::createClient(Source src)
{
  ClientData * d = new ClientData;

  d->buff = QImage(QSize(96,48),QImage::Format_RGB32);
  d->buff.fill(0);
  d->src = src;
  d->prio = 10;
  d->z = 10;

  m_buffers << d;

  std::cout << this << " : Buffer created, count: " << m_buffers.size() << std::endl;

  return d;
}

void LEDServer::render()
{
  QPainter p(m_matrix->paintDevice());
  for(ClientData * d : m_buffers)
    {
      p.setOpacity(qBound(0.0,d->z/4.0,1.0));
      p.drawImage(0,0,d->buff);

    }
  m_matrix->update();
  /*
  if(m_client < m_buffers.size())
    {
      QPainter p(m_matrix->paintDevice());

      p.drawImage(0,0,m_buffers[m_client]->buff);

      m_matrix->update();
    }*/
}

LEDServer::LEDServer(QLEDMatrix *matrix, QObject *parent) : QObject(parent)
{
  m_server = new QUdpSocket(this);
  connect(m_server,SIGNAL(readyRead()),this,SLOT(readDatagrams()));
  connect(m_server,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(socketError(QAbstractSocket::SocketError)));

  m_server->bind(QHostAddress::Any,1337);

  m_matrix = matrix;

  m_switchTimer = startTimer(500);

  m_client = 0;
}

void LEDServer::timerEvent(QTimerEvent *ev)
{
  if(m_server->hasPendingDatagrams())
    readDatagrams();
  if(m_switchTimer == ev->timerId())
    {
      QMutableListIterator<ClientData*> i(m_buffers);

      ClientData *a, *b;
      while(i.hasNext())
        {
          i.next();
          a = i.value();

          if(a->z < -2)
            {
              i.remove();
              delete a;
            }
          else
            {

              if(i.hasNext())
                {
                  i.next();
                  b = i.value();

                  if(a->z < b->z)
                    {
                      i.previous();

                      i.value() = a;

                      i.previous();

                      i.value() = b;

                      //if(i.hasPrevious())
                      //  i.previous();
                    }
                }

            }
        }
      m_client ++;
      if(m_client >= m_buffers.size())
        m_client = 0;

      int I = m_buffers.size()-1;
      std::cout << this << "Timer: client: " << m_client << std::endl;
      for(ClientData * d : m_buffers)
        {
          if(I<=0)
            d->z --;
          I--;
        }
      render();
    }
}

void LEDServer::readDatagrams()
{
  while(m_server->hasPendingDatagrams())
    {
      //      std::cout << this << " : Data received!" << std::endl;
      QByteArray datagram;
      datagram.resize(m_server->pendingDatagramSize());
      QHostAddress sender;
      quint16 senderPort;
      m_server->readDatagram(datagram.data(),datagram.size(),&sender,&senderPort);

      uchar h = datagram.at(0);
      datagram.remove(0,1);

      ClientData *src = findClient(toSource(sender,senderPort));

      switch(h)
        {
        case 0x01:
          //          std::cout << this << " : Swap buffer" << std::endl;
          src->z = src->prio;
          render();
          break;
        case 0x02:
          src->buff.fill(0);
          break;
        case 0x03:
          if(datagram.size() >= 2)
            src->prio = (*(quint16*)datagram.data());
          break;
        case 0x10:
          {
            //            std::cout << this << " : Write bar" << std::endl;
            uchar row = datagram.at(0);
            //            std::cout << "R: " << (int)row << std::endl;
            datagram.remove(0,1);

            for(int y=0;y<8;y++)
              {
                for(int x=0;x<96;x++)
                  {
                    uchar c = datagram.at(0);
                    //                  std::cout << "(" << (int)c << ")";
                    datagram.remove(0,1);

                    QPainter p(&src->buff);

                    p.setPen(QColor(c,c,c));
                    p.drawPoint(x,y+row*8);
                  }
                //              std::cout << std::endl;
              }
          }

          break;
        case 0x11:
          //          std::cout << this << " : Write area" << std::endl;
          break;
        case 0x20:
          {
            uchar x = datagram.at(0);
            uchar y = datagram.at(1);
            uchar c = datagram.at(2);

            datagram.remove(0,3);

            QString s = QString::fromUtf8(datagram);

            QPoint pt(x*6,y*8);

            QFont f("monospace");
            f.setPixelSize(9);
            f.setFixedPitch(1);
            f.setStyleStrategy(QFont::PreferBitmap);
            f.setStyleStrategy(QFont::NoAntialias);

            char t;

            QPainter p(&src->buff);
            p.setFont(f);
            p.setBrush(Qt::black);
            p.setPen(QColor(c,c,c));


            while(s.length())
              {
                QRectF r(pt,QSize(6,8));
                QRectF r2(pt,QSize(6,7));

                p.setPen(Qt::black);
                p.drawRect(r2);
                p.setPen(QColor(c,c,c));
                p.drawText(r,Qt::AlignCenter,s.mid(0,1).toUpper());

                s = s.mid(1);
                pt += QPoint(6,0);
                datagram.remove(0,1);
              }
          }
        default:
          //          std::cout << this << " : Unhandled: " << (int)h << std::endl;

          break;
        }
    }
}

void LEDServer::socketError(QAbstractSocket::SocketError err)
{
  std::cout << this << " : Error! " << (int)err << " - " << m_server->errorString().toStdString() << std::endl;
}

