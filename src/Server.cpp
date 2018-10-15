#include "Server.h"

#include <QTcpSocket>
#include <QApplication>
#include <unistd.h>
#include <iostream>

using namespace LuvaLamp;

Server::Server()
{
	socket = new QTcpServer();
	started = false;
	stopRequest = false;
	stopped = false;
	lampColor = Qt::black;		// OFF

	pollTimer = new QTimer();
	pollTimer->start(100);

	keepAliveTimer = new QTimer();
	keepAliveTimer->start(5000);

	connect(socket, &QTcpServer::newConnection, this, &Server::clientConnected);
	connect(pollTimer, &QTimer::timeout, this, &Server::poll);
	connect(keepAliveTimer, &QTimer::timeout, this, &Server::keepAlive);
	connect(this, &Server::requestStop, this, &Server::close);
}

Server::~Server()
{
	delete socket;
}

void Server::start(unsigned int port)
{
	bool success = socket->listen(QHostAddress("192.168.0.2"), port);

	if (success)
	{
		std::cout << "Server started. Port: " << port << std::endl;
		started = true;
	}
	else
	{
		std::cout << "Server failed to start - " << socket->errorString().toStdString().c_str() << std::endl;
	}
}

void Server::stop()
{
	std::cout << "Server requested to stop..." << std::endl;
	emit requestStop();
	stopRequest = true;
}

void Server::waitForStop()
{
	while (started && !stopped)
	{
		usleep(100000);				// 100ms
		qApp->processEvents();
	}
}

void Server::clientConnected()
{
	QTcpSocket* client = socket->nextPendingConnection();
	addClient(client);

	connect (client, &QTcpSocket::disconnected, this, &Server::clientDisconnected);

	std::cout << "Client connected - " << client->localAddress().toString().toStdString().c_str() << std::endl;
}

void Server::clientDisconnected()
{
	QTcpSocket* client = dynamic_cast<QTcpSocket*>(sender());
	removeClient(client);

	std::cout << "Client disconnected - " << client->localAddress().toString().toStdString().c_str() << std::endl;
}

void Server::poll()
{
	pthread_mutex_lock(&clientListMutex);

	for (ClientList::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		QTcpSocket* client = *it;
		QByteArray bytes = client->readAll();

		if (bytes.size() > 0)
		{
			if (bytes.size() == 5)
			{
				changeColor(client, QColor((unsigned char)bytes[0], (unsigned char)bytes[1], (unsigned char)bytes[2]));
			}
			else
			{
				std::string msg(bytes.toHex().constData(), bytes.toHex().length());
				std::cout << "Irregular message - " << msg.c_str() << std::endl;
			}
		}
	}

	pthread_mutex_unlock(&clientListMutex);
}

void Server::close()
{
	socket->close();
	stopped = true;
	std::cout << "Server stopped." << std::endl;
}

void Server::keepAlive()
{
	if (clients.size() == 0) return;

	QByteArray msg;
	msg.append((char)0);
	msg.append((char)0);
	msg.append((char)0);
	msg.append('\r');
	msg.append('\n');

	std::cout << "Sending keep alive msg..." << std::endl;

	pthread_mutex_lock(&clientListMutex);

	for (ClientList::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		(*it)->write(msg);
	}

	pthread_mutex_unlock(&clientListMutex);
}

void Server::addClient(QTcpSocket* client)
{
	pthread_mutex_lock(&clientListMutex);
	clients.push_back(client);
	pthread_mutex_unlock(&clientListMutex);
}

void Server::removeClient(QTcpSocket* client)
{
	pthread_mutex_lock(&clientListMutex);
	for (ClientList::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		if (*it == client)
		{
			clients.erase(it);
			break;
		}
	}
	pthread_mutex_unlock(&clientListMutex);
}

void Server::changeColor(QTcpSocket* client, QColor color)
{
	lampColor = color;

	QByteArray msg;
	msg.append(color.red());
	msg.append(color.green());
	msg.append(color.blue());
	msg.append('\r');
	msg.append('\n');

	for (ClientList::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		if (*it != client)
		{
			(*it)->write(msg);
		}
	}

	std::cout << "New color - " << color.red() << " " << color.green() << " " << color.blue() << std::endl;
}
