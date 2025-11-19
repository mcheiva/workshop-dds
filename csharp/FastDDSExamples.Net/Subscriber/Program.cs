using System;
using EMWCore.Enums;
using EMWCore.Interfaces;
using EMWCore.Models;
using IDL.EIVA.VSLAM;

internal class Program
{
    static void OnDataAvailable(LiveMap data)
    {
        Console.WriteLine($"Received message with [{data.num_points()}] points");
    }
    static void OnSubscriptionMatched(IDDSMatchedStatus status)
    {
        Console.WriteLine($"[DDS SubscriptionMatched] Current: {status.CurrentCount} - Total: {status.TotalCount} - CurrentCountChange: {status.CurrentCountChange} - TotalCountChange: {status.TotalCountChange}");
    }
    static void OnRequestedIncompatibleQos(IDDSIncompatibleQosStatus status)
    {
        Console.WriteLine($"[DDS RequestedIncompatibleQos] Total: {status.TotalCount} - Policy: {status.LastPolicyId}");
    }
    static void OnRequestedDeadlineMissed(IDDSDeadlineMissedStatus status)
    {
        Console.WriteLine($"[DDS RequestedDeadlineMissed] Total: {status.TotalCount}");
    }
    static void OnLivelinessChanged(IDDSLivelinessChangedStatus status)
    {
        Console.WriteLine($"[DDS LivelinessLost] Alive: {status.AliveCount} - AliveChange: {status.AliveCountChange} - NotAlive: {status.NotAliveCount} - NotAliveChange: {status.NotAliveCountChange}");
    }
    static void OnSampleLost(IDDSSampleLostStatus status)
    {
        Console.WriteLine($"[DDS SampleLost] Total: {status.TotalCount}");
    }
    static void OnSampleRejected(IDDSSampleRejectedStatus status)
    {
        Console.WriteLine($"[DDS SampleRejected] Total: {status.TotalCount}");
    }


    static void Main(string[] args)
    {
        // ----------- SETUP ----------- //
        IDDSNode node = EMW.DDS.CreateNode(0, "EIVA/DDSSubscriberExample");
        IDDSDiscovery discovery = node.CreateDiscovery();
        EQosSettings qos = EQosSettings.DataReaderQos();
        qos.Durability.Kind = EDurabilityKind.Volatile;
        //qos.History.Depth = 5;
        qos.ResourceLimits.AllocatedSamples = 1;
        qos.DataRepresentation.Kind = EDataRepresentationKind.XCDRv2;

        IDDSSubscriber<LiveMap> subscriber = node.CreateSubscriber<IDL.EIVA.VSLAM.LiveMap>("MCH-PREC7680/VSLAM/LiveMap", qos);
        IDDSSubscriber<MapUpdate> sub2 = node.CreateSubscriber<MapUpdate>("MCH-PREC7680/VSLAM/MapUpdate", qos);

        subscriber.DataAvailable += OnDataAvailable;
        subscriber.SubscriptionMatched += status =>
        {
            Console.WriteLine($"Current: {status.CurrentCount}, CurrentCountChange: {status.CurrentCountChange}, Total: {status.TotalCount}, TotalCountChange: {status.TotalCountChange}");
        };
        subscriber.RequestedIncompatibleQos += OnRequestedIncompatibleQos;
        subscriber.RequestedDeadlineMissed += OnRequestedDeadlineMissed;
        subscriber.LivelinessChanged += OnLivelinessChanged;
        subscriber.SampleLost += OnSampleLost;
        subscriber.SampleRejected += OnSampleRejected;

        sub2.SubscriptionMatched += OnSubscriptionMatched;


        // ----------- Run Subscriber ----------- //
        Console.WriteLine("Listening... press to stop");
        subscriber.StartListening();
        sub2.StartListening();
        Console.ReadKey();
        subscriber.StopListening();
        sub2.StopListening();
    }
}