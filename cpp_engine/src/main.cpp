#include <iostream>
#include <print>
#include <bits/stdc++.h>

#include "include/pricebuffer.h"
#include "include/dbhandler.h"
#include "include/client.h"

/*
todo
make client initialise a buffer object
and pass that buffer to the reader class 

create an database reading class 


*/
using namespace std;

int main(){

    PriceBuffer buffer;
    thread t(clientloop, std::ref(buffer));
    t.join();
    return 0;
}