//
// drr_measure.cpp
//


#include "../drr.hpp"
#include <iomanip>
#include <sstream>
#include <random>
#include <chrono>



/* parameters */
const double offset = 1051196402.165037; // start time
const int quantum = static_cast<int>(198.958928351); // quantum for drr in bytes/10us  approximate load = 0.9
// const int quantum = static_cast<int>(223.8287944); // quantum for drr in bytes/10us  approximate load = 0.8
const double len_per_time_slot = 10 * (1e-6); // time slot length 10 us

const int measure_start_ts = static_cast<int>(1800 / len_per_time_slot); // when to start sampling
// const int measure_start_ts = static_cast<int>(200 / len_per_time_slot); // when to start sampling
const int total_num_ts = 2 * measure_start_ts;// total number of time slots (approximated value)


const int number_measures = 1000;
std::vector<double> timeouts = {120, 10, 1, 0.5, 0.2, 0.1, 0.02, 0.001, 0.0005};


inline long measure_active_flow(std::unordered_map<long, double>& flows, double current_time, double timeout){
    long active_flows = 0;
    for (std::unordered_map<long, double>::iterator it = flows.begin();it != flows.end();++ it){
        if (current_time - it->second <= timeout) ++ active_flows;
    }
    return active_flows;
}

inline void skip_header_lines(std::istream& is){
    int skip_rows = 5;
    std::string skip;
    for (int i = 0; i < skip_rows; ++i) std::getline(is, skip); // skip first few lines (headers)
}

inline void write_header(std::ostream& os, std::vector<double>& timeouts){

    os << std::setw(10)
       << std::left
       << "Id"
       << std::setw(15)
       << std::left
       << "timeslot"
       << std::setw(15)
       << std::left
       << "DRR";

    std::string temp;
    for (int i = 0;i < timeouts.size();++ i) {
        std::stringstream ss;
        ss << "flow_active_time_out_" << timeouts[i];

        ss >> temp;

        os << std::setw(30)
           << std::left
           << temp;
    }


    os << std::endl;
}

int main(int argc, char* argv[]) {
    // trace
    std::string trace = (argc == 2?"/home/longgong/git-reps/test-data/UNC_first_10000_lines.txt":"D:\\data-sets\\traces\\UNC.FULL");
    std::ifstream is(trace, std::ios_base::in);

    skip_header_lines(is);// skip header line



    // output file
    // obtain a seed from the system clock:
    unsigned seed1 = std::chrono::system_clock::now().time_since_epoch().count();
    std::string trace_mout = (argc == 2?"/home/longgong/git-reps/test-data/UNC_first_10000_lines_DRR.txt":("D:\\data-sets\\traces\\UNC_DRR_" + std::to_string(seed1) + ".txt"));
    std::ofstream os(trace_mout, std::ios_base::out);

    write_header(os, timeouts);




    // loop variables
    int cur_time_slot = 0, fid = 0; // time slot and flow id
    double next_schedule_time = offset + len_per_time_slot; // next schedule time

    Packet pkt;
    Drr sch_drr(quantum);
    std::unordered_map<long, double> flow_active;// flow table (recoding flow last active time)

    // sampling prepare
    std::default_random_engine generator;
    std::uniform_real_distribution<double> distribution(0.0,1.0);
    double sample_rate = (argc == 2? 0.1:(1.0 * number_measures) / (total_num_ts - measure_start_ts));
    int cur_sample_id = 0;

    bool stop_flag = false;


    // parsing trace
    while (is.good() && (!stop_flag)){
        try {
            is >> pkt;
        }
        catch (std::exception &e) {
            std::cerr << e.what() << std::endl;
            continue;
        }
        catch (...) {
            std::cerr << "Unknown exception caught" << std::endl;
            continue;
        }
        if (!pkt.valid()) {
            std::cerr << "Input packet is invalid: " << pkt << std::endl;
            continue;
        }

        while (pkt.arrival_time > next_schedule_time){// do scheduling (should be while)

            sch_drr.dequeue();// dequeue

            if (cur_time_slot >= (argc == 2?100:measure_start_ts) && cur_sample_id < number_measures){
                if (distribution(generator) < sample_rate){// recording
                    std::cout << "start recording ..." << std::endl;

                    os << std::setw(10)
                       << std::left
                       << cur_sample_id
                       << std::setw(15)
                       << std::left
                       << cur_time_slot
                       << std::setw(15)
                       << sch_drr.backlogged_flow_number();

                    for (auto timeout: timeouts){
                        os << std::setw(30)
                           << std::left
                           << measure_active_flow(flow_active,\
                           next_schedule_time, timeout);
                    }
                    os << std::endl;

                   ++ cur_sample_id;
                }
            }

            if (cur_time_slot % (total_num_ts / 100) == 0) {
                std::cout << std::setw(10)
                   << std::left
                   << cur_sample_id
                   << std::setw(15)
                   << std::left
                   << cur_time_slot
                   << std::setw(15)
                   << sch_drr.backlogged_flow_number()
                   << std::endl;
            }
            // if (cur_time_slot == total_num_ts) break;

            ++ cur_time_slot; // update current time slot
            next_schedule_time += len_per_time_slot;// update next schedule time

        }

        sch_drr.enqueue(pkt);// enqueue packet
        fid = sch_drr.flows.extract_flow_id(pkt);
        flow_active[fid] = pkt.arrival_time;
    }

    is.close();

    os.close();

    return 0;
}