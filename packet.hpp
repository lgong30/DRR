// packet.hpp
//
//
#ifndef PACKET_HPP
#define PACKET_HPP

#include <iostream>
#include <string>
#include <functional>  // for std::hash
#include <fstream>
#include <boost/asio.hpp> // for ip

class Packet{
public:
    std::string src_ip;
    std::string dst_ip;
    unsigned protocol;
    unsigned src_port;
    unsigned dst_port;
    unsigned length; // length in bytes
    double arrival_time;

    Packet(): src_ip(""), dst_ip(""), protocol(0), src_port(0), dst_port(0), length(0), arrival_time(0) {}
    Packet(std::string& sip, std::string dip, unsigned proto, unsigned sp, unsigned dp, unsigned len, double ats):\
    src_ip(sip), dst_ip(dip), protocol(proto), src_port(sp), dst_port(dp), length(len), arrival_time(ats) {}

    std::string to_string(bool only_ip=false) const{
        if (only_ip) return src_ip + dst_ip;
        else return src_ip + std::to_string(src_port) + dst_ip + std::to_string(dst_port) + std::to_string(protocol);
    }

    bool operator==(const Packet& other) const{
        return  (src_ip == other.src_ip &&
                 dst_ip == other.dst_ip &&
                 src_port == other.src_port &&
                 dst_port == other.dst_port &&
                 protocol == other.protocol &&
                 length == other.length);
    }

    bool valid(){
        bool vflag = true;
        boost::system::error_code ec1, ec2;
        boost::asio::ip::address::from_string(src_ip,ec1);
        if (ec1) {vflag = false; std::cerr << "Source IP Error: " + ec1.message() << std::endl;}
        boost::asio::ip::address::from_string(dst_ip,ec2);
        if (ec2) {vflag = false; std::cerr << "Destionation IP Error: " + ec2.message() << std::endl;}
        if (!(protocol < 256)) {vflag = false; std::cerr << "Protocol Invalid (" << protocol << ")" << std::endl;}
        if (!(src_port < 65536)) {vflag = false; std::cerr << "Source Port Invalid (" << src_port << ")" << std::endl;}
        if (!(dst_port < 65536)) {vflag = false; std::cerr << "Destination Port Invalid (" << dst_port << ")" << std::endl;}
        return vflag;
    }

    unsigned size() const {
        return length;
    }

};



#endif