#pragma once

#include <QColor>
#include <QTimer>
#include <QTcpServer>
#include <QObject>
#include <vector>
#include <pthread.h>

namespace LuvaLamp {

class Server : public QObject
{
	Q_OBJECT

public:
	explicit Server();
	virtual ~Server();

	void start(unsigned int port);
	void stop();

	void waitForStop();

signals:
	void requestStop();

private slots:
	void clientConnected();
	void clientDisconnected();
	void poll();
	void close();
	void keepAlive();

private:
	void addClient(QTcpSocket* client);
	void removeClient(QTcpSocket* client);
	void changeColor(QTcpSocket* client, QColor color);

private:
	QTcpServer* socket;
	bool started;
	bool stopRequest;
	bool stopped;
	QColor lampColor;
	pthread_mutex_t clientListMutex;
	QTimer* pollTimer;
	QTimer* keepAliveTimer;

	typedef std::vector<QTcpSocket*> ClientList;
	ClientList clients;
};

} // namespace LuvaLamp
