/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2012 Tsinghua University, P.R.China
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
 * Author: Xiaoke Jiang <shock.jiang@gmail.com>
 */

#include "ndn-consumer-zipf-mandelbrot-39.h"

#include "ns3/ndn-app-face.h"
#include "ns3/ndn-interest.h"
#include "ns3/ndn-data.h"

#include "ns3/ndnSIM/utils/ndn-fw-hop-count-tag.h"

#include <math.h>


NS_LOG_COMPONENT_DEFINE ("ndn.ConsumerZipfMandelbrot39");

namespace ns3 {
namespace ndn {

NS_OBJECT_ENSURE_REGISTERED (ConsumerZipfMandelbrot39);

TypeId
ConsumerZipfMandelbrot39::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::ndn::ConsumerZipfMandelbrot39")
    .SetGroupName ("Ndn")
    .SetParent<ConsumerCbr> ()
    .AddConstructor<ConsumerZipfMandelbrot39> ()

    .AddAttribute ("NumberOfContents", "Number of the Contents in total",
                   StringValue ("100"),
                   MakeUintegerAccessor (&ConsumerZipfMandelbrot39::SetNumberOfContents, &ConsumerZipfMandelbrot39::GetNumberOfContents),
                   MakeUintegerChecker<uint32_t> ())

    .AddAttribute ("q", "parameter of improve rank",
                   StringValue ("0.7"),
                   MakeDoubleAccessor (&ConsumerZipfMandelbrot39::SetQ, &ConsumerZipfMandelbrot39::GetQ),
                   MakeDoubleChecker<double> ())
    
    .AddAttribute ("s", "parameter of power",
                   StringValue ("0.7"),
                   MakeDoubleAccessor (&ConsumerZipfMandelbrot39::SetS, &ConsumerZipfMandelbrot39::GetS),
                   MakeDoubleChecker<double> ())
    ;

  return tid;
}


ConsumerZipfMandelbrot39::ConsumerZipfMandelbrot39()
  : m_N (100) // needed here to make sure when SetQ/SetS are called, there is a valid value of N
  , m_q (0.7)
  , m_s (1.5)
  , m_SeqRng (0.0, 1.0)
{
   NS_LOG_INFO ("Entering ConsumerZipfMandelbrot3939 at Time: " << Simulator::Now ());
  // SetNumberOfContents is called by NS-3 object system during the initialization
   NS_LOG_INFO ("Leaving ConsumerZipfMandelbrot3939 at Time: " << Simulator::Now ());

   std::srand (3);

   //uint32_t numOfObjects = GetNumberOfContents();
   uint32_t numOfObjects = 10000;

   for (uint32_t i = 0; i < numOfObjects; i++)
        objectsToRepos[i] = i;
   

   //for (uint32_t i = 0; i < numOfObjects; i++)
        //std::cout << ' ' << objectsToRepos[i];

   //std::cout << '\n';

   // using built-in random generator:
   std::random_shuffle (&objectsToRepos[0], &objectsToRepos[numOfObjects]);

   //for (uint32_t i = 0; i < numOfObjects; i++) {
   //     std::cout << ' ' << objectsToRepos[i];
   //     if (i % 30 == 0 && i > 0)
   //             std::cout << '\n';        
   //}

   //std::cout << '\n';

   edgeNodes[0] = 8; edgeNodes[1] = 9; edgeNodes[2] = 10; edgeNodes[3] = 11; edgeNodes[4] = 12;
   edgeNodes[5] = 13; edgeNodes[6] = 14; edgeNodes[7] = 15; edgeNodes[8] = 16; edgeNodes[9] = 17;
   edgeNodes[10] = 18; edgeNodes[11] = 19; edgeNodes[12] = 20; edgeNodes[13] = 21; edgeNodes[14] = 22;
   edgeNodes[15] = 23; edgeNodes[16] = 24; edgeNodes[17] = 25; edgeNodes[18] = 26; edgeNodes[19] = 27;
   edgeNodes[20] = 28; edgeNodes[21] = 29; edgeNodes[22] = 30; edgeNodes[23] = 31; edgeNodes[24] = 32;    
   edgeNodes[25] = 33; edgeNodes[26] = 34; edgeNodes[27] = 35; edgeNodes[28] = 36; edgeNodes[29] = 37;
   edgeNodes[30] = 38; 
}

ConsumerZipfMandelbrot39::~ConsumerZipfMandelbrot39()
{
}

void
ConsumerZipfMandelbrot39::SetNumberOfContents (uint32_t numOfContents)
{
  m_N = numOfContents;

  NS_LOG_DEBUG (m_q << " and " << m_s << " and " << m_N);

  m_Pcum = std::vector<double> (m_N + 1);

  m_Pcum[0] = 0.0;
  for (uint32_t i=1; i<=m_N; i++)
    {
      m_Pcum[i] = m_Pcum[i-1] + 1.0 / std::pow(i+m_q, m_s);
    }

  for (uint32_t i=1; i<=m_N; i++)
    {
      m_Pcum[i] = m_Pcum[i] / m_Pcum[m_N];
      NS_LOG_LOGIC ("Cumulative probability [" << i << "]=" << m_Pcum[i]);
  }
}

uint32_t
ConsumerZipfMandelbrot39::GetNumberOfContents () const
{
  return m_N;
}

void
ConsumerZipfMandelbrot39::SetQ (double q)
{
  m_q = q;
  SetNumberOfContents (m_N);
}

double
ConsumerZipfMandelbrot39::GetQ () const
{
  return m_q;
}

void
ConsumerZipfMandelbrot39::SetS (double s)
{
  m_s = s;
  SetNumberOfContents (m_N);
}

double
ConsumerZipfMandelbrot39::GetS () const
{
  return m_s;
}

void
ConsumerZipfMandelbrot39::SendPacket() {
  if (!m_active) return;

  NS_LOG_FUNCTION_NOARGS ();

  uint32_t seq=std::numeric_limits<uint32_t>::max (); //invalid
  uint32_t objectId;

  // std::cout << Simulator::Now ().ToDouble (Time::S) << "s max -> " << m_seqMax << "\n";

  while (m_retxSeqs.size ())
    {
      seq = *m_retxSeqs.begin ();
      m_retxSeqs.erase (m_retxSeqs.begin ());
      objectId = m_objectSeqMapping[seq];
	  
      // NS_ASSERT (m_seqLifetimes.find (seq) != m_seqLifetimes.end ());
      // if (m_seqLifetimes.find (seq)->time <= Simulator::Now ())
      //   {

      //     NS_LOG_DEBUG ("Expire " << seq);
      //     m_seqLifetimes.erase (seq); // lifetime expired. Trying to find another unexpired sequence number
      //     continue;
      //   }
      NS_LOG_DEBUG("=interest seq "<<seq<<" from m_retxSeqs");
      break;
    }

  if (seq == std::numeric_limits<uint32_t>::max ()) //no retransmission
    {
      if (m_seqMax != std::numeric_limits<uint32_t>::max ())
        {
          if (m_seq >= m_seqMax)
            {
              return; // we are totally done
            }
        }

      objectId = ConsumerZipfMandelbrot39::GetNextSeq();

      /* The SrcNode and Owning Node should be different */
      while ((int(m_face->GetNode()->GetId())) == edgeNodes[objectsToRepos[objectId] % 31]) {
           objectId = ConsumerZipfMandelbrot39::GetNextSeq();          
      }

      seq = m_seq++;
      m_objectSeqMapping[seq] = objectId;
    }

  std::string prefix = "/";
  //std::cout <<"Node ID: " << (edgeNodes[objectsToRepos[objectId] % 31]) << " ObjectId: "<<objectId
              //<<" objectsToRepos[objectId]: "<<objectsToRepos[objectId]<<"\n";

  std::stringstream ss;
  ss << (edgeNodes[objectsToRepos[objectId] % 31]);
  std::string ownernode = ss.str();

  prefix = prefix.append(ownernode);

  //std::cout <<"Interest name: " << prefix << "\n";
  
  Ptr<Name> nameWithSequence = Create<Name> (prefix);
  nameWithSequence->appendSeqNum (objectId);

  //std::cout<<"ObjectID: "<<*nameWithSequence<<std::endl;

  //std::cout <<"REQUEST GENERATED AT REQ NODE: Node ID: " << GetNode()->GetId() << " ObjectId: "<<*nameWithSequence<<std::endl;

  Ptr<Interest> interest = Create<Interest> ();
  interest->SetNonce (m_rand.GetValue ());
  interest->SetName  (nameWithSequence);

  // NS_LOG_INFO ("Requesting Interest: \n" << *interest);
  NS_LOG_INFO ("> Interest for " << objectId<<", Total: "<<m_seq<<", face: "<<m_face->GetId());
  NS_LOG_DEBUG ("Trying to add " << objectId << " with " << Simulator::Now () << ". already " << m_seqTimeouts.size () << " items");

  m_seqTimeouts.insert (SeqTimeout (objectId, Simulator::Now ()));
  m_seqFullDelay.insert (SeqTimeout (objectId, Simulator::Now ()));

  m_seqLastDelay.erase (objectId);
  m_seqLastDelay.insert (SeqTimeout (objectId, Simulator::Now ()));

  m_seqRetxCounts[objectId] ++;

  m_rtt->SentSeq (SequenceNumber32 (objectId), 1);

  FwHopCountTag hopCountTag;
  interest->GetPayload ()->AddPacketTag (hopCountTag);

  m_transmittedInterests (interest, this, m_face);
  m_face->ReceiveInterest (interest);

  ConsumerZipfMandelbrot39::ScheduleNextPacket ();
}


uint32_t
ConsumerZipfMandelbrot39::GetNextSeq()
{
  uint32_t content_index = 1; //[1, m_N]
  double p_sum = 0;

  double p_random = m_SeqRng.GetValue();
  while (p_random == 0)
    {
      p_random = m_SeqRng.GetValue();
    }
  //if (p_random == 0)
  NS_LOG_LOGIC("p_random="<<p_random);
  for (uint32_t i=1; i<=m_N; i++)
    {
      p_sum = m_Pcum[i];   //m_Pcum[i] = m_Pcum[i-1] + p[i], p[0] = 0;   e.g.: p_cum[1] = p[1], p_cum[2] = p[1] + p[2]
      if (p_random <= p_sum)
        {
          content_index = i;
          break;
        } //if
    } //for
  //content_index = 1;
  NS_LOG_DEBUG("RandomNumber="<<content_index);
  return content_index;
}

void
ConsumerZipfMandelbrot39::ScheduleNextPacket() {

  if (m_firstTime)
    {
      m_sendEvent = Simulator::Schedule (Seconds (0.0),
                                         &ConsumerZipfMandelbrot39::SendPacket, this);
      m_firstTime = false;
    }
  else if (!m_sendEvent.IsRunning ())
    m_sendEvent = Simulator::Schedule (
                                       (m_random == 0) ?
                                       Seconds(1.0 / m_frequency)
                                       :
                                       Seconds(m_random->GetValue ()),
                                       &ConsumerZipfMandelbrot39::SendPacket, this);
}

} /* namespace ndn */
} /* namespace ns3 */
