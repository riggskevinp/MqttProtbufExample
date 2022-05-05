#ifndef MACHINEHANDLE_H
#define MACHINEHANDLE_H

#include <string>
#include "pugixml.hpp"

#include "machine.pb.h"

#include <vector>

class MachineHandle
{
public:
    MachineHandle(std::string s);
    std::string model;
    int serial_number;
    std::vector<std::string> subscription_messages;
    std::vector<std::string> publish_messages;
};

#endif // MACHINEHANDLE_H
