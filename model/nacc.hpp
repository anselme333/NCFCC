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
// nacc.h
#include "ns3/queue.h"
#include "ns3/data-rate.h"
#include "../src/network/utils/simple-net-device.h"
#include "ns3/drop-tail-queue.h"
#include "ns3/ndnSIM/utils/ndn-rtt-estimator.hpp"
#include "ns3/ndnSIM/model/ndn-common.hpp"
#include "ns3/ndnSIM/model/ndn-face.hpp"
#include "ns3/ndnSIM/model/ndn-net-device-face.hpp"
#include "ns3/net-device.h"
#include "core/notification-stream.hpp"
#include <ndn-cxx/interest.hpp>
#include "ns3/ndnSIM/model/ndn-common.hpp"
#include "ns3/ndnSIM/NFD/daemon/face/local-face.hpp"
namespace ns3 {
namespace ndn {
class NaccMecIcn : public ndn::NetDeviceFace
{
public:

// register NS-3 type "NaccMecIcn"
static TypeId GetTypeId();
NaccMecIcn(Ptr<Node> node, const Ptr<NetDevice>& netDevice);

virtual ~NaccMecIcn();

public: // from nfd::Face
  virtual void sendInterest(const Interest& interest);
//const Interest& interest
  virtual void
  sendData(const Data& data);

  virtual void
  close();

public:
  /**
   * \brief Get NetDevice associated with the face
   *
   * \returns smart pointer to NetDevice associated with the face
   */
  Ptr<NetDevice>
  GetNetDevice() const;

public:
  void
  send(Ptr<Packet> packet);

  /// \brief callback from lower layers
   void
   receiveFromNetDevice(Ptr<NetDevice> device, Ptr<const Packet> p, uint16_t protocol,
                        const Address& from, const Address& to, NetDevice::PacketType packetType);

   uint32_t GetQueueLength()
   {
	   return m_QueueLength.size();

   };

  //////////////////////////////////////////////////////
  bool NaccIcnActive() {return m_NaccIcn;};// today
  ///////////////////////////////////////////////////
  uint32_t GetIntQueueSizePerFlow(ndn::Name prefix);
  //Time ComputeRates();

  uint32_t  GetFlowNumber()
  {
  	return m_Nflows;

  };

  enum admitted
   //
    {
  	   //no active flow
      notactive,
      //active flow
      active
    };
   std::map <ndn::Name, uint32_t> m_nIntQueueSizePerFlow;
   admitted  m_admitted;
   virtual void TrafficShaping();
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   virtual void SetLimits (double rate, double delay);

   virtual double GetMaxLimit () const
    {
      return  m_tokenMax;
    }
uint64_t getBitrateNacc(shared_ptr<NetDeviceFace> face);
std::queue<Ptr<Packet> > m_QueueLength;

//virtual void OnRsr();
private:
   DataRate* rate1;
   ns3::ndn::RttEstimator* RTTmax;
   Queue* transmission_queue;
   nfd::Face* RSR;
   Interest* Reduce;
public:
  uint32_t m_buffer;
  uint32_t m_bufferthreshold;
  double m_shapingRate;
  int m_Nflows;
  Ptr<Node> m_node;
  Ptr<NetDevice> m_netDevice;
  /////////////////////////////////////////////////////////////////////////////////////
  Time m_leakRandomizationInteral;
  bool m_isLeakScheduled;
  bool m_NaccIcn;
  double m_tokenMax;   ///< \brief Maximum Interest allowance for this face (maximum tokens that can be issued at the same time)
  double m_tokenLeak;  ///< \brief Normalized amount that should be leaked every second (token token leak rate)
  double m_token;      ///< \brief Value representing current size of the Interest allowance for this face (current size of token token)
////////////////////////////////////////////////////////////////////////////////////////////////////////////
};
}// namespace ndn
} // namespace ns3
