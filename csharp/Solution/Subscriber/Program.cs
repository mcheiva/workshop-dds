using Eiva.DDSBus;
using Eiva.DDSBus.Core;
using FastDDS;
using IDL;
using Subscriber = Eiva.DDSBus.Subscriber;

// ----------- SETUP ----------- //

Participant parcipant = new Participant(0);
Subscriber subscriber = parcipant.CreateSubscriber();
Topic<Awesome> topic = parcipant.CreateTopic<Awesome>("MCH/AwesomeTopic");

MyExampleListener listener = new MyExampleListener();
FastDDS.DataReaderQos dataReaderQos = subscriber.GetDefaultDataReaderQos();
dataReaderQos.reliability().kind = FastDDS.ReliabilityQosPolicyKind.RELIABLE_RELIABILITY_QOS;
dataReaderQos.history().kind = FastDDS.HistoryQosPolicyKind.KEEP_LAST_HISTORY_QOS;
dataReaderQos.history().depth = 5;
dataReaderQos.durability().kind = DurabilityQosPolicyKind_t.TRANSIENT_LOCAL_DURABILITY_QOS;

IDataReader dataReader = subscriber.CreateDataReader(topic, dataReaderQos, listener);



// ----------- Start Listening ----------- //
dataReader.Start();

// ----------- Block Application ----------- //
Console.WriteLine("Subscriber is running. Press any key to stop...");
Console.ReadKey();

// ----------- Stop Listening ----------- //
dataReader.Stop();


// ----------- Example Listener Class ---------//
class MyExampleListener : Eiva.DDSBus.Core.DataReaderListener<Awesome>
{
    public override void OnDataAvailable(Awesome data)
    {
        Console.WriteLine($"[Subscriber] Received data sample with id = {data.id()}");
    }

    public override void OnSubscriptionMatched(FastDDS.SubscriptionMatchedStatus status)
    {
        Console.WriteLine(
            $"[Subscriber] Match status changed: {status.current_count} publisher(s) connected." +
            $" {status.total_count} total connection(s)");
    }

    public override void OnRequestedIncompatibleQos(FastDDS.IncompatibleQosStatus status)
    {
        Console.WriteLine(
            $"[Subscriber] Incompatible QoS requested. Total count: {status.total_count} " +
            $", last violated policy ID: {status.last_policy_id}");
    }
}