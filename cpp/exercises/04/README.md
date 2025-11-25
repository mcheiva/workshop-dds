# Exercise 4: Quality of Service (QoS)

This exercise focuses on understanding and applying DataWriter and DataReader QoS policies in Fast DDS to control durability, reliability, history depth, and resource limits (and more).

## Objectives
1. Configure Domain Participant QoS
2. Configure a DataWriter with explicit QoS policies (history depth, durability, reliability, resource limits).
3. Configure a DataReader to request historical samples and reliable delivery.
4. Explore incompatible QoS configurations.
5. Experiment with XML based profiles

## 1. Domain Participant QoS Configuration
In general you will not be altering the default participant settings a lot, but in case you would like to there is a static method to fetch the default `DomainParticipantQos` settings. From here you could give your Participant a name for it to be recognizable on discovery tools (we will cover this later).  Remember to provide the qos settings to the participant constructor for the changes to take effect:

```cpp
eprosima::fastdds::dds::DomainParticipantQos domainParticipantQos = ddsbus::fastdds::Participant::get_default_participant_qos();
domainParticipantQos.name("MyService/");

ddsbus::fastdds::Participant participant(0, domainParticipantQos);
```

## 2. DataWriter QoS Configuration
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

## 3. DataReader QoS Configuration
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

## 4. Incompatible / Mismatched QoS Exploration
This section lets you reason about and experiment with QoS mismatches. Read each question, make the suggested change(s), observe behavior, then expand the answer. Try NOT to open the answer until after you test.

### 4.1 How to Run Experiments
1. Start from the working configuration in Sections 1–3.
2. Change only the QoS parameters mentioned in a question.
3. Restart only the affected participant(s) (writer or reader) if possible to isolate late-join behavior.
4. Use a late-joining subscriber where durability or history effects are involved.

### 4.2 Questions (Attempt First)

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
<details> <summary>Answer</summary>
The endpoints do not match. It is not compatible for the DataReader to request samples when the DataWriter is unable to provide them. You should see that the Listener callback for both `RequestedIncompatibleQos` and `OfferedIncompatibleQos` on both DataReader and DataWriter has been called indicating exactly what QoS policy was violated. You can either step into the `QosPolicyId_t` to see what the code corresponds to or look it up on https://fast-dds.docs.eprosima.com/en/latest/fastdds/api_reference/dds_pim/core/policy/qospolicyid_t.html. In this case you should see that the violated Policy was 2, or Durability.
</details>


## 5. XML Profiles

In both the Publisher and Subscriber project a `EIVAQos.xml` profile is already included similar to this:

```cpp
<?xml version="1.0" encoding="UTF-8" ?>
<dds xmlns="http://www.eprosima.com">
    <profiles>
        <participant profile_name="eiva_participant_profile" is_default_profile="true">
	        <domainId>69</domainId>
			<rtps>
                <name>EIVA/</name>
            </rtps>
        </participant>

        <data_writer profile_name="eiva_datawriter_profile" is_default_profile="true">
        </data_writer>

        <data_reader profile_name="eiva_datareader_profile" is_default_profile="true">
        </data_reader>
		
    </profiles>
</dds>
```

notice how multiple profiles are defined, namely a default profile setup for Participant, DataWriter and DataReader. In the above example they all have empty configurations which will result in using the defaults provided by eProsima (so it has no effect). By doing this it is possible to have ONE default setup for DataWriter, DataReader etc by setting `is_default_profile="true"` in the profile header whilst allowing us to have alternative profiles (with different names) that might implement different behaviors. Now lets try to mimic the original setup we had in sections 1-3 by altering the profiles accordingly.

In order to alter the Durability and Reliabilty of DataReader and DataWriter we will append a qos section in that looks similar to this:
```xml
<qos> 
    <durability>
        <kind>TRANSIENT_LOCAL</kind>
    </durability>
    <reliability>
        <kind>RELIABLE</kind>
    </reliability>
</qos>
```
To set the History and ResourceLimits for the topic we will append a topic section just below 
```xml
<topic>
    <historyQos>
        <kind>KEEP_LAST</kind>
        <depth>5</depth>
    </historyQos>
    <resourceLimitsQos>
        <allocated_samples>5</allocated_samples>
    </resourceLimitsQos>
</topic>
```

Note, in this way you can change ANY qos setting you want directly from the xml profiles without hardcoding anything internally.

In order to use the XML profiles inside our client code we will need to load the XML profiles prior to instantiating our DDS entities, this is easily done through the API by calling (before creating the participant)

```cpp
ddsbus::fastdds::Participant::load_xml("EIVAQos.xml");
```
This is a static method that ensures that the defined XML profiles are loaded and will throw if the .XML profile is not present in the specified path next to the executable. In the example code, i have ensured that the profile is copied to output directory on-change, so no need to worry :)

Since we specified that the profile should be the default profile, we now only have to call e.g:

```cpp
eprosima::fastdds::dds::DataReaderQos dataReaderQos = subscriber.get_default_datareader_qos();
```
to get the default config specified in the XML profile. From here you could make hardcoded modifications in-code if you desired, but for now you should remove the orignal in-code config we did and just provide the dataReaderQos directly. In case you have multiple configs you would call e.g:

```cpp
eprosima::fastdds::dds::DataReaderQos dataReaderQos = subscriber.get_datareader_qos_from_profile("profileName");
```
Please not that this call will throw in case that the profileName is either not present or did not map to the qos type that you specified.

Lastly, in order to use the participant profile we have to get the configuration, since the profile also defines the domainId we have to use an extended version of the `DomainParticipantQos` we originally used in Section 1 called `DomainParticipantExtendedQos` which also encodes the domainId inside, to get this we simply alter the Participant part of the code to do:

```cpp
eprosima::fastdds::dds::DomainParticipantExtendedQos domainParticipantExtendedQos =
    ddsbus::fastdds::Participant::get_participant_extended_qos_from_default_profile();

ddsbus::fastdds::Participant participant(domainParticipantExtendedQos);
```
Notice how the constructor no longer takes in a domain id as this is solely defined inside the XML profile (unless you explicitly modify it in-code after reading the profile)

Modify both Publisher and Subsciber code to import the default profiles correct and rerun the test.




