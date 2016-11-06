//
// Created by Long Gong on 11/5/16.
//

#ifndef DRR_DRR_H
#define DRR_DRR_H

#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <limits>
#include <list>

#include "Packet.hpp"
#include "PacketAPIs.hpp"


class Drr {
public:
    typedef std::queue<std::shared_ptr<Packet> > Queue;
    std::unordered_map<int, int> deficit_counter;
    std::vector<int> active_list;
    int quantum;

    std::unordered_set<int> active_flows;


    Flows flows;
    std::unordered_map<int /*flow id*/, Queue> queues;

    Drr(int Q): deficit_counter(), active_list(),\
    quantum(Q), active_flows(), flows(), queues() {}


    void enqueue(const Packet& pkt){

        int i = flows.extract_flow_id(pkt);// flow id

        if (active_flows.count(i) == 0) {// not exists
            if (queues.find(i) == queues.end()) queues[i] = Queue();

            active_list.push_back(i);
            active_flows.insert(i);

            deficit_counter[i] = 0;
        }

        queues[i].push(std::make_shared(pkt));

    }

    void dequeue() {

        if (active_list.empty()) return;

        int i = active_list[0];

        active_list.erase(active_list.begin());

        active_flows.erase(i);


        deficit_counter[i] += quantum;

        while (deficit_counter[i] > 0 && (~queues[i].empty())) {
            int pkt_size = queues[i].front()->size();
            if (pkt_size <= deficit_counter[i]) {
                queues[i].pop();
                deficit_counter[i] -= pkt_size;
            } else {
                break;
            }
        }

        if (!queues[i].empty()) {
            active_list.push_back(i);
            active_flows.insert(i);
        } else {
            deficit_counter[i] = 0;
        }
    }




};


#endif //DRR_DRR_H
