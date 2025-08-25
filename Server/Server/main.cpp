#include <enet/enet.h>
#include <iostream>

int main() {
	// Initialize the ENet library. Returns non-zero on failure.
	if (enet_initialize() != 0) {
		std::cerr << "Failed to initialize ENet.\n";
		return EXIT_FAILURE;
	}

	ENetAddress address;
	address.host = ENET_HOST_ANY; // Accept connections from any IP
	address.port = 1234; // Listen on port 1234

	// Create an ENet server host with up to 32 clients, 2 channels per connection, no bandwidth limit
	ENetHost* server = enet_host_create(&address, 32, 2, 0, 0);
	if (!server) {
		std::cerr << "Failed to create ENet server.\n";
		return EXIT_FAILURE;
	}

	std::cout << "Server ready on port 1234...\n";

	ENetEvent event;
	while (true) {
		// Wait for network events for up to 1000 milliseconds
		while (enet_host_service(server, &event, 1000) > 0) {
			switch (event.type) {
			case ENET_EVENT_TYPE_CONNECT:
				std::cout << "Client connected from "
					<< event.peer->address.host << ":" << event.peer->address.port << "\n";
				break;

			case ENET_EVENT_TYPE_DISCONNECT:
				std::cout << "Client disconnected.\n";
				break;

			default:
				break;
			}
		}
	}

	// Clean up the ENet server
	enet_host_destroy(server);
	enet_deinitialize();
	return EXIT_SUCCESS;
}