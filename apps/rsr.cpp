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
//rsr.cpp
#include "rsr.hpp"
#include "ns3/ptr.h"
#include "ns3/log.h"
#include <iostream>
#include "ns3/queue.h"
#include "ns3/nstime.h"
#include "ns3/packet.h"
#include "ns3/uinteger.h"
#include "ns3/data-rate.h"
#include "ns3/simulator.h"
#include "ns3/ndnSIM/NFD/daemon/fw/forwarder.hpp"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/ndnSIM-module.h"
#include "ns3/drop-tail-queue.h"
#include "build/ns3/uinteger.h"
//#include "ns3/random-variable.h"
//#include "ns3/random-variable.h"
//#include "ns3/ndnSIM/model/nacc.hpp"
#include "ns3/point-to-point-net-device.h"
#include "ns3/ndnSIM/utils/ndn-rtt-estimator.hpp"
#include "../src/network/utils/simple-net-device.h"
#include "ns3/ndnSIM/helper/ndn-stack-helper.hpp"
#include "ns3/ndnSIM/helper/ndn-fib-helper.hpp"
#include "ns3/random-variable-stream.h"
NS_LOG_COMPONENT_DEFINE("ndn.NaccApp");
namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED(NaccApp);
// register NS-3 type
TypeId NaccApp::GetTypeId()
{
  static TypeId tid = TypeId("NaccApp")
		  .SetParent<ndn::App>()
		  .AddConstructor<NaccApp>();
  return tid;
}

void NaccApp::StartApplication()
{

   ndn::App::StartApplication();
   ndn::FibHelper::AddRoute(GetNode(), "/RSR",m_face, 0);
   Simulator::Schedule(Seconds(1.0), &NaccApp::SendInterest, this);

}

// Processing when application is stopped
void NaccApp::StopApplication()
{
  // cleanup ndn::App
  ndn::App::StopApplication();
}

void NaccApp::SendInterest()
{
  /////////////////////////////////////
  // Reduce Sending Rate notification//
  /////////////////////////////////////
  // Create and configure ndn::Interest
  auto ReduceSendRate = std::make_shared<ndn::Interest>("/RSR");
  Ptr<UniformRandomVariable> rand = CreateObject<UniformRandomVariable>();
  ReduceSendRate->setNonce(rand->GetValue(0, std::numeric_limits<uint32_t>::max()));
  ReduceSendRate->setInterestLifetime(ndn::time::milliseconds(15));
  NS_LOG_DEBUG("Sending Interest packet for " << *ReduceSendRate);
  m_transmittedInterests(ReduceSendRate, this, m_face);
  m_face->onReceiveInterest(*ReduceSendRate);
  Ptr<const Packet> origPacket;
  NS_LOG_DEBUG ("Sending RSR " << origPacket<< " for [" << ReduceSendRate<< "]");
  std::cout << "Reduce Sending Rate notification"<< std::endl;
}
// Callback that will be called when Interest arrives
void NaccApp::OnInterest(std::shared_ptr<const ndn::Interest> ReduceSendRate)
{
	 ndn::App::OnInterest(ReduceSendRate);
	 NS_LOG_DEBUG("Do nothing, it is just RSR notification" << ReduceSendRate->getName());
}
} // namespace ns3
