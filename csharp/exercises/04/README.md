# Exercise 4: Quality of Service (QoS)

This exercise focuses on understanding and applying DataWriter and DataReader QoS policies in Fast DDS to control durability, reliability, history depth, and resource limits (and more).

## Objectives
1. Configure Domain Participant QoS
2. Configure a DataWriter with explicit QoS policies (history depth, durability, reliability, resource limits).
3. Configure a DataReader to request historical samples and reliable delivery.
4. Explore incompatible QoS configurations.
5. Experiment with XML based profiles

## 1. Domain Participant QoS Configuration
The Domain Participant establishes process-wide DDS behavior (discovery, transports, resource limits, and a human-readable name). For many scenarios defaults are sufficient; For now, we will just refine Participant QoS to set a meaningful `name` for tooling and logs.

It is important to note that the name of the Participant has no impact on the internal discovery mechnanims in DDS, it is merely to aid in understanding what services are running on the network - for that reason, multiple Participants could easily be spawned with the same name.

Retrieve and modify the defaults, then pass them into the constructor. This is done by utilizing a static helper method.
```cpp
eprosima::fastdds::dds::DomainParticipantQos domainParticipantQos =
    ddsbus::fastdds::Participant::get_default_participant_qos();

domainParticipantQos.name("MyService/"); // Use structured naming: <System>/<Component>/

ddsbus::fastdds::Participant participant(0, domainParticipantQos); // domain 0
```
Recommendation: Adopt a consistent naming convention (e.g. `Company/Subsystem/Role/`) to simplify filtering in discovery tools.

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
XML profiles allow declarative QoS configuration, reducing in‑code duplication and enabling environment-specific tuning without recompilation.

### 5.1 Base Example
In both Publisher and Subscriber projects an `EIVAQos.xml` similar to the one below is present:
```xml
<?xml version="1.0" encoding="UTF-8"?>
<dds xmlns="http://www.eprosima.com">
    <profiles>
        <participant profile_name="eiva_participant_profile" is_default_profile="true">
            <domainId>69</domainId>
            <rtps>
                <name>EIVA/</name>
            </rtps>
        </participant>

        <data_writer profile_name="eiva_datawriter_profile" is_default_profile="true">
            <!-- YOUR QOS GOES HERE! -->
        </data_writer>

        <data_reader profile_name="eiva_datareader_profile" is_default_profile="true">
            <!-- YOUR QOS GOES HERE! -->
        </data_reader>
    </profiles>
</dds>
```
The example defines separate profiles for the Participant, DataWriter, and DataReader, each marked as the default yet currently empty—meaning they simply fall back to the Fast DDS library defaults. Declaring a profile with `is_default_profile="true"` establishes the baseline QoS retrieved by the standard get_default_*_qos() calls while still allowing additional, named profiles to express alternative behaviors. This means that we could have a multitude of e.g. `data_writer` profiles that are loaded by the client code that are applicable for different performance scenarios. Next, we will replicate the explicit QoS configuration from Sections 1–3 by modifying these profiles with the required policies. 
### 5.2 Adding QoS Policies
Extend writer/reader profiles with QoS and topic sections mirroring earlier in‑code settings:
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

These should be inserted inside the `data_writer` and `data_reader` brackets

### 5.3 Loading Profiles
In order to use the XML profiles inside our client code we will need to load the XML profiles prior to instantiating our DDS entities, this is easily done through the API by calling (before creating the participant)
```cpp
ddsbus::fastdds::Participant::load_xml("EIVAQos.xml");
```
This is a static method that ensures that the defined XML profiles are loaded and will throw if the .XML profile is not present in the specified path next to the executable. In the example code, i have ensured that the profile is copied to output directory on-change, so no need to worry :)

### 5.4 Retrieving QoS From Profiles
When an XML configuration has been successfully loaded, the default settings are overridden by the specified profiles, we can therefore rely mostly on the same calls we used in the original sections 1-3.For default profiles (uses the profile marked `is_default_profile`):

```cpp
eprosima::fastdds::dds::DataReaderQos dr_qos = subscriber.get_default_datareader_qos();
```

Named profile:
```cpp
eprosima::fastdds::dds::DataReaderQos dr_alt = subscriber.get_datareader_qos_from_profile("alternative_reader_profile");
```
An invalid profile name triggers an exception; handle or validate names at startup.

### 5.5 Participant Extended QoS
When domain ID is encoded in the participant profile use the extended QoS accessor:
```cpp
eprosima::fastdds::dds::DomainParticipantExtendedQos ext_qos =
        ddsbus::fastdds::Participant::get_participant_extended_qos_from_default_profile();
ddsbus::fastdds::Participant participant(ext_qos); // domain comes from XML
```

Note how we are no longer parsing in the domain id into the participant constructor.

### 5.6 When to Prefer XML Profiles
- Promote consistency across multiple applications.
- Enable runtime tuning (swap file, restart without recompilation).
- Simplify audits and versioning of QoS changes.
- Reduce code complexity by externalizing policy.

Recall that we defined the profiles inside the xml as default, therefore we just rely on on the e.g: `get_default_datareader_qos` for retrieving the configuration. This means that all of the in-code qos settings can be removed.

Refactor publisher/subscriber code to rely on profile-derived QoS (remove hardcoded writer/reader QoS construction) and rerun previous experiments to confirm identical behavior.





