using Eiva.DDSBus;


// ----------- SETUP ----------- //

Participant.LoadXMLProfile("EIVAQos.xml");

FastDDS.DomainParticipantExtendedQos domainParticipantExtendedQos = Participant.GetParticipantExtendedQosFromProfile("eiva_discoveryserver_profile");
Participant participant = new Participant(domainParticipantExtendedQos, null, FastDDS.StatusMask.none());

// ----------- Keep alive ----------- //
Console.WriteLine("### EIVA Discovery Server is running ###");
Console.WriteLine($"  Participant Type:   {domainParticipantExtendedQos.wire_protocol().builtin.discovery_config.discoveryProtocol}");
Console.WriteLine($"  Server GUID prefix: {participant.GetNative().guid().guidPrefix}");

Console.WriteLine("Press any key to close discovery server....");
Console.ReadKey();


