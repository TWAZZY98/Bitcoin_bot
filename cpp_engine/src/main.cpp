#include <iostream>
#include <print>
#include <bits/stdc++.h>

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
    int test_ammount{2};
    StansTestStrat strategy(test_ammount);
    while (true) {
        auto ticks = buffer.last_n(test_ammount);
        Sygnal s = strategy.generate(ticks);
        if (s.state != SignalStates::HOLD) {
            cout<<"BUY"<<"\n";
        }else{
            cout<<"HOLD"<<"\n";
        }
        this_thread::sleep_for(chrono::seconds(1));
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