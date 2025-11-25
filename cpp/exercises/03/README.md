# Exercise 3: Make Your Own Subscriber
In this exercise we create a simple Subscriber with a DataReader that connects and listens on a single topic based on the initial IDL provided in the `idls` folder.
Open the Subscriber project under `Solution/Subscribers` and include the headers required for the API. These headers pull in the native Fast DDS components, reduce boilerplate, and manage internal DDS entity lifecycles just like the Publisher example.

```cpp
// Include DDSBus Fast DDS headers
#include <ddsbus/fastdds/Participant.hpp>
#include <ddsbus/fastdds/Subscriber.hpp>
#include <ddsbus/fastdds/WaitsetDataReader.hpp>
#include <ddsbus/fastdds/Topic.hpp>
```

Next, include the topic type. This is identical to the Publisher example since we wish to connect to the same topic type.

```cpp
// Include PubType header of your generated type
#include <idls/AwesomePubSubTypes.hpp>
```

Next define a listener. We create `MyExampleListener` inheriting from the API abstract class `DataReaderListener<TopicType>`. This is where the Subscriber code differs slightly from the Publisher code because the main logic for handling incoming samples lives here.

Key points for the listener implementation:
- We override `on_subscription_matched` and `on_requested_incompatible_qos` just like the Publisher example.
- We also implement `on_data_available`, invoked for every received data sample.
- The class is templated so it knows the concrete `<TopicType>` (here `Awesome`).
- The callback parameter is an rvalue reference, allowing the caller to take ownership (e.g. move into a queue) without copying.
- Moving the sample is useful when pushing into a work queue or processing thread to avoid expensive copies.
- Keep listener callbacks lightweight; heavy processing should be offloaded to another thread to avoid blocking the DataReader.
- In this example we only acknowledge the received sample (no storage or expensive processing).
```cpp
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
```
Similarly to the DataWriter, the actual setup of the DDS entities is trivial and closely resembles the Publisher. There are two types of DataReaders: a special implementation called `WaitsetDataReader` and a regular `DataReader` (constructed using `create_datareader`). The `WaitsetDataReader` uses an internal thread to process and invoke listener callbacks, freeing resources so the underlying DataReader can continue processing new samples. In the regular DataReader, callbacks such as `on_data_available` run inline and block further processing. This is fine for small topics, but client code should generally favor `WaitsetDataReader` as advised by eProsima.
```cpp
// Perform DDS Setup
ddsbus::fastdds::Participant participant(<domain_id>);
ddsbus::fastdds::Subscriber subscriber = participant.create_subscriber();
ddsbus::fastdds::Topic<AwesomePubSubType> topic = participant.create_topic<AwesomePubSubType>(<SomeTopicName>);

MyExampleListener listener;
ddsbus::fastdds::WaitsetDataReader<AwesomePubSubType> dataReader = subscriber.create_waitset_datareader(topic, &listener);
```
Similarly to the Publisher example, in production code you might instantiate the `WaitsetDataReader` inside the `MyExampleListener` constructor so logging and lifecycle concerns remain co‑located. In that case pass `this` instead of `&listener`. For reference (not explicitly used in the exercises to follow) such an example might look like:
```cpp
class MyExampleCombined : public ddsbus::core::DataReaderListener<Awesome>
{
  public:
    MyExampleCombined(const ddsbus::fastdds::Participant &participant, const ddsbus::fastdds::Subscriber &subscriber)
        :   m_topic(participant, "MyAwesomeTopicName"),
            m_dataReader(subscriber, m_topic, this)
    {

    }

  private:
    // your overrides
    // .....
    ddsbus::fastdds::Topic<AwesomePubSubType> m_topic;
    ddsbus::fastdds::WaitsetDataReader<AwesomePubSubType> m_dataReader;

};
```

Client code after setting up the DDS entities is simple for the DataReader use case: tell the DataReader to start processing callbacks. In the case of `WaitsetDataReader` this starts an internal processing thread for maximum performance.

```cpp
// Start executing listener callbacks
dataReader.start();
```

Similarly to the Publisher we keep the application alive until user input tells us to stop. This can be done in many ways; here we simply block the terminal.
```cpp
// Block to keep the subscriber alive
std::cout << "Press any key to stop the subscriber..." << std::endl;
std::cin.ignore();
```

After the user prompts exit, stop the internal thread to allow DDS to exit gracefully.
```cpp
// Stop dataReader internals
dataReader.stop();
```