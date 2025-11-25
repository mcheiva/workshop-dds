# Exercise 2: Make your own Publisher

```cpp
// Include DDSBus Fast DDS headers
#include <ddsbus/fastdds/Participant.hpp>
#include <ddsbus/fastdds/Publisher.hpp>
#include <ddsbus/fastdds/Topic.hpp>
#include <ddsbus/fastdds/DataWriter.hpp>
```

```cpp
// Include PubType header of your generated type
#include <idls/AwesomePubSubTypes.hpp>
```

```cpp
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
```

```cpp
// Perform DDS Setup
ddsbus::fastdds::Participant participant(0);
ddsbus::fastdds::Publisher publisher = participant.create_publisher();
ddsbus::fastdds::Topic<AwesomePubSubType> topic = participant.create_topic<AwesomePubSubType>("ExampleTopicName");

MyExampleListener listener;
ddsbus::fastdds::DataWriter<AwesomePubSubType> dataWriter = publisher.create_datawriter(topic, &listener);
```

```cpp
// Create data sample to publish
Awesome sample;
sample.id(1337);
```

```cpp
// Publish data sample
std::cout << "Press any key to send a sample..." << std::endl;
std::cin.ignore();

dataWriter.publish(sample);
std::cout << "Published sample with id = " << sample.id() << std::endl;
```

```cpp
// Block to keep the publisher alive
std::cout << "Press any key to stop the publisher..." << std::endl;
std::cin.ignore();
```