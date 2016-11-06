//
// measureArrivalRate.cpp
//


#include "../packetAPIs.hpp"
#include <fstream>
#include <string>

int main(int argc, char* argv[])
{
    std::string trace = "D:\\data-sets\\traces\\UNC.FULL";
    std::ifstream is(trace, std::ios_base::in);

    double start_ts; // start time stamp
    double end_ts; // end time stamp
    long total_num_flows = 0; // number of flows
    long total_num_bytes = 0;// number of bytes
    double average_rate;



    int skip_rows = 5;
    std::string skip;

    for(int i = 0;i < skip_rows;++ i) std::getline(is, skip); // skip first few lines (headers)

    Flows flows;

    Packet pkt;

    long packet_counter = 0;
    long maximum_sim_packets = -1;

    if (argc == 2) {
        maximum_sim_packets = std::stoi(argv[1]);
    }

    while (is) {
        is >> pkt;
        flows.extract_flow_id(pkt);
        total_num_bytes += pkt.size();
        if (packet_counter == 0){
            start_ts = pkt.arrival_time;
        }

        // for debug purpose
        if (maximum_sim_packets != -1 && packet_counter == maximum_sim_packets) break;

    }
    end_ts = pkt.arrival_time;
    total_num_flows = flows.size();

    average_rate = total_num_bytes / (end_ts - start_ts);

    std::cout << "{"
              << "\n\t\"start-ts\":"
              << start_ts
              << ",\n\t\"end-ts\":"
              << end_ts
              << ",\n\t\"number-flows\":"
              << total_num_flows
              << ",\n\t\"number-bytes\":"
              << total_num_bytes
              << ",\n\t\"average-rate\":"
              << average_rate
              << ",\n}"
              << std::endl;

    return 0;

}