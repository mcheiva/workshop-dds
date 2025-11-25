using Eiva.DDSBus;
using Eiva.DDSBus.Core;

using IDL;
using System;
using FastDDS;
using Subscriber = Eiva.DDSBus.Subscriber;

// ----------- SETUP ----------- //

Participant parcipant = new Participant(69);
Subscriber publisher = parcipant.CreateSubscriber();
Topic<Awesome> topic = parcipant.CreateTopic<Awesome>("MCH/AwesomeTopic");

MyExampleListener listener = new MyExampleListener();
IDataReader reader = publisher.CreateWaitsetDataReader(topic, listener: listener);



// ----------- Start Listening ----------- //
reader.Start();
Console.WriteLine("Subscriber is running. Press any key to stop...");
Console.ReadKey();

// ----------- Stop Listening ----------- //
reader.Stop();


// ----------- Example Listener Class ---------//
class MyExampleListener : Eiva.DDSBus.Core.DataReaderListener<Awesome>
{
    public override void OnDataAvailable(Awesome data)
    {
        Console.WriteLine($"[Subscriber] Received data sample with id = { data.id() }");
    }

    public override void OnSubscriptionMatched(SubscriptionMatchedStatus status)
    {
        Console.WriteLine(
            $"[Subscriber] Match status changed: {status.current_count} publisher(s) connected." +
            $" {status.total_count} total connection(s)");
    }

    public override void OnRequestedIncompatibleQos(IncompatibleQosStatus status)
    {
        Console.WriteLine(
            $"[Subscriber] Incompatible QoS requested. Total count: { status.total_count } " +
            $", last violated policy ID: { status.last_policy_id }");
    }
}