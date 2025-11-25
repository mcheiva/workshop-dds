# Exercise 5: Discover entitites on the domain
In this exercise you will try setting up a DomainParticipantListener, just like the DataWriter and DataReader we can attach a listener to the Participant itself and listen in on the discovery information it receives, this can be useful for applications, such as NaviModel that would like to provide a list of available Participants, DataWriters and DataReaders on the domain. The content of this exercise is not something you would implement in a normal service, but it gives good insight into what information the Participant actually receives. 

Start by including the Participant header, in this case we do not require any other entity than the Participant itself as we will merely be listening.
```cpp
// Include DDSBus Fast DDS headers
#include <ddsbus/fastdds/Participant.hpp>
```

Next we implement a DomainParticipantListener, note that in this case we use the direct native eprosima fastdds type and not a abstract class hidden behind our API, this is because this is not a standard case and is more for demonstrative purposes and currently we have not exposed a simplified listener through our API (yet).

DISCLAIMER: There are many callbacks that can be overriden in the DomainParticipantListener - in fact you can even directly implement `on_subscription_matched` and fetch the subscription data from here. This is highly discuraged and listener callbacks should always be attached directly to the entity that signals them! In this case `on_participant_discovery`, `on_data_reader_discovery` and `on_data_writer_discovery` are unique to the DomainParticipantListener and are exactly what we wish to fetch.

In the implemented listener we print out various information and inspecting the amount of information that is accessible in the `info` is encouraged. The `reason` variable indicates weather the callback was called because the entity was `discovered = 0`, or `removed = 2`. 

As can be seen in the DataReader and DataWriter discovery callbacks we get access to topic name and type information, and we can even extract qos settings, this could be used in e.g. NaviModel to automatically spawn a compatible subscriber to a given publisher on the network!

```cpp
// Create your own DomainParticipantListener by inheriting from eprosima::fastdds::dds::DomainParticipantListener
class ExampleDiscoveryListener : public eprosima::fastdds::dds::DomainParticipantListener
{
  public:
    ExampleDiscoveryListener() = default;

  private:
    void on_participant_discovery(eprosima::fastdds::dds::DomainParticipant *participant,
                                  eprosima::fastdds::rtps::ParticipantDiscoveryStatus reason,
                                  const eprosima::fastdds::dds::ParticipantBuiltinTopicData &info,
                                  bool &/*should_be_ignored*/) override
    {
        std::cout << "Participant discovery event!";
        std::cout << "  Reason: " << static_cast<int>(reason);
        std::cout << "  GUID: " << info.guid;
        std::cout << "  Participant Name: " << info.participant_name << '\n';
    }
    void on_data_reader_discovery(eprosima::fastdds::dds::DomainParticipant *participant,
                                  eprosima::fastdds::rtps::ReaderDiscoveryStatus reason,
                                  const eprosima::fastdds::dds::SubscriptionBuiltinTopicData &info,
                                  bool &/*should_be_ignored*/) override
    {
        std::cout << "DataReader discovery event!";
        std::cout << "  Reason: " << static_cast<int>(reason);
        std::cout << "  Topic Name: " << info.topic_name;
        std::cout << "  Type Name: " << info.type_name;
        std::cout << "  GUID: " << info.guid << '\n';
    }
    void on_data_writer_discovery(eprosima::fastdds::dds::DomainParticipant *participant,
                                  eprosima::fastdds::rtps::WriterDiscoveryStatus reason,
                                  const eprosima::fastdds::dds::PublicationBuiltinTopicData &info,
                                  bool &/*should_be_ignored*/) override
    {
        std::cout << "DataWriter discovery event!";
        std::cout << "  Reason: " << static_cast<int>(reason);
        std::cout << "  Topic Name: " << info.topic_name;
        std::cout << "  Type Name: " << info.type_name;
        std::cout << "  GUID: " << info.guid << '\n';
    }
};
```
Setting up the DDS entites are similar to previous exercises but without any subscriber or publisher containers. Note that a `StatusMask` is provided to the Participant that reads `eprosima::fastdds::dds::StatusMask::none()`, this garuantees that even if someone overrides additional callbacks that those overridden above, they will not be called... so don't try! ;)

In this case the participant is spawned using default profile with a specific domain, if you already have a xml profile setup from the previous exercise you could merely load the xml and load the `DomainParticipantExtenedQos` and ensure that the discovery listener is spawned on the same domain as your Publisher and Subscriber applications.
```cpp
// Perform DDS Setup
eprosima::fastdds::dds::DomainParticipantQos domainParticipantQos = ddsbus::fastdds::Participant::get_default_participant_qos();
ExampleDiscoveryListener discoveryListener;
ddsbus::fastdds::Participant participant(<domain_id>, domainParticipantQos, &discoveryListener, eprosima::fastdds::dds::StatusMask::none());
```

Simply block the console of the application to allow the service to run 
```cpp
std::cout << "DomainParticipant created. Listening for discovery events...\n";
// Keep the application running to listen for discovery events
std::cin.ignore();
```