#include <iostream>

// Include DDSBus Fast DDS headers
#include <ddsbus/fastdds/Participant.hpp>
#include <ddsbus/fastdds/Publisher.hpp>
#include <ddsbus/fastdds/Topic.hpp>
#include <ddsbus/fastdds/DataWriter.hpp>

// Include PubType header of your generated type
#include <idls/MoveCommandPubSubTypes.hpp>
#include <idls/WeaponCommandPubSubTypes.hpp>
#include <thread>

// Create your own DataWriterListener by inheriting from ddsbus::core::DataWriterListener
class MyExampleListener : public ddsbus::core::DataWriterListener
{
public:
    MyExampleListener() = default;
private:
    void on_publication_matched(const eprosima::fastdds::dds::PublicationMatchedStatus &status) override
    {
        std::cout << "Adis, [Publisher] Match status changed: "
        << status.current_count << " subscriber(s) connected. "
        << status.total_count << " total connection(s)\n";
    }
    void on_offered_incompatible_qos(const eprosima::fastdds::dds::OfferedIncompatibleQosStatus &status) override
    {
        std::cout << "Adis, [Publisher] Incompatible QoS offered. Total count: "
        << status.total_count << ", last violated policy ID: " << status.last_policy_id << '\n';
    }
};

int main(int argc, char **argv)
{
    //// Perform DDS Setup// Perform DDS Setup
    //// Load QoS profiles from XML file
    //ddsbus::fastdds::Participant::load_xml("EIVAQos.xml");

    //// Retrieve DomainParticipantQos from discovery server profile
    //eprosima::fastdds::dds::DomainParticipantExtendedQos domainParticipantExtendedQos =
    //    ddsbus::fastdds::Participant::get_participant_extended_qos_from_default_profile();

    //// Spawn DomainParticipant with the retrieved QoS
    //ddsbus::fastdds::Participant participant(domainParticipantExtendedQos, nullptr, eprosima::fastdds::dds::StatusMask::none());    ddsbus::fastdds::Publisher publisher = participant.create_publisher();

    ddsbus::fastdds::Participant participant(0);
    ddsbus::fastdds::Publisher publisher = participant.create_publisher();

    ddsbus::fastdds::Topic<EIVA::Game::MoveCommandPubSubType> moveTopic = participant.create_topic<EIVA::Game::MoveCommandPubSubType>("MCH/EIVA/Game/MoveCommand");
    ddsbus::fastdds::Topic<EIVA::Game::WeaponCommandPubSubType> weaponTopic = participant.create_topic<EIVA::Game::WeaponCommandPubSubType>("MCH/EIVA/Game/WeaponCommand");

    ddsbus::fastdds::DataWriter<EIVA::Game::MoveCommandPubSubType> moveCommandWritter = publisher.create_datawriter(moveTopic);
    ddsbus::fastdds::DataWriter<EIVA::Game::WeaponCommandPubSubType> weaponCommandWritter = publisher.create_datawriter(weaponTopic);

    // Create data sample to publish

    EIVA::Game::MoveCommand up;
    up.dir_x(0);
    up.dir_y(1);
    up.player_id(69);
    EIVA::Game::MoveCommand down;
    down.dir_x(0);
    down.dir_y(-1);
    down.player_id(69);
    EIVA::Game::MoveCommand right;
    right.dir_x(1);
    right.dir_y(0);
    right.player_id(69);
    EIVA::Game::MoveCommand left;
    left.dir_x(-1);
    left.dir_y(0);
    left.player_id(69);

    moveCommandWritter.publish(up);
    std::cout << "Published up" << std::endl;
    std::cin.ignore();

    return 0;
}
