#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/ndnSIM-module.h"

using namespace ns3;

int main (int argc, char *argv[])
{
	CommandLine cmd;
	cmd.Parse (argc, argv);

	AnnotatedTopologyReader topologyReader ("", 25);
	topologyReader.SetFileName ("src/ndnSIM/examples/topologies/A1-topo.txt");
	topologyReader.Read ();

	// Install NDN stack on all nodes
	ndn::StackHelper ndnHelper;
	ndnHelper.SetForwardingStrategy ("ns3::ndn::fw::BestRoute");
	ndnHelper.SetContentStore ("ns3::ndn::cs::Lru",
			"MaxSize", "3");
	ndnHelper.Install (Names::Find<Node> ("A"));
	ndnHelper.Install (Names::Find<Node> ("B")); 

	ndnHelper.SetContentStore ("ns3::ndn::cs::Lru",
			"MaxSize", "0");
	ndnHelper.Install (Names::Find<Node> ("C1"));
	ndnHelper.Install (Names::Find<Node> ("C2"));
	ndnHelper.Install (Names::Find<Node> ("P1"));
	ndnHelper.Install (Names::Find<Node> ("P2"));

	// Installing global routing interface on all nodes
	ndn::GlobalRoutingHelper ndnGlobalRoutingHelper;
	ndnGlobalRoutingHelper.InstallAll ();
	//ndnGlobalRoutingHelper.Install (Names::Find<Node> ("A"));
	//ndnGlobalRoutingHelper.Install (Names::Find<Node> ("B"));

	// Getting containers for the consumer/producer
	Ptr<Node> consumer1 = Names::Find<Node> ("C1");
	Ptr<Node> consumer2 = Names::Find<Node> ("C2");

	Ptr<Node> producer1 = Names::Find<Node> ("P1");
	Ptr<Node> producer2 = Names::Find<Node> ("P2");

	// Setting up helperts for Consumer 1
	ndn::AppHelper consumerHelperC11 ("ns3::ndn::ConsumerCbr");
	consumerHelperC11.SetAttribute ("Frequency", StringValue ("1"));
	//consumerHelperC11.SetAttribute ("StartSeq", StringValue ("0.0"));  
	consumerHelperC11.SetPrefix ("/P1");
	//Schedule Consumer as per specification
	ApplicationContainer consumer = consumerHelperC11.Install (consumer1);
	consumer.Start (Seconds (0.0));

	ndn::AppHelper consumerHelperC12 ("ns3::ndn::ConsumerCbr");
	consumerHelperC12.SetAttribute ("Frequency", StringValue ("1")); 
	//consumerHelperC11.SetAttribute ("StartSeq", StringValue ("1.0"));  
	consumerHelperC12.SetPrefix ("/P2");
	//Schedule Consumer as per specification
	consumer = consumerHelperC12.Install (consumer1);
	consumer.Start (Seconds (1.0));

	// Setting up helperts for Consumer 2	
	ndn::AppHelper consumerHelperC21 ("ns3::ndn::ConsumerCbr");
	consumerHelperC21.SetAttribute ("Frequency", StringValue ("1")); 
	//consumerHelperC11.SetAttribute ("StartSeq", StringValue ("0.0"));  
	consumerHelperC21.SetPrefix ("/P2");
	//Schedule Consumer as per specification
	consumer = consumerHelperC21.Install (consumer2);
	consumer.Start (Seconds (0.0));

	ndn::AppHelper consumerHelperC22 ("ns3::ndn::ConsumerCbr");
	consumerHelperC22.SetAttribute ("Frequency", StringValue ("1")); 
	//consumerHelperC11.SetAttribute ("StartSeq", StringValue ("1.0"));  
	consumerHelperC22.SetPrefix ("/P1");
	//Schedule Consumer as per specification
	consumer = consumerHelperC22.Install (consumer2);
	consumer.Start (Seconds (1.0));


	ndn::AppHelper producerHelper ("ns3::ndn::Producer");
	producerHelper.SetAttribute ("PayloadSize", StringValue("1024"));  

	// Register /P1 prefix with global routing controller and
	// install producer that will satisfy Interests in /dst1 namespace
	ndnGlobalRoutingHelper.AddOrigins ("/P1", producer1);
	producerHelper.SetPrefix ("/P1");
	producerHelper.Install (producer1);

	// Register /P2 prefix with global routing controller and
	// install producer that will satisfy Interests in /dst2 namespace
	ndnGlobalRoutingHelper.AddOrigins ("/P2", producer2);
	producerHelper.SetPrefix ("/P2");
	producerHelper.Install (producer2);

	// Calculate and install FIBs
	ndn::GlobalRoutingHelper::CalculateRoutes ();

	Simulator::Stop (Seconds (5.0));

	ndn::AppDelayTracer::InstallAll ("app-delays-trace.txt");	
	ndn::CsTracer::InstallAll ("cs-trace.txt", Seconds (1));
		
	Simulator::Run ();
	Simulator::Destroy ();

	return 0;
}
