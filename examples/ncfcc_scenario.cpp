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
#include "ns3/flow-monitor-helper.h"
#include "ns3/node.h"
#include "ns3/packet.h"
#include "ns3/core-module.h"
#include "ns3/ndnSIM-module.h"
#include "ns3/network-module.h"
#include "ns3/flow-monitor-helper.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/traced-callback.h"
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
	/*
		if(ndnqueue != NULL)
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


int
main(int argc, char* argv[])
{
  CommandLine cmd;
  cmd.Parse (argc, argv);
  bool writeForPlot = true;
  bool printStats = true;
  cmd.AddValue ("wfp", "<0/1> to write results for plot (gnuplot)", writeForPlot);
  std::string pathOut;
  pathOut = "."; // Current directory
  cmd.AddValue ("pathOut", "Path to save results from --writeForPlot/--writePcap/--writeFlowMonitor", pathOut);

  AnnotatedTopologyReader topologyReader("", 25);
  topologyReader.SetFileName("src/ndnSIM/examples/topologies/ncfcc_scenario.txt");
  Config::SetDefault ("ns3::DropTailQueue::MaxPackets", StringValue ("100"));
  topologyReader.Read();

  // Install NDN stack on all nodes
  ndn::StackHelper ndnHelper;
  ndnHelper.SetOldContentStore("ns3::ndn::cs::Lru", "MaxSize", "1000");
  uint32_t max_buffer = 100;
  uint32_t min_threshold = max_buffer/2;
  ndnHelper.MemoryEfficient(true, max_buffer, min_threshold);
  ndnHelper.Popularity_based(false);
  ndnHelper.InstallAll();

  // Choosing forwarding strategy

  ndn::StrategyChoiceHelper::InstallAll("/com/news", "/localhost/nfd/strategy/multicast");
  ndn::StrategyChoiceHelper::InstallAll("/com/video", "/localhost/nfd/strategy/multicast");
  ndn::StrategyChoiceHelper::InstallAll("/com/ccn", "/localhost/nfd/strategy/multicast");

  //ndn::StrategyChoiceHelper::InstallAll("/com/news", "/localhost/nfd/strategy/best-route");
  //ndn::StrategyChoiceHelper::InstallAll("/com/video", "/localhost/nfd/strategy/best-route");
  //ndn::StrategyChoiceHelper::InstallAll("/com/ccn", "/localhost/nfd/strategy/best-route");

  //ndn::StrategyChoiceHelper::InstallAll("/com/news", "/localhost/nfd/strategy/ncc");
 //ndn::StrategyChoiceHelper::InstallAll("/com/video", "/localhost/nfd/strategy/ncc");
 //ndn::StrategyChoiceHelper::InstallAll("/com/ccn", "/localhost/nfd/strategy/ncc");


 // ndn::StrategyChoiceHelper::InstallAll("/com/news", "/localhost/nfd/strategy/client-control");
  //ndn::StrategyChoiceHelper::InstallAll("/com/video", "/localhost/nfd/strategy/client-control");
  //ndn::StrategyChoiceHelper::InstallAll("/com/ccn", "/localhost/nfd/strategy/client-control");


   //Installing global routing interface on all nodes
   //ndn::GlobalRoutingHelper ndnGlobalRoutingHelper;
   //ndnGlobalRoutingHelper.InstallAll();

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

  Ptr<Node> access_router1 = Names::Find<Node>("A_R1");
  Ptr<Node> access_router2  = Names::Find<Node>("A_R2");

  Ptr<Node> core_router1= Names::Find<Node>("R1");
  Ptr<Node> core_router2= Names::Find<Node>("R2");
  Ptr<Node> cp1 = Names::Find<Node>("CP1");
  Ptr<Node> cp2 = Names::Find<Node>("CP2");
  Ptr<Node> cp3 = Names::Find<Node>("CP3");

          /////////////////////////////////////////////////////////////////////////////////////////////////////

  // Install the queue model in all the nodes

               // Creating queue access_router1

                    ObjectFactory m_queueFactory1;
                    Ptr<ndn::L3Protocol> ndn1 = access_router1->GetObject <ndn::L3Protocol> ();
                    Ptr<PointToPointNetDevice> device1 = access_router1->GetDevice(access_router1->GetNDevices()-1)->GetObject<PointToPointNetDevice> ();
                    std::shared_ptr<nfd::Face> face1 = ndn1->getFaceById(1);
                    std::shared_ptr<ndn::NetDeviceFace> device11=  std::dynamic_pointer_cast<ndn::NetDeviceFace>(face1);
                    Ptr<PointToPointNetDevice> p2p1 = StaticCast<PointToPointNetDevice> (device1);
                    m_queueFactory1.SetTypeId("ns3::DropTailQueue");
                    Config::SetDefault ("ns3::DropTailQueue::MaxPackets", UintegerValue (100));
                    Ptr<Queue> queuef1= m_queueFactory1.Create<Queue> ();
                    Ptr<ns3::DropTailQueue> ndnqueue1 = StaticCast<ns3::DropTailQueue> (queuef1);
                    ndnqueue1->SetMode(ns3::DropTailQueue::QUEUE_MODE_PACKETS);
                    p2p1->SetQueue(queuef1);
                    Simulator::ScheduleNow (&QueueSizeStatus, queuef1,p2p1 );


               // Creating queue in access_router2

                    ObjectFactory m_queueFactory2;
                    Ptr<ndn::L3Protocol> ndn2 = access_router2->GetObject <ndn::L3Protocol> ();
                    Ptr<PointToPointNetDevice> device2 = access_router2->GetDevice(access_router2->GetNDevices()-1)->GetObject<PointToPointNetDevice> ();
                    std::shared_ptr<nfd::Face> face2 = ndn2->getFaceById(1);
                    std::shared_ptr<ndn::NetDeviceFace> device22=  std::dynamic_pointer_cast<ndn::NetDeviceFace>(face2);
                    Ptr<PointToPointNetDevice> p2p2 =DynamicCast<PointToPointNetDevice>(device2);
                    m_queueFactory2.SetTypeId("ns3::DropTailQueue");
                    Config::SetDefault ("ns3::DropTailQueue::MaxPackets", UintegerValue (100));
                    Ptr<Queue> queuef2= m_queueFactory2.Create<Queue> ();
                    Ptr<ns3::DropTailQueue> ndnqueue2 = StaticCast<ns3::DropTailQueue> (queuef2);
                    ndnqueue2->SetMode(ns3::DropTailQueue::QUEUE_MODE_PACKETS);
                    p2p2->SetQueue(queuef2);
                    Simulator::ScheduleNow (&QueueSizeStatus, queuef2,p2p2 );

               // Creating queue in core_router1

                     ObjectFactory m_queueFactory3;
                     Ptr<ndn::L3Protocol> ndn3 = core_router1->GetObject <ndn::L3Protocol> ();
                     Ptr<PointToPointNetDevice> device3 = core_router1->GetDevice(core_router1->GetNDevices()-1)->GetObject<PointToPointNetDevice> ();
                     std::shared_ptr<nfd::Face> face3 = ndn3->getFaceById(1);
                     std::shared_ptr<ndn::NetDeviceFace> device33=  std::dynamic_pointer_cast<ndn::NetDeviceFace>(face3);
                     Ptr<PointToPointNetDevice> p2p3 =DynamicCast<PointToPointNetDevice>(device3);
                     m_queueFactory3.SetTypeId("ns3::DropTailQueue");
                     Config::SetDefault ("ns3::DropTailQueue::MaxPackets", UintegerValue (100));
                     Ptr<Queue> queuef3= m_queueFactory3.Create<Queue> ();
                     Ptr<ns3::DropTailQueue> ndnqueue3 = StaticCast<ns3::DropTailQueue> (queuef3);
                     ndnqueue3->SetMode(ns3::DropTailQueue::QUEUE_MODE_PACKETS);
                     p2p3->SetQueue(queuef3);
                     Simulator::ScheduleNow (&QueueSizeStatus, queuef3,p2p3);


                 // Creating queue in core_router2

                     ObjectFactory m_queueFactory4;
                     Ptr<ndn::L3Protocol> ndn4 = core_router2->GetObject <ndn::L3Protocol> ();
                     Ptr<PointToPointNetDevice> device4 = core_router2->GetDevice(core_router2->GetNDevices()-1)->GetObject<PointToPointNetDevice> ();
                     std::shared_ptr<nfd::Face> face4 = ndn3->getFaceById(1);
                     std::shared_ptr<ndn::NetDeviceFace> device44=  std::dynamic_pointer_cast<ndn::NetDeviceFace>(face3);
                     Ptr<PointToPointNetDevice> p2p4 =DynamicCast<PointToPointNetDevice>(device4);
                     m_queueFactory4.SetTypeId("ns3::DropTailQueue");
                     Config::SetDefault ("ns3::DropTailQueue::MaxPackets", UintegerValue (100));
                     Ptr<Queue> queuef4= m_queueFactory4.Create<Queue> ();
                     Ptr<ns3::DropTailQueue> ndnqueue4 = StaticCast<ns3::DropTailQueue> (queuef4);
                     ndnqueue4->SetMode(ns3::DropTailQueue::QUEUE_MODE_PACKETS);
                     p2p3->SetQueue(queuef4);
                     Simulator::ScheduleNow (&QueueSizeStatus, queuef4,p2p4);




          ////////////////////////////////////////////////////////////////////////////////////////////////

                     // install application helper



                                    // FullyDistributed settings
                                     ndn::AppHelper consumerHelper1("ns3::ndn::FullyDistributed");
                                     consumerHelper1.SetAttribute ("Window", StringValue ("10"));
                                     consumerHelper1.SetAttribute ("Size", DoubleValue (10000));
                                     consumerHelper1.SetAttribute ("PayloadSize", UintegerValue (1040));

                 ////////////////////////////////////////////////////////////////////////////////////////////////



                // that will express interests in /com/ccn namespace
                consumerHelper1.SetPrefix("/com/ccn");
                consumerHelper1.Install(consumer1);
                consumerHelper1.Install(consumer2);
                consumerHelper1.Install(consumer3);
                consumerHelper1.SetPrefix("/com/news");
                consumerHelper1.Install(consumer4);
                consumerHelper1.Install(consumer5);
                consumerHelper1.Install(consumer6);
                consumerHelper1.Install(consumer7);
                consumerHelper1.SetPrefix("/com/video");
                consumerHelper1.Install(consumer8);
                consumerHelper1.Install(consumer9);
                consumerHelper1.Install(consumer10);


 // App for sending RSR
   ndn::AppHelper app("NaccApp");
   app.Install(access_router1);
   app.Install(access_router2);
   app.Install(core_router1);
   app.Install(core_router2);

   // install producer that will satisfy Interests in /com/ccn
   // ndnGlobalRoutingHelper.AddOrigins("/com/ccn", cp1);
     ndn::AppHelper producerHelper1("ns3::ndn::Producer");
     producerHelper1.SetAttribute("PayloadSize", StringValue("1040"));
     producerHelper1.SetPrefix("/com/ccn");
     producerHelper1.Install(cp1);
     producerHelper1.SetPrefix("/com/news");
     producerHelper1.Install(cp2);
     producerHelper1.SetPrefix("/com/video");
     producerHelper1.Install(cp3);

      //Calculate and install FIBs
      // ndn::GlobalRoutingHelper::CalculateRoutes();

  // Manually configure FIB routes

   ndn::FibHelper::AddRoute("Src1", "/com/ccn", "A_R1", 1);
   ndn::FibHelper::AddRoute("Src2", "/com/ccn", "A_R1", 1);
   ndn::FibHelper::AddRoute("Src3", "/com/ccn", "A_R1", 1);
   ndn::FibHelper::AddRoute("Src4", "/com/news", "A_R1", 1);
   ndn::FibHelper::AddRoute("Src5", "/com/news", "A_R1", 1);
   ndn::FibHelper::AddRoute("Src6", "/com/news", "A_R2", 1);
   ndn::FibHelper::AddRoute("Src7", "/com/news", "A_R2", 1);
   ndn::FibHelper::AddRoute("Src8", "/com/video", "A_R2", 1);
   ndn::FibHelper::AddRoute("Src9", "/com/video", "A_R2", 1);
   ndn::FibHelper::AddRoute("Src10", "/com/video", "A_R2", 1);


   ndn::FibHelper::AddRoute("A_R1", "/com/ccn", "R1", 1);
   ndn::FibHelper::AddRoute("A_R1", "/com/news", "R1", 1);
   ndn::FibHelper::AddRoute("A_R1", "/com/video", "R1", 1);

   ndn::FibHelper::AddRoute("A_R1", "/com/ccn", "A_R2", 1);
    ndn::FibHelper::AddRoute("A_R1", "/com/news", "A_R2", 1);
    ndn::FibHelper::AddRoute("A_R1", "/com/video", "A_R2", 1);

   ndn::FibHelper::AddRoute("A_R2", "/com/ccn", "R1", 1);
   ndn::FibHelper::AddRoute("A_R2", "/com/news", "R1", 1);
   ndn::FibHelper::AddRoute("A_R2", "/com/video", "R1", 1);

   ndn::FibHelper::AddRoute("A_R2", "/com/ccn", "R2", 1);
   ndn::FibHelper::AddRoute("A_R2", "/com/news", "R2", 1);
   ndn::FibHelper::AddRoute("A_R2", "/com/video", "R2", 1);

   ndn::FibHelper::AddRoute("R1", "/com/ccn", "CP1", 1);
   ndn::FibHelper::AddRoute("R1", "/com/news", "CP2", 1);
   ndn::FibHelper::AddRoute("R1", "/com/video", "CP3", 1);

   ndn::FibHelper::AddRoute("R2", "/com/ccn", "CP1", 1);
   ndn::FibHelper::AddRoute("R2", "/com/news", "CP2", 1);
   ndn::FibHelper::AddRoute("R2", "/com/video", "CP3", 1);


   //  Reduce Sending Rate
   ndn::FibHelper::AddRoute("R1", "/RSR", "A_R1", 1);
   ndn::FibHelper::AddRoute("R1", "/RSR", "A_R2", 1);
   ndn::FibHelper::AddRoute("R2", "/RSR", "A_R2", 1);

   ndn::FibHelper::AddRoute("A_R1", "/RSR", "Src1", 1);
   ndn::FibHelper::AddRoute("A_R1", "/RSR", "Src2", 1);
   ndn::FibHelper::AddRoute("A_R1", "/RSR", "Src3", 1);
   ndn::FibHelper::AddRoute("A_R1", "/RSR", "Src4", 1);
   ndn::FibHelper::AddRoute("A_R1", "/RSR", "Src5", 1);

   ndn::FibHelper::AddRoute("A_R2", "/RSR", "Src6", 1);
   ndn::FibHelper::AddRoute("A_R2", "/RSR", "Src7", 1);
   ndn::FibHelper::AddRoute("A_R2", "/RSR", "Src8", 1);
   ndn::FibHelper::AddRoute("A_R2", "/RSR", "Src9", 1);
   ndn::FibHelper::AddRoute("A_R2", "/RSR", "Src10", 1);


    // Drop tracer

     L2RateTracer::InstallAll("drop-trace_myproposal.txt", Seconds(5.0));

     // Rate tracer

     ndn::L3RateTracer::InstallAll("rate-trace_myproposal.txt", Seconds(5.0));

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
*/
     // Delay Rate tracer
     ndn::AppDelayTracer::InstallAll("Delay-trace_myproposal.txt");

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
*/

  Simulator::Stop(Seconds(50.0));
  Simulator::Run();

  Simulator::Destroy();
  return 0;
}

} // namespace ns3

int
main(int argc, char* argv[])
{
  return ns3::main(argc, argv);

}
