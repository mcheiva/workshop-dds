using Eiva.DDSBus;



// ----------- SETUP ----------- //

FastDDS.DomainParticipantQos domainParticipantQos = Participant.GetDefaultParticipantQos();
Participant participant = new Participant(0, domainParticipantQos);

