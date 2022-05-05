#include "machinehandle.h"

#include <iostream>

MachineHandle::MachineHandle(std::string s)
{
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(s.c_str());
    std::cout << result << std::endl;
    if(result){
        //pugi::xml_node xmodel = doc.child("machine").child("model");
        pugi::xml_node xmodel = doc.child("machine").child("model");
        model = xmodel.attribute("name").as_string();
        serial_number = xmodel.attribute("serial_number").as_int();

        auto topic_xml_to_vector = [](const pugi::xml_node& node, std::vector<std::string>& s){
            for(const auto& topic : node){
                s.emplace_back(std::string(topic.child_value()));
            }
        };

        topic_xml_to_vector(doc.child("machine").child("messages").child("pub"), publish_messages);
        topic_xml_to_vector(doc.child("machine").child("messages").child("sub"), subscription_messages);

    } else {
        std::cout << "Parse Failed" << std::endl;
        throw "Parse Failed";
    }
}
