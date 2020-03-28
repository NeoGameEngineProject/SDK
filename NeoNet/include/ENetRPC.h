#include <enet/enet.h>
#include <functional>
#include <unordered_map>
#include <string>
#include <sstream>

#include <Property.h>
#include <NeoEngineFlatbuffer.h>

namespace Neo
{

typedef flatbuffers::Vector<flatbuffers::Offset<FlatBuffer::Property>> RPCParameters;

template<typename T>
T GetValue(const FlatBuffer::Property& prop)
{
	if constexpr (std::is_same<std::string, T>())
	{
		return std::string((const char*) prop.data()->data(), prop.size());
	}
	else
	{
		if(prop.size() != sizeof(T))
		{
			throw std::runtime_error("Invalid property size!");
		}

		return *reinterpret_cast<const T*>(prop.data()->data());
	}
}

template<typename T>
T GetValue(const FlatBuffer::Property* prop)
{
	return GetValue<T>(*prop);
}

///< bool(const std::stringstream& input, std::stringstream& output)
typedef std::function<bool(const RPCParameters&, std::stringstream&)> RPCDefinition;

class ENetRPC
{
public:
	void registerCall(const std::string& name, const RPCDefinition& def) { m_calls[name] = def; }
	bool call(const std::string& name, const RPCParameters& input, std::stringstream& output);

	bool operator()(const std::string& name, const RPCParameters& input, std::stringstream& output) { return call(name, input, output); }

	void send(ENetPeer* dest, void* buffer, size_t size);
	void broadcast(ENetHost* host, void* buffer, size_t size);

	template<typename... Args>
	flatbuffers::FlatBufferBuilder createPacket(const std::string& name, Args&&... args)
	{
		constexpr size_t ArgCount = sizeof...(args);
		flatbuffers::FlatBufferBuilder fbb;
		std::array<flatbuffers::Offset<FlatBuffer::Property>, ArgCount> params;

		constructFlatbuffer<ArgCount, 0>(name, fbb, params, args...);
		return fbb;
	}

	template<typename... Args>
	void send(ENetPeer* dest, const std::string& name, Args&&... args)
	{
		constexpr size_t ArgCount = sizeof...(args);
		flatbuffers::FlatBufferBuilder fbb;
		std::array<flatbuffers::Offset<FlatBuffer::Property>, ArgCount> params;

		constructFlatbuffer<ArgCount, 0>(name, fbb, params, args...);
		send(dest, fbb.GetBufferPointer(), fbb.GetSize());
	}

	template<typename... Args>
	void broadcast(ENetHost* host, const std::string& name, Args&&... args)
	{
		constexpr size_t ArgCount = sizeof...(args);
		flatbuffers::FlatBufferBuilder fbb;
		std::array<flatbuffers::Offset<FlatBuffer::Property>, ArgCount> params;

		constructFlatbuffer<ArgCount, 0>(name, fbb, params, args...);
		broadcast(host, fbb.GetBufferPointer(), fbb.GetSize());
	}

	void send(const std::string& name, const std::unique_ptr<IProperty>* parameters, size_t count);
	void send(const std::string& name, const std::vector<std::unique_ptr<IProperty>>& parameters)
	{
		send(name, parameters.data(), parameters.size());
	}

private:
	std::unordered_map<std::string, RPCDefinition> m_calls;

	template<int Size, int Idx, typename Head, typename... Args>
	void constructFlatbuffer(
		const std::string& name,
		flatbuffers::FlatBufferBuilder& fbb,
		std::array<flatbuffers::Offset<FlatBuffer::Property>, Size>& params,
		Head&& head,
		Args&&... tail)
	{
		size_t length = 0;
		flatbuffers::Offset<flatbuffers::Vector<uint8_t>> data;

		if constexpr(std::is_same<Head, std::string>())
		{
			length = head.size();
			data = fbb.CreateVector(head.c_str(), length);
		}
		else
		{
			length = sizeof(Head);
			data = fbb.CreateVector((uint8_t*) &head, length);
		}

		params[Idx] = FlatBuffer::CreateProperty(fbb, 0, (FlatBuffer::PROPERTY_TYPES) typeOf<Head>(), length, data);

		if constexpr (Idx == Size - 1)
		{
			auto paramList = fbb.CreateVector(params.data(), params.size());
			auto fnName = fbb.CreateString(name);

			FlatBuffer::RPCCallBuilder rpcCall(fbb);
			rpcCall.add_function(fnName);
			rpcCall.add_arguments(paramList);
			auto rpc = rpcCall.Finish();

			fbb.Finish(rpc);
		}
		else
		{
			constexpr int NewIdx = Idx + 1;
			constructFlatbuffer<Size, NewIdx>(name, fbb, params, tail...);
		}
	}
};

}
