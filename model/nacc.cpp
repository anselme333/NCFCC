/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2011-2015  Regents of the University of California.
 *
 * This file is part of ndnSIM. See AUTHORS for complete list of ndnSIM authors and
 * contributors.
 *
 * ndnSIM is free software: you can redistribute it and/or modify it under the terms
 * of the GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 *
 * ndnSIM is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * ndnSIM, e.g., in COPYING.md file.  If not, see <http://www.gnu.org/licenses/>.
 **/
// nacc.cpp
#include "ns3/ptr.h"
#include "ns3/log.h"
#include <iostream>
#include "ns3/queue.h"
#include "ns3/nstime.h"
#include "ns3/uinteger.h"
#include "ns3/data-rate.h"
#include "ns3/simulator.h"
#include "ndn-ns3.hpp"
#include "ns3/channel.h"
#include "ns3/net-device.h"
#include "ns3/packet.h"
#include "ns3/node.h"
#include "ns3/pointer.h"
#include "ns3/ndnSIM/model/nacc.hpp"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/ndnSIM-module.h"
#include "build/ns3/uinteger.h"
#include "ns3/drop-tail-queue.h"
#include "ns3/point-to-point-module.h"
#include "ns3/ndnSIM/model/ndn-net-device-face.hpp"
#include "ns3/ndnSIM/utils/ndn-rtt-estimator.hpp"
#include "../src/network/utils/simple-net-device.h"
#include "ns3/ndnSIM/helper/ndn-stack-helper.hpp"
#include "ns3/ndnSIM/helper/ndn-fib-helper.hpp"
#include "ndn-net-device-face.hpp"
#include "ns3/ndnSIM/model/ndn-face.hpp"
#include "ndn-l3-protocol.hpp"
#include "ns3/point-to-point-net-device.h"
#include "../utils/ndn-fw-hop-count-tag.hpp"
#include "core/notification-stream.hpp"

NS_LOG_COMPONENT_DEFINE("ndn.NaccMecIcn");
namespace ns3 {
namespace ndn {
// register NS-3 type
TypeId NaccMecIcn::GetTypeId()
{
  static TypeId tid = TypeId("NaccMecIcn")
		  .SetParent<ndn::NaccMecIcn>()
		  .SetGroupName ("Ndn")
          .AddAttribute ("MaxBuffer",
      					   "Maximum buffer.",
      					   UintegerValue (100),
      					   MakeUintegerAccessor (&NaccMecIcn::m_buffer),
      					   MakeUintegerChecker<uint32_t> ())
      	  .AddAttribute ("BufferThreshold",
      					  "Buffer Threshold.",
      					   UintegerValue (50),
      					   MakeUintegerAccessor (&NaccMecIcn::m_bufferthreshold),
      					   MakeUintegerChecker<uint32_t> ());
  return tid;
}
NaccMecIcn::NaccMecIcn(Ptr<Node> node, const Ptr<NetDevice>& netDevice)
: NetDeviceFace (node, netDevice)
, m_isLeakScheduled (false)
    , m_tokenMax (0)
    , m_tokenLeak (1)
    , m_token (0)
{
	DataRateValue dataRate;
	netDevice->GetAttribute ("DataRate", dataRate);
	m_shapingRate = dataRate.Get().GetBitRate(); // assume symmetric bandwidth
}
NaccMecIcn::~NaccMecIcn()
{
  NS_LOG_FUNCTION_NOARGS();
  close();
}
void NaccMecIcn::close()
{
  m_node->UnregisterProtocolHandler(MakeCallback(&NaccMecIcn::receiveFromNetDevice, this));
  this->fail("Close connection");
}

Ptr<NetDevice> NaccMecIcn::GetNetDevice() const
{
  return m_netDevice;
}
void NaccMecIcn::send(Ptr<Packet> packet)
{
	NS_ASSERT_MSG(packet->GetSize() <= m_netDevice->GetMtu(),
	                "Packet size " << packet->GetSize() << " exceeds device MTU "
	                               << m_netDevice->GetMtu());

	  FwHopCountTag tag;
	  packet->RemovePacketTag(tag);
	  tag.Increment();
	  packet->AddPacketTag(tag);
	  m_netDevice->Send(packet, m_netDevice->GetBroadcast(), L3Protocol::ETHERNET_FRAME_TYPE);
}
void NaccMecIcn::sendInterest(const Interest& interest)
{
	  NS_LOG_FUNCTION(this << &interest);
	  this->emitSignal(onSendInterest, interest);
	  Ptr<Packet> packet = Convert::ToPacket(interest);
	  send(packet);
}
void NaccMecIcn::sendData(const Data& data)
{
	NS_LOG_FUNCTION(this << &data);
	this->emitSignal(onSendData, data);
	Ptr<Packet> packet = Convert::ToPacket(data);
	send(packet);
}

// callback
void
NaccMecIcn::receiveFromNetDevice(Ptr<NetDevice> device, Ptr<const Packet> p, uint16_t protocol,
                                    const Address& from, const Address& to,
                                    NetDevice::PacketType packetType)
{
  NS_LOG_FUNCTION(device << p << protocol << from << to << packetType);

  Ptr<Packet> packet = p->Copy();
  try {
    uint32_t type = Convert::getPacketType(p);
    if (type == ::ndn::tlv::Interest) {
      shared_ptr<const Interest> i = Convert::FromPacket<Interest>(packet);
      this->emitSignal(onReceiveInterest, *i);
    }
    else if (type == ::ndn::tlv::Data) {
      shared_ptr<const Data> d = Convert::FromPacket<Data>(packet);
      this->emitSignal(onReceiveData, *d);
    }
    else {
      NS_LOG_ERROR("Unsupported TLV packet");
    }
  }
  catch (::ndn::tlv::Error&) {
    NS_LOG_ERROR("Unrecognized TLV packet");
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

double m_shapingRate;
double m_Nflows=1.0;
uint32_t QueueLength;
///////////////////////////////////////////////////////////////////////
//Memory-efficient Token Bucket created by anselme

void NaccMecIcn::SetLimits (double rate, double delay)
{
 double interval=1.0;
  // maximum allowed burst
  m_tokenMax = rate1->GetBitRate () * RTTmax->m_maxRto.ToDouble (Time::S);
  // amount of packets allowed every second
  m_tokenLeak = rate1->GetBitRate ();
  //double tokenOld = m_token;
  const double leak = m_tokenLeak * interval;
  m_token = std::max (0.0, m_token - leak);
  if (m_tokenMax - m_token >= 1.0)
  m_token += 1;
}
uint64_t NaccMecIcn::getBitrateNacc(shared_ptr<NetDeviceFace> face)
{

	if(ns3::ndn::NetDeviceFace *netf = dynamic_cast<ns3::ndn::NetDeviceFace*>(&(*face)))
  {
    ns3::Ptr<ns3::PointToPointNetDevice> nd1 = netf->GetNetDevice()->GetNode ()->GetDevice(netf->getId ())->GetObject<ns3::PointToPointNetDevice>();
    ns3::DataRateValue ratevalue;
    nd1->GetAttribute("DataRate", ratevalue);
    uint64_t dRate = ratevalue.Get().GetBitRate();
    return dRate;

  }
	else
	{
		    ns3::Ptr<ns3::PointToPointNetDevice> nd1 = netf->GetNetDevice()->GetNode ()->GetDevice(netf->getId ())->GetObject<ns3::PointToPointNetDevice>();
		    ns3::DataRateValue ratevalue;
		    nd1->GetAttribute("DataRate", ratevalue);
		    ns3::DataRate capacity = ratevalue.Get();
		    return capacity.GetBitRate();
	}
  return ULONG_MAX;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

uint32_t NaccMecIcn::GetIntQueueSizePerFlow(ndn::Name prefix)
 {
 	std::map<ndn::Name, uint32_t>::iterator
 	mit(m_nIntQueueSizePerFlow.find(prefix)),
 	mend(m_nIntQueueSizePerFlow.end());
    m_Nflows= double(m_nIntQueueSizePerFlow.size());
 	if(mit!=mend)
 	{
 		uint32_t & queue_size = mit->second;
 		uint32_t ql = queue_size;
 		if(ql + 1 <= m_buffer)
 		queue_size++;
 		return ql;
 	}
 return 0;
 }
 void NaccMecIcn::TrafficShaping ()
 {
	 double buf_interface;
     Time     m_avgRtt;
     std::map <ndn::Name, uint32_t> m_nQueueSizePerFlow;
	 shared_ptr<NetDeviceFace> face;
	 ns3::ndn::NetDeviceFace *netf = dynamic_cast<ns3::ndn::NetDeviceFace*>(&(*face));
	 ns3::Ptr<ns3::PointToPointNetDevice>devicep2p= netf->GetNetDevice()->GetNode ()->GetDevice(netf->getId ())->GetObject<ns3::PointToPointNetDevice>();
	 ns3::DataRateValue ratevalue;
     devicep2p->GetAttribute("DataRate", ratevalue);
     TimeValue linkDelay;
     devicep2p->GetAttribute ("Delay", linkDelay);
     devicep2p->SetAttribute("m_buffer", UintegerValue (m_buffer));
     devicep2p->SetAttribute("m_bufferthreshold", UintegerValue(m_bufferthreshold));
     devicep2p->SetAttribute("NaccIcnActive", BooleanValue(m_NaccIcn));
     NS_LOG_INFO("DataRate for this link is " << ratevalue.Get());
     double cab = ratevalue.Get ().GetBitRate ();
     buf_interface = (double(cab)) * m_avgRtt.ToDouble (Time::S);
     nfd::NetworkLayerCounters  count;
     double OutInterests = count.getNOutInterests();// Counting Interest;
     double InInterests=count.getNInInterests();
     double reducerate=InInterests/(InInterests+OutInterests);
     double  maxFairRates = (cab /m_Nflows )+ ((buf_interface - m_QueueLength.size()))/ (m_Nflows * m_avgRtt.ToDouble (Time::S));
     NS_LOG_INFO ("Max packets per second: " <<  maxFairRates);
     NS_LOG_INFO ("MaxLimit: " << (int)(m_avgRtt.ToDouble (Time::S) *  maxFairRates));
     DataRate((int) maxFairRates);
     devicep2p->SetDataRate((int) maxFairRates);
 }
}// namespace ndn
} // namespace ns3
