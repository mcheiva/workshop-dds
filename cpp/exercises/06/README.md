# Exercise 6: Setup Discovery Server!
So far we have chosen a specific domain, and while it is very convenient and easy it also forces every Participant to continuesly broadcast discovery information to all nodes and services avaiable on the domain, this is very inefficient and can cause a lot of congestion on the network for large setups with a lot of services. Instead, a Discovery Server can be used to route all discovery information through using unicast, this is ALOT more efficient and effectively isolates everyone that connects to the discovery server. In other words, the discovery server acts as its own domain, and not one easily obtained by others as it requires the IP and port of the server! for this reason it has potential of severely simpliying topic names as a there is a lot less chance of clashes between names on the network (if done correctly).

In this exercise we will go over setting up a discovery server participant that acts as a `Server` for others, i.e. the `Clients` to use for endpoint discovery.

Assuming that your code is setup to use XML based profiles, the change to using and setting up a discovery server is somewhat trivial, we will basically inject two new participant configurations that no longer uses a simple domain, like e.g. 69. One profile that allows applications to connect as a client and another that allows users to setup a server participant. In the updated profiles below please note that
1) The original `eiva_participant_profile` used in previous exercises has been overriden by a new one, if wanted you could rename the new profile to e.g. `eiva_discoveryserver_client` and remove the `is_default_profile="true"` keeping all 3 participant configurations intact.
2) The default profile is setting the participant as being a `CLIENT` and the locaters defined in the `discoveryServersList` indicates what IPs that clients will try to find the `SERVER` on. This is important when attempting to connect to a `SERVER` not located on your local machine, as you will need the IP and port. Furthermore, it is possible to assign multiple locators, this is not needed in this exmaple however.
3) The `SERVER` profile is definiing the IP and port the discovery server can be found on. The port should needs to available as it will not do similar checking as the default discovery mechanism do for regular domain discovery which contains fallbacks if the ports are unavailable during setup.

Copy the profiles below into your `EIVAQos.xml`

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

Much like the previous exercises we merely need to setup a participant with no internal entites attached
```cpp
// Include DDSBus Fast DDS headers
#include <ddsbus/fastdds/Participant.hpp>
```

Setting up the DDS entites are very trivial, ensure that you fetch the correct profile for the server (and for the client).

```cpp
// Load QoS profiles from XML file
ddsbus::fastdds::Participant::load_xml("EIVAQos.xml");

// Retrieve DomainParticipantQos from discovery server profile
eprosima::fastdds::dds::DomainParticipantExtendedQos domainParticipantExtendedQos =
    ddsbus::fastdds::Participant::get_participant_extended_qos_from_profile("eiva_discoveryserver_profile");
ddsbus::fastdds::Participant participant(domainParticipantExtendedQos, nullptr, eprosima::fastdds::dds::StatusMask::none());
```
You might ask yourself, but why would we create a seperate service that just contains the `SERVER` participant and nothing else? Couldn't we just make either the Publisher and Subscriber act as the `SERVER` and solve the problem that way? Technically yes, but in time critical systems you do not want you rservices to perform unneeded tasks such as forwarding discovery information internally, and similarly, if the discovery server crashes for whatever reason starting discovery up again is as simple as relaunching a simple microservice compared to potentailly crashing an important e.g. sensor service. Futhermore, you can have multiple Discovery Server services running providing redundancy should one of them crash.

The following code snippet prints basic information about the `SERVER` such as the address it is currently avaialble on (this case localhost), and ensures that the application stays alive until user desides to stop it.
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

std::cout << '\n';
std::cout << "press any key to stop the server..." << std::endl;
std::cin.ignore();
```

If you completed the XML profile setup from previous exercises, you just have to make sure that you fetch the default qos profile for the publisher and subscriber we created earlier and then the applications will automatically use the `CLIENT` version of the profile.

```cpp
eprosima::fastdds::dds::DomainParticipantExtendedQos qos = ddsbus::fastdds::Participant::get_participant_extended_qos_from_default_profile();
ddsbus::fastdds::Participant participant(qos);
```

- Try spawning both publisher and subscriber prior to launching the discovery server, you should notice that no matching happens because they are unable to find eachother. Now try to start the discovery server and see that the services connect!
- Try starting a publisher and subscriber and let them connect via discovery server, then close the discovery server and spawn a new subscriber. The data should still flow between the previously connected datawriter and datareader however the newly spawned datareader does not get any data until a disovery server isntance is spawned again! Try starting discovery server again and let it all connect! Voila!
