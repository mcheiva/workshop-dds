// Include DDSBus Fast DDS headers
#include <ddsbus/fastdds/Participant.hpp>

// Create your own DomainParticipantListener by inheriting from eprosima::fastdds::dds::DomainParticipantListener
class ExampleDiscoveryListener : public eprosima::fastdds::dds::DomainParticipantListener
{
  public:
    ExampleDiscoveryListener() = default;

  private:
    void on_participant_discovery(eprosima::fastdds::dds::DomainParticipant *participant,
                                  eprosima::fastdds::rtps::ParticipantDiscoveryStatus reason,
                                  const eprosima::fastdds::dds::ParticipantBuiltinTopicData &info,
                                  bool &should_be_ignored) override
    {
        std::cout << "Participant discovery event!";
        std::cout << "  Reason: " << static_cast<int>(reason);
        std::cout << "  GUID: " << info.guid;
        std::cout << "  Participant Name: " << info.participant_name << '\n';
    }
    void on_data_reader_discovery(eprosima::fastdds::dds::DomainParticipant *participant,
                                  eprosima::fastdds::rtps::ReaderDiscoveryStatus reason,
                                  const eprosima::fastdds::dds::SubscriptionBuiltinTopicData &info,
                                  bool &should_be_ignored) override
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
                                  bool &should_be_ignored) override
    {
        std::cout << "DataWriter discovery event!";
        std::cout << "  Reason: " << static_cast<int>(reason);
        std::cout << "  Topic Name: " << info.topic_name;
        std::cout << "  Type Name: " << info.type_name;
        std::cout << "  GUID: " << info.guid << '\n';
    }
};

int main(int argc, char **argv)
{
    // Perform DDS Setup
    eprosima::fastdds::dds::DomainParticipantQos domainParticipantQos = ddsbus::fastdds::Participant::get_default_participant_qos();
    ExampleDiscoveryListener discoveryListener;
    ddsbus::fastdds::Participant participant(0, domainParticipantQos, &discoveryListener, eprosima::fastdds::dds::StatusMask::none());

    std::cout << "DomainParticipant created. Listening for discovery events...\n";
    // Keep the application running to listen for discovery events
    std::cin.ignore();

    return 0;
}
