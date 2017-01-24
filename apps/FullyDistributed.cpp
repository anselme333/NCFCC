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

#include "FullyDistributed.hpp"
#include "ns3/ptr.h"
#include "ns3/log.h"
#include "ns3/simulator.h"
#include "ns3/packet.h"
#include "ns3/callback.h"
#include "ns3/string.h"
#include "ns3/uinteger.h"
#include "ns3/double.h"

NS_LOG_COMPONENT_DEFINE("ndn.FullyDistributed");

namespace ns3 {
namespace ndn {

NS_OBJECT_ENSURE_REGISTERED(FullyDistributed);

TypeId
FullyDistributed::GetTypeId(void)
{
  static TypeId tid =
    TypeId("ns3::ndn::FullyDistributed")
      .SetGroupName("Ndn")
      .SetParent<Consumer>()
      .AddConstructor<FullyDistributed>()

      .AddAttribute("Window", "Initial size of the window", StringValue("10"),
                    MakeUintegerAccessor(&FullyDistributed::GetWindow, &FullyDistributed::SetWindow),
                    MakeUintegerChecker<uint32_t>())

      .AddAttribute("PayloadSize",
                    "Average size of content object size (to calculate interest generation rate)",
                    UintegerValue(1040), MakeUintegerAccessor(&FullyDistributed::GetPayloadSize,
                                                              &FullyDistributed::SetPayloadSize),
                    MakeUintegerChecker<uint32_t>())

      .AddAttribute("Size", "Amount of data in megabytes to request, relying on PayloadSize "
                            "parameter (alternative to MaxSeq attribute)",
                    DoubleValue(-1), // don't impose limit by default
                    MakeDoubleAccessor(&FullyDistributed::GetMaxSize, &FullyDistributed::SetMaxSize),
                    MakeDoubleChecker<double>())

      .AddAttribute("MaxSeq", "Maximum sequence number to request (alternative to Size attribute, "
                              "would activate only if Size is -1). "
                              "The parameter is activated only if Size negative (not set)",
                    IntegerValue(std::numeric_limits<uint32_t>::max()),
                    MakeUintegerAccessor(&FullyDistributed::GetSeqMax, &FullyDistributed::SetSeqMax),
                    MakeUintegerChecker<uint32_t>())

      .AddAttribute("InitialWindowOnTimeout", "Set window to initial value when timeout occurs",
                    BooleanValue(true),
                    MakeBooleanAccessor(&FullyDistributed::m_setInitialWindowOnTimeout),
                    MakeBooleanChecker())

      .AddTraceSource("WindowTrace",
                      "Window that controls how many outstanding interests are allowed",
                      MakeTraceSourceAccessor(&FullyDistributed::m_window),
                      "ns3::ndn::FullyDistributed::WindowTraceCallback")
      .AddTraceSource("InFlight", "Current number of outstanding interests",
                      MakeTraceSourceAccessor(&FullyDistributed::m_inFlight),
                      "ns3::ndn::FullyDistributed::WindowTraceCallback");

  return tid;
}

FullyDistributed::FullyDistributed()
  : m_payloadSize(1040)
  , m_inFlight(0)
{
}

void
FullyDistributed::SetWindow(uint32_t window)
{
  m_initialWindow = window;
  m_window = m_initialWindow;
  m_ssthresh=std::numeric_limits<uint32_t>::max();
}

uint32_t
FullyDistributed::GetWindow() const
{
  return m_initialWindow;
}

uint32_t
FullyDistributed::GetPayloadSize() const
{
  return m_payloadSize;
}

void
FullyDistributed::SetPayloadSize(uint32_t payload)
{
  m_payloadSize = payload;
}

double
FullyDistributed::GetMaxSize() const
{
  if (m_seqMax == 0)
    return -1.0;

  return m_maxSize;
}

void
FullyDistributed::SetMaxSize(double size)
{
  m_maxSize = size;
  if (m_maxSize < 0) {
    m_seqMax = 0;
    return;
  }

  m_seqMax = floor(1.0 + m_maxSize * 1024.0 * 1024.0/ m_payloadSize);
  NS_LOG_DEBUG("MaxSeqNo: " << m_seqMax);

}

uint32_t
FullyDistributed::GetSeqMax() const
{
  return m_seqMax;
}

void
FullyDistributed::SetSeqMax(uint32_t seqMax)
{
  if (m_maxSize < 0)
    m_seqMax = seqMax;

  // ignore otherwise
}

void
FullyDistributed::ScheduleNextPacket()
{
  if (m_window == static_cast<uint32_t>(0)) {
    Simulator::Remove(m_sendEvent);

    NS_LOG_DEBUG(
      "Next event in " << (std::min<double>(0.5, m_rtt->RetransmitTimeout().ToDouble(Time::S)))
                       << " sec");
    m_sendEvent =
      Simulator::Schedule(Seconds(
                            std::min<double>(0.5, m_rtt->RetransmitTimeout().ToDouble(Time::S))),
                          &Consumer::SendPacket, this);
  }
  else if (m_inFlight >= m_window) {
	  m_sendEvent =
	      Simulator::Schedule(Seconds(
	                            std::min<double>(1.0, m_rtt->RetransmitTimeout().ToDouble(Time::S))),
	                          &Consumer::SendPacket, this);
  }
  else {
    if (m_sendEvent.IsRunning()) {
      Simulator::Remove(m_sendEvent);
    }

    m_sendEvent = Simulator::ScheduleNow(&Consumer::SendPacket, this);
  }
}

///////////////////////////////////////////////////
//          Process incoming packets             //
///////////////////////////////////////////////////

void
FullyDistributed::OnData(shared_ptr<const Data> contentObject)
{
  Consumer::OnData(contentObject);
  m_ssthresh=m_window;
 if(m_window<150)
  {
  m_window = m_window + 1;
  if (m_inFlight > static_cast<uint32_t>(0))
    m_inFlight--;
  NS_LOG_DEBUG("Window: " << m_window << ", InFlight: " << m_inFlight);
  ScheduleNextPacket();
  // Added by Anselme
   Time time = Simulator::Now();
   std::cout<<time.ToDouble(Time::S)<<" ";
   //std::cout<<m_inFlight<<" Outstanding_interests"<<"\n";
   std::cout<<m_window<<" "<<"Window_size"<<"\n";
   //std::cout<<m_ssthresh<<" slow-start threshold"<<"\n";
  }
  else
  {
  m_ssthresh=150;
}

 // Added by Anselme
  //Time time = Simulator::Now();
  //std::cout<<time.ToDouble(Time::S)<<" ";
  //std::cout<<m_inFlight<<" Outstanding_interests"<<"\n";
  //std::cout<<m_window<<" Window_size"<<"\n";
  //std::cout<<m_ssthresh<<" slow-start threshold"<<"\n";

}

///////////////////////////////////////////////////////////////////////

//Modified by Anselme
//

void FullyDistributed::OnRsr(std::shared_ptr<const ndn::Interest> ReduceSendRate)
{
	Consumer::OnRsr(ReduceSendRate); // tracing inside
	NS_LOG_FUNCTION(this << ReduceSendRate);
	Name prefix = ReduceSendRate->getName();
	if(prefix=="/RSR")
	{
//On reception of RSR message, cwnd will be reduced 1/2 and  current cwnd  will be saved as slow-start threshold (ssthresh)
	 m_ssthresh=m_window;
	 m_window = m_window * (1/2);
	}
	else
	{

	}
}

void FullyDistributed::OnTimeout(uint32_t sequenceNumber)
{
  if (m_inFlight > static_cast<uint32_t>(0))
    m_inFlight--;

  if (m_setInitialWindowOnTimeout) {

	//On timeout, cwnd will be reduced 1/3 and  current cwnd  will be saved as slow-start threshold (ssthresh)
     m_ssthresh=m_window;
     m_window = m_window * (1/3);
  }

  NS_LOG_DEBUG("Window: " << m_window << ", InFlight: " << m_inFlight);
  ScheduleNextPacket();
  Consumer::OnTimeout(sequenceNumber);
}

void
FullyDistributed::OnTimeout1(uint32_t sequenceNumber)
{
  if (m_inFlight > static_cast<uint32_t>(0))
    m_inFlight--;

  if (m_setInitialWindowOnTimeout) {
	 m_ssthresh=m_window;
     m_window = m_initialWindow;
  }
  NS_LOG_DEBUG("Window: " << m_window << ", InFlight: " << m_inFlight);
  Consumer::OnTimeout1(sequenceNumber);
}
void FullyDistributed::fast_retransmission(uint32_t sequenceNumber)
{
 NS_LOG_DEBUG("Window: " << m_window << ", InFlight: " << m_inFlight);
 Consumer::fast_retransmission(sequenceNumber);

}
///////////////////////////////////////////////////////////////////////
void
FullyDistributed::WillSendOutInterest(uint32_t sequenceNumber)
{
  m_inFlight++;
  Consumer::WillSendOutInterest(sequenceNumber);
}
} // namespace ndn
} // namespace ns3
