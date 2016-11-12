//
// packetAPIs.hpp
//
//

#ifndef PACKET_API_HPP
#define PACKET_API_HPP

#include "packet.hpp"
//#include <tuple>        // std::tuple, std::make_tuple, std::get

#include <unordered_map>
#include <exception>

class unknownPacketFormat: std::exception {
    const char* what() const noexcept {return "Unknown packet format!\n";}
};

class emptyLine: std::exception {
    const char * what() const noexcept {return "Empty line occurs!\n"; }
};

std::istream& operator>>(std::istream& is, Packet& obj){

    std::string arrTime, len, cLen, sIp, dIp, proto, sPort, dPort, flags;

    is >> arrTime;

    if (arrTime.empty()){
        throw emptyLine();
    }

    is >> len
       >> cLen
       >> sIp
       >> dIp
       >> proto
       >> sPort
       >> dPort
       >> flags;

    try {
        obj.arrival_time = std::stod(arrTime);
        obj.length = std::stoi(len);
        obj.src_ip = sIp;
        obj.dst_ip = dIp;
        obj.protocol = proto[0];
    }
    catch (std::invalid_argument){
        std::cerr << "Get Invalid arguments: "
                  << arrTime
                  << " "
                  << len
                  << " "
                  << cLen
                  << " "
                  << sIp
                  << " "
                  << dIp
                  << " "
                  << proto
                  << " "
                  << sPort
                  << " "
                  << dPort
                  << " "
                  << flags
                  << std::endl;
        throw unknownPacketFormat();
    }

    try {
        obj.src_port = std::stoi(sPort);
        obj.dst_port = std::stoi(dPort);
    }
    catch (std::invalid_argument){
//        std::cerr << "Get non-UDP and non-TCP packets, set as ports as 0s" << std::endl;
        obj.src_port = 0;
        obj.dst_port = 0;
    }

    return is;
}

std::ostream& operator<<(std::ostream& os, const Packet& obj){
    os << "(" << obj.src_ip << ", "
       << obj.src_port << ", "
       << obj.dst_ip   << ", "
       << obj.dst_port << ", "
       << obj.protocol << ")"
       << ": length = " << obj.length << "\n";
    return os;
}



template <bool option>
class MyHash {
public:
    std::size_t operator()(Packet const& obj) const
    {
        return std::hash<std::string>{}(obj.to_string(option));
    }
};

class Flows {
public:
    int next_flow_id;
    std::unordered_map<Packet, int, MyHash<true> > f;
    Flows(): next_flow_id(0), f(){}
    ~Flows(){
        f.clear();
    }
    int extract_flow_id(const Packet& pkt){
        if (f.find(pkt) == f.end()) {
            f[pkt] = next_flow_id;
            ++ next_flow_id;
        }
        return f[pkt];
    }
    unsigned size() const {
      return next_flow_id;
    }
};
#endif