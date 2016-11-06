//
// packetAPIs.hpp
//
//

#ifndef PACKET_API_HPP
#define PACKET_API_HPP

#include "packet.hpp"
//#include <tuple>        // std::tuple, std::make_tuple, std::get

#include <unordered_map>

std::istream& operator>>(std::istream& is, Packet& obj){
    std::string ignore;
//    is >> ignore
//       >> ignore
//       >> ignore;

    is >> obj.arrival_time
       >> obj.length
       >> ignore   // ip_capture_len
       >> obj.src_ip
       >> obj.dst_ip
       >> obj.protocol
       >> obj.src_port
       >> obj.dst_port
       >> ignore; // tcp_flags

    return is;
}

std::ostream& operator<<(std::ostream& os, const Packet& obj){
    os << "(" << obj.src_ip << ", "
       << obj.src_port << ", "
       << obj.dst_ip   << ", "
       << obj.dst_port << ", "
       << obj.protocol << ")";
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