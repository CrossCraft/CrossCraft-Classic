#include "Client.h"

Client::Client(std::string ip, u16 port){
	Network::g_NetworkDriver.Init();
	conn = new Network::ClientSocket();
	connected = conn->Connect(port, ip.c_str());

	if (!connected) {
		throw std::runtime_error("Could not connect to " + ip + "at port " + std::to_string(port));
	}

	Network::g_NetworkDriver.setSocket(conn);
	conn->SetBlock(false);
}

Client::~Client()
{
	if (!connected) {
		conn->Close();
	}

	delete conn;
}

auto Client::update() -> void
{
	//Network Update
	for (auto i = 0; i < 50; i++) {
		Network::g_NetworkDriver.ReceivePacket();
	}

	Network::g_NetworkDriver.HandlePackets();
	Network::g_NetworkDriver.SendPackets();
}

auto Client::draw() -> void
{
}
