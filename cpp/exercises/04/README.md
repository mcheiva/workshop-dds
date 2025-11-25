# Exercise 4: Quality of Service (QoS)

This exercise focuses on understanding and applying DataWriter and DataReader QoS policies in Fast DDS to control durability, reliability, history depth, and resource limits (and more).

## Objectives
1. Configure a publisher with explicit QoS policies (history depth, durability, reliability, resource limits).
2. Configure a subscriber to request historical samples and reliable delivery.
3. Observe sample reception behavior when joining late.
4. Explore incompatible QoS configurations.

## 1. Publisher QoS Configuration
Start from the default `DataWriterQos` and override the fields needed for this scenario.

Default DataWriter configuration (Fast DDS typical defaults):
1. Durability: `TRANSIENT_LOCAL`
2. History: `KEEP_LAST` depth = 1
3. Reliability: `RELIABLE`

Effect: The writer retains the latest sample and makes it available to late-joining readers requesting transient local durability.

For this exercise, adjust the configuration to:
1. Increase history depth to 5 (retain last 5 samples).
2. Set `allocated_samples` to 5 to align memory usage with maximum stored samples.

OBS: For clarity we explicitly set things such as Durability and Reliability even though they are already defaulted to same values
```cpp
eprosima::fastdds::dds::DataWriterQos dataWriterQos = publisher.get_default_datawriter_qos();
dataWriterQos.reliability().kind = eprosima::fastdds::dds::RELIABLE_RELIABILITY_QOS;
dataWriterQos.history().kind = eprosima::fastdds::dds::KEEP_LAST_HISTORY_QOS;
dataWriterQos.history().depth = 5;
dataWriterQos.durability().kind = eprosima::fastdds::dds::TRANSIENT_LOCAL_DURABILITY_QOS;
dataWriterQos.resource_limits().allocated_samples = 5;

ddsbus::fastdds::DataWriter<AwesomePubSubType> dataWriter = publisher.create_datawriter(topic, &listener, dataWriterQos);
```

### Publishing Multiple Samples
Modify the publishing loop to send multiple samples. You can either:
- Wait for user input between samples, or
- Sleep between publishes (e.g. `std::this_thread::sleep_for(500ms);`).

Include the headers and literals as needed:
```cpp
#include <chrono>
using namespace std::chrono_literals;
```

Example loop:
```cpp
Awesome sample;
int num_samples = 100;

for (int i = 0; i < num_samples; ++i)
{
    std::cout << "Press Enter to send sample " << i << "...\n";
    std::cin.ignore();
    sample.id(i);
    dataWriter.publish(sample);
}
```

Run the publisher and send several samples before starting the subscriber.

## 2. Subscriber QoS Configuration
Configure the subscriber to request previously published samples and ensure reliable delivery.

Default DataReader configuration (typical):
1. Durability: `VOLATILE`
2. Reliability: `BEST_EFFORT`

Effect: A default reader does not retrieve historical transient samples and may drop samples under load (best-effort). For this exercise, adjust:
1. History: `KEEP_LAST` depth = 5 (match publisher depth).
2. Durability: `TRANSIENT_LOCAL` (retrieve retained samples on join).
3. Reliability: `RELIABLE` (ensure retransmission of lost samples).
4. Resource limits: `allocated_samples = 5` (align memory use).

```cpp
eprosima::fastdds::dds::DataReaderQos dataReaderQos = subscriber.get_default_datareader_qos();
dataReaderQos.reliability().kind = eprosima::fastdds::dds::RELIABLE_RELIABILITY_QOS;
dataReaderQos.history().kind = eprosima::fastdds::dds::KEEP_LAST_HISTORY_QOS;
dataReaderQos.history().depth = 5;
dataReaderQos.durability().kind = eprosima::fastdds::dds::TRANSIENT_LOCAL_DURABILITY_QOS;
dataReaderQos.resource_limits().allocated_samples = 5;

ddsbus::fastdds::WaitsetDataReader<AwesomePubSubType> dataReader = subscriber.create_waitset_datareader(topic, &listener, dataReaderQos);
```

### Late Join Behavior Verification
Start the subscriber after the publisher has already sent several samples. Verify:
1. Previously published samples (up to depth) are delivered immediately.
2. Sample order is preserved.
3. New samples continue to arrive reliably.

You could spawn multiple subscribers to see that data is effectively distributed to all applications.

## 3. Incompatible / Mismatched QoS Exploration
This section lets you reason about and experiment with QoS mismatches. Read each question, make the suggested change(s), observe behavior, then expand the answer. Try NOT to open the answer until after you test.

### 3.1 How to Run Experiments
1. Start from the working configuration in Sections 1–2.
2. Change only the QoS parameters mentioned in a question.
3. Restart only the affected participant(s) (writer or reader) if possible to isolate late-join behavior.
4. Use a late-joining subscriber where durability or history effects are involved.

### 3.2 Questions (Attempt First)

Q1 - History Mismatch: What happens in the case where the DataReader requests more samples than the DataWriter stores? Will this even be a valid configuration?

Modify: Set reader `history().depth = 10` while writer stays at `5`.

<details> <summary>Answer</summary>
The endpoints match. Only the last 5 samples are delivered because the writer only retained 5. Additional requested depth does not produce more data; the reader's depth is an upper bound, not a demand contract.
</details>

Q2 - Reliability Mismatch: What happens if the DataReader is configured to use `TRANSIENT_LOCAL` but the DataWriter is `VOLATILE`. In other words, the DataReader is requesting past samples on late-join but the DataWriter is not storing any.

Modify the durability of the DataWriter and rerun the test setup.
```cpp
dataWriterQos.durability().kind = eprosima::fastdds::dds::VOLATILE_DURABILITY_QOS;
```
asd
<details> <summary>Answer</summary>
The endpoints do not match. It is not compatible for the DataReader to request samples when the DataWriter is unable to provide them. You should see that the Listener callback for both `RequestedIncompatibleQos` and `OfferedIncompatibleQos` on both DataReader and DataWriter has been called indicating exactly what QoS policy was violated. You can either step into the `QosPolicyId_t` to see what the code corresponds to or look it up on [QosPolicyId_t](https://fast-dds.docs.eprosima.com/en/latest/fastdds/api_reference/dds_pim/core/policy/qospolicyid_t.html)
</details>





