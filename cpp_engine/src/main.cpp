#include <iostream>
#include <print>
#include <bits/stdc++.h>
#include <thread>
#include <chrono>
#include <arpa/inet.h>
#include <cstring>
#include <sys/socket.h>
#include <unistd.h>

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

void send_signal(Sygnal& s, float ammout){
    int sock = socket(AF_INET, SOCK_DGRAM, 0);

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(5051);
    inet_pton(AF_INET, "172.17.16.1", &addr.sin_addr);
    string purchase_state{};

    switch(s.state){
        case SignalStates::SELL:
            purchase_state = "SELL";
            break;
        case SignalStates::BUY:
            purchase_state = "BUY";
            break;
        default:
            close(sock);
    }


    string message = purchase_state + ",BTCUSDT,"+ to_string(ammout);

    sendto(
        sock,
        message.c_str(),
        message.size(),
        0,
        (sockaddr*)&addr,
        sizeof(addr)
    );

    close(sock);
}


void strategy_test(PriceBuffer& buffer){
    uint64_t compare{0};
    //this limits the buffer
    constexpr int test_ammount{100};
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
        std::cout << "[DEBUG] Calling send_signal, state="
          << static_cast<int>(s.state)
          << ", amount=" << 0.001 << std::endl;
        send_signal(s, 0.001);
        std::cout << "[DEBUG] send_signal returned" << std::endl;
        int not_hold{};
        if (s.state == SignalStates::BUY) {
            cout<<"BUY"<<"\n";
            not_hold++;
        }else if(s.state == SignalStates::SELL){
            cout<<"SELL"<<"\n";
            not_hold++;
        }else if(s.state == SignalStates::HOLD){
            cout<<"HOLD"<<"\n";
        } 
        cout<<"not hold: " << not_hold<<"\n";

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