// TODO: use QApplication for GUI implementation
#include <QApplication>
#include <iostream>
#include <string>
#include "Simulator.h"
#include "MainWindow.h"

using namespace std;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv); // initialize Qt application for GUI frontend

    if (argc < 4) // argc check
    {
        qDebug("Usage: ./client <world> <bug1> <bug2> [ticks_per_frame] [fps]");
        return 1;
    }
    QString world_map = argv[1];
    QString bug1 = argv[2];
    QString bug2 = argv[3];

    int ticks_per_frame = 50;
    int fps = 10;
    if (argc >= 5)
        ticks_per_frame = QString(argv[4]).toInt();
    if (argc >= 6)
        fps = QString(argv[5]).toInt();

    MainWindow window(world_map, bug1, bug2, ticks_per_frame, fps, nullptr);
    window.show();

    return app.exec();
}

/* CONSOLE frontend
int main(int argc, char *argv[])
{
    if (argc < 4) // argc check
    {
        cerr << "Usage: ./client <world> <bug1> <bug2> [ticks_per_frame] [fps]" << endl;
        return 1;
    }
    string world_map = argv[1];
    string bug1 = argv[2];
    string bug2 = argv[3];

    int ticks_per_frame = 50;
    int fps = 10;
    if (argc >= 5)
        ticks_per_frame = stoi(argv[4]);
    if (argc >= 6)
        fps = stoi(argv[5]);

    Simulator sim(world_map, bug1, bug2);

    if (!sim.openSimulator())
    {
        cerr << "Failed to establish simulator connection." << endl;
        return 1;
    }

    string command;
    string response;

    cout << "Type the following commands:" << endl;
    cout << "FETCH - Return the current world state without advancing" << endl;
    cout << "STEP N - Advance N simulation ticks, then return the state" << endl;
    cout << "QUIT - Shut down the simulator cleanly" << endl;

    while (true)
    {
        cout << "> ";
        // input your command
        if (!getline(cin, command) || command.empty())
            continue;
        // if invalid command, ask user to type in valid input
        if (command != "FETCH" && command != "QUIT" && command.find("STEP ") != 0)
        {
            cout << "Invalid command. Use: FETCH, STEP N, or QUIT" << endl;
            continue;
        }

        sim.sendCommand(command);

        // if "QUIT", exit loop
        if (command == "QUIT")
            break;

        response = sim.readResponse();
        cout << response << endl;
    }

    // save current worldstate to parse
    WorldState parse = sim.parseResponse(response);
    // print worldstate
    sim.printParse(parse);

    return 0;
}

*/
