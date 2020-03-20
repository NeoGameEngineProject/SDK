#include <ENetRPC.h>
#include <Log.h>

#include <enet/enet.h>

using namespace Neo;

bool ENetRPC::call(const std::string& name, const RPCParameters& input, std::stringstream& output)
{
	const auto cb = m_calls.find(name);
	if(cb == m_calls.end())
	{
		LOG_ERROR("Invalid RPC call: " << name);
		return false;
	}

	return cb->second(input, output);
}

void ENetRPC::send(const std::string& name, const std::unique_ptr<IProperty>* parameters, size_t count)
{

}

void ENetRPC::send(ENetPeer* dest, void* buffer, size_t size)
{
	ENetPacket* packet = enet_packet_create(buffer, size, ENET_PACKET_FLAG_RELIABLE);
	enet_peer_send(dest, 0, packet);
}

void ENetRPC::broadcast(ENetHost* host, void* buffer, size_t size)
{
	ENetPacket* packet = enet_packet_create(buffer, size, ENET_PACKET_FLAG_RELIABLE);
	enet_host_broadcast(host, 0, packet);
}

