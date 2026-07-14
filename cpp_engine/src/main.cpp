#include <iostream>
#include <print>

#include "include/pricebuffer.h"
#include "include/dbhandler.h"

/*
todo
make client initialise a buffer object
and pass that buffer to the reader class 

create an database reading class 


*/
using namespace std;

int main(){

    PriceBuffer buffer;

    DBHandler dbhandler;

    dbhandler.get_data();

    auto buf = buffer.get_buffer();
    cout<<"[INFO] buffer test -> "<<buf<<"\n";

    try{
        buffer.write_to_buffer("New price");
    }catch(int code){
        print("Error when writing to buffer");
    }
    buf = buffer.get_buffer();
    cout<<"[INFO] buffer test -> "<<buf<<"\n";
    return 0;
}