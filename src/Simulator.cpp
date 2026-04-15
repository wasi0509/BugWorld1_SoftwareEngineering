#include "Simulator.h"
#include <iostream>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <cerrno>
#include <cstring>
#include <signal.h>
#include <sstream>
#include <sys/wait.h>

Simulator::Simulator(const string &world_map, const string &bug1, const string &bug2)
    : worldFile(world_map), bug1File(bug1), bug2File(bug2)
{
    // Save the file paths passed from main
}

Simulator::~Simulator()
{
    // Close command pipe if it was opened
    if (cmd_fd != -1)
    {
        string quit = "QUIT\n";
        ssize_t result = write(cmd_fd, quit.c_str(), quit.length());
        (void)result;
        close(cmd_fd);
    }

    // Close data pipe if it was opened
    if (data_fd != -1)
    {
        close(data_fd);
    }

    // Stop simulator process if it is still running
    if (sim_pid > 0)
    {
        // kill(sim_pid, SIGTERM);
        waitpid(sim_pid, nullptr, 0); // Wait for simulator process to exit
    }

    // Remove command pipe file
    if (!cmdPipePath.empty())
    {
        unlink(cmdPipePath.c_str());
    }

    // Remove data pipe file
    if (!dataPipePath.empty())
    {
        unlink(dataPipePath.c_str());
    }

    // Remove temporary directory created for pipes
    if (!pipeDir.empty())
    {
        rmdir(pipeDir.c_str());
    }
}

bool Simulator::openSimulator()
{
    // Create a unique temp directory for this simulator session
    // pid for unique name
    pipeDir = "/tmp/bugworld_" + to_string(getpid());

    // Full paths for the two named pipes ( /tmp/bugworld_XXXX/... )
    cmdPipePath = pipeDir + "/cmd.pipe";
    dataPipePath = pipeDir + "/data.pipe";

    // Create temp directory if it does not already exist
    if (mkdir(pipeDir.c_str(), 0777) == -1 && errno != EEXIST)
    {
        cerr << "Failed to create temp directory: " << strerror(errno) << endl;
        return false;
    }

    // Create pipe for sending commands to simulator
    if (mkfifo(cmdPipePath.c_str(), 0666) == -1 && errno != EEXIST)
    {
        cerr << "Failed to create command pipe: " << strerror(errno) << endl;
        return false;
    }

    // Create pipe for receiving simulator output
    if (mkfifo(dataPipePath.c_str(), 0666) == -1 && errno != EEXIST)
    {
        cerr << "Failed to create data pipe: " << strerror(errno) << endl;
        return false;
    }

    // Create child process for running the simulator
    sim_pid = fork();

    // Fork failed
    if (sim_pid < 0)
    {
        cerr << "Failed to fork simulator process." << endl;
        return false;
    }

    // Child process: replace itself with ./sim and pass pipe paths as arguments
    if (sim_pid == 0)
    {
        execl("./sim", "./sim",
              "--cmd-pipe", cmdPipePath.c_str(),
              "--data-pipe", dataPipePath.c_str(),
              worldFile.c_str(), bug1File.c_str(), bug2File.c_str(),
              (char *)nullptr);

        // If execl returns, simulator start failed
        cerr << "Failed to start simulator: " << strerror(errno) << endl;
        _exit(1);
    }

    // Small delay so simulator has time to start and open pipes
    usleep(100000);

    // Open command pipe for writing first
    cmd_fd = open(cmdPipePath.c_str(), O_WRONLY);
    if (cmd_fd == -1)
    {
        cerr << "Failed to open cmd pipe for writing: " << strerror(errno) << endl;
        return false;
    }

    // Open data pipe for reading second
    data_fd = open(dataPipePath.c_str(), O_RDONLY);
    if (data_fd == -1)
    {
        cerr << "Failed to open data pipe for reading: " << strerror(errno) << endl;
        return false;
    }

    // Everything worked
    return true;
}

bool Simulator::sendCommand(const string &cmd)
{
    // check if command pipe is open
    if (cmd_fd == -1)
        return false;
    //
    string full_cmd = cmd + '\n';
    // push corresponding command into command pipe buffer
    int write_check = write(cmd_fd, full_cmd.c_str(), full_cmd.length());

    // check whether something was written
    if (write_check != -1)
        return true;
    else
        return false;
}

string Simulator::readResponse()
{
    string buffer;
    char chunk[1024];
    // bool endFound = false;

    // Continue until we see the END marker
    while (true)
    {
        // read bytes from data pipe to chunk
        ssize_t bytesRead = read(data_fd, chunk, sizeof(chunk) - 1);

        if (bytesRead <= 0)
            break;
        if (bytesRead > 0)
        {
            // Null-terminate so we can treat it as a string
            chunk[bytesRead] = '\0';
            buffer += chunk;

            // Check if END marker has arrived yet.
            if (buffer.find("END") != string::npos)
            {
                break;
            }
            // if nothing found in data pipe
        }
        /* else if (bytesRead == -1 && errno != EAGAIN)
        {
            break;
        }*/

        // wait 100ms for simulator
        // usleep(100000);
    }
    return buffer;
}

WorldState Simulator::parseResponse(const string &result)
{
    WorldState state;
    state.mapData.clear(); // Clear any existing map data
    stringstream ss(result);
    string line;

    while (getline(ss, line))
    {
        stringstream line_stream(line);
        string tag;
        // Get the first word of the line
        line_stream >> tag;

        if (tag == "CYCLE")
        {
            line_stream >> state.cycle;
        }
        else if (tag == "MAP")
        {
            line_stream >> state.rows >> state.cols;
        }
        else if (tag == "ROW")
        {
            string cell;
            string rowChars;
            // line_stream >> rowChars;
            while (line_stream >> cell) // read each cell in the row
            {
                rowChars += cell; // append to a string (cell)
            }
            // Save the string of #, R, B, etc.
            state.mapData.push_back(rowChars);
        }
        else if (tag == "STATS")
        {
            line_stream >> state.redAlive >> state.blackAlive >> state.redFood >> state.blackFood;
        }
    }
    return state;
}

void Simulator::printParse(WorldState &parse)
{
    cout << "Cycle: " << parse.cycle << endl;
    cout << "Red Bugs: " << parse.redAlive << endl;
    cout << "Black Bugs: " << parse.blackAlive << endl;
    cout << "Map Size: " << parse.rows << " by " << parse.cols << endl;
}