using Eiva.DDSBus;




// ----------- SETUP ----------- //

FastDDS.DomainParticipantExtendedQos domainParticipantQos = Participant.GetDefaultParticipantExtendedQos();

ExampleDiscoveryListener listener = new ExampleDiscoveryListener();
Participant domainParticipant = new Participant(0, domainParticipantQos, listener, FastDDS.StatusMask.none());
class ExampleDiscoveryListener : FastDDS.DomainParticipantListener
{
    public override void on_participant_discovery(FastDDS.DomainParticipant participant,
        FastDDS.ParticipantDiscoveryStatus reason,
        FastDDS.ParticipantBuiltinTopicData info,
        FastDDS.SWIGTYPE_p_bool should_be_ignored)
    {
        Console.WriteLine($"Participant discovery event! " +
                          $" Reason: { reason }" +
                          $" GUID: { info.guid }" +
                          $" Participant Name: {info.participant_name.to_string()}");
    }

    public override void on_data_reader_discovery(FastDDS.DomainParticipant participant,
        FastDDS.ReaderDiscoveryStatus reason,
        FastDDS.SubscriptionBuiltinTopicData info,
        FastDDS.SWIGTYPE_p_bool should_be_ignored)
    {
        Console.WriteLine($"DataReader discovery event! " +
                          $" Reason: {reason}" +
                          $" Topic Name {info.topic_name.c_str()}" +
                          $" Type Name: {info.type_name.c_str()}" +
                          $" GUID: {info.guid}");
    }

    public override void on_data_writer_discovery(FastDDS.DomainParticipant participant,
        FastDDS.WriterDiscoveryStatus reason,
        FastDDS.PublicationBuiltinTopicData info,
        FastDDS.SWIGTYPE_p_bool should_be_ignored)
    {
        Console.WriteLine($"DataWriter discovery event! " +
                          $" Reason: {reason}" +
                          $" Topic Name {info.topic_name.c_str()}" +
                          $" Type Name: {info.type_name.c_str()}" +
                          $" GUID: {info.guid}");
    }
}
