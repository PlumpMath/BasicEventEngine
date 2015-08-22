/*
* Copyright (c) 2015 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_UTIL_NETWORK_H
#define _BEE_UTIL_NETWORK_H 1

// Networking functions

#include <string>
#include <SDL2/SDL_net.h>

int network_init() {
        int r = SDLNet_Init();
        if (r) {
                std::cerr << "Error initializing network functionality: " << SDLNet_GetError() << "\n";
        }
        return r;
}
int network_quit() {
        SDLNet_Quit();
        return 0;
}

TCPsocket network_tcp_open(std::string ip, int port) {
        IPaddress ipa;
        TCPsocket tcp;

        if (ip.empty()) { // Host
                if (SDLNet_ResolveHost(&ipa, NULL, port)) {
                        std::cerr << "Failed to resolve host: " << SDLNet_GetError() << "\n";
                        return NULL;
                }

                tcp = SDLNet_TCP_Open(&ipa);
                if (!tcp) {
                        std::cerr << "Failed to host TCP on port " << port << ": " << SDLNet_GetError();
                        return NULL;
                }
        } else { // Client
                if (SDLNet_ResolveHost(&ipa, ip.c_str(), port)) {
                        std::cerr << "Failed to resolve host: " << SDLNet_GetError() << "\n";
                        return NULL;
                }

                tcp = SDLNet_TCP_Open(&ipa);
                if (!tcp) {
                        std::cerr << "Failed to connect via TCP to \"" << ip << ":" << port << "\": " << SDLNet_GetError();
                        return NULL;
                }
        }

        return tcp;
}
int network_tcp_close(TCPsocket tcp) {
        SDLNet_TCP_Close(tcp);
        tcp = NULL;
        return 0;
}
TCPsocket network_tcp_accept(TCPsocket listen) {
        return SDLNet_TCP_Accept(listen);
}
IPaddress* network_get_peer_address(TCPsocket tcp) {
        return SDLNet_TCP_GetPeerAddress(tcp);
}
int network_tcp_send(TCPsocket tcp, const void* data, int len) {
        int r = SDLNet_TCP_Send(tcp, data, len);
        if (r < len) {
                std::cerr << "Failed to send full data over TCP: " << SDLNet_GetError() << "\n";
        }
        return r;
}
int network_tcp_send(TCPsocket tcp, std::string str) {
        return network_tcp_send(tcp, str.c_str(), str.length()+1);
}
int network_tcp_recv(TCPsocket tcp, void* data, int maxlen) {
        int r = SDLNet_TCP_Recv(tcp, data, maxlen);
        if (r <= 0) {
                std::cerr << "Failed to recieve data over TCP: " << SDLNet_GetError() << "\n";
        }
        return r;
}
std::string network_tcp_recv(TCPsocket tcp, int maxlen) {
        char data[maxlen];
        network_tcp_recv(tcp, data, maxlen);
        return std::string(data);
}

UDPsocket network_udp_open(int port) {
        UDPsocket udp;
        udp = SDLNet_UDP_Open(port);
        if (!udp) {
                std::cerr << "Failed to open UDP on port " << port << ": " << SDLNet_GetError() << "\n";
                return NULL;
        }
        return udp;
}
int network_udp_close(UDPsocket udp) {
        SDLNet_UDP_Close(udp);
        udp = NULL;
        return 0;
}
int network_udp_bind(UDPsocket udp, int channel, IPaddress* ipa) {
        int c = SDLNet_UDP_Bind(udp, channel, ipa);
        if (c == -1) {
                std::cerr << "Failed to bind UDP on channel " << channel << ": " << SDLNet_GetError() << "\n";
        }
        return c;
}
int network_udp_bind(UDPsocket udp, int channel, std::string ip, int port) {
        IPaddress ipa;
        if (SDLNet_ResolveHost(&ipa, ip.c_str(), port)) {
                std::cerr << "Failed to resolve host: " << SDLNet_GetError() << "\n";
                return -1;
        }

        return network_udp_bind(udp, channel, &ipa);
}
int network_udp_unbind(UDPsocket udp, int channel) {
        SDLNet_UDP_Unbind(udp, channel);
        return 0;
}
IPaddress* network_get_peer_address(UDPsocket udp, int channel) {
        IPaddress* ipa;
        ipa = SDLNet_UDP_GetPeerAddress(udp, channel);
        if (!ipa) {
                std::cerr << "Failed to get UDP peer address: " << SDLNet_GetError() << "\n";
                return NULL;
        }
        return ipa;
}
int network_udp_send(UDPsocket udp, int channel, UDPpacket* packet) {
        int sent = 0;
        sent = SDLNet_UDP_Send(udp, channel, packet);
        if (sent == 0) {
                std::cerr << "Failed to send packet over UDP: " << SDLNet_GetError() << "\n";
        }
        return sent;
}
int network_udp_recv(UDPsocket udp, UDPpacket* packet) {
        int recv = 0;
        recv = SDLNet_UDP_Recv(udp, packet);
        if (recv == -1) {
                std::cerr << "Failed to recieve data over UDP: " << SDLNet_GetError() << "\n";
        }
        return recv;
}
int network_udp_sendv(UDPsocket udp, UDPpacket** packets, int amount) {
        int sent = 0;
        sent = SDLNet_UDP_SendV(udp, packets, amount);
        if (sent == 0) {
                std::cerr << "Failed to send vector data over UDP: " << SDLNet_GetError() << "\n";
        }
        return sent;
}
int network_udp_recvv(UDPsocket udp, UDPpacket** packets) {
        int recv = 0;
        recv = SDLNet_UDP_RecvV(udp, packets);
        if (recv == -1) {
                std::cerr << "Failed to recieve vector data over UDP: " << SDLNet_GetError() << "\n";
        }
        return recv;
}

UDPpacket* network_packet_alloc(int size) {
        UDPpacket* packet;
        packet = SDLNet_AllocPacket(size);
        if (!packet) {
                std::cerr << "Failed to allocate UDP packet: " << SDLNet_GetError() << "\n";
                return NULL;
        }
        return packet;
}
UDPpacket* network_packet_resize(UDPpacket* packet, int size) {
        int newsize = SDLNet_ResizePacket(packet, size);
        if (newsize < size) {
                std::cerr << "Failed to resize UDP packet: " << SDLNet_GetError() << "\n";
                return NULL;
        }
        return packet;
}
int network_packet_free(UDPpacket* packet) {
        SDLNet_FreePacket(packet);
        packet = NULL;
        return 0;
}
UDPpacket** network_packet_allocv(int amount, int size) {
        UDPpacket** packets;
        packets = SDLNet_AllocPacketV(amount, size);
        if (!packets) {
                std::cerr << "Failed to allocate UDP vector packets: " << SDLNet_GetError() << "\n";
                return NULL;
        }
        return packets;
}
int network_packet_freev(UDPpacket** packets) {
        SDLNet_FreePacketV(packets);
        packets = NULL;
        return 0;
}

#endif // _BEE_UTIL_NETWORK_H
