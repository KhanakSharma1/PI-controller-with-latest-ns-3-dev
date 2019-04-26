.. include:: replace.txt
.. highlight:: cpp

PI queue disc
-------------

This chapter describes the Proportional Integral controller (PI) [Hollot01]_  queue disc
implementation in |ns3|. 

PI controller is a queuing discipline, proposed as an alternative to RED. PI model in ns-3
is a port of C.V. Hollot, Vishal Misra, Don Towsley and Wei-Bo Gong's ns-2 PI model.

Model Description
*****************

The source code for the PI model is located in the directory ``src/traffic-control/model``
and consists of 2 files `pi-queue-disc.h` and `pi-queue-disc.cc` defining a PiQueueDisc
class. The code was ported to |ns3| by Mohit P. Tahiliani, Priya S. Tavarmani and Viyom
Mittal based on the ns-2 code implemented by C.V. Hollot, Vishal Misra, Don Towsley and
Wei-Bo Gong.

* class :cpp:class:`PiQueueDisc`: This class implements the main PI algorithm:

  * ``PiQueueDisc::DoEnqueue ()``: This routine checks whether the queue is full, and if so, drops the packets and records the number of drops due to queue overflow. If queue is not full, this routine calls ``PiQueueDisc::DropEarly()``, and depending on the value returned, the incoming packet is either enqueued or dropped.

  * ``PiQueueDisc::DropEarly ()``: The decision to enqueue or drop the packet is taken by invoking this routine, which returns a boolean value; false indicates enqueue and true indicates drop.

  * ``PiQueueDisc::CalculateP ()``: This routine is called at a regular interval of `1.0/m_w` where 'm_w' is a sampling frequency and updates the drop probability, which is required by ``PiQueueDisc::DropEarly()``

  * ``PiQueueDisc::DoDequeue ()``: This routine dequeues the packet from queue if it not empty.  

References
==========
.. [Hollot01] C. V. Hollot, Vishal Misra, Don Towsley and Wei-BoGong, "On Designing Improved Controllers for AQM Routers Supporting TCP Flows", IEEE/INFOCOM, 2001. Available online at `<ftp://gaia.cs.umass.edu/pub/MisraInfocom01-AQM-Controller.pdf>`_.

Attributes
==========

The key attributes that the PiQueue class holds include the following: 

* ``Mode:`` PI operating mode (BYTES or PACKETS). The default mode is PACKETS. 
* ``MeanPktSize:`` Mean packet size in bytes. The default value is 500 bytes.
* ``QueueRef:`` Desired queue size. The default value is 50 bytes / packets. 
* ``MaxSize:`` The maximum number of bytes or packets the queue can hold. The default value is 25 packets.
* ``A:`` Value of alpha. The default value is 0.00001822.
* ``B:`` Value of beta. The default value is 0.00001816.
* ``W:`` Sampling frequency. The default value is 170.

Examples
========

The example for PI is `pi-vs-pie.cc` located in ``src/traffic-control/examples``. To run the file (the first invocation below shows the available command-line options):

:: 

   $ ./waf --run "pi-vs-pie --PrintHelp"
   $ ./waf --run "pi-vs-pie --queueDiscType=PI"
   
Validation
**********

The PI model is tested using :cpp:class:`PiQueueDiscTestSuite` class defined in `src/traffic-control/test/pi-queue-test-suite.cc`. The suite includes 3 test cases:

* Test 1: The first test checks the enqueue/dequeue with no drops and makes sure that PI attributes can be set correctly.
* Test 2: drops according to default PI algorithm
* Test 3: high value of m_w

The test suite can be run using the following commands: 

::

  $ ./waf configure --enable-examples --enable-tests
  $ ./waf build
  $ ./test.py -s pi-queue-disc

or  

::

  $ NS_LOG="PiQueueDisc" ./waf --run "test-runner --suite=pi-queue-disc"
