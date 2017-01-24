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

#include "ndn-stack-helper.hpp"
#include "ns3/log.h"
#include "ns3/names.h"
#include "ns3/string.h"
#include "ns3/point-to-point-net-device.h"
#include "model/ndn-l3-protocol.hpp"
#include "model/ndn-net-device-face.hpp"
#include "utils/ndn-time.hpp"
#include "utils/dummy-keychain.hpp"
#include "model/cs/ndn-content-store.hpp"
#include <limits>
#include <map>
#include "ns3/packet.h"
#include "ns3/ndnSIM/model/nacc.hpp"
#include <algorithm>
#include <boost/lexical_cast.hpp>
#include "ns3/channel.h"
#include "ndn-stack-helper.hpp"
#include "ns3/ndnSIM/apps/rsr.hpp"
#include "ndn-face-container.hpp"
#include "ns3/point-to-point-net-device.h"
#include "ns3/uinteger.h"
#include "ns3/boolean.h"
#include "model/ndn-l3-protocol.hpp"
#include "model/ndn-net-device-face.hpp"
#include "utils/ndn-time.hpp"
#include "utils/dummy-keychain.hpp"
#include "ns3/drop-tail-queue.h"
#include "model/cs/ndn-content-store.hpp"
#include "ns3/point-to-point-channel.h"
#include "utils/ndn-rtt-mean-deviation.hpp"
#include "ns3/ndnSIM/helper/ndn-link-control-helper.hpp"
#include "model/ndn-l3-protocol.hpp"
#include <boost/lexical_cast.hpp>
#include "fw/forwarder.hpp"
#include "ns3/assert.h"
#include "ns3/point-to-point-channel.h"
#include "ns3/error-model.h"
#include "ns3/double.h"
#include "ns3/pointer.h"
#include <boost/random/uniform_int_distribution.hpp>
#include "ns3/ndnSIM/helper/ndn-stack-helper.hpp"
#include "utils//tracers/ndn-l3-tracer.hpp"

NS_LOG_COMPONENT_DEFINE("ndn.StackHelper");

namespace ns3 {
namespace ndn {

StackHelper::StackHelper()
  : m_needSetDefaultRoutes(false)
  , m_maxCsSize(100)
  , m_limitsEnabled (false)
  ,popularity_based_activated(false)
  , a_CMTB (false)
{
  setCustomNdnCxxClocks();

  m_ndnFactory.SetTypeId("ns3::ndn::L3Protocol");
  m_contentStoreFactory.SetTypeId("ns3::ndn::cs::Lru");

  m_netDeviceCallbacks.push_back(
    std::make_pair(PointToPointNetDevice::GetTypeId(),
                   MakeCallback(&StackHelper::PointToPointNetDeviceCallback, this)));
  // default callback will be fired if non of others callbacks fit or did the job
}

StackHelper::~StackHelper()
{
}

KeyChain&
StackHelper::getKeyChain()
{
  static ::ndn::KeyChain keyChain("pib-dummy", "tpm-dummy");
  return keyChain;
}

void
StackHelper::setCustomNdnCxxClocks()
{
  ::ndn::time::setCustomClocks(make_shared<ns3::ndn::time::CustomSteadyClock>(),
                               make_shared<ns3::ndn::time::CustomSystemClock>());
}

void
StackHelper::SetDefaultRoutes(bool needSet)
{
  NS_LOG_FUNCTION(this << needSet);
  m_needSetDefaultRoutes = needSet;
}

void
StackHelper::SetStackAttributes(const std::string& attr1, const std::string& value1,
                                const std::string& attr2, const std::string& value2,
                                const std::string& attr3, const std::string& value3,
                                const std::string& attr4, const std::string& value4)
{
  if (attr1 != "")
    m_ndnFactory.Set(attr1, StringValue(value1));
  if (attr2 != "")
    m_ndnFactory.Set(attr2, StringValue(value2));
  if (attr3 != "")
    m_ndnFactory.Set(attr3, StringValue(value3));
  if (attr4 != "")
    m_ndnFactory.Set(attr4, StringValue(value4));
}

void
StackHelper::SetOldContentStore(const std::string& contentStore, const std::string& attr1,
                                const std::string& value1, const std::string& attr2,
                                const std::string& value2, const std::string& attr3,
                                const std::string& value3, const std::string& attr4,
                                const std::string& value4)
{
  m_maxCsSize = 0;

  m_contentStoreFactory.SetTypeId(contentStore);
  if (attr1 != "")
    m_contentStoreFactory.Set(attr1, StringValue(value1));
  if (attr2 != "")
    m_contentStoreFactory.Set(attr2, StringValue(value2));
  if (attr3 != "")
    m_contentStoreFactory.Set(attr3, StringValue(value3));
  if (attr4 != "")
    m_contentStoreFactory.Set(attr4, StringValue(value4));
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Added By Anselme

void
StackHelper::EnableLimits (bool enable, Time avgRtt, uint32_t avgContentObject, uint32_t avgInterest)
{
  NS_LOG_INFO ("EnableLimits: " << enable);
  m_limitsEnabled = enable;
  m_avgRtt = avgRtt;
  m_avgContentObjectSize = avgContentObject;
  m_avgInterestSize = avgInterest;
}
void StackHelper::MemoryEfficient(bool enable,uint32_t totalbuffer, uint32_t  min_bufferthreshold)
{
   //NS_LOG_INFO ("NaccIcn: " << enable);
	a_CMTB = enable;
	m_buffer = totalbuffer;
	m_bufferthreshold = min_bufferthreshold;
}

struct prediction_table
   {
	  std::string content_name;
	  double p_t_1;
	  double y_t_1;
	  double p_t;
	  int rank;
   };
void StackHelper::Popularity_based(bool enable)
{
	popularity_based_activated = enable;

}
void StackHelper::PrintQueueSizePerFlow()
{
	const std::map <ndn::Name, uint32_t> & QueueSizePerFlow = GetQLengthPerFlow ();

	std::cout<<"Queue size per flow"<<std::endl;
	std::map<ndn::Name, uint32_t>::const_iterator
	mit(QueueSizePerFlow.begin()),
	mend(QueueSizePerFlow.end());
	for(;mit!=mend;++mit)
	{
		std::cout <<"prefix name : "<< mit->first << "\tqueue size : " << mit->second << std::endl;
	}

}

uint32_t StackHelper::GetQueueSizePerFlow(ndn::Name prefix)
{

	std::map <ndn::Name, uint32_t> & QueueSizePerFlow = GetQLengthPerFlow ();
	std::map<ndn::Name, uint32_t>::iterator
	mit(QueueSizePerFlow.find(prefix)),
	mend(QueueSizePerFlow.end());
	if(mit!=mend)
	{
		uint32_t & queue_size = mit->second;
		uint32_t ql = queue_size;
		return ql;
	}
return 0;
}

uint32_t  StackHelper:: GetFlowNumber(ndn::Name prefix)
{
	Nflows=0.1;
	std::map <ndn::Name, uint32_t> & QueueSizePerFlow = GetQLengthPerFlow ();
	std::map<ndn::Name, uint32_t>::iterator
	mit(QueueSizePerFlow.find(prefix)),
	mend(QueueSizePerFlow.end());
	Nflows= double(QueueSizePerFlow.size());
	return Nflows;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void
StackHelper::setCsSize(size_t maxSize)
{
  m_maxCsSize = maxSize;
}

Ptr<FaceContainer>
StackHelper::Install(const NodeContainer& c) const
{
  Ptr<FaceContainer> faces = Create<FaceContainer>();
  for (NodeContainer::Iterator i = c.Begin(); i != c.End(); ++i) {
    faces->AddAll(Install(*i));
  }
  return faces;
}

Ptr<FaceContainer>
StackHelper::InstallAll() const
{
  return Install(NodeContainer::GetGlobal());
}

Ptr<FaceContainer>
StackHelper::Install(Ptr<Node> node) const
{
  Ptr<FaceContainer> faces = Create<FaceContainer>();

  if (node->GetObject<L3Protocol>() != 0) {
    NS_FATAL_ERROR("Cannot re-install NDN stack on node "
                   << node->GetId());
    return 0;
  }

  Ptr<L3Protocol> ndn = m_ndnFactory.Create<L3Protocol>();
  ndn->getConfig().put("tables.cs_max_packets", (m_maxCsSize == 0) ? 1 : m_maxCsSize);

  // Create and aggregate content store if NFD's contest store has been disabled
  if (m_maxCsSize == 0) {
    ndn->AggregateObject(m_contentStoreFactory.Create<ContentStore>());
  }

  // Aggregate L3Protocol on node (must be after setting ndnSIM CS)
  node->AggregateObject(ndn);

  for (uint32_t index = 0; index < node->GetNDevices(); index++) {
    Ptr<NetDevice> device = node->GetDevice(index);

//////////////////////////////////////////////////////////////////////////////////////////////////
   //Added by Anselme
    shared_ptr<NetDeviceFace> face;

   // for activated Cooperative and Memory-efficient Token Bucket algorithm

     if (a_CMTB)
            {
              if(ns3::ndn::NetDeviceFace *netf = dynamic_cast<ns3::ndn::NetDeviceFace*>(&(*face)))
            		   {
            		     ns3::Ptr<ns3::PointToPointNetDevice> devicep2p = netf->GetNetDevice()->GetNode ()->GetDevice(netf->getId ())->GetObject<ns3::PointToPointNetDevice>();
            		     ns3::DataRateValue ratevalue;
            		     devicep2p->GetAttribute("DataRate", ratevalue);
            		     TimeValue linkDelay;
            		     devicep2p->GetAttribute ("Delay", linkDelay);
            		     // Set maximum buffer
            		     devicep2p->SetAttribute("m_buffer", UintegerValue (m_buffer));
            		     //Set buffer sharing parameter
            		     uint32_t  buffer_sharing_parameter= 1;
            		     Ptr<ndn::L3Protocol> ndn1 = node->GetObject<ndn::L3Protocol> ();
            		    //number of the face in each node stack
            		     uint32_t number_faces=m_faces.size();
            		     //Calculate buffer share in each node stack
            		     uint32_t share_buffer_product = buffer_sharing_parameter * m_buffer;
            		     uint32_t buffer_share= share_buffer_product/ number_faces;
            		     Ptr<ns3::Queue> queue;
            		     Ptr<NetDevice>netDevice1;
            		     Ptr<ns3::DropTailQueue> ndnqueue = StaticCast<ns3::DropTailQueue> (queue);
            		     uint32_t BytesInQueue=ndnqueue->GetTotalReceivedBytes();
            		     uint32_t available_buffer = buffer_share - m_QueueLength.size();
            		     // Dynamic buffer threshold
            		     uint32_t dynamic_buffer_thresould= std::max(available_buffer, m_bufferthreshold);
            		     devicep2p->SetAttribute("m_bufferthreshold", UintegerValue(dynamic_buffer_thresould));
            		     devicep2p->SetAttribute("MemoryEfficient", BooleanValue(a_CMTB));
            		     NS_LOG_INFO("DataRate for this link is " << ratevalue.Get());
            		     double cab = ratevalue.Get ().GetBitRate ();
            		     double buf_interface;
            		     buf_interface = (double(cab)) * m_avgRtt.ToDouble (Time::S);
            		     double  maxFairRates = (cab/Nflows) + ((buf_interface - m_QueueLength.size()) / (Nflows * m_avgRtt.ToDouble (Time::S)));
            		     NS_LOG_INFO ("Max packets per second: " <<  maxFairRates);
            		     NS_LOG_INFO ("MaxLimit: " << (int)(m_avgRtt.ToDouble (Time::S) *  maxFairRates));
            		     DataRate((int) maxFairRates);
                         double packet_adm;
                         double interval=1.0;
                         for (number_faces = 1; BytesInQueue<= (maxFairRates*interval); number_faces++)
                         {
                         packet_adm=buf_interface-m_QueueLength.size(); // Packets can be admitted to fill up the buffer.
                         if((packet_adm>=dynamic_buffer_thresould))
                         {
                         devicep2p->SetDataRate((int) maxFairRates);
            		     }
                         else
                         {
                        &NaccMecIcn::TrafficShaping;
                        &NaccApp::SendInterest;// Send Reduce Sending rate notification
                         }
                         }
            }
            }
     else if (!a_CMTB)
     {
    	std::cout<<device<<"NCFCC"<<"Deactivated"<<"\t";

     }
     if(popularity_based_activated)
      {

    	 struct prediction_tab
    	      {
    		 std::string content_name;
    	 	  double p_t_1;
    	 	  double y_t_1;
    	 	  double p_t;
    	 	  int rank;
    	      };
    	   prediction_tab popularity_prediction[5];
    	   popularity_prediction [0] = {"/kndu/ngn/image/i1.jpg",10.5,11,10.65,1};
    	   popularity_prediction [1] = {"/kndu/ngn/image/i2.jpg",9.8,8,9.26,3};
    	   popularity_prediction [2] = {"/kndu/ngn/video/v.mpg",7.1,14,9.17,4};
    	   popularity_prediction [3] = {"/kndu/ngn/image/i3.jpg",4.8,16,8.16,5};
    	   popularity_prediction [4] = {"/kndu/ngn/image/a.mp3",8.8,13,10.6,2};
    	   uint32_t max_Iout;
    	     	    if(ns3::ndn::NetDeviceFace *netf = dynamic_cast<ns3::ndn::NetDeviceFace*>(&(*face)))
    	     	          {
    	     	            //-256 because 0-255 is reserved by ndn local/app faces;
    	     	            ns3::Ptr<ns3::PointToPointNetDevice> nd1 = netf->GetNetDevice()->GetNode ()->GetDevice(netf->getId () - 256)->GetObject<ns3::PointToPointNetDevice>();
    	     	            ns3::DataRateValue dv;
    	     	            nd1->GetAttribute("DataRate", dv);
    	     	            double bandwidth = dv.Get().GetBitRate();

    	     	            // incoming Data
    	     	            nfd::NetworkLayerCounters incomingdata;
    	     	            double InData= incomingdata.getNInDatas();

    	     	            /// outgoing Data
    	     	            nfd::NetworkLayerCounters outdata;
    	     	           double OutData =outdata. getNOutDatas();

    	     	           /// incoming Interest
    	     	            nfd::NetworkLayerCounters inInterest;
    	     	            double InInterests=inInterest.getNInInterests();

    	     	           /// outgoing Interest
    	     	           nfd::NetworkLayerCounters  outInterest;
    	     	           double OutInterests = outInterest.getNOutInterests();// Counting Interest;

    	     	            // calculate maximum Interest rate to upstream link
    	                   max_Iout = bandwidth/(InData/InInterests);
    	                   double ration_popularity=OutInterests/max_Iout;
    	                   ns3::Ptr<ns3::PointToPointNetDevice>devicep2p= netf->GetNetDevice()->GetNode ()->GetDevice(netf->getId ())->GetObject<ns3::PointToPointNetDevice>();
    	                   devicep2p->SetDataRate((int)(max_Iout));
    	                   if(InInterests>max_Iout)
    	                   {
    	                    &NaccApp::SendInterest;
    	                   if(OutInterests>=max_Iout)
    	                   {
    	                		 devicep2p->SetDataRate((int)(dv.Get().GetBitRate()/5));
    	                   }
    	                   }
    	     	      }
      }
      else if(!popularity_based_activated)
      {
    	  std::cout<<device<<"popularity_based"<<"Deactivated"<<"\t";
      }

     NS_LOG_LOGIC ("Node " << node->GetId () << ": added NetDeviceFace as face #" << *face);
   ///////////////////////////////////////////////////////////////////////////////////////////////
    for (const auto& item : m_netDeviceCallbacks) {
      if (device->GetInstanceTypeId() == item.first ||
          device->GetInstanceTypeId().IsChildOf(item.first)) {
        face = item.second(node, ndn, device);
        //if ( != 0)
          //break;
      }
    }
    if (face == 0)
    {
      face = DefaultNetDeviceCallback(node, ndn, device);


    }
    if (m_needSetDefaultRoutes)
    {
      // default route with lowest priority possible
      FibHelper::AddRoute(node, "/", face, std::numeric_limits<int32_t>::max());
    }
    faces->Add(this->createAndRegisterFace(node, ndn, device));
  }
    return faces;
  }

void
StackHelper::AddNetDeviceFaceCreateCallback(TypeId netDeviceType,
                                            StackHelper::NetDeviceFaceCreateCallback callback)
{
  m_netDeviceCallbacks.push_back(std::make_pair(netDeviceType, callback));
}

void
StackHelper::UpdateNetDeviceFaceCreateCallback(TypeId netDeviceType,
                                               NetDeviceFaceCreateCallback callback)
{
  for (auto& i : m_netDeviceCallbacks) {
    if (i.first == netDeviceType) {
      i.second = callback;
      return;
    }
  }
}

void
StackHelper::RemoveNetDeviceFaceCreateCallback(TypeId netDeviceType,
                                               NetDeviceFaceCreateCallback callback)
{
  m_netDeviceCallbacks.remove_if([&] (const std::pair<TypeId, NetDeviceFaceCreateCallback>& i) {
      return (i.first == netDeviceType);
    });
}

shared_ptr<NetDeviceFace>
StackHelper::DefaultNetDeviceCallback(Ptr<Node> node, Ptr<L3Protocol> ndn,
                                      Ptr<NetDevice> netDevice) const
{
  NS_LOG_DEBUG("Creating default NetDeviceFace on node " << node->GetId());

  shared_ptr<NetDeviceFace> face = std::make_shared<NetDeviceFace>(node, netDevice);

  ndn->addFace(face);
  NS_LOG_LOGIC("Node " << node->GetId() << ": added NetDeviceFace as face #"
                       << face->getLocalUri());

  return face;
}

shared_ptr<NetDeviceFace>
StackHelper::PointToPointNetDeviceCallback(Ptr<Node> node, Ptr<L3Protocol> ndn,
                                           Ptr<NetDevice> device) const
{
  NS_LOG_DEBUG("Creating point-to-point NetDeviceFace on node " << node->GetId());

  shared_ptr<NetDeviceFace> face = std::make_shared<NetDeviceFace>(node, device);

  ndn->addFace(face);
  NS_LOG_LOGIC("Node " << node->GetId() << ": added NetDeviceFace as face #"
                       << face->getLocalUri());

  return face;
}

Ptr<FaceContainer>
StackHelper::Install(const std::string& nodeName) const
{
  Ptr<Node> node = Names::Find<Node>(nodeName);
  return Install(node);
}


void
StackHelper::Update(Ptr<Node> node)
{
  if (node->GetObject<L3Protocol>() == 0) {
    Install(node);
    return;
  }

  Ptr<L3Protocol> ndn = node->GetObject<L3Protocol>();

  for (uint32_t index = 0; index < node->GetNDevices(); index++) {

    Ptr<NetDevice> device = node->GetDevice(index);

    if (ndn->getFaceByNetDevice(device) == nullptr) {
      this->createAndRegisterFace(node, ndn, device);

    }
  }
}


void
StackHelper::Update(const NodeContainer& c)
{
  for (NodeContainer::Iterator i = c.Begin(); i != c.End(); ++i) {
    Update(*i);
  }
}

void
StackHelper::Update(const std::string& nodeName)
{
  Ptr<Node> node = Names::Find<Node>(nodeName);
  Update(node);
}

void
StackHelper::UpdateAll()
{
  Update(NodeContainer::GetGlobal());
}

shared_ptr<NetDeviceFace>
StackHelper::createAndRegisterFace(Ptr<Node> node, Ptr<L3Protocol> ndn, Ptr<NetDevice> device) const
{
  shared_ptr<NetDeviceFace> face;

  for (const auto& item : m_netDeviceCallbacks) {
    if (device->GetInstanceTypeId() == item.first ||
        device->GetInstanceTypeId().IsChildOf(item.first)) {
      face = item.second(node, ndn, device);
      if (face != 0)
        break;
    }
  }

  if (face == 0) {
    face = DefaultNetDeviceCallback(node, ndn, device);
  }

  if (m_needSetDefaultRoutes) {
    // default route with lowest priority possible
    FibHelper::AddRoute(node, "/", face, std::numeric_limits<int32_t>::max());
  }
  return face;
}

} // namespace ndn
} // namespace ns3
