#include <Server.h>
#include <Log.h>

#include <ArrayStream.h>

using namespace Neo;

Server::Server()
{
	
}

Server::~Server()
{
	if(m_host)
	{
		enet_host_destroy(m_host);
	}
}

void Server::flush()
{
	enet_host_flush(m_host);
}

bool Server::serve(const char* address, unsigned int port, unsigned short clients, unsigned short channels)
{
	LOG_DEBUG("Serving at: " << (address ? address : ENET_HOST_ANY) << ":" << port);
	
	if(address)
		enet_address_set_host(&m_address, address);
	else
		m_address.host = ENET_HOST_ANY;

	m_address.port = port;
	m_host = enet_host_create(&m_address, clients, channels, 0, 0);

	if(!m_host)
	{
		LOG_ERROR("Could not create server host!");
		return false;
	}

	return true;
}


bool Server::connect(const char* host, unsigned int port)
{
	LOG_DEBUG("Connecting to: " << host << ":" << port);
	
	m_host = enet_host_create(nullptr, 1, 2, 0, 0);

	enet_address_set_host(&m_address, host);
	m_address.port = port;

	if(!m_host)
	{
		LOG_ERROR("Could not create client host!");
		return false;
	}

	auto* serverPeer = enet_host_connect(m_host, &m_address, 2, 0);
	
	if(!serverPeer)
	{
		LOG_ERROR("Could not connect to server!");
		return false;
	}

	m_connectedPeers.push_back(serverPeer);
	serverPeer->data = (void*) &m_connectedPeers.back();

	enet_peer_timeout(serverPeer, 0, 0, 5000);
	return true;
}


void Server::handleEvents(unsigned int msec)
{
	ENetEvent event;
	while (enet_host_service(m_host, &event, msec) > 0)
	{
		switch (event.type)
		{
		case ENET_EVENT_TYPE_CONNECT:
			LOG_INFO("A new client connected from "
					<< event.peer->address.host
					<< event.peer->address.port);

			m_connectedPeers.push_back(event.peer);
			event.peer->data = (void*) (&m_connectedPeers.back()); // References of an std::list will stay the same on push_back etc.!

			onConnect(event);
			break;
		
		case ENET_EVENT_TYPE_RECEIVE:
		{
			std::stringstream result;
			flatbuffers::Verifier verifier(event.packet->data, event.packet->dataLength);
			verifier.VerifyBuffer<FlatBuffer::RPCCall>();

			const auto* params = flatbuffers::GetRoot<FlatBuffer::RPCCall>(event.packet->data);
			call(params->function()->str(), *params->arguments(), result);

			enet_packet_destroy(event.packet);
		}
		break;
		
		case ENET_EVENT_TYPE_DISCONNECT:

			LOG_INFO((size_t) event.peer->data << " disconnected.");
			m_connectedPeers.erase(std::remove(m_connectedPeers.begin(), m_connectedPeers.end(), event.peer->data));

			onDisconnect(event);
			event.peer->data = nullptr;
		}
	}
}

void Server::sendFullSnapshot(const Level& l)
{

}
