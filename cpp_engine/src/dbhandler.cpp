#include "include/dbhandler.h"

DBHandler::DBHandler(){
    cout << "[INFO] created database handler" << "\n";
}

void DBHandler::connect(){
    int message = sqlite3_open(PATH_TO_DB,&db);
    if (message){
        cerr<< "[ERROR]" << sqlite3_errmsg(db) << endl;
    }else{
        cout<<"[INFO] Data base oppened succesuly";
    }
}

void DBHandler::close(){
    sqlite3_close(db);
}

static int callback(void* data, int argc, char** argv, char** azColName)
{
    int i;
    fprintf(stderr, "%s: ", (const char*)data);
    cout << argc << "\n";
    for (i = 0; i < argc; i++) {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }

    printf("\n");
    return 0;
}

void DBHandler::get_data(){
    connect();

    string data("CALLBACK FUNCTION");

    string sqlquerry("SELECT close FROM BTCUSDT_candle");

    int rc = sqlite3_exec(db,sqlquerry.c_str(),callback,(void*)data.c_str(),NULL);

    if (rc != SQLITE_OK)
        cerr << "Error SELECT" << endl;
    else {
        cout << "Operation OK!" << endl;
    }
    close();

}