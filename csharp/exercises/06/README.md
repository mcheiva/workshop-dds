# Exercise 6: Set Up a Discovery Server
So far we have used a specific domain. While convenient, this forces every Participant to continuously broadcast discovery information to all nodes and services available on the domain. In large setups this is inefficient and can cause network congestion. A Discovery Server routes discovery using unicast, which is far more efficient and effectively isolates Participants that connect to it. In practice, the discovery server behaves like its own domain and is not discoverable without the server’s IP and port. This helps reduce topic name clashes (if configured correctly).

In this exercise we set up a Discovery Server participant that acts as a `SERVER` for other Participants (`CLIENTS`) to use for endpoint discovery.

Assuming your code uses XML profiles, switching to a discovery server is straightforward. Inject two new participant configurations that do not use a simple domain (e.g., 69): one profile for `CLIENT` Participants and one for the `SERVER` Participant. In the profiles below note:
1) The original `eiva_participant_profile` is overridden. Optionally rename it to `eiva_discoveryserver_client` and remove `is_default_profile="true"` to keep all three participant configurations.
2) The default profile sets the Participant as a `CLIENT`. The `discoveryServersList` locators define the IPs where clients look for the `SERVER`. This matters when connecting to a remote server — you need the IP and port. Multiple locators can be assigned (not needed in this example).
3) The `SERVER` profile defines the IP and port where the discovery server is reachable. The port must be available; unlike default domain discovery, there are no fallback port checks during setup.

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

As in previous exercises, we set up a Participant with no additional entities attached.
```csharp
using Eiva.DDSBus;
```

Setting up the DDS entities is trivial. Ensure you fetch the correct profile for the server (and for the client).

```csharp
// ----------- SETUP ----------- //

Participant.LoadXMLProfile("EIVAQos.xml");

FastDDS.DomainParticipantExtendedQos domainParticipantExtendedQos = Participant.GetParticipantExtendedQosFromProfile("eiva_discoveryserver_profile");
Participant participant = new Participant(domainParticipantExtendedQos, null, FastDDS.StatusMask.none());
```
You might ask: why create a separate service that contains only the `SERVER` participant? Could the Publisher or Subscriber act as the `SERVER`? Technically yes, but in time‑critical systems you should avoid burdening services with forwarding discovery. If the discovery server crashes, restarting discovery is simpler with a small microservice than risking a critical (e.g., sensor) process. Additionally, you can run multiple Discovery Server services for redundancy.

The following code prints basic information about the `SERVER` (e.g., address; here `localhost`) and keeps the application alive until the user chooses to stop it.
```csharp
// ----------- Keep alive ----------- //
Console.WriteLine("### EIVA Discovery Server is running ###");
Console.WriteLine($"  Participant Type:   {domainParticipantExtendedQos.wire_protocol().builtin.discovery_config.discoveryProtocol}");
Console.WriteLine($"  Server GUID prefix: {participant.GetNative().guid().guidPrefix}");

Console.WriteLine("Press any key to close discovery server....");
Console.ReadKey();
```

If you completed the XML profile setup in previous exercises, fetch the default QoS profile for the Publisher and Subscriber and the applications will automatically use the `CLIENT` configuration.

```csharp
DomainParticipantExtendedQos domainParticipantExtendedQos = Participant.GetDefaultParticipantExtendedQos();
Participant participant = new Participant(domainParticipantExtendedQos);
```

- Try starting both Publisher and Subscriber before launching the discovery server. No matching occurs because they cannot find each other. Start the discovery server and observe that the services connect.
- Start a Publisher and Subscriber and let them connect via the discovery server, then close the server and spawn a new Subscriber. Data continues to flow between the previously connected DataWriter and DataReader, but the new DataReader receives nothing until a discovery server instance is started again. Restart the discovery server and observe reconnection.
