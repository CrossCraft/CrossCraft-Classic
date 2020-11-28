/*****************************************************************//**
 * \file   Client.h
 * \brief  The basic client of the game.
 *
 * \author Iridescence - Nathan Bourgeois <iridescentrosesfall@gmail.com>
 * \date   November 2020
 *********************************************************************/
#pragma once
#include <Network/NetworkDriver.h>
#include "Types.h"
#include <queue>

using namespace Stardust;

class Client {
	Network::ClientSocket* conn;
	bool connected;

public:
	
	/**
	 * Creates the Client object.
	 * 
	 * \param ip - IP to connect to
	 * \param port - Port required
	 */
	Client(std::string ip, u16 port = 'K' * 'O' + 'F');

	/**
	 * Destroys the client.
	 *
	 */
	~Client();

	inline auto isConnected() -> bool {
		return connected;
	}

	/**
	 * Updates the client state.
	 */
	auto update() -> void;

	/**
	 * Updates the screen state.
	 */
	auto draw() -> void;
};