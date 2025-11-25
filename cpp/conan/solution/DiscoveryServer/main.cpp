// Include DDSBus Fast DDS headers
#include <ddsbus/fastdds/Participant.hpp>


int main(int argc, char **argv)
{
    // Load QoS profiles from XML file
    ddsbus::fastdds::Participant::load_xml("EIVAQos.xml");

    // Retrieve DomainParticipantQos from discovery server profile
    eprosima::fastdds::dds::DomainParticipantExtendedQos domainParticipantExtendedQos =
        ddsbus::fastdds::Participant::get_participant_extended_qos_from_profile("eiva_discoveryserver_profile");
    ddsbus::fastdds::Participant participant(domainParticipantExtendedQos, nullptr, eprosima::fastdds::dds::StatusMask::none());

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


    return 0;
}
