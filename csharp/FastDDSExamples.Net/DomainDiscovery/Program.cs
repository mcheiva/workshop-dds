using Eiva.DDSBus;
using Eiva.DDSBus.Core;
using FastDDS;
using IDL;
using Publisher = Eiva.DDSBus.Publisher;


// ----------- SETUP ----------- //

FastDDS.DomainParticipantQos domainParticipantQos = Participant.GetDefaultParticipantQos();
Participant participant = new Participant(0, domainParticipantQos);

ExampleDiscoveryListener listener = new ExampleDiscoveryListener();
Participant domainParticipant = new Participant(0, domainParticipantQos, listener, StatusMask.none());
class ExampleDiscoveryListener : FastDDS.DomainParticipantListener
{
    public override void on_participant_discovery(DomainParticipant participant,
        ParticipantDiscoveryStatus reason,
        ParticipantBuiltinTopicData info,
        SWIGTYPE_p_bool should_be_ignored)
    {
        Console.WriteLine($"Participant discovery event! " +
                          $" Reason: { reason }" +
                          $" GUID: { info.guid }" +
                          $" Participant Name: {info.participant_name.to_string()}");
    }

    public override void on_data_reader_discovery(DomainParticipant participant,
        ReaderDiscoveryStatus reason,
        SubscriptionBuiltinTopicData info,
        SWIGTYPE_p_bool should_be_ignored)
    {
        Console.WriteLine($"DataReader discovery event! " +
                          $" Reason: {reason}" +
                          $" Topic Name {info.topic_name.c_str()}" +
                          $" Type Name: {info.type_name.c_str()}" +
                          $" GUID: {info.guid}");
    }

    public override void on_data_writer_discovery(DomainParticipant participant,
        WriterDiscoveryStatus reason,
        PublicationBuiltinTopicData info,
        SWIGTYPE_p_bool should_be_ignored)
    {
        Console.WriteLine($"DataWriter discovery event! " +
                          $" Reason: {reason}" +
                          $" Topic Name {info.topic_name.c_str()}" +
                          $" Type Name: {info.type_name.c_str()}" +
                          $" GUID: {info.guid}");
    }
}
