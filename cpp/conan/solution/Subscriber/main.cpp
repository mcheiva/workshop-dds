#include <iostream>

// Include DDSBus Fast DDS headers
#include <ddsbus/fastdds/Participant.hpp>
#include <ddsbus/fastdds/Subscriber.hpp>
#include <ddsbus/fastdds/WaitsetDataReader.hpp>
#include <ddsbus/fastdds/Topic.hpp>

// Include PubType header of your generated type
#include <idls/AwesomePubSubTypes.hpp>

// Create your own DataReaderListener by inheriting from ddsbus::core::DataReaderListener<TopicType>
class MyExampleListener : public ddsbus::core::DataReaderListener<Awesome>
{
  public:
    MyExampleListener() = default;

  private:
    void on_data_available(Awesome &&data) override
    {
        std::cout << "[Subscriber] Received data sample with id = " << data.id() << std::endl;
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
    ddsbus::fastdds::Participant participant(1);
    ddsbus::fastdds::Subscriber subscriber = participant.create_subscriber();
    ddsbus::fastdds::Topic<AwesomePubSubType> topic = participant.create_topic<AwesomePubSubType>("AHO/AwesomeTopicName");

    MyExampleListener listener;
    ddsbus::fastdds::WaitsetDataReader<AwesomePubSubType> dataReader = subscriber.create_waitset_datareader(topic, &listener);
    
    // Start executing listener callbacks
    dataReader.start();

    // Block to keep the subscriber alive
    std::cout << "Press any key to stop the subscriber..." << std::endl;
    std::cin.ignore();

    // Stop dataReader internals
    dataReader.stop();

    return 0;
}
