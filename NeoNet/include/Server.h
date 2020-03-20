#ifndef __SERVER_H__
#define __SERVER_H__

#include <list>

#include <enet/enet.h>
#include <Level.h>

#include <ENetRPC.h>

namespace Neo
{
class Server : public ENetRPC
{
public:
	Server(const Server&) = delete;
	Server();
	~Server();

	void flush();
	bool serve(const char* address, unsigned int port, unsigned short clients, unsigned short channels);
	bool connect(const char* host, unsigned int port);

	void handleEvents(unsigned int msec = 1000);
	void sendFullSnapshot(const Level& l);

	virtual void onConnect();
	virtual void onDisconnect();
	virtual void onUpdate();

	std::list<ENetPeer*>& getPeers() { return m_connectedPeers; }

	template<typename... Args>
	void send(const std::string& name, Args&&... args)
	{
		return ENetRPC::send(m_connectedPeers.front(), name, args...);
	}

	template<typename... Args>
	void broadcast(const std::string& name, Args&&... args)
	{
		return ENetRPC::broadcast(m_host, name, args...);
	}

	template<typename... Args>
	void sendAll(const std::string& name, Args&&... args)
	{
		auto fbb = ENetRPC::createPacket(name, args...);

		for(auto& p : m_connectedPeers)
		{
			send(p, fbb.GetBufferPointer(), fbb.GetSize());
		}
	}

	using ENetRPC::send;

private:
	ENetAddress m_address;
	ENetHost* m_host = nullptr;

	// Use list so all references and iterators stay valid
	// when a user disconnects
	// https://en.cppreference.com/w/cpp/container/list
	std::list<ENetPeer*> m_connectedPeers;
};
}

#endif
