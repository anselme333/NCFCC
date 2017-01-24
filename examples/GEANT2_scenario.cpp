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
// congestioncontrolsenario2.cpp

#include <memory>
#include "ns3/ptr.h"
#include "ns3/log.h"
#include "ns3/command-line.h"
#include <list>
#include <vector>
#include "ns3/queue.h"
#include "model/ndn-net-device-face.hpp"
#include "ns3/config.h"
#include "ns3/uinteger.h"
#include "ns3/string.h"
#include "ns3/drop-tail-queue.h"
#include "ns3/node.h"
#include "ns3/packet.h"
#include "ns3/core-module.h"
#include "ns3/ndnSIM-module.h"
#include "ns3/network-module.h"
#include "ns3/flow-monitor-helper.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/traced-callback.h"
#include "ns3/ndnSIM/model/nacc.hpp"
#include "ns3/ndnSIM/model/ndn-common.hpp"
#include "ns3/ndnSIM/model/ndn-face.hpp"
#include "ns3/point-to-point-net-device.h"
#include "ns3/ndnSIM/NFD/daemon/face/face.hpp"
#include "ns3/ndnSIM/model/ndn-l3-protocol.hpp"
#include "ns3/ndnSIM/model/ndn-ns3.hpp"

namespace ns3 {
std::stringstream filePlotQueue;
void QueueSizeStatus(Ptr<ns3::Queue> queue,Ptr<NetDevice>device)
{
uint32_t QtotalSize;
uint32_t QtotalSizebyte = 0;
uint32_t queuedrop = 0;
uint32_t queuedropbyte = 0;
uint32_t queuesize = 0;
Ptr<ns3::DropTailQueue> ndnqueue = StaticCast<ns3::DropTailQueue> (queue);
/*if(ndnqueue != NULL)
{
	QtotalSize = ndnqueue->GetTotalReceivedPackets();
	QtotalSizebyte = ndnqueue->GetTotalReceivedBytes();
	queuedrop = ndnqueue->GetTotalDroppedPackets();
	queuedropbyte= ndnqueue->GetTotalDroppedBytes ();
	queuedropbyte= ndnqueue->GetTotalDroppedBytes ();
	queuesize = ndnqueue->GetNPackets();


   // check queue size every 1/100 of a second
	Simulator::Schedule (Seconds (0.01), &QueueSizeStatus,queue,device);
	Time time = Simulator::Now();
	double Newtime=time.ToDouble(Time::S);
	std::cout << Newtime <<" ";
	std::cout << "\t " <<QtotalSize <<" "<< "Total_packet_received" << std::endl;

	std::cout << Newtime <<" ";
	std::cout << "\t " <<QtotalSizebyte <<" "<< "Total_byte received" << std::endl;

	std::cout << Newtime <<" ";
	std::cout << "\t " <<queuedrop<<" "<< "Number_packet_dropped" << std::endl;

	std::cout << Newtime <<" ";
	std::cout << "\t " <<queuedrop<<" "<< "Number_byte_dropped" << std::endl;

	std::cout << Newtime <<" ";
	std::cout << "\t " <<queuesize<<" "<< "packets_storedinQueue" << std::endl;

std::ofstream fPlotQueue (filePlotQueue.str ().c_str (), std::ios::out|std::ios::app);
fPlotQueue << Simulator::Now ().GetSeconds () << " @ "<< queue <<" Queue "<<QtotalSize<<" Received Packet "<<QtotalSizebyte<<" Received Byte"<<queuedrop<< " Dropped Packet  " <<queuedropbyte<<"Dropped Byte  "<< std::endl;
fPlotQueue.close ();
}*/
}


int main(int argc, char* argv[])
{
CommandLine cmd;
cmd.Parse (argc, argv);
cmd.Parse(argc, argv);

AnnotatedTopologyReader topologyReader("", 25);
topologyReader.SetFileName("src/ndnSIM/examples/topologies/ncfcc_GEANT2_scenario.txt");
Config::SetDefault ("ns3::DropTailQueue::MaxPackets", StringValue ("100"));
topologyReader.Read();

// Install NDN stack on all nodes
ndn::StackHelper ndnHelper;
//char popularity_prediction [5][5] = { {'/kndu/ngn/image/i1.jpg',10.5,11,10.65,1}, {'/kndu/ngn/image/i2.jpg',9.8,8,9.26,3}, {'/kndu/ngn/video/v.mpg',7.1,14,9.17,4}, {'/kndu/ngn/image/i3.jpg',4.8,16,8.16,5}, {'/kndu/ngn/image/a.mp3',8.8,13,10.6,2} };
ndnHelper.SetOldContentStore("ns3::ndn::cs::Lru", "MaxSize", "1000");
//ndnHelper.Popularity_based(true, *popularity_prediction);
uint32_t max_buffer = 100;
uint32_t min_threshold = max_buffer/2;
ndnHelper.MemoryEfficient(true, max_buffer, min_threshold);
ndnHelper.Popularity_based(false);
ndnHelper.InstallAll();

// Choosing forwarding strategy

//ndn::StrategyChoiceHelper::InstallAll("/com/news", "/localhost/nfd/strategy/multicast");
//ndn::StrategyChoiceHelper::InstallAll("/com/video", "/localhost/nfd/strategy/multicast");
//ndn::StrategyChoiceHelper::InstallAll("/com/ccn", "/localhost/nfd/strategy/multicast");

//ndn::StrategyChoiceHelper::InstallAll("/com/news", "/localhost/nfd/strategy/best-route");
//ndn::StrategyChoiceHelper::InstallAll("/com/video", "/localhost/nfd/strategy/best-route");
//ndn::StrategyChoiceHelper::InstallAll("/com/ccn", "/localhost/nfd/strategy/best-route");

//ndn::StrategyChoiceHelper::InstallAll("/com/news", "/localhost/nfd/strategy/ncc");
//ndn::StrategyChoiceHelper::InstallAll("/com/video", "/localhost/nfd/strategy/ncc");
//ndn::StrategyChoiceHelper::InstallAll("/com/ccn", "/localhost/nfd/strategy/ncc");


ndn::StrategyChoiceHelper::InstallAll("/com/news", "/localhost/nfd/strategy/client-control");
ndn::StrategyChoiceHelper::InstallAll("/com/video", "/localhost/nfd/strategy/client-control");
ndn::StrategyChoiceHelper::InstallAll("/com/ccn", "/localhost/nfd/strategy/client-control");


//Installing global routing interface on all nodes
ndn::GlobalRoutingHelper ndnGlobalRoutingHelper;
ndnGlobalRoutingHelper.InstallAll();

// Getting containers for the consumer/producer
Ptr<Node> consumer1 = Names::Find<Node>("Src1");
Ptr<Node> consumer2 = Names::Find<Node>("Src2");
Ptr<Node> consumer3 = Names::Find<Node>("Src3");
Ptr<Node> consumer4 = Names::Find<Node>("Src4");
Ptr<Node> consumer5 = Names::Find<Node>("Src5");
Ptr<Node> consumer6 = Names::Find<Node>("Src6");
Ptr<Node> consumer7 = Names::Find<Node>("Src7");
Ptr<Node> consumer8 = Names::Find<Node>("Src8");
Ptr<Node> consumer9 = Names::Find<Node>("Src9");
Ptr<Node> consumer10 = Names::Find<Node>("Src10");
Ptr<Node> consumer11 = Names::Find<Node>("Src11");
Ptr<Node> consumer12 = Names::Find<Node>("Src12");
Ptr<Node> consumer13 = Names::Find<Node>("Src13");
Ptr<Node> consumer14 = Names::Find<Node>("Src14");
Ptr<Node> consumer15 = Names::Find<Node>("Src15");
Ptr<Node> consumer16 = Names::Find<Node>("Src16");
Ptr<Node> consumer17 = Names::Find<Node>("Src17");
Ptr<Node> consumer18 = Names::Find<Node>("Src18");
Ptr<Node> consumer19 = Names::Find<Node>("Src19");
Ptr<Node> consumer20 = Names::Find<Node>("Src20");
Ptr<Node> core_router1= Names::Find<Node>("R1");
Ptr<Node> core_router2= Names::Find<Node>("R2");
Ptr<Node> core_router3= Names::Find<Node>("R3");
Ptr<Node> core_router4= Names::Find<Node>("R4");
Ptr<Node> core_router5= Names::Find<Node>("R5");
Ptr<Node> core_router6= Names::Find<Node>("R6");
Ptr<Node> core_router7= Names::Find<Node>("R7");
Ptr<Node> core_router8= Names::Find<Node>("R8");
Ptr<Node> core_router9= Names::Find<Node>("R9");
Ptr<Node> core_router10= Names::Find<Node>("R10");
Ptr<Node> core_router11= Names::Find<Node>("R11");
Ptr<Node> core_router12= Names::Find<Node>("R12");
Ptr<Node> core_router13= Names::Find<Node>("R13");
Ptr<Node> core_router14= Names::Find<Node>("R14");
Ptr<Node> core_router15= Names::Find<Node>("R15");
Ptr<Node> core_router16= Names::Find<Node>("R16");
Ptr<Node> core_router17= Names::Find<Node>("R17");
Ptr<Node> core_router18= Names::Find<Node>("R18");
Ptr<Node> core_router19= Names::Find<Node>("R19");
Ptr<Node> core_router20= Names::Find<Node>("R20");
Ptr<Node> core_router21= Names::Find<Node>("R21");
Ptr<Node> core_router22= Names::Find<Node>("R22");
Ptr<Node> core_router23= Names::Find<Node>("R23");
Ptr<Node> cp1 = Names::Find<Node>("CP1");

/////////////////////////////////////////////////////////////////////////////////////////////////////

// Install the queue model in all the nodes



// Creating queue core_router1

		   ObjectFactory m_queueFactory1;
		   Ptr<ndn::L3Protocol> ndn1 = core_router1->GetObject <ndn::L3Protocol> ();
		   Ptr<PointToPointNetDevice> device1 = core_router1->GetDevice(core_router1->GetNDevices()-1)->GetObject<PointToPointNetDevice> ();
		   std::shared_ptr<nfd::Face> face1 = ndn1->getFaceById(1);
		   std::shared_ptr<ndn::NetDeviceFace> device999=  std::dynamic_pointer_cast<ndn::NetDeviceFace>(face1);
		   Ptr<PointToPointNetDevice>p1 = StaticCast<PointToPointNetDevice> (device1);
		   m_queueFactory1.SetTypeId("ns3::DropTailQueue");
		   Config::SetDefault ("ns3::DropTailQueue::MaxPackets", UintegerValue (100));
		   Ptr<Queue> queuef1= m_queueFactory1.Create<Queue> ();
		   Ptr<ns3::DropTailQueue> ndnqueue1 = StaticCast<ns3::DropTailQueue> (queuef1);
		   ndnqueue1->SetMode(ns3::DropTailQueue::QUEUE_MODE_PACKETS);
		   p1->SetQueue(queuef1);
		   Simulator::ScheduleNow (&QueueSizeStatus, queuef1, p1 );


		   // Creating queue core_router2

			 ObjectFactory m_queueFactory2;
			 Ptr<ndn::L3Protocol> ndn2 = core_router2->GetObject <ndn::L3Protocol> ();
			 Ptr<PointToPointNetDevice> device2 = core_router2->GetDevice(core_router2->GetNDevices()-1)->GetObject<PointToPointNetDevice> ();
			 std::shared_ptr<nfd::Face> face2 = ndn2->getFaceById(1);
			 std::shared_ptr<ndn::NetDeviceFace> device2222=  std::dynamic_pointer_cast<ndn::NetDeviceFace>(face2);
			 Ptr<PointToPointNetDevice> p2 = StaticCast<PointToPointNetDevice> (device2);
			 m_queueFactory2.SetTypeId("ns3::DropTailQueue");
			 Config::SetDefault ("ns3::DropTailQueue::MaxPackets", UintegerValue (100));
			 Ptr<Queue> queuef2= m_queueFactory2.Create<Queue> ();
			 Ptr<ns3::DropTailQueue> ndnqueue2 = StaticCast<ns3::DropTailQueue> (queuef2);
			 ndnqueue1->SetMode(ns3::DropTailQueue::QUEUE_MODE_PACKETS);
			 p2->SetQueue(queuef2);
			 Simulator::ScheduleNow (&QueueSizeStatus, queuef2,p2 );

			  // Creating queue core_router3

			  ObjectFactory m_queueFactory3;
			  Ptr<ndn::L3Protocol> ndn3 = core_router3->GetObject <ndn::L3Protocol> ();
			  Ptr<PointToPointNetDevice> device3 = core_router3->GetDevice(core_router3->GetNDevices()-1)->GetObject<PointToPointNetDevice> ();
			  std::shared_ptr<nfd::Face> face3= ndn3->getFaceById(1);
			  std::shared_ptr<ndn::NetDeviceFace> device3396=  std::dynamic_pointer_cast<ndn::NetDeviceFace>(face3);
			  Ptr<PointToPointNetDevice> p3 = StaticCast<PointToPointNetDevice> (device3);
			  m_queueFactory3.SetTypeId("ns3::DropTailQueue");
			  Config::SetDefault ("ns3::DropTailQueue::MaxPackets", UintegerValue (100));
			  Ptr<Queue> queuef3= m_queueFactory3.Create<Queue> ();
			  Ptr<ns3::DropTailQueue> ndnqueue3= StaticCast<ns3::DropTailQueue> (queuef3);
			  ndnqueue1->SetMode(ns3::DropTailQueue::QUEUE_MODE_PACKETS);
			  p3->SetQueue(queuef3);
			  Simulator::ScheduleNow (&QueueSizeStatus, queuef3,p3);

			  // Creating queue core_router4

				ObjectFactory m_queueFactory4;
				Ptr<ndn::L3Protocol> ndn4 = core_router4->GetObject <ndn::L3Protocol> ();
				Ptr<PointToPointNetDevice> device4 = core_router4->GetDevice(core_router4->GetNDevices()-1)->GetObject<PointToPointNetDevice> ();
				std::shared_ptr<nfd::Face> face4= ndn4->getFaceById(1);
				std::shared_ptr<ndn::NetDeviceFace> device445= std::dynamic_pointer_cast<ndn::NetDeviceFace>(face4);
				Ptr<PointToPointNetDevice> p4= StaticCast<PointToPointNetDevice> (device4);
				m_queueFactory4.SetTypeId("ns3::DropTailQueue");
				Config::SetDefault ("ns3::DropTailQueue::MaxPackets", UintegerValue (100));
				Ptr<Queue> queuef4= m_queueFactory4.Create<Queue> ();
				Ptr<ns3::DropTailQueue> ndnqueue4= StaticCast<ns3::DropTailQueue> (queuef4);
				ndnqueue4->SetMode(ns3::DropTailQueue::QUEUE_MODE_PACKETS);
				p4->SetQueue(queuef4);
				Simulator::ScheduleNow (&QueueSizeStatus, queuef4,p4);
				// Creating queue core_router5

								 ObjectFactory m_queueFactory5;
								 Ptr<ndn::L3Protocol> ndn5 = core_router5->GetObject <ndn::L3Protocol> ();
								 Ptr<PointToPointNetDevice> device5 = core_router5->GetDevice(core_router5->GetNDevices()-1)->GetObject<PointToPointNetDevice> ();
								 std::shared_ptr<nfd::Face> face5= ndn5->getFaceById(1);
								 std::shared_ptr<ndn::NetDeviceFace> device556= std::dynamic_pointer_cast<ndn::NetDeviceFace>(face5);
								 Ptr<PointToPointNetDevice> p5= StaticCast<PointToPointNetDevice> (device5);
								 m_queueFactory5.SetTypeId("ns3::DropTailQueue");
								 Config::SetDefault ("ns3::DropTailQueue::MaxPackets", UintegerValue (100));
								 Ptr<Queue> queuef5= m_queueFactory5.Create<Queue> ();
								 Ptr<ns3::DropTailQueue> ndnqueue5= StaticCast<ns3::DropTailQueue> (queuef5);
								 ndnqueue1->SetMode(ns3::DropTailQueue::QUEUE_MODE_PACKETS);
								 p5->SetQueue(queuef5);
								 Simulator::ScheduleNow (&QueueSizeStatus, queuef5,p5);
								 // Creating queue core_router6

								 ObjectFactory m_queueFactory6;
								 Ptr<ndn::L3Protocol> ndn6 = core_router6->GetObject <ndn::L3Protocol> ();
								 Ptr<PointToPointNetDevice> device6 = core_router6->GetDevice(core_router6->GetNDevices()-1)->GetObject<PointToPointNetDevice> ();
								 std::shared_ptr<nfd::Face> face6 = ndn6->getFaceById(1);
								 std::shared_ptr<ndn::NetDeviceFace> device666=  std::dynamic_pointer_cast<ndn::NetDeviceFace>(face6);
								 Ptr<PointToPointNetDevice>p6 = StaticCast<PointToPointNetDevice> (device6);
								 m_queueFactory6.SetTypeId("ns3::DropTailQueue");
								 Config::SetDefault ("ns3::DropTailQueue::MaxPackets", UintegerValue (100));
								 Ptr<Queue> queuef6= m_queueFactory6.Create<Queue> ();
								 Ptr<ns3::DropTailQueue> ndnqueue6= StaticCast<ns3::DropTailQueue> (queuef6);
								 ndnqueue6->SetMode(ns3::DropTailQueue::QUEUE_MODE_PACKETS);
								 p6->SetQueue(queuef6);
								 Simulator::ScheduleNow (&QueueSizeStatus, queuef6,p6 );


			 // Creating queue core_router7

			 ObjectFactory m_queueFactory7;
			 Ptr<ndn::L3Protocol> ndn7 = core_router7->GetObject <ndn::L3Protocol> ();
			 Ptr<PointToPointNetDevice> device7 = core_router7->GetDevice(core_router7->GetNDevices()-1)->GetObject<PointToPointNetDevice> ();
			 std::shared_ptr<nfd::Face> face7 = ndn7->getFaceById(1);
			 std::shared_ptr<ndn::NetDeviceFace> device77=  std::dynamic_pointer_cast<ndn::NetDeviceFace>(face7);
			 Ptr<PointToPointNetDevice> p7 = StaticCast<PointToPointNetDevice> (device7);
			 m_queueFactory7.SetTypeId("ns3::DropTailQueue");
			 Config::SetDefault ("ns3::DropTailQueue::MaxPackets", UintegerValue (100));
			 Ptr<Queue> queuef7= m_queueFactory7.Create<Queue> ();
			 Ptr<ns3::DropTailQueue> ndnqueue7= StaticCast<ns3::DropTailQueue> (queuef7);
			 ndnqueue7->SetMode(ns3::DropTailQueue::QUEUE_MODE_PACKETS);
			 p7->SetQueue(queuef7);
			 Simulator::ScheduleNow (&QueueSizeStatus, queuef7, p7 );

			 // Creating queue core_router8

			 ObjectFactory m_queueFactory8;
			 Ptr<ndn::L3Protocol> ndn8 = core_router8->GetObject <ndn::L3Protocol> ();
			 Ptr<PointToPointNetDevice> device8 = core_router8->GetDevice(core_router8->GetNDevices()-1)->GetObject<PointToPointNetDevice> ();
			 std::shared_ptr<nfd::Face> face8= ndn8->getFaceById(1);
			 std::shared_ptr<ndn::NetDeviceFace> device88=  std::dynamic_pointer_cast<ndn::NetDeviceFace>(face8);
			 Ptr<PointToPointNetDevice> p8 = StaticCast<PointToPointNetDevice> (device8);
			 m_queueFactory8.SetTypeId("ns3::DropTailQueue");
			 Config::SetDefault ("ns3::DropTailQueue::MaxPackets", UintegerValue (100));
			 Ptr<Queue> queuef8= m_queueFactory8.Create<Queue> ();
			 Ptr<ns3::DropTailQueue> ndnqueue8= StaticCast<ns3::DropTailQueue> (queuef8);
			 ndnqueue8->SetMode(ns3::DropTailQueue::QUEUE_MODE_PACKETS);
			 p8->SetQueue(queuef8);
			 Simulator::ScheduleNow (&QueueSizeStatus, queuef8, p8);

			 // Creating queue core_router9

			 ObjectFactory m_queueFactory9;
			 Ptr<ndn::L3Protocol> ndn9 = core_router9->GetObject <ndn::L3Protocol> ();
			 Ptr<PointToPointNetDevice> device9 = core_router9->GetDevice(core_router9->GetNDevices()-1)->GetObject<PointToPointNetDevice> ();
			 std::shared_ptr<nfd::Face> face9= ndn9->getFaceById(1);
			 std::shared_ptr<ndn::NetDeviceFace> device99= std::dynamic_pointer_cast<ndn::NetDeviceFace>(face9);
			 Ptr<PointToPointNetDevice> p9= StaticCast<PointToPointNetDevice> (device9);
			 m_queueFactory9.SetTypeId("ns3::DropTailQueue");
			 Config::SetDefault ("ns3::DropTailQueue::MaxPackets", UintegerValue (100));
			 Ptr<Queue> queuef9= m_queueFactory9.Create<Queue> ();
			 Ptr<ns3::DropTailQueue> ndnqueue9= StaticCast<ns3::DropTailQueue> (queuef9);
			 ndnqueue9->SetMode(ns3::DropTailQueue::QUEUE_MODE_PACKETS);
			 p9->SetQueue(queuef9);
			 Simulator::ScheduleNow (&QueueSizeStatus, queuef9, p9);

			 // Creating queue core_router10

			 ObjectFactory m_queueFactory10;
			 Ptr<ndn::L3Protocol> ndn10 = core_router10->GetObject <ndn::L3Protocol> ();
			 Ptr<PointToPointNetDevice> device10 = core_router10->GetDevice(core_router10->GetNDevices()-1)->GetObject<PointToPointNetDevice> ();
			 std::shared_ptr<nfd::Face> face10= ndn10->getFaceById(1);
			 std::shared_ptr<ndn::NetDeviceFace> device100= std::dynamic_pointer_cast<ndn::NetDeviceFace>(face10);
			 Ptr<PointToPointNetDevice> p10= StaticCast<PointToPointNetDevice> (device10);
			 m_queueFactory10.SetTypeId("ns3::DropTailQueue");
			 Config::SetDefault ("ns3::DropTailQueue::MaxPackets", UintegerValue (100));
			 Ptr<Queue> queuef10= m_queueFactory10.Create<Queue> ();
			 Ptr<ns3::DropTailQueue> ndnqueue10= StaticCast<ns3::DropTailQueue> (queuef10);
			 ndnqueue10->SetMode(ns3::DropTailQueue::QUEUE_MODE_PACKETS);
			 p10->SetQueue(queuef10);
			 Simulator::ScheduleNow (&QueueSizeStatus, queuef10, p10);


// Creating queue core_router11

   ObjectFactory m_queueFactory11;
   Ptr<ndn::L3Protocol> ndn11 = core_router11->GetObject <ndn::L3Protocol> ();
   Ptr<PointToPointNetDevice> device112= core_router11->GetDevice(core_router11->GetNDevices()-1)->GetObject<PointToPointNetDevice> ();
   std::shared_ptr<nfd::Face> face11 = ndn11->getFaceById(1);
   std::shared_ptr<ndn::NetDeviceFace> device110=  std::dynamic_pointer_cast<ndn::NetDeviceFace>(face11);
   Ptr<PointToPointNetDevice>p11 = StaticCast<PointToPointNetDevice> (device112);
   m_queueFactory11.SetTypeId("ns3::DropTailQueue");
   Config::SetDefault ("ns3::DropTailQueue::MaxPackets", UintegerValue (100));
   Ptr<Queue> queuef11= m_queueFactory11.Create<Queue> ();
   Ptr<ns3::DropTailQueue> ndnqueue11 = StaticCast<ns3::DropTailQueue> (queuef11);
   ndnqueue11->SetMode(ns3::DropTailQueue::QUEUE_MODE_PACKETS);
   p11->SetQueue(queuef11);
   Simulator::ScheduleNow (&QueueSizeStatus, queuef11,p11 );


   // Creating queue core_router12

	 ObjectFactory m_queueFactory12;
	 Ptr<ndn::L3Protocol> ndn12 = core_router12->GetObject <ndn::L3Protocol> ();
	 Ptr<PointToPointNetDevice> device12 = core_router12->GetDevice(core_router12->GetNDevices()-1)->GetObject<PointToPointNetDevice> ();
	 std::shared_ptr<nfd::Face> face12 = ndn12->getFaceById(1);
	 std::shared_ptr<ndn::NetDeviceFace> device120=  std::dynamic_pointer_cast<ndn::NetDeviceFace>(face12);
	 Ptr<PointToPointNetDevice> p12 = StaticCast<PointToPointNetDevice> (device12);
	 m_queueFactory12.SetTypeId("ns3::DropTailQueue");
	 Config::SetDefault ("ns3::DropTailQueue::MaxPackets", UintegerValue (100));
	 Ptr<Queue> queuef12= m_queueFactory12.Create<Queue> ();
	 Ptr<ns3::DropTailQueue> ndnqueue12 = StaticCast<ns3::DropTailQueue> (queuef12);
	 ndnqueue12->SetMode(ns3::DropTailQueue::QUEUE_MODE_PACKETS);
	 p12->SetQueue(queuef12);
	 Simulator::ScheduleNow (&QueueSizeStatus, queuef12, p12 );

	  // Creating queue core_router13

	  ObjectFactory m_queueFactory13;
	  Ptr<ndn::L3Protocol> ndn13 = core_router13->GetObject <ndn::L3Protocol> ();
	  Ptr<PointToPointNetDevice> device13 = core_router13->GetDevice(core_router13->GetNDevices()-1)->GetObject<PointToPointNetDevice> ();
	  std::shared_ptr<nfd::Face> face13= ndn13->getFaceById(1);
	  std::shared_ptr<ndn::NetDeviceFace> device130=  std::dynamic_pointer_cast<ndn::NetDeviceFace>(face13);
	  Ptr<PointToPointNetDevice> p13 = StaticCast<PointToPointNetDevice> (device13);
	  m_queueFactory13.SetTypeId("ns3::DropTailQueue");
	  Config::SetDefault ("ns3::DropTailQueue::MaxPackets", UintegerValue (100));
	  Ptr<Queue> queuef13= m_queueFactory13.Create<Queue> ();
	  Ptr<ns3::DropTailQueue> ndnqueue13= StaticCast<ns3::DropTailQueue> (queuef13);
	  ndnqueue13->SetMode(ns3::DropTailQueue::QUEUE_MODE_PACKETS);
	  p13->SetQueue(queuef13);
	  Simulator::ScheduleNow (&QueueSizeStatus, queuef13,p13);

	  // Creating queue core_router14

		 ObjectFactory m_queueFactory14;
		Ptr<ndn::L3Protocol> ndn14 = core_router14->GetObject <ndn::L3Protocol> ();
		Ptr<PointToPointNetDevice> device14 = core_router14->GetDevice(core_router14->GetNDevices()-1)->GetObject<PointToPointNetDevice> ();
		std::shared_ptr<nfd::Face> face14= ndn14->getFaceById(1);
		std::shared_ptr<ndn::NetDeviceFace> device140= std::dynamic_pointer_cast<ndn::NetDeviceFace>(face14);
		Ptr<PointToPointNetDevice> p14= StaticCast<PointToPointNetDevice> (device14);
		m_queueFactory14.SetTypeId("ns3::DropTailQueue");
		Config::SetDefault ("ns3::DropTailQueue::MaxPackets", UintegerValue (100));
		Ptr<Queue> queuef14= m_queueFactory14.Create<Queue> ();
		Ptr<ns3::DropTailQueue> ndnqueue14= StaticCast<ns3::DropTailQueue> (queuef14);
		ndnqueue14->SetMode(ns3::DropTailQueue::QUEUE_MODE_PACKETS);
		p14->SetQueue(queuef14);
		Simulator::ScheduleNow (&QueueSizeStatus, queuef14, p14);

		// Creating queue core_router15

		 ObjectFactory m_queueFactory15;
		 Ptr<ndn::L3Protocol> ndn15 = core_router15->GetObject <ndn::L3Protocol> ();
		 Ptr<PointToPointNetDevice> device15 = core_router15->GetDevice(core_router15->GetNDevices()-1)->GetObject<PointToPointNetDevice> ();
		 std::shared_ptr<nfd::Face> face15= ndn15->getFaceById(1);
		 std::shared_ptr<ndn::NetDeviceFace> device150= std::dynamic_pointer_cast<ndn::NetDeviceFace>(face15);
		 Ptr<PointToPointNetDevice> p15= StaticCast<PointToPointNetDevice> (device15);
		 m_queueFactory15.SetTypeId("ns3::DropTailQueue");
		 Config::SetDefault ("ns3::DropTailQueue::MaxPackets", UintegerValue (100));
		 Ptr<Queue> queuef15= m_queueFactory15.Create<Queue> ();
		 Ptr<ns3::DropTailQueue> ndnqueue15= StaticCast<ns3::DropTailQueue> (queuef15);
		 ndnqueue15->SetMode(ns3::DropTailQueue::QUEUE_MODE_PACKETS);
		 p15->SetQueue(queuef15);
		 Simulator::ScheduleNow (&QueueSizeStatus, queuef15, p15);

// Creating queue core_router16

	   ObjectFactory m_queueFactory16;
	   Ptr<ndn::L3Protocol> ndn16 = core_router16->GetObject <ndn::L3Protocol> ();
	   Ptr<PointToPointNetDevice> device16 = core_router16->GetDevice(core_router16->GetNDevices()-1)->GetObject<PointToPointNetDevice> ();
	   std::shared_ptr<nfd::Face> face16 = ndn16->getFaceById(1);
	   std::shared_ptr<ndn::NetDeviceFace> device160=  std::dynamic_pointer_cast<ndn::NetDeviceFace>(face16);
	   Ptr<PointToPointNetDevice>p16 = StaticCast<PointToPointNetDevice> (device16);
	   m_queueFactory16.SetTypeId("ns3::DropTailQueue");
	   Config::SetDefault ("ns3::DropTailQueue::MaxPackets", UintegerValue (100));
	   Ptr<Queue> queuef16= m_queueFactory16.Create<Queue> ();
	   Ptr<ns3::DropTailQueue> ndnqueue16 = StaticCast<ns3::DropTailQueue> (queuef16);
	   ndnqueue16->SetMode(ns3::DropTailQueue::QUEUE_MODE_PACKETS);
	   p16->SetQueue(queuef16);
	   Simulator::ScheduleNow (&QueueSizeStatus, queuef16, p16 );


	   // Creating queue core_router17

		 ObjectFactory m_queueFactory17;
		 Ptr<ndn::L3Protocol> ndn17 = core_router17->GetObject <ndn::L3Protocol> ();
		 Ptr<PointToPointNetDevice> device17 = core_router17->GetDevice(core_router17->GetNDevices()-1)->GetObject<PointToPointNetDevice> ();
		 std::shared_ptr<nfd::Face> face17 = ndn17->getFaceById(1);
		 std::shared_ptr<ndn::NetDeviceFace> device170=  std::dynamic_pointer_cast<ndn::NetDeviceFace>(face17);
		 Ptr<PointToPointNetDevice> p17 = StaticCast<PointToPointNetDevice> (device17);
		 m_queueFactory17.SetTypeId("ns3::DropTailQueue");
		 Config::SetDefault ("ns3::DropTailQueue::MaxPackets", UintegerValue (100));
		 Ptr<Queue> queuef17= m_queueFactory17.Create<Queue> ();
		 Ptr<ns3::DropTailQueue> ndnqueue17 = StaticCast<ns3::DropTailQueue> (queuef17);
		 ndnqueue17->SetMode(ns3::DropTailQueue::QUEUE_MODE_PACKETS);
		 p17->SetQueue(queuef17);
		 Simulator::ScheduleNow (&QueueSizeStatus, queuef17,p17 );

		  // Creating queue core_router18

		  ObjectFactory m_queueFactory18;
		  Ptr<ndn::L3Protocol> ndn18 = core_router18->GetObject <ndn::L3Protocol> ();
		  Ptr<PointToPointNetDevice> device18 = core_router18->GetDevice(core_router18->GetNDevices()-1)->GetObject<PointToPointNetDevice> ();
		  std::shared_ptr<nfd::Face> face18= ndn18->getFaceById(1);
		  std::shared_ptr<ndn::NetDeviceFace> device180=  std::dynamic_pointer_cast<ndn::NetDeviceFace>(face18);
		  Ptr<PointToPointNetDevice> p18 = StaticCast<PointToPointNetDevice> (device18);
		  m_queueFactory18.SetTypeId("ns3::DropTailQueue");
		  Config::SetDefault ("ns3::DropTailQueue::MaxPackets", UintegerValue (100));
		  Ptr<Queue> queuef18= m_queueFactory18.Create<Queue> ();
		  Ptr<ns3::DropTailQueue> ndnqueue18= StaticCast<ns3::DropTailQueue> (queuef18);
		  ndnqueue18->SetMode(ns3::DropTailQueue::QUEUE_MODE_PACKETS);
		  p18->SetQueue(queuef18);
		  Simulator::ScheduleNow (&QueueSizeStatus, queuef18, p18);

		  // Creating queue core_router19

			 ObjectFactory m_queueFactory19;
			Ptr<ndn::L3Protocol> ndn19 = core_router19->GetObject <ndn::L3Protocol> ();
			Ptr<PointToPointNetDevice> device19 = core_router19->GetDevice(core_router19->GetNDevices()-1)->GetObject<PointToPointNetDevice> ();
			std::shared_ptr<nfd::Face> face19= ndn19->getFaceById(1);
			std::shared_ptr<ndn::NetDeviceFace> device190= std::dynamic_pointer_cast<ndn::NetDeviceFace>(face19);
			Ptr<PointToPointNetDevice> p19= StaticCast<PointToPointNetDevice> (device19);
			m_queueFactory19.SetTypeId("ns3::DropTailQueue");
			Config::SetDefault ("ns3::DropTailQueue::MaxPackets", UintegerValue (100));
			Ptr<Queue> queuef19= m_queueFactory19.Create<Queue> ();
			Ptr<ns3::DropTailQueue> ndnqueue19= StaticCast<ns3::DropTailQueue> (queuef19);
			ndnqueue19->SetMode(ns3::DropTailQueue::QUEUE_MODE_PACKETS);
			p19->SetQueue(queuef19);
			Simulator::ScheduleNow (&QueueSizeStatus, queuef19, p19);

			// Creating queue core_router20

			 ObjectFactory m_queueFactory20;
			 Ptr<ndn::L3Protocol> ndn20 = core_router20->GetObject <ndn::L3Protocol> ();
			 Ptr<PointToPointNetDevice> device20 = core_router20->GetDevice(core_router20->GetNDevices()-1)->GetObject<PointToPointNetDevice> ();
			 std::shared_ptr<nfd::Face> face20= ndn20->getFaceById(1);
			 std::shared_ptr<ndn::NetDeviceFace> device200= std::dynamic_pointer_cast<ndn::NetDeviceFace>(face20);
			 Ptr<PointToPointNetDevice> p20= StaticCast<PointToPointNetDevice> (device20);
			 m_queueFactory20.SetTypeId("ns3::DropTailQueue");
			 Config::SetDefault ("ns3::DropTailQueue::MaxPackets", UintegerValue (100));
			 Ptr<Queue> queuef20= m_queueFactory20.Create<Queue> ();
			 Ptr<ns3::DropTailQueue> ndnqueue20= StaticCast<ns3::DropTailQueue> (queuef20);
			 ndnqueue20->SetMode(ns3::DropTailQueue::QUEUE_MODE_PACKETS);
			 p20->SetQueue(queuef20);
			 Simulator::ScheduleNow (&QueueSizeStatus, queuef20, p20);

		// Creating queue core_router21

		 ObjectFactory m_queueFactory21;
		 Ptr<ndn::L3Protocol> ndn21 = core_router21->GetObject <ndn::L3Protocol> ();
		 Ptr<PointToPointNetDevice> device21 = core_router21->GetDevice(core_router21->GetNDevices()-1)->GetObject<PointToPointNetDevice> ();
		 std::shared_ptr<nfd::Face> face21= ndn21->getFaceById(1);
		 std::shared_ptr<ndn::NetDeviceFace> device210= std::dynamic_pointer_cast<ndn::NetDeviceFace>(face21);
		 Ptr<PointToPointNetDevice> p21= StaticCast<PointToPointNetDevice> (device21);
		 m_queueFactory21.SetTypeId("ns3::DropTailQueue");
		 Config::SetDefault ("ns3::DropTailQueue::MaxPackets", UintegerValue (100));
		 Ptr<Queue> queuef21= m_queueFactory21.Create<Queue> ();
		 Ptr<ns3::DropTailQueue> ndnqueue21= StaticCast<ns3::DropTailQueue> (queuef21);
		 ndnqueue21->SetMode(ns3::DropTailQueue::QUEUE_MODE_PACKETS);
		 p21->SetQueue(queuef21);
		 Simulator::ScheduleNow (&QueueSizeStatus, queuef21, p21);


			// Creating queue core_router22

			 ObjectFactory m_queueFactory22;
			 Ptr<ndn::L3Protocol> ndn22 = core_router22->GetObject <ndn::L3Protocol> ();
			 Ptr<PointToPointNetDevice> device222 = core_router22->GetDevice(core_router22->GetNDevices()-1)->GetObject<PointToPointNetDevice> ();
			 std::shared_ptr<nfd::Face> face22= ndn22->getFaceById(1);
			 std::shared_ptr<ndn::NetDeviceFace> device220= std::dynamic_pointer_cast<ndn::NetDeviceFace>(face22);
			 Ptr<PointToPointNetDevice> p22= StaticCast<PointToPointNetDevice> (device222);
			 m_queueFactory22.SetTypeId("ns3::DropTailQueue");
			 Config::SetDefault ("ns3::DropTailQueue::MaxPackets", UintegerValue (100));
			 Ptr<Queue> queuef22= m_queueFactory22.Create<Queue> ();
			 Ptr<ns3::DropTailQueue> ndnqueue22= StaticCast<ns3::DropTailQueue> (queuef22);
			 ndnqueue20->SetMode(ns3::DropTailQueue::QUEUE_MODE_PACKETS);
			 p22->SetQueue(queuef22);
			 Simulator::ScheduleNow (&QueueSizeStatus, queuef22, p22);


				// Creating queue core_router23

				 ObjectFactory m_queueFactory23;
				 Ptr<ndn::L3Protocol> ndn23 = core_router23->GetObject <ndn::L3Protocol> ();
				 Ptr<PointToPointNetDevice> device23 = core_router23->GetDevice(core_router23->GetNDevices()-1)->GetObject<PointToPointNetDevice> ();
				 std::shared_ptr<nfd::Face> face23= ndn23->getFaceById(1);
				 std::shared_ptr<ndn::NetDeviceFace> device233= std::dynamic_pointer_cast<ndn::NetDeviceFace>(face23);
				 Ptr<PointToPointNetDevice> p23= StaticCast<PointToPointNetDevice> (device23);
				 m_queueFactory23.SetTypeId("ns3::DropTailQueue");
				 Config::SetDefault ("ns3::DropTailQueue::MaxPackets", UintegerValue (100));
				 Ptr<Queue> queuef23= m_queueFactory23.Create<Queue> ();
				 Ptr<ns3::DropTailQueue> ndnqueue23= StaticCast<ns3::DropTailQueue> (queuef23);
				 ndnqueue23->SetMode(ns3::DropTailQueue::QUEUE_MODE_PACKETS);
				 p20->SetQueue(queuef23);
				 Simulator::ScheduleNow (&QueueSizeStatus, queuef23, p23);

////////////////////////////////////////////////////////////////////////////////////////////////



	   // install application helper

					   //FullyDistributed settings
					   ndn::AppHelper consumerHelper1("ns3::ndn::FullyDistributed");
					   consumerHelper1.SetAttribute ("Window", StringValue ("10"));
					   consumerHelper1.SetAttribute ("Size", DoubleValue(10000));
					   consumerHelper1.SetAttribute ("PayloadSize", UintegerValue (1040));


   ////////////////////////////////////////////////////////////////////////////////////////////////



		// that will express interests in /com/video namespace

		consumerHelper1.SetPrefix("/com/video");
		consumerHelper1.Install(consumer1);
		consumerHelper1.Install(consumer2);
		consumerHelper1.Install(consumer3);
		consumerHelper1.Install(consumer4);
		consumerHelper1.Install(consumer5);
		consumerHelper1.Install(consumer6);
		consumerHelper1.Install(consumer7);
		consumerHelper1.Install(consumer8);
		consumerHelper1.Install(consumer9);
		consumerHelper1.Install(consumer10);
		consumerHelper1.Install(consumer11);
		consumerHelper1.Install(consumer12);
		consumerHelper1.Install(consumer13);
		consumerHelper1.Install(consumer14);
		consumerHelper1.Install(consumer15);
		consumerHelper1.Install(consumer16);
		consumerHelper1.Install(consumer17);
		consumerHelper1.Install(consumer19);
		consumerHelper1.Install(consumer20);


// App for sending RSR
ndn::AppHelper app("NaccApp");
app.Install(core_router1);
app.Install(core_router2);
app.Install(core_router3);
app.Install(core_router4);
app.Install(core_router5);
app.Install(core_router6);;
app.Install(core_router7);
app.Install(core_router8);
app.Install(core_router9);
app.Install(core_router10);
app.Install(core_router11);
app.Install(core_router12);
app.Install(core_router13);
app.Install(core_router14);
app.Install(core_router15);
app.Install(core_router16);
app.Install(core_router17);
app.Install(core_router18);
app.Install(core_router19);
app.Install(core_router20);

// install producer that will satisfy Interests in /com/ccn
ndnGlobalRoutingHelper.AddOrigins("/com/video", cp1);
ndn::AppHelper producerHelper1("ns3::ndn::Producer");
producerHelper1.SetAttribute("PayloadSize", StringValue("1040"));
producerHelper1.SetPrefix("/com/video");
producerHelper1.Install(cp1);

//Calculate and install FIBs
ndn::GlobalRoutingHelper::CalculateRoutes();




// Drop tracer

L2RateTracer::InstallAll("drop-trace_GEANT2_myproposal.txt", Seconds(5.0));

// Rate tracer
ndn::L3RateTracer::InstallAll("rate-trace_GEANT2_myproposal.txt", Seconds(5.0));
/*
ndn::L3RateTracer::Install(consumer1,"rate-trace_consumer1.txt", Seconds(5.0));
ndn::L3RateTracer::Install(consumer2,"rate-trace_consumer2.txt", Seconds(5.0));
ndn::L3RateTracer::Install(consumer3,"rate-trace_consumer3.txt", Seconds(5.0));
ndn::L3RateTracer::Install(consumer4,"rate-trace_consumer4.txt", Seconds(5.0));
ndn::L3RateTracer::Install(consumer5,"rate-trace_consumer5.txt", Seconds(5.0));
ndn::L3RateTracer::Install(consumer6,"rate-trace_consumer6.txt", Seconds(5.0));
ndn::L3RateTracer::Install(consumer7,"rate-trace_consumer7.txt", Seconds(5.0));
ndn::L3RateTracer::Install(consumer8,"rate-trace_consumer8.txt", Seconds(5.0));
ndn::L3RateTracer::Install(consumer9,"rate-trace_consumer9.txt", Seconds(5.0));
ndn::L3RateTracer::Install(consumer10,"rate-trace_consumer10.txt", Seconds(5.0));
ndn::L3RateTracer::Install(consumer11,"rate-trace_consumer11.txt", Seconds(5.0));
ndn::L3RateTracer::Install(consumer12,"rate-trace_consumer12.txt", Seconds(5.0));
ndn::L3RateTracer::Install(consumer13,"rate-trace_consumer13.txt", Seconds(5.0));
ndn::L3RateTracer::Install(consumer14,"rate-trace_consumer14.txt", Seconds(5.0));
ndn::L3RateTracer::Install(consumer15,"rate-trace_consumer15.txt", Seconds(5.0));
ndn::L3RateTracer::Install(consumer16,"rate-trace_consumer16.txt", Seconds(5.0));
ndn::L3RateTracer::Install(consumer17,"rate-trace_consumer17.txt", Seconds(5.0));
ndn::L3RateTracer::Install(consumer18,"rate-trace_consumer18.txt", Seconds(5.0));
ndn::L3RateTracer::Install(consumer19,"rate-trace_consumer19.txt", Seconds(5.0));
ndn::L3RateTracer::Install(consumer20,"rate-trace_consumer20.txt", Seconds(5.0));
*/
// Delay Rate tracer
ndn::AppDelayTracer::InstallAll("Delay-trace_GEANT2_myproposal.txt");
/*
ndn::AppDelayTracer::Install(consumer1,"Delay-trace_consumer1.txt");
ndn::AppDelayTracer::Install(consumer2,"Delay-trace_consumer2.txt");
ndn::AppDelayTracer::Install(consumer3,"Delay-trace_consumer3.txt");
ndn::AppDelayTracer::Install(consumer4,"Delay-trace_consumer4.txt");
ndn::AppDelayTracer::Install(consumer5,"Delay-trace_consumer5.txt");
ndn::AppDelayTracer::Install(consumer6,"Delay-trace_consumer6.txt");
ndn::AppDelayTracer::Install(consumer7,"Delay-trace_consumer7.txt");
ndn::AppDelayTracer::Install(consumer8,"Delay-trace_consumer8.txt");
ndn::AppDelayTracer::Install(consumer9,"Delay-trace_consumer9.txt");
ndn::AppDelayTracer::Install(consumer10,"Delay-trace_consumer10.txt");
ndn::AppDelayTracer::Install(consumer11,"Delay-trace_consumer11.txt");
ndn::AppDelayTracer::Install(consumer12,"Delay-trace_consumer12.txt");
ndn::AppDelayTracer::Install(consumer13,"Delay-trace_consumer13.txt");
ndn::AppDelayTracer::Install(consumer14,"Delay-trace_consumer14.txt");
ndn::AppDelayTracer::Install(consumer15,"Delay-trace_consumer15.txt");
ndn::AppDelayTracer::Install(consumer16,"Delay-trace_consumer16.txt");
ndn::AppDelayTracer::Install(consumer17,"Delay-trace_consumer17.txt");
ndn::AppDelayTracer::Install(consumer18,"Delay-trace_consumer18.txt");
ndn::AppDelayTracer::Install(consumer19,"Delay-trace_consumer19.txt");
ndn::AppDelayTracer::Install(consumer20,"Delay-trace_consumer20.txt");
*/
Simulator::Stop(Seconds(50.0));
Simulator::Run();

return 0;
}

} // namespace ns3

int
main(int argc, char* argv[])
{
return ns3::main(argc, argv);

}
