#include "include/pricebuffer.h"


void PriceBuffer::write_to_buffer(string in){
    auto new_pointer = make_shared<const string>(in);
    buffer.store(new_pointer);
}

string PriceBuffer::get_buffer(){
    shared_ptr<const string> current_ptr = buffer.load();
    if(current_ptr){
        return *current_ptr;
    }else{
        return "";
    }
}

string PriceBuffer::exchange_buffer(string in){
    auto new_pointer = make_shared<const string>(in);
    return *buffer.exchange(new_pointer);
}

PriceBuffer::PriceBuffer(string in){
    write_to_buffer(in);
    cout<<"[INFO] buffer initialised with valude \""<< in << "\""<<"\n";
}
PriceBuffer::PriceBuffer(){
    cout<<"[INFO] buffer initialised"<<"\n";
}