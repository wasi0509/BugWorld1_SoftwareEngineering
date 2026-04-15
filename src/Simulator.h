#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <vector>

using namespace std;

// world state for display
struct WorldState
{
    int cycle = 0;
    int rows = 0, cols = 0;
    vector<string> mapData;
    int redAlive = 0, blackAlive = 0, redFood = 0, blackFood = 0;
};

class Simulator
{
public:
    Simulator(const string &world_map, const string &bug1, const string &bug2);
    // clean up pipe
    ~Simulator();
    // function for pipe creation
    bool openSimulator();
    // function for sending command to the command pipe
    bool sendCommand(const string &cmd);
    // function for reading responses in the data pipe until END marker
    string readResponse();
    // function to parse response and store it in WorldState
    WorldState parseResponse(const string &result);
    // function to print the parsed worldstate
    void printParse(WorldState &parse);

private:
    // store path in cmd stated by the user
    string worldFile;
    string bug1File;
    string bug2File;
    // store pipe paths for future argument feed
    string cmdPipePath;
    string dataPipePath;
    // tmp directory path
    string pipeDir;
    // initial simulator state (not started yet)
    pid_t sim_pid = -1;
    int cmd_fd = -1;
    int data_fd = -1;
};

#endif
