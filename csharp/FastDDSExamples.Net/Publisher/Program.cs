using Eiva.DDSBus;
using Eiva.DDSBus.Core;
using FastDDS;
using IDL;
using IDL.EIVA.Game;
using Publisher = Eiva.DDSBus.Publisher;

// ----------- SETUP ----------- //

Eiva.DDSBus.Participant node = new Participant(69);
Publisher publisher = node.CreatePublisher();
Topic<MoveCommand> topic = node.CreateTopic<IDL.EIVA.Game.MoveCommand>("MCH/EIVA/Game/MoveCommand");
WriterListener listener = new WriterListener();
DataWriterQos qos = publisher.GetDefaultDataWriterQos();
IDataWriter<MoveCommand> writer = publisher.CreateDataWriter(topic, qos, listener);



// ----------- Construct Sample ----------- //
IDL.EIVA.Game.MoveCommand sample = new IDL.EIVA.Game.MoveCommand();
sample.player_id(50);
// ----------- Publish Sample ----------- //
for (int i = 0; i < 100; i++)
{
    Console.WriteLine("Press any key to send a sample...");
    Console.ReadKey();
    sample.dir_x(0.5f);
    sample.dir_y(-1.0f);
    //sample.latitude(10.0);
    //sample.id((ulong)i);
    writer.Publish(sample);

}

Console.WriteLine("Press any key to close publisher....");
Console.ReadKey();

// ----------- EVENT CALLBACKS ---------//
class WriterListener : Eiva.DDSBus.Core.DataWriterListener
{
    public override void OnPublicationMatched(PublicationMatchedStatus info)
    {
        Console.WriteLine($"[Publisher]: Currently connected to [ {info.current_count} ] subscribers!");
    }
}
