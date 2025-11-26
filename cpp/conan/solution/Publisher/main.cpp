// Include DDSBus Fast DDS headers
#include <ddsbus/fastdds/Participant.hpp>
#include <ddsbus/fastdds/Publisher.hpp>
#include <ddsbus/fastdds/Topic.hpp>
#include <ddsbus/fastdds/DataWriter.hpp>

// Include PubType header of your generated type
#include <idls/MoveCommandPubSubTypes.hpp>

// Create your own DataWriterListener by inheriting from ddsbus::core::DataWriterListener
class MyExampleListener : public ddsbus::core::DataWriterListener
{
  public:
    MyExampleListener() = default;

  private:
    void on_publication_matched(const eprosima::fastdds::dds::PublicationMatchedStatus &status) override
    {
        std::cout << "[Publisher] Match status changed: " << status.current_count << " subscriber(s) connected.\n"
                  << status.total_count << " total connection(s)\n";
    }
    void on_offered_incompatible_qos(const eprosima::fastdds::dds::OfferedIncompatibleQosStatus &status) override
    {
        std::cout << "[Publisher] Incompatible QoS offered. Total count: " << status.total_count
                  << ", last violated policy ID: " << status.last_policy_id << '\n';
    }
};

int main(int argc, char **argv)
{
    // Perform DDS Setup
    //ddsbus::fastdds::Participant::load_xml("EIVAQos.xml");
    //auto qos = ddsbus::fastdds::Participant::get_participant_extended_qos_from_default_profile();
    ddsbus::fastdds::Participant participant(69);
    ddsbus::fastdds::Publisher publisher = participant.create_publisher();
    ddsbus::fastdds::Topic<EIVA::Game::MoveCommandPubSubType> topic = participant.create_topic<EIVA::Game::MoveCommandPubSubType>("MCH/EIVA/Game/MoveCommand");

    MyExampleListener listener;
    eprosima::fastdds::dds::DataWriterQos dataWriterQos = eprosima::fastdds::dds::DATAWRITER_QOS_DEFAULT;

    dataWriterQos.reliability().kind = eprosima::fastdds::dds::BEST_EFFORT_RELIABILITY_QOS;
    dataWriterQos.history().kind = eprosima::fastdds::dds::KEEP_LAST_HISTORY_QOS;
    dataWriterQos.history().depth = 5;
    dataWriterQos.durability().kind = eprosima::fastdds::dds::TRANSIENT_LOCAL_DURABILITY_QOS;
    dataWriterQos.resource_limits().allocated_samples = 5;

    ddsbus::fastdds::DataWriter<EIVA::Game::MoveCommandPubSubType> dataWriter = publisher.create_datawriter(topic, &listener, dataWriterQos);

    // Create data sample to publish
    EIVA::Game::MoveCommand sample;
    sample.player_id(10);
    sample.dir_x(0.5);
    sample.dir_y(0.5);

    // Publish data sample
    std::cout << "Press any key to send a sample..." << std::endl;
    
    for (int i = 0; i < 100; i++)
    {
        std::cout << "Published sample with id = " << sample.player_id() << std::endl;
        std::cin.ignore();
        dataWriter.publish(sample);
    }
    

    // Block to keep the publisher alive
    std::cout << "Press any key to stop the publisher..." << std::endl;
    std::cin.ignore();
    return 0;
}
