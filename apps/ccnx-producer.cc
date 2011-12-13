/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2011 University of California, Los Angeles
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
 * Author: Ilya Moiseenko <iliamo@cs.ucla.edu>
 *         Alexander Afanasyev <alexander.afanasyev@ucla.edu>
 */

#include "ccnx-producer.h"
#include "ns3/log.h"
#include "ns3/ccnx-interest-header.h"
#include "ns3/ccnx-content-object-header.h"
#include "ns3/string.h"
#include "ns3/uinteger.h"
#include "ns3/packet.h"

#include "ns3/ccnx-local-face.h"
#include "ns3/ccnx-fib.h"

#include <boost/ref.hpp>

NS_LOG_COMPONENT_DEFINE ("CcnxProducer");

namespace ns3
{    

NS_OBJECT_ENSURE_REGISTERED (CcnxProducer);
    
TypeId
CcnxProducer::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::CcnxProducer")
    .SetParent<CcnxApp> ()
    .AddConstructor<CcnxProducer> ()
    .AddAttribute ("Prefix","Prefix, for which producer has the data",
                   StringValue ("/"),
                   MakeCcnxNameComponentsAccessor (&CcnxProducer::m_prefix),
                   MakeCcnxNameComponentsChecker ())
    .AddAttribute ("PayloadSize", "Virtual payload size for Content packets",
                   UintegerValue (1024),
                   MakeUintegerAccessor(&CcnxProducer::m_virtualPayloadSize),
                   MakeUintegerChecker<uint32_t>())
    // .AddTraceSource ("InterestTrace", "Interests that were received",
    //                 MakeTraceSourceAccessor (&CcnxProducer::m_interestsTrace))
    // .AddTraceSource ("ContentObjectTrace", "ContentObjects that were sent",
    //                 MakeTraceSourceAccessor (&CcnxProducer::m_contentObjectsTrace))
    ;
        
  return tid;
}
    
CcnxProducer::CcnxProducer ()
{
  // NS_LOG_FUNCTION_NOARGS ();
}

// inherited from Application base class.
void
CcnxProducer::StartApplication ()
{
  NS_LOG_FUNCTION_NOARGS ();
  NS_ASSERT (GetNode ()->GetObject<CcnxFib> () != 0);

  CcnxApp::StartApplication ();

  GetNode ()->GetObject<CcnxFib> ()->Add (m_prefix, m_face, 0);
}

void
CcnxProducer::StopApplication ()
{
  NS_LOG_FUNCTION_NOARGS ();
  NS_ASSERT (GetNode ()->GetObject<CcnxFib> () != 0);

  CcnxApp::StopApplication ();
}


void
CcnxProducer::OnInterest (const Ptr<const CcnxInterestHeader> &interest)
{
  NS_LOG_FUNCTION (this << interest);

  if (!m_active) return;
    
  static CcnxContentObjectTail tail;
  Ptr<CcnxContentObjectHeader> header = Create<CcnxContentObjectHeader> ();
  header->SetName (Create<CcnxNameComponents> (interest->GetName ()));

  NS_LOG_INFO ("Respodning with ContentObject:\n" << boost::cref(*header));
  
  Ptr<Packet> packet = Create<Packet> (m_virtualPayloadSize);
  packet->AddHeader (*header);
  packet->AddTrailer (tail);

  m_protocolHandler (packet);
}

} // namespace ns3