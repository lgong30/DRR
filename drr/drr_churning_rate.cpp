//
// Created by gtuser on 11/12/2016.
//

//
// drr_churning_rate.cpp
//


#include "../drr.hpp"
#include <iomanip>
#include <chrono>



/* parameters */
const double offset = 1051196402.165037; // start time
const int quantum = static_cast<int>(198.958928351); // quantum for drr in bytes/10us  approximate load = 0.9
// const int quantum = static_cast<int>(223.8287944); // quantum for drr in bytes/10us  approximate load = 0.8
const double len_per_time_slot = 10 * (1e-6); // time slot length 10 us

const int measure_start_ts = static_cast<int>(1000 / len_per_time_slot); // when to start sampling
// const int measure_start_ts = static_cast<int>(200 / len_per_time_slot); // when to start sampling
const int total_num_ts = static_cast<int>(3600 / len_per_time_slot);// total number of time slots (approximated value)
const int print_numbers = 1000;

inline void skip_header_lines(std::istream& is){
    int skip_rows = 5;
    std::string skip;
    for (int i = 0; i < skip_rows; ++i) std::getline(is, skip); // skip first few lines (headers)
}

inline void write_header(std::ostream& os){

    os << std::setw(15)
       << std::left
       << "timeslot"
       << std::setw(25)
       << std::left
       << "backlogged_flow_number"
       << std::setw(25)
       << std::left
       << "flow-in-number"
       << std::setw(25)
       << std::left
       << "flow-out-number"
       << std::endl;
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

    write_header(os);

    // loop variables
    int cur_time_slot = 0; // time slot
    double next_schedule_time = offset + len_per_time_slot; // next schedule time

    Packet pkt;
    Drr sch_drr(quantum);

    bool stop_flag = false;
    int flow_in = 0;
    int flow_out = 0;

    const int buf_size = 1 << 25;

    std::vector<int> flows_in(buf_size, 0);
    std::vector<int> flows_out(buf_size, 0);
    std::vector<long> backlogged_flows(buf_size, 0);

    int cur_buf_id = 0;
    long buf_start_ts = 0;
    long temp;


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

            temp = sch_drr.backlogged_flow_number();
            sch_drr.dequeue();// dequeue
            if (temp > sch_drr.backlogged_flow_number()) ++ flow_out;

            if (cur_buf_id == 0) buf_start_ts = cur_time_slot;
            flows_out[cur_buf_id] = flow_out;
            flows_in[cur_buf_id] = flow_in;
            backlogged_flows[cur_buf_id] = sch_drr.backlogged_flow_number();

            ++ cur_buf_id;

            if (cur_buf_id == buf_size) {// buffer is full, move buffer to file
                for (int i = 0;i < cur_buf_id;++ i){
                    os << std::setw(15)
                       << std::left
                       << (buf_start_ts + i)
                       << std::setw(25)
                       << std::left
                       << backlogged_flows[i]
                       << std::setw(25)
                       << std::left
                       << flows_in[i]
                       << std::setw(25)
                       << std::left
                       << flows_out[i]
                       << std::endl;
                }
                cur_buf_id = 0;
            }

            flow_in = 0;
            flow_out = 0;

            if (cur_time_slot % (total_num_ts / print_numbers) == 0) {
                std::cout << std::setw(15)
                          << std::left
                          << cur_time_slot
                          << std::setw(15)
                          << sch_drr.backlogged_flow_number()
                          << std::endl;
            }

            ++ cur_time_slot; // update current time slot
            next_schedule_time += len_per_time_slot;// update next schedule time

        }

        temp = sch_drr.backlogged_flow_number();
        sch_drr.enqueue(pkt);// enqueue packet
        if (temp < sch_drr.backlogged_flow_number()) ++ flow_in;

    }

    // move buffer to file
    for (int i = 0;i < cur_buf_id;++ i){
        os << std::setw(15)
           << std::left
           << (buf_start_ts + i)
           << std::setw(25)
           << std::left
           << backlogged_flows[i]
           << std::setw(25)
           << std::left
           << flows_in[i]
           << std::setw(25)
           << std::left
           << flows_out[i]
           << std::endl;
    }

    is.close();

    os.close();

    return 0;
}

