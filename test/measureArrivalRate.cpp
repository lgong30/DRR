//
// measureArrivalRate.cpp
//


#include "../packetAPIs.hpp"
//#include <fstream>
//#include <string>
#include <iomanip>

int main(int argc, char* argv[])
{
    std::string trace = "D:\\data-sets\\traces\\UNC.FULL";
    std::ifstream is(trace, std::ios_base::in);

    double start_ts = 0; // start time stamp
    double end_ts; // end time stamp
    double total_num_flows = 0; // number of flows
    double total_num_bytes = 0;// number of bytes
    double average_rate;



    int skip_rows = 5;
    std::string skip;

    for(int i = 0;i < skip_rows;++ i) std::getline(is, skip); // skip first few lines (headers)

    Flows flows;

    Packet pkt;

    long long packet_counter = 0;
    int overflow_counter = 0;
    double maximum_sim_packets = -1;

    if (argc == 2) {
        std::cout << "setting maximum_sim_packets" << std::endl;
        maximum_sim_packets = std::stod(argv[1]);
    }

    while (is.good()) {
        try{
            is >> pkt;
        }
        catch (std::exception &e)
        {
            std::cerr << e.what() << std::endl;
            continue;
        }
        catch (...)
        {
            std::cerr << "Unknown exception caught" << std::endl;
            continue;
        }
        if (!pkt.valid()){
            std::cerr << "Input packet is invalid: " << pkt << std::endl;
        }

        flows.extract_flow_id(pkt);
        total_num_bytes += pkt.size();
        if (packet_counter == 0){
            start_ts = pkt.arrival_time;
        }

//        if (maximum_sim_packets != -1 && packet_counter % 100 == 0) std::cout << pkt << std::endl;

        // for debug purpose
        if (maximum_sim_packets != -1 && packet_counter == maximum_sim_packets) break;

        // for safe
        if (packet_counter == std::numeric_limits<long long>::max())
        {
            ++ overflow_counter;
            packet_counter = 0;
        }
        ++ packet_counter;
        if (packet_counter % 1000000 == 0) {

            std::cout << "Current packet information is as follows (for debug purpose):\n\t" << pkt << std::endl;
            std::cout << "Current statistical information is as follows (for debug purpose):\n";
            std::cout << "{"
                      << "\n\t\"start-ts\":"
                      << std::fixed
                      << start_ts
                      << "\n\t\"number-packets\":"
                      << (overflow_counter == 0?packet_counter:(static_cast<double>(std::numeric_limits<long long>::max()) * overflow_counter + packet_counter))
                      << ",\n\t\"number-flows\":"
                      << flows.size()
                      << ",\n\t\"number-bytes\":"
                      << total_num_bytes
                      << ",\n}"
                      << std::endl;
        }
    }
    is.close();

    end_ts = pkt.arrival_time;
    total_num_flows = flows.size();

    average_rate = total_num_bytes / (end_ts - start_ts);

    std::cout << "{"
              << "\n\t\"start-ts\":"
              << std::fixed
              << start_ts
              << ",\n\t\"end-ts\":"
              << end_ts
              << ",\n\t\"number-flows\":"
              << total_num_flows
              << "\n\t\"number-packets\":"
              << (overflow_counter == 0?packet_counter:(static_cast<double>(std::numeric_limits<long long>::max()) * overflow_counter + packet_counter))
              << ",\n\t\"number-bytes\":"
              << total_num_bytes
              << ",\n\t\"average-rate\":"
              << average_rate
              << ",\n}"
              << std::endl;

    return 0;

}