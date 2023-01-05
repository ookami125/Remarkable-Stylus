#pragma once
#include <functional>

#include <libssh/libssh.h>
#include "concurrentqueue.h"

struct Event
{
    uint32_t time;
    uint32_t millis;
    uint16_t type;
    uint16_t code;
    int32_t value;
};

typedef std::function<void(const Event&)> SSHEventHandler;

class SSHHandler
{
    ssh_session session;
    SSHEventHandler callback;

public:
    std::string username = "root";
    std::string password = "";
    std::string hostname = "10.11.99.1";

private:
    int VerifyKnownhost();

public:
    SSHHandler();
    ~SSHHandler();
    int ConnectToRemarkable();
    void SetCallback(SSHEventHandler callback);
    int Process();
};