/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
#ifndef UDPTEST_H_
#define UDPTEST_H_

#include "engine/service/DatagramServiceThread.h"

	class DatagramHandler: public ServiceHandler {
	public:
		DatagramHandler() {
		}

		void initialize() {
		}

		ServiceClient* createConnection(Socket* sock, SocketAddress& addr) {
			return nullptr;
		}

		bool deleteConnection(ServiceClient* session) {
			return false;
		}

		void handleMessage(ServiceClient* session, Packet* message) {
			printf("message received\n");
		}

		void processMessage(Message* message) {
		}

		bool handleError(ServiceClient* client, Exception& e) {
			return true;
		}
	};

class UDPTest {
	DatagramServiceThread* datagramService;

public:
	UDPTest() {
	}

	void run() {
		try {
			datagramService = new DatagramServiceThread("DatagramService");
			datagramService->setLogging(true);

			datagramService->setHandler(new DatagramHandler());

			datagramService->start(45555, 100);
			sleep(1);

			Packet packet;
			packet.insertInt(1);

			printf("sending packet\n");

			DatagramServiceClient client("127.0.0.1", 45555);

			for (int i = 0; i < 5; ++i) {
				int length = client.send(&packet);

				printf("send resulted in %i\n", length);
			}

			sleep(5);
		} catch (const Exception& e) {
			e.printStackTrace();
		}
	}
};

void udptest() {
	UDPTest test;
	test.run();
}


#endif /* LOGGERTEST_H_ */
