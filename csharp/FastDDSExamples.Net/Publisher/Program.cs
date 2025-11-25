using Eiva.DDSBus;
using Eiva.DDSBus.Core;
using FastDDS;
using IDL;
using Publisher = Eiva.DDSBus.Publisher;

// ----------- SETUP ----------- //

Participant node = new Participant(69);
Publisher publisher = node.CreatePublisher();
Topic<Awesome> topic = node.CreateTopic<Awesome>("MCH/AwesomeTopic");

MyExampleListener listener = new MyExampleListener();
IDataWriter<Awesome> writer = publisher.CreateDataWriter(topic, listener: listener);



// ----------- Construct Sample ----------- //
Awesome sample = new Awesome();
sample.id(1337);

// ----------- Publish Sample ----------- //
Console.WriteLine("Press any key to send a sample...");
Console.ReadKey();

writer.Publish(sample);
Console.WriteLine($"Published sample with id = { sample.id() }"); 

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

    public override void OnOfferedIncompatibleQos(IncompatibleQosStatus status)
    {
        Console.WriteLine(
            $"[Subscriber] Incompatible QoS requested. Total count: {status.total_count} " +
            $", last violated policy ID: {status.last_policy_id}");
    }
}
