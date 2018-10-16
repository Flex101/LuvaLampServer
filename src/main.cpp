#include <iostream>

#include "Server.h"
#include <QApplication>
#include <csignal>				// for SIGINT

using namespace std;

static LuvaLamp::Server* server;

static void signalHandler(int signum)
{
	switch (signum)
	{
		case SIGINT:
		case SIGTERM:
			server->stop();
			break;
	}
}

int main(int argc, char *argv[])
{
	QCoreApplication app(argc, argv);

	unsigned int port = 50005;
	if (argc > 1) port = atoi(argv[1]);

	signal(SIGINT, signalHandler);
	signal(SIGTERM, signalHandler);

	server = new LuvaLamp::Server();
	server->start(port);

	server->waitForStop();

	return 0;
}
