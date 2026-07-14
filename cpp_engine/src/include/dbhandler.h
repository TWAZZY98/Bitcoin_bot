#ifndef DB_HANDLER
#define DB_HANDLER
#include <iostream>
#include <sqlite3.h>

#define PATH_TO_DB "/home/stan/programing/Bitcoin_bot/python_app/src/database/market_data.db"

using namespace std;

class DBHandler {
    public:
    DBHandler();
    void get_data();
    
    private:
    sqlite3* db;
    void connect();
    void close();
};

#endif