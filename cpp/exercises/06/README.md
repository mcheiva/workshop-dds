# Exercise 6: Setup Discovery Server!



```cpp
// Include DDSBus Fast DDS headers
#include <ddsbus/fastdds/Participant.hpp>
```

```cpp
// Load QoS profiles from XML file
ddsbus::fastdds::Participant::load_xml("EIVAQos.xml");

// Retrieve DomainParticipantQos from discovery server profile
eprosima::fastdds::dds::DomainParticipantExtendedQos domainParticipantExtendedQos =
    ddsbus::fastdds::Participant::get_participant_extended_qos_from_profile("eiva_discoveryserver_profile");
ddsbus::fastdds::Participant participant(domainParticipantExtendedQos, nullptr, eprosima::fastdds::dds::StatusMask::none());
```



```cpp
std::cout << "### EIVA Discovery Server is running ###" << std::endl;
std::cout << "  Participant Type:   " << domainParticipantExtendedQos.wire_protocol().builtin.discovery_config.discoveryProtocol << std::endl;
std::cout << "  Server GUID prefix: " << participant.get_native()->guid().guidPrefix << std::endl;
std::cout << "  Server Addresses:   ";
for (auto locator_it = domainParticipantExtendedQos.wire_protocol().builtin.metatrafficUnicastLocatorList.begin();
     locator_it != domainParticipantExtendedQos.wire_protocol().builtin.metatrafficUnicastLocatorList.end();)
{
    std::cout << *locator_it;
    if (++locator_it != domainParticipantExtendedQos.wire_protocol().builtin.metatrafficUnicastLocatorList.end())
    {
        std::cout << std::endl << "                      ";
    }
}

std::cout << "press any key to stop the server..." << std::endl;
std::cin.ignore();
```

```xml
        <participant profile_name="eiva_participant_profile" is_default_profile="true">
			<rtps>
                <name>EIVA/</name>
				<builtin>
					<discovery_config>
						<!-- Set participant as CLIENT -->
						<discoveryProtocol>CLIENT</discoveryProtocol>
						<!--
                        Set a list of remote servers (locators) to which this
                        client connects.
                    -->
						<discoveryServersList>
							<!-- Set REMOTE SERVER's listening locator for PDP -->
							<locator>
								<udpv4>
									<address>127.0.0.1</address>
									<port>14520</port>
								</udpv4>
							</locator>
						</discoveryServersList>
						<!-- Set ping period to 250 ms -->
						<clientAnnouncementPeriod>
							<nanosec>250000000</nanosec>
						</clientAnnouncementPeriod>
					</discovery_config>
				</builtin>
            </rtps>
        </participant>

		<participant profile_name="eiva_discoveryserver_profile">
			<rtps>
				<name>EIVA/DISCOVERY_SERVER/</name>
				<builtin>
					<discovery_config>
						<discoveryProtocol>SERVER</discoveryProtocol>
					</discovery_config>
					<metatrafficUnicastLocatorList>
						<locator>
							<udpv4>
								<address>localhost</address>
								<port>14520</port>
							</udpv4>
						</locator>
					</metatrafficUnicastLocatorList>
				</builtin>
			</rtps>
		</participant>
```