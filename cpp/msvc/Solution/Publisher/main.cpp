#include <iostream>

// Include DDSBus Fast DDS headers
#include <ddsbus/fastdds/Participant.hpp>
#include <ddsbus/fastdds/Publisher.hpp>
#include <ddsbus/fastdds/Topic.hpp>
#include <ddsbus/fastdds/DataWriter.hpp>

// Include PubType header of your generated type
#include <idls/AwesomePubSubTypes.hpp>

// Create your own DataWriterListener by inheriting from ddsbus::core::DataWriterListener
class MyExampleListener : public ddsbus::core::DataWriterListener
{
public:
    MyExampleListener() = default;
private:
    void on_publication_matched(const eprosima::fastdds::dds::PublicationMatchedStatus &status) override
    {
        std::cout << "[Publisher] Match status changed: "
        << status.current_count << " subscriber(s) connected. "
        << status.total_count << " total connection(s)\n";
    }
    void on_offered_incompatible_qos(const eprosima::fastdds::dds::OfferedIncompatibleQosStatus &status) override
    {
        std::cout << "[Publisher] Incompatible QoS offered. Total count: "
        << status.total_count << ", last violated policy ID: " << status.last_policy_id << '\n';
    }
};

int main()
{
    // Perform DDS Setup
    ddsbus::fastdds::Participant participant(0);
    ddsbus::fastdds::Publisher publisher = participant.create_publisher();
    ddsbus::fastdds::Topic<AwesomePubSubType> topic = participant.create_topic<AwesomePubSubType>("ExampleTopicName");

    MyExampleListener listener;

    eprosima::fastdds::dds::DataWriterQos dataWriterQos = publisher.get_default_datawriter_qos();
    dataWriterQos.reliability().kind = eprosima::fastdds::dds::RELIABLE_RELIABILITY_QOS;
    dataWriterQos.history().kind = eprosima::fastdds::dds::KEEP_LAST_HISTORY_QOS;
    dataWriterQos.history().depth = 5;
    dataWriterQos.durability().kind = eprosima::fastdds::dds::TRANSIENT_LOCAL_DURABILITY_QOS;
    dataWriterQos.resource_limits().allocated_samples = 5;

    ddsbus::fastdds::DataWriter<AwesomePubSubType> dataWriter = publisher.create_datawriter(topic, &listener, dataWriterQos);


    // Construct data sample
    Awesome sample;

    // Publish data sample
    int num_samples = 10;
    using namespace std::chrono_literals;
    for (int i = 0; i < num_samples; i++)
    {
        std::cout << "Press any key to send a sample... \n";
        std::cin.ignore();
        std::this_thread::sleep_for(100ms);

        sample.id(i);
        dataWriter.publish(sample);
    }

    // Block to keep the publisher alive
    std::cout << "Press any key to stop the publisher..." << std::endl;
    std::cin.ignore();

    return 0;
}

