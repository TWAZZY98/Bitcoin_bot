## build cpp app
<p>cmake -S . -B build</p>
<p>cmake --build build</p>

<p>source myfirstproject/bin/activate  </p>

<p> sample compilation of the c++ code </p>
g++ main.cpp client.cpp dbhandler.cpp pricebuffer.cpp signalgen.cpp -std=c++23 -pthread -lsqlite3 -o btc_engine
<p> Enter linux terminal on Windows</p>
wsl -d Ubuntu

