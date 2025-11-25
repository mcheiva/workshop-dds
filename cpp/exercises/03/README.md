# Exercise 3: Make your own Subscriber
In this exercise we create a very simple Subscriber with a DataReader that connects and listens on a single topic based on the initial IDL provided in the `idls` folder.
Open the Subscriber project under `Solution/Subscribers` and let's include the headers required for the API. These headers pull in the native Fast DDS components, reduce boilerplate, and manage internal DDS entity lifecycles just like the Publisher example.

```cpp
// Include DDSBus Fast DDS headers
#include <ddsbus/fastdds/Participant.hpp>
#include <ddsbus/fastdds/Subscriber.hpp>
#include <ddsbus/fastdds/WaitsetDataReader.hpp>
#include <ddsbus/fastdds/Topic.hpp>
```

Next, include the topic type. This is identical to the Publisher example as we wish to connect to the same topic type.

```cpp
// Include PubType header of your generated type
#include <idls/AwesomePubSubTypes.hpp>
```

Next define a listener. Here we create `MyExampleListener` inheriting from the API abstract class `DataReaderListener<TopicType>`. This is where the Subscriber code differs slightly from Publishing code as the main function logic will exist here dealing with what to do with incoming samples. Like the Publisher listener we override the corresponding `on_subscription_matched` and `on_requested_incompatible_qos` callback, but we also implement the `on_data_available` which is called for every data sample that the DataReader receives. This is the reason the class is the templated as it needs to know about the `<TopicType>` it is expected to receive, in this case an instance of our `Awesome` class. Notice how the callback receives an R-value reference to an instance, indicating that client code could (and should) take ownership of the sample if it choose to do so. This is especially useful in cases where you wish to push the incoming data into a queue or pooling thread as it allows you to directly move the sample avoiding a potential expensive copy. Generally client code should avoid spending considerable time inside the listener callbacks and move processing into a thread. You should see the callbacks as time critical functions and any time spent here might block the DataReader from doing important operations in the background. In the case of this example we are not interesting in storing the samples or doing any expensive processing we will simply print an acknowledgedment that the data was received to the console.
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
Similarily to the DataWriter, the actual setup of the DDS entities are trivial and closely resembles that of the Publisher. Generally there a two types of DataReaders a special implementation called `WaitsetDataReader` and just a regular `DataReader` (constructed using `create_datareader` instead), the main difference being that the former uses an internal thread to process and call the provided listener callbacks free resources for the DataReader to continue processing new incoming samples. In the "normal" DataReader the callbacks that are called such as `on_data_available` actually blocks the internal DataReader from doing any further processing, this is fine for small topics, but client code should in general favor the use of `WaitsetDataReader` as advised by eProsima.
```cpp
// Perform DDS Setup
ddsbus::fastdds::Participant participant(<domain_id>);
ddsbus::fastdds::Subscriber subscriber = participant.create_subscriber();
ddsbus::fastdds::Topic<AwesomePubSubType> topic = participant.create_topic<AwesomePubSubType>(<SomeTopicName>);

MyExampleListener listener;
ddsbus::fastdds::WaitsetDataReader<AwesomePubSubType> dataReader = subscriber.create_waitset_datareader(topic, &listener);
```
Similarily to the Publisher example, in production code you might instantiate the WaitsetDataReader inside the `MyExampleListener` constructor so logging and lifecycle concerns remain co-located. This also. In that case pass `this` instead of `&listener`. For reference (not explicitly used in the exercises to follow) such an example might look like
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

Client code after setting up the DDS entites are very trivial for the DataReader usecase as we merely need to tell the DataReader to start processing the callbacks, in the case of `WaitsetDataReader` it starts an internal processing thread to allow maximum performance on the internal reader.

```cpp
// Start executing listener callbacks
dataReader.start();
```

```cpp
// Block to keep the subscriber alive
std::cout << "Press any key to stop the subscriber..." << std::endl;
std::cin.ignore();
```

```cpp
// Stop dataReader internals
dataReader.stop();
```