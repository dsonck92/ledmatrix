#ifndef LEDSERVER_H
#define LEDSERVER_H

#include "qledmatrix.h"

#include <QImage>
#include <QObject>
#include <QUdpSocket>

typedef quint64 Source;

struct ClientData {
  Source src;
  int prio,z;
  QImage buff;
};

class LEDServer : public QObject
{
  Q_OBJECT

  QUdpSocket *m_server;

  QList<ClientData*> m_buffers;
  static Source toSource(QHostAddress addr, quint16 port);

  ClientData *findClient(Source src);
  ClientData *createClient(Source src);

  QLEDMatrix *m_matrix;

  int m_switchTimer;
  int m_client;

  void render();

public:
  explicit LEDServer(QLEDMatrix *matrix,QObject *parent = 0);

  void timerEvent(QTimerEvent *ev);

signals:

public slots:
  void readDatagrams();
  void socketError(QAbstractSocket::SocketError err);
};

#endif // LEDSERVER_H
