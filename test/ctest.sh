cp ../inc/fsm.h .
cp ../src/fsm.cpp .
cp ../src/client.cpp .
g++ fsm.cpp client.cpp -o client -g
