using Eiva.DDSBus;
using Eiva.DDSBus.Core;
using FastDDS;
using IDL;
using Publisher = Eiva.DDSBus.Publisher;


// ----------- SETUP ----------- //
Participant.LoadXMLProfile("EIVAQos.xml");
FastDDS.DomainParticipantQos domainParticipantQos = Participant.GetDefaultParticipantQos();
domainParticipantQos.name("MyService/");
Participant participant = new Participant(0, domainParticipantQos);

Publisher publisher = participant.CreatePublisher();
Topic<Awesome> topic = participant.CreateTopic<Awesome>("MCH/AwesomeTopic");

MyExampleListener listener = new MyExampleListener();

FastDDS.DataWriterQos dataWriterQos = new FastDDS.DataWriterQos();
dataWriterQos.reliability().kind = FastDDS.ReliabilityQosPolicyKind.RELIABLE_RELIABILITY_QOS;
dataWriterQos.history().kind = FastDDS.HistoryQosPolicyKind.KEEP_LAST_HISTORY_QOS;
dataWriterQos.history().depth = 5;
dataWriterQos.durability().kind = FastDDS.DurabilityQosPolicyKind_t.TRANSIENT_LOCAL_DURABILITY_QOS;

IDataWriter<Awesome> dataWriter = publisher.CreateDataWriter(topic, listener: listener);



// ----------- Construct Sample ----------- //
Awesome sample = new Awesome();

// ----------- Publish Sample ----------- //
int numSamples = 100;
for (int i = 0; i < numSamples; i++)
{
    Console.WriteLine($"Press any key to send sample { i }");
    Console.ReadKey();
    sample.id(i);
    dataWriter.Publish(sample);
}

// ----------- Keep alive ----------- //
Console.WriteLine("Press any key to close publisher....");
Console.ReadKey();

// ----------- Example Listener Class ----------- //
class MyExampleListener : Eiva.DDSBus.Core.DataWriterListener
{
    public override void OnPublicationMatched(FastDDS.PublicationMatchedStatus info)
    {
        Console.WriteLine(
            $"[Publisher] Match status changed: {info.current_count} subscribers(s) connected." +
            $" {info.total_count} total connection(s)");
    }

    public override void OnOfferedIncompatibleQos(FastDDS.IncompatibleQosStatus status)
    {
        Console.WriteLine(
            $"[Subscriber] Incompatible QoS requested. Total count: {status.total_count} " +
            $", last violated policy ID: {status.last_policy_id}");
    }

    public override void OnUnacknowledgedSampleRemoved(InstanceHandle_t status)
    {
        Console.WriteLine($"Unawknowledged Sample removed!");
    }
}
