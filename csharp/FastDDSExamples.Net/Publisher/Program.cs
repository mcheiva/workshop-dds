using EMWCore.Interfaces;
using EMWCore.Models;
using IDL;


// ----------- EVENT CALLBACKS ---------//
void OnPublicationMatched(IDDSMatchedStatus status)
{
    Console.WriteLine($"[DDS PublicationMatched] Current: {status.CurrentCount} - Total: {status.TotalCount}");
}

void OnOfferedIncompatibleQos(IDDSIncompatibleQosStatus status)
{
    Console.WriteLine($"[DDS OfferedIncompatibleQos] Total: {status.TotalCount} - Policy: {status.LastPolicyId}");
}

void OnOfferedDeadlineMissed(IDDSDeadlineMissedStatus status)
{
    Console.WriteLine($"[DDS OfferedDeadlineMissed] Total: {status.TotalCount}");
}

void OnLivelinessLost(IDDSLivelinessLostStatus status)
{
    Console.WriteLine($"[DDS LivelinessLost] Total: {status.TotalCount}");
}

void OnUnacknowledgedSampleRemoved()
{
    Console.WriteLine($"[DDS UnacknowledgedSampleRemoved]!");
}

// ----------- SETUP ----------- //
IDDSNode node = EMW.DDS.CreateNode(69, "EIVA/DDSPublisherExample");

EQosSettings qos = EQosSettings.DataWriterQos();
qos.History.Depth = 5;

IDDSPublisher<IDL.EIVA.Game.MoveCommand> publisher = node.CreatePublisher<IDL.EIVA.Game.MoveCommand>("MCH/EIVA/Game/MoveCommand", qos);
publisher.PublicationMatched += OnPublicationMatched;
publisher.OfferedIncompatibleQos += OnOfferedIncompatibleQos;
publisher.OfferedDeadlineMissed += OnOfferedDeadlineMissed;
publisher.LivelinessLost += OnLivelinessLost;
publisher.UnacknowledgedSampleRemoved += OnUnacknowledgedSampleRemoved;


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
    publisher.Publish(sample);

}

Console.WriteLine("Press any key to close publisher....");
Console.ReadKey();