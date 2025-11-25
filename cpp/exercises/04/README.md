# Exercise 4: Learn about Quality of Service!

## 1. Publisher: In-code Quality of Service
We get the default dataWriterQos settings and modify the defaults to suit our desired setup.

The default DataWriterQos provided by eProsima already configure the writer to be:
1) Durability TRANSIENT_LOCAL,
2) History set to KEEP_LAST with a depth of 1
3) Reliability set to Reliable

in other words a default datawriter will store the latest sample by default and make it available to any subscriber that requests it. Below we will modify and set the qos settings explicitly to what we desire, in this case we wish to:

1) Increase the amount of samples saved, i.e. Depth to 5
2) Reduce the pre-allocated samples down to 5 to reduce memory footprint since we will never store more, given that we do not use keys and therefore ALL published samples will relate to the same sample instance

remember to give the quality of service settings to the datawriter, otherwise the api will automatically use the defaults.

```cpp
eprosima::fastdds::dds::DataWriterQos dataWriterQos = publisher.get_default_datawriter_qos();
dataWriterQos.reliability().kind = eprosima::fastdds::dds::RELIABLE_RELIABILITY_QOS;
dataWriterQos.history().kind = eprosima::fastdds::dds::KEEP_LAST_HISTORY_QOS;
dataWriterQos.history().depth = 5;
dataWriterQos.durability().kind = eprosima::fastdds::dds::TRANSIENT_LOCAL_DURABILITY_QOS;
dataWriterQos.resource_limits().allocated_samples = 5;

ddsbus::fastdds::DataWriter<AwesomePubSubType> dataWriter = publisher.create_datawriter(topic, &listener, dataWriterQos);
```

Alter the publish flow to allow yourself to publish more than one sample, either by accepting user input or implementating something like 
```cpp
std::this_thread::sleep_for(100ms);
```
remember to ```#include <chrono> ``` and for ease of use do ```using namespace std::chrono_literals;  ```

below is an example that simply modifies the orignal flow to send ``` num_samples``` amount of samples waiting for user input between each sample 
```cpp
// Construct data sample
Awesome sample;

// Publish data sample
int num_samples = 10;

for (int i = 0; i < num_samples; i++)
{
    std::cout << "Press any key to send a sample... \n";
    std::cin.ignore();

    sample.id(i);
    dataWriter.publish(sample);
}
```

Start the publisher can publish a few samples before moving up
## 2. Subscriber: In-code Quality of Service
Making the change on the subscriber side is a lot easier, we simply change the qos and parse it to the subscriber, and that is it!

The default DataReaderQos provided by eProsima already configure the reader to be:
1) Durability VOLATILE,
3) Reliability set to BEST_EFFORT

in other words a default datareader will not request the latest published sample by default. Moreover, it will spawn in BEST_EFFORT state meaning that it will not re-request lost samples. Below we will modify and set the qos settings explicitly to what we desire, in this case we wish to:

1) Increase the amount of samples requested, History set to KEEP_LAST with a Depth set to 5
2) Change Durability to TRANSIENT_LOCAL to actually request past samples 
3) Change transport to Reliable communication
4) Reduce memory footprint similarily to the DataWriter

remember to give the quality of service settings to the datareader, otherwise the api will automatically use the defaults.
```cpp
eprosima::fastdds::dds::DataReaderQos dataReaderQos = subscriber.get_default_datareader_qos();
dataReaderQos.reliability().kind = eprosima::fastdds::dds::RELIABLE_RELIABILITY_QOS;
dataReaderQos.history().kind = eprosima::fastdds::dds::KEEP_LAST_HISTORY_QOS;
dataReaderQos.history().depth = 5;
dataReaderQos.durability().kind = eprosima::fastdds::dds::TRANSIENT_LOCAL_DURABILITY_QOS;
dataReaderQos.resource_limits().allocated_samples = 5;

ddsbus::fastdds::WaitsetDataReader<AwesomePubSubType> dataReader = subscriber.create_waitset_datareader(topic, &listener, dataReaderQos);
```

