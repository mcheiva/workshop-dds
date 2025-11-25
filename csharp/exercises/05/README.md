# Exercise 5: Discover Entities on the Domain
In this exercise you will set up a `DomainParticipantListener`. Just like with a DataWriter or DataReader, you can attach a listener to the Participant and inspect discovery information it receives. This can be useful for applications (e.g. NaviModel) that want to list available Participants, DataWriters, and DataReaders on the domain. The code shown here is primarily diagnostic/educational; typical services do not implement this level of inspection, but it provides valuable insight into what the Participant sees.

Start by including the base using statements, in this case we do not require any other entity than the Participant itself as we will merely be listening.
```csharp
using Eiva.DDSBus;
using Eiva.DDSBus.Core;
```

Next, implement a `DomainParticipantListener`. Here we use the native eProsima Fast DDS type directly rather than an abstract wrapper in our API. This is a non‑standard, demonstration scenario and a simplified wrapper is not yet provided.

DISCLAIMER: Many callbacks can be overridden in a `DomainParticipantListener`—you could even implement `OnSubscriptionMatched` here. This is highly discouraged; callbacks should be attached directly to the entity that originates them. In this exercise we focus only on the callbacks unique to the participant: `on_participant_discovery`, `on_data_reader_discovery`, and `on_data_writer_discovery`.

In the implemented listener we print out selected fields. Inspect the full extent of information available in `info`—there is more than shown. The `reason` variable indicates whether the callback was triggered because the entity was `discovered = 0` or `removed = 2`.

As shown in the DataReader and DataWriter discovery callbacks, you have access to topic name, type name, and QoS information. This could be used (for example) by NaviModel to automatically create a compatible Subscriber for a discovered Publisher.

```csharp
// Create your own DomainParticipantListener by inheriting from FastDDS.DomainParticipantListener
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
```
Setting up the DDS entities is similar to previous exercises but without Subscriber or Publisher containers. Note the `StatusMask` provided to the Participant: `eprosima::fastdds::dds::StatusMask::none()`. This guarantees that callbacks other than those explicitly overridden above will not be invoked.

In this case the Participant uses the default profile on a specific domain. If you already have an XML profile from earlier exercises you can load it and obtain `DomainParticipantExtendedQos`, ensuring the discovery listener uses the same domain as your Publisher and Subscriber applications.
```csharp
// ----------- SETUP ----------- //

FastDDS.DomainParticipantQos domainParticipantQos = Participant.GetDefaultParticipantQos();

ExampleDiscoveryListener listener = new ExampleDiscoveryListener();
Participant domainParticipant = new Participant(0, domainParticipantQos, listener, StatusMask.none());
```

Simply block the console so the process can continue listening.
```csharp
Console.WriteLine("DomainParticipant created. Listening for discovery events...");
Console.ReadKey();
```

Once running you should observe all Participants, DataWriters, and DataReaders available on your chosen domain. You could use this to inspect topic names and types. (Do not misuse this to interfere with other users. No, seriously. Dont ;) ) 