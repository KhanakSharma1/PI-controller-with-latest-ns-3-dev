/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2016 NITK Surathkal
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
 * Authors: Priya S Tavarmani <priyast663@gmail.com>
 *          Viyom Mittal <viyommittal@gmail.com>
 *          Mohit P. Tahiliani <tahiliani@nitk.edu.in>
 *
 */

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/traffic-control-module.h"

using namespace ns3;

class PiQueueDiscTestItem : public QueueDiscItem {
public:
  PiQueueDiscTestItem (Ptr<Packet> p, const Address & addr, uint16_t protocol);
  virtual ~PiQueueDiscTestItem ();
  virtual void AddHeader (void);
  virtual bool Mark(void);

private:
  PiQueueDiscTestItem ();
  PiQueueDiscTestItem (const PiQueueDiscTestItem &);
  PiQueueDiscTestItem &operator = (const PiQueueDiscTestItem &);
};

PiQueueDiscTestItem::PiQueueDiscTestItem (Ptr<Packet> p, const Address & addr, uint16_t protocol)
  : QueueDiscItem (p, addr, protocol)
{
}

PiQueueDiscTestItem::~PiQueueDiscTestItem ()
{
}

void
PiQueueDiscTestItem::AddHeader (void)
{
}
bool
PiQueueDiscTestItem::Mark (void)
{
  
  return false;
}
class PiQueueDiscTestCase : public TestCase
{
public:
  PiQueueDiscTestCase ();
  virtual void DoRun (void);
private:
  void Enqueue (Ptr<PiQueueDisc> queue, uint32_t size, uint32_t nPkt);
  void EnqueueWithDelay (Ptr<PiQueueDisc> queue, uint32_t size, uint32_t nPkt);
  void RunPiTest (QueueSizeUnit mode);
};

PiQueueDiscTestCase::PiQueueDiscTestCase ()
  : TestCase ("Sanity check on the PI queue disc implementation")
{
}

void
PiQueueDiscTestCase::RunPiTest (QueueSizeUnit mode)
{
  uint32_t pktSize = 0;
  // 1 for packets; pktSize for bytes
  uint32_t modeSize = 1;
  uint32_t qSize = 8;
  Ptr<PiQueueDisc> queue = CreateObject<PiQueueDisc> ();

  // test 1: simple enqueue/dequeue with no drops
  
  NS_TEST_EXPECT_MSG_EQ (queue->SetAttributeFailSafe ("MaxSize", QueueSizeValue (QueueSize (mode, qSize))),
                         true, "Verify that we can actually set the attribute MaxSize");
  NS_TEST_EXPECT_MSG_EQ (queue->SetAttributeFailSafe ("A", DoubleValue (0.125)), true,
                         "Verify that we can actually set the attribute A");
  NS_TEST_EXPECT_MSG_EQ (queue->SetAttributeFailSafe ("B", DoubleValue (1.25)), true,
                         "Verify that we can actually set the attribute B");
  NS_TEST_EXPECT_MSG_EQ (queue->SetAttributeFailSafe ("MeanPktSize", UintegerValue (1000)), true,
                         "Verify that we can actually set the attribute MeanPktSize");
  NS_TEST_EXPECT_MSG_EQ (queue->SetAttributeFailSafe ("QueueRef", DoubleValue (4)), true,
                         "Verify that we can actually set the attribute QueueRef");
  NS_TEST_EXPECT_MSG_EQ (queue->SetAttributeFailSafe ("W", DoubleValue (125)), true,
                         "Verify that we can actually set the attribute W");

  Address dest;

  if (mode == QueueSizeUnit::BYTES)
    {
      pktSize = 500;
      modeSize = pktSize;
      queue->SetMaxSize (QueueSize (mode, qSize * modeSize));
    }

  Ptr<Packet> p1, p2, p3, p4, p5, p6, p7, p8;
  p1 = Create<Packet> (pktSize);
  p2 = Create<Packet> (pktSize);
  p3 = Create<Packet> (pktSize);
  p4 = Create<Packet> (pktSize);
  p5 = Create<Packet> (pktSize);
  p6 = Create<Packet> (pktSize);
  p7 = Create<Packet> (pktSize);
  p8 = Create<Packet> (pktSize);

  queue->Initialize ();
  NS_TEST_EXPECT_MSG_EQ (queue->GetCurrentSize ().GetValue (), 0 * modeSize, "There should be no packets in there");
  queue->Enqueue (Create<PiQueueDiscTestItem> (p1, dest, false));
  NS_TEST_EXPECT_MSG_EQ (queue->GetCurrentSize ().GetValue (), 1 * modeSize, "There should be one packet in there");
  queue->Enqueue (Create<PiQueueDiscTestItem> (p2, dest, false));
  NS_TEST_EXPECT_MSG_EQ (queue->GetCurrentSize ().GetValue (), 2 * modeSize, "There should be two packets in there");
  queue->Enqueue (Create<PiQueueDiscTestItem> (p3, dest, false));
  queue->Enqueue (Create<PiQueueDiscTestItem> (p4, dest, false));
  queue->Enqueue (Create<PiQueueDiscTestItem> (p5, dest, false));
  queue->Enqueue (Create<PiQueueDiscTestItem> (p6, dest, false));
  queue->Enqueue (Create<PiQueueDiscTestItem> (p7, dest, false));
  queue->Enqueue (Create<PiQueueDiscTestItem> (p8, dest, false));
  NS_TEST_EXPECT_MSG_EQ (queue->GetCurrentSize ().GetValue (), 8 * modeSize, "There should be eight packets in there");

  Ptr<QueueDiscItem> item;

  item = queue->Dequeue ();
  NS_TEST_EXPECT_MSG_EQ ((item != 0), true, "I want to remove the first packet");
  NS_TEST_EXPECT_MSG_EQ (queue->GetCurrentSize ().GetValue (), 7 * modeSize, "There should be seven packets in there");
  NS_TEST_EXPECT_MSG_EQ (item->GetPacket ()->GetUid (), p1->GetUid (), "was this the first packet ?");

  item = queue->Dequeue ();
  NS_TEST_EXPECT_MSG_EQ ((item != 0), true, "I want to remove the second packet");
  NS_TEST_EXPECT_MSG_EQ (queue->GetCurrentSize ().GetValue (), 6 * modeSize, "There should be six packet in there");
  NS_TEST_EXPECT_MSG_EQ (item->GetPacket ()->GetUid (), p2->GetUid (), "Was this the second packet ?");

  item = queue->Dequeue ();
  NS_TEST_EXPECT_MSG_EQ ((item != 0), true, "I want to remove the third packet");
  NS_TEST_EXPECT_MSG_EQ (queue->GetCurrentSize ().GetValue (), 5 * modeSize, "There should be five packets in there");
  NS_TEST_EXPECT_MSG_EQ (item->GetPacket ()->GetUid (), p3->GetUid (), "Was this the third packet ?");

  item = queue->Dequeue ();
  item = queue->Dequeue ();
  item = queue->Dequeue ();
  item = queue->Dequeue ();
  item = queue->Dequeue ();

  item = queue->Dequeue ();
  NS_TEST_EXPECT_MSG_EQ ((item == 0), true, "There are really no packets in there");

  struct d {
    uint32_t test2;
    uint32_t test3;
  } drop;


  // test 2: default values for PI parameters
  queue = CreateObject<PiQueueDisc> ();
  qSize = 300 * modeSize;
  
  NS_TEST_EXPECT_MSG_EQ (queue->SetAttributeFailSafe ("MeanPktSize", UintegerValue (pktSize)), true,
                         "Verify that we can actually set the attribute MeanPktSize");
  NS_TEST_EXPECT_MSG_EQ (queue->SetAttributeFailSafe ("QueueRef", DoubleValue (50)), true,
                         "Verify that we can actually set the attribute QueueRef");
  NS_TEST_EXPECT_MSG_EQ (queue->SetAttributeFailSafe ("MaxSize", QueueSizeValue (QueueSize (mode, qSize))),
                         true, "Verify that we can actually set the attribute MaxSize");
  NS_TEST_EXPECT_MSG_EQ (queue->SetAttributeFailSafe ("W", DoubleValue (170)), true,
                         "Verify that we can actually set the attribute W");
  queue->Initialize ();
  EnqueueWithDelay (queue, pktSize, 300);
  Simulator::Stop (Seconds (40));
  Simulator::Run ();
  QueueDisc::Stats st = queue->GetStats ();
  drop.test2 = st.GetNDroppedPackets (PiQueueDisc::UNFORCED_DROP);
  NS_TEST_EXPECT_MSG_NE (drop.test2, 0, "There should be some unforced drops");


  // test 3: high value of W
  queue = CreateObject<PiQueueDisc> ();
  qSize = 300 * modeSize;
  
  NS_TEST_EXPECT_MSG_EQ (queue->SetAttributeFailSafe ("MeanPktSize", UintegerValue (pktSize)), true,
                         "Verify that we can actually set the attribute MeanPktSize");
  NS_TEST_EXPECT_MSG_EQ (queue->SetAttributeFailSafe ("QueueRef", DoubleValue (50)), true,
                         "Verify that we can actually set the attribute QueueRef");
  NS_TEST_EXPECT_MSG_EQ (queue->SetAttributeFailSafe ("MaxSize", QueueSizeValue (QueueSize (mode, qSize))),
                         true, "Verify that we can actually set the attribute MaxSize");
  NS_TEST_EXPECT_MSG_EQ (queue->SetAttributeFailSafe ("W", DoubleValue (600)), true,
                         "Verify that we can actually set the attribute W");
  queue->Initialize ();
  EnqueueWithDelay (queue, pktSize, 300);
  Simulator::Stop (Seconds (40));
  Simulator::Run ();
  st = queue->GetStats ();
  drop.test3 = st.GetNDroppedPackets (PiQueueDisc::UNFORCED_DROP);
  NS_TEST_EXPECT_MSG_GT (drop.test3, drop.test2, "Test 3 should have more unforced drops than Test 2");
}

void 
PiQueueDiscTestCase::Enqueue (Ptr<PiQueueDisc> queue, uint32_t size, uint32_t nPkt)
{
  Address dest;
  for (uint32_t i = 0; i < nPkt; i++)
    {
      queue->Enqueue (Create<PiQueueDiscTestItem> (Create<Packet> (size), dest, false));
    }
}

void 
PiQueueDiscTestCase::EnqueueWithDelay (Ptr<PiQueueDisc> queue, uint32_t size, uint32_t nPkt)
{
  Address dest;
  double delay = 0.1;
  for (uint32_t i=0;i<nPkt;i++)
  {
      Simulator::Schedule (Time (Seconds ((i + 1) * delay)), &PiQueueDiscTestCase::Enqueue,this,queue,size, 1);
  }

}

void
PiQueueDiscTestCase::DoRun (void)
{
  RunPiTest (QueueSizeUnit::PACKETS);
  RunPiTest (QueueSizeUnit::BYTES);
  Simulator::Destroy ();
}

static class PiQueueDiscTestSuite : public TestSuite
{
public:
  PiQueueDiscTestSuite ()
    : TestSuite ("pi-queue-disc", UNIT)
  {
    AddTestCase (new PiQueueDiscTestCase (), TestCase::QUICK);
  }
} g_piQueueTestSuite;
