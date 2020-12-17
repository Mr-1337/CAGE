#include <SDL2/SDL_net.h>

namespace cage
{
	namespace networking
	{
		class Endpoint
		{
			IPaddress m_address;
			UDPpacket *m_sendPacket, *m_recvPacket;
			UDPsocket m_socket = nullptr;
			bool m_local;
		public:
			// Constructs a local endpoint listening on the given port
			Endpoint(unsigned short port);
			// Constructs a remote endpoint that we can send data to
			Endpoint(IPaddress address);
			~Endpoint();

			inline IPaddress GetIP() const { return m_address; }
			inline bool IsLocal() const { return m_local; }
			void Send(char* dataBuffer, size_t size, const Endpoint& destination);
			void Receive(void* dataBuffer, size_t& size);
		};
	}
}