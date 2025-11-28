#include <iostream>
#include <chrono>

// Include DDSBus Fast DDS headers
#include <ddsbus/fastdds/Participant.hpp>
#include <ddsbus/fastdds/Publisher.hpp>
#include <ddsbus/fastdds/Topic.hpp>
#include <ddsbus/fastdds/DataWriter.hpp>

// Include PubType header of your generated type
#include <idls/MoveCommandPubSubTypes.hpp>
#include <idls/WeaponCommandPubSubTypes.hpp>
#include <thread>

constexpr int playerId = 69;// Forward declare only the needed WinAPI function
extern "C" __declspec(dllimport) short __stdcall GetAsyncKeyState(int vkey);

// Define keycode constants to avoid windows.h macro pollution
constexpr int VK_SPACE = 0x20;

class GameListener : public ddsbus::core::DataWriterListener
{
public:
    GameListener() = default;
private:
    void on_publication_matched(const eprosima::fastdds::dds::PublicationMatchedStatus &status) override
    {
        std::cout << "[Publisher] Match status changed: "
        << status.current_count << " subscriber(s) connected. "
        << status.total_count << " total connection(s)\n";
    }

    void on_offered_incompatible_qos(const eprosima::fastdds::dds::OfferedIncompatibleQosStatus &status) override
    {
        std::cout << "[Publisher] Incompatible QoS offered. Total count: "
        << status.total_count << ", last violated policy ID: " << status.last_policy_id << '\n';
    }
};

int main(int argc, char **argv)
{
    // Perform DDS Setup// Perform DDS Setup
    // Load QoS profiles from XML file
    ddsbus::fastdds::Participant::load_xml("EIVAQos.xml");

    // Retrieve DomainParticipantQos from discovery server profile
    eprosima::fastdds::dds::DomainParticipantExtendedQos domainParticipantExtendedQos =
        ddsbus::fastdds::Participant::get_participant_extended_qos_from_default_profile();

    // Spawn DomainParticipant with the retrieved QoS
    ddsbus::fastdds::Participant participant(domainParticipantExtendedQos);    
    ddsbus::fastdds::Publisher publisher = participant.create_publisher();

    //ddsbus::fastdds::Participant participant(0);
    //ddsbus::fastdds::Publisher publisher = participant.create_publisher();

    ddsbus::fastdds::Topic<EIVA::Game::MoveCommandPubSubType> moveTopic = participant.create_topic<EIVA::Game::MoveCommandPubSubType>("MCH/EIVA/Game/MoveCommand");
    ddsbus::fastdds::Topic<EIVA::Game::WeaponCommandPubSubType> weaponTopic = participant.create_topic<EIVA::Game::WeaponCommandPubSubType>("MCH/EIVA/Game/WeaponCommand");

    GameListener moveListener;
    GameListener weaponListener;
    ddsbus::fastdds::DataWriter<EIVA::Game::MoveCommandPubSubType> moveCommandWritter = publisher.create_datawriter(moveTopic, &moveListener);
    ddsbus::fastdds::DataWriter<EIVA::Game::WeaponCommandPubSubType> weaponCommandWritter = publisher.create_datawriter(weaponTopic, &weaponListener);

    // Create data sample to publish
    EIVA::Game::WeaponCommand gun;
    gun.player_id(playerId);
    gun.weapon(EIVA::Game::WeaponType::Gun);
    EIVA::Game::WeaponCommand shield;
    shield.player_id(playerId);
    shield.weapon(EIVA::Game::WeaponType::Shield);
    EIVA::Game::WeaponCommand slow;
    slow.player_id(playerId);
    slow.weapon(EIVA::Game::WeaponType::SlowField);

    bool running = true;
    // Old key states for edge-detection
    bool oldSpace = false, oldJ = false, oldK = false, oldP = false;

    auto is_key_pressed = [](int vkey, bool &oldState) {
        bool down = (GetAsyncKeyState(vkey) & 0x8000) != 0;
        bool pressed = down && !oldState;
        oldState = down;
        return pressed;
    };

    while (running) {
        // Movement direction: combine keys
        int x = 0, y = 0;
        if (GetAsyncKeyState('F') & 0x8000) x += 1;
        if (GetAsyncKeyState('S') & 0x8000) x -= 1;
        if (GetAsyncKeyState('E') & 0x8000) y -= 1;
        if (GetAsyncKeyState('D') & 0x8000) y += 1;

        // Always send move command (so holding keys keeps moving)
        EIVA::Game::MoveCommand cmd;
        cmd.player_id(playerId);
        cmd.dir_x(x);
        cmd.dir_y(y);
        moveCommandWritter.publish(cmd);

        // Weapons (fire-once per press)
        if (is_key_pressed(VK_SPACE, oldSpace)) {
            weaponCommandWritter.publish(gun);
            std::cout << "Shoot (Gun)\n";
        }
        if (is_key_pressed('J', oldJ)) {
            weaponCommandWritter.publish(shield);
            std::cout << "Shield\n";
        }
        if (is_key_pressed('K', oldK)) {
            weaponCommandWritter.publish(slow);
            std::cout << "Slow Field\n";
        }
        // Quit
        if (is_key_pressed('P', oldP)) {
            running = false;
            std::cout << "Quitting...\n";
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(15));
    }

    return 0;
}
