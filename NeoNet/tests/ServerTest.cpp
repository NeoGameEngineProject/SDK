#include <gtest/gtest.h>

#include <Server.h>

#include <thread>

using namespace Neo;

TEST(Server, Connect)
{
	Server client;
	Server server;

	server.serve("localhost", 8080, 32, 2);
	client.connect("localhost", 8080);

	for(int i = 0; i < 100; i++)
	{
		server.handleEvents(10);
		client.handleEvents(10);
	}
}

TEST(Server, RPC)
{
	Server client;
	Server server;

	client.registerCall("test", [](const RPCParameters& params, std::stringstream&) -> bool
	{
		std::cout << "STRING: " << GetValue<std::string>(params[3]) << std::endl;
		std::cout << "TEST WAS CALLED! YAY! " << GetValue<int>(params[0]) + GetValue<int>(params[1]) + GetValue<int>(params[2]) << std::endl;
	});

	server.serve("localhost", 8080, 32, 2);
	client.connect("localhost", 8080);

	for(int i = 0; i < 100; i++)
	{
		server.handleEvents(10);
		client.handleEvents(10);
	}

	auto* peer = server.getPeers().front();
	server.send(peer, "test", 1, 2, 3, "This is a string man!");
	server.flush();

	for(int i = 0; i < 100; i++)
	{
		server.handleEvents(10);
		client.handleEvents(10);
	}
}

