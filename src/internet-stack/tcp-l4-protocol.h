/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2007 Georgia Tech Research Corporation
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Raj Bhattacharjea <raj.b@gatech.edu>
 */

#ifndef TCP_L4_PROTOCOL_H
#define TCP_L4_PROTOCOL_H

#include <stdint.h>

#include "ns3/packet.h"
#include "ns3/ipv4-address.h"
#include "ns3/ptr.h"
#include "ns3/object-factory.h"
#include "ipv4-end-point-demux.h"
#include "ipv4-l4-protocol.h"
#include "ipv4-interface.h"

#include "tcp-header.h"
#include "tcp-typedefs.h"

namespace ns3 {

class Node;
class Socket;
class TcpHeader;

/**
 * \brief A layer between the sockets interface and IP
 * 
 * This class allocates "endpoint" objects (ns3::Ipv4EndPoint) for TCP,
 * and SHOULD checksum packets its receives from the socket layer going down
 * the stack , but currently checksumming is disabled.  It also recieves 
 * packets from IP, and forwards them up to the endpoints.
*/

class TcpL4Protocol : public Ipv4L4Protocol {
public:
  static TypeId GetTypeId (void);
  static const uint8_t PROT_NUMBER;
  /**
   * \brief Constructor
   */
  TcpL4Protocol ();
  virtual ~TcpL4Protocol ();

  void SetNode (Ptr<Node> node);

  virtual int GetProtocolNumber (void) const;
  virtual int GetVersion (void) const;

  /**
   * \return A smart Socket pointer to a TcpSocketImpl, allocated by this instance
   * of the TCP protocol
   */
  Ptr<Socket> CreateSocket (void);

  Ipv4EndPoint *Allocate (void);
  Ipv4EndPoint *Allocate (Ipv4Address address);
  Ipv4EndPoint *Allocate (uint16_t port);
  Ipv4EndPoint *Allocate (Ipv4Address address, uint16_t port);
  Ipv4EndPoint *Allocate (Ipv4Address localAddress, uint16_t localPort,
                          Ipv4Address peerAddress, uint16_t peerPort);

  void DeAllocate (Ipv4EndPoint *endPoint);

//   // called by TcpSocketImpl.
//   bool Connect (const Ipv4Address& saddr, const Ipv4Address& daddr,
//                 uint16_t sport, uint16_t dport);

  /**
   * \brief Send a packet via TCP
   * \param packet The packet to send
   * \param saddr The source Ipv4Address
   * \param daddr The destination Ipv4Address
   * \param sport The source port number
   * \param dport The destination port number
   */
  void Send (Ptr<Packet> packet,
             Ipv4Address saddr, Ipv4Address daddr, 
             uint16_t sport, uint16_t dport);
  /**
   * \brief Recieve a packet up the protocol stack
   * \param p The Packet to dump the contents into
   * \param source The source's Ipv4Address
   * \param destination The destinations Ipv4Address
   * \param incomingInterface The Ipv4Interface it was received on
   */
  virtual void Receive (Ptr<Packet> p,
                       Ipv4Address const &source,
                       Ipv4Address const &destination,
                       Ptr<Ipv4Interface> incomingInterface);

protected:
  virtual void DoDispose (void);
private:
  Ptr<Node> m_node;
  Ipv4EndPointDemux *m_endPoints;
  ObjectFactory m_rttFactory;
private:
  friend class TcpSocketImpl;
  void SendPacket (Ptr<Packet>, TcpHeader,
                  Ipv4Address, Ipv4Address);
  static ObjectFactory GetDefaultRttEstimatorFactory (void);
};

}; // namespace ns3

#endif /* TCP_L4_PROTOCOL_H */