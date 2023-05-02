#include <iostream>
#include <olc_net.h>
#include <cstdint>
#include <net_message.h>

enum class CustomMessageTypes : uint32_t
{
    FireBullet,
    MovePlayer
};

int main()
{
    olc::net::message<CustomMessageTypes>msg;
    msg.header.id = CustomMessageTypes::FireBullet;
    return 0;
}

