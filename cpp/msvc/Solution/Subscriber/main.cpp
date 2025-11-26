#include <iostream>

// Include DDSBus Fast DDS headers
#include <ddsbus/fastdds/Participant.hpp>
#include <ddsbus/fastdds/Subscriber.hpp>
#include <ddsbus/fastdds/WaitsetDataReader.hpp>
#include <ddsbus/fastdds/Topic.hpp>

// Include PubType header of your generated type
#include <__msvc_filebuf.hpp>
#include <idls/AwesomePubSubTypes.hpp>


// Create your own DataReaderListener by inheriting from ddsbus::core::DataReaderListener<TopicType>
class MyExampleListener : public ddsbus::core::DataReaderListener<Awesome>
{
  public:
    MyExampleListener() = default;

  private:
    void on_data_available(Awesome &&data) override
    {
        std::cout << "[Subscriber] Received data sample with ButAmI = " << data.id() << std::endl;
    }
    void on_subscription_matched(const eprosima::fastdds::dds::SubscriptionMatchedStatus &status) override
    {
        std::cout << "[Subscriber] Match status changed: "
          << status.current_count << " publisher(s) connected. "
          << status.total_count << " total connection(s)\n";
    }
    void on_requested_incompatible_qos(const eprosima::fastdds::dds::RequestedIncompatibleQosStatus &status) override
    {
        std::cout << "[Subscriber] Incompatible QoS requested. Total count: " << status.total_count
                  << ", last violated policy ID: " << status.last_policy_id << '\n';
    }
    
};

int main()
{
    // Perform DDS Setup
    // Load QoS profiles from XML file
    ddsbus::fastdds::Participant::load_xml("EIVAQos.xml");
    

    // Retrieve DomainParticipantQos from discovery server profile
    eprosima::fastdds::dds::DomainParticipantExtendedQos domainParticipantExtendedQos =
        ddsbus::fastdds::Participant::get_participant_extended_qos_from_profile("eiva_participant_profile");
    ddsbus::fastdds::Participant participant(domainParticipantExtendedQos);
    ddsbus::fastdds::Subscriber subscriber = participant.create_subscriber();
    ddsbus::fastdds::Topic<AwesomePubSubType> topic = participant.create_topic<AwesomePubSubType>("ExampleTopicName");

    MyExampleListener listener;

    eprosima::fastdds::dds::DataReaderQos dataReaderQos = subscriber.get_default_datareader_qos();
    //dataReaderQos.reliability().kind = eprosima::fastdds::dds::RELIABLE_RELIABILITY_QOS;
    //dataReaderQos.history().kind = eprosima::fastdds::dds::KEEP_LAST_HISTORY_QOS;
    //dataReaderQos.history().depth = 5;
    //dataReaderQos.durability().kind = eprosima::fastdds::dds::TRANSIENT_LOCAL_DURABILITY_QOS;
    //dataReaderQos.resource_limits().allocated_samples = 5;

    ddsbus::fastdds::WaitsetDataReader<AwesomePubSubType> dataReader = subscriber.create_waitset_datareader(topic, &listener, dataReaderQos);

    // Start executing listener callbacks
    dataReader.start();

    // Block to keep the subscriber alive
    std::cout << "Press any key to stop the subscriber..." << std::endl;
    std::cin.ignore();

    // Stop dataReader internals
    dataReader.stop();

    return 0;
}
