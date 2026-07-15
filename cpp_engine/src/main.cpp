#include <iostream>
#include <print>
#include <bits/stdc++.h>
#include <thread>
#include <chrono>

#include "include/pricebuffer.h"
#include "include/dbhandler.h"
#include "include/client.h"
#include "include/signalgen.h"

/*
todo
make client initialise a buffer object
and pass that buffer to the reader class 

create an database reading class 

*/
using namespace std;


void strategy_test(PriceBuffer& buffer){
    uint64_t compare{0};
    constexpr int test_ammount{2};
    StansTestStrat strategy(test_ammount);
    while (true) {

        auto snapshot = buffer.getSnapshot(test_ammount);

        if(snapshot.id == compare){
            this_thread::sleep_for(chrono::milliseconds(100));
            continue;
        }
        compare = snapshot.id;
        auto ticks = snapshot.data;

        Sygnal s = strategy.generate(ticks);

        if (s.state != SignalStates::HOLD) {
            cout<<"BUY"<<"\n";
        }else{
            cout<<"HOLD"<<"\n";
        }

    }

}

int main(){

    PriceBuffer buffer;
    thread t1(strategy_test, std::ref(buffer));
    thread t2(clientloop, std::ref(buffer));
    t2.join();
    t1.join();
    return 0;
}