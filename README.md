# BugWorld Client – Project 09
Date: 12 March 2026 to 24 March 2026

## Overview

In this project we implemented the client side for the BugWorld simulator.
The client communicates with the simulator using two named pipes (FIFOs). One pipe is used to send commands to the simulator and the other one is used to receive responses.

The simulator itself runs as a separate process. Our client starts the simulator and then interacts with it through the pipes.

## Named Pipes

The client creates two named pipes inside a temporary directory:

- cmd.pipe – used to send commands to the simulator
- data.pipe – used to receive responses from the simulator

The temporary directory is created inside `/tmp` and includes the process id so that each run is unique.

Example directory:

/tmp/bugworld_<pid>/

This prevents conflicts if multiple clients are running at the same time.

## Launching the Simulator

After creating the pipes, the client launches the simulator using `fork()` and `execl()`.

The simulator is started with the following arguments:

./sim --cmd-pipe <cmd.pipe> --data-pipe <data.pipe> <world_file> <red_bug_file> <black_bug_file>

These arguments allow the simulator to know which pipes to use for communication.

## Opening the Pipes

To avoid deadlocks, the pipes must be opened in a specific order.

First, the command pipe is opened for writing.
After that, the data pipe is opened for reading.

This matches the order used by the simulator and ensures that both processes can connect correctly.

## Communication

The client sends commands through the command pipe and reads the simulator responses from the data pipe.

Some example commands that can be sent are:

STEP N
FETCH 
QUIT

The simulator processes these commands and sends back the current world state or other information.

## Cleanup

When the program exits, the client performs cleanup to avoid leaving unused files on the system.

This includes:

- closing the pipe file descriptors
- stopping the simulator process if it is still running
- removing the named pipes
- deleting the temporary directory

## Simulator Class

Most of the logic is implemented in the `Simulator` class.

This class is responsible for:

- creating the temporary pipe directory
- creating the named pipes
- launching the simulator process
- opening the pipes
- cleaning up resources when the program finishes

The main initialization happens in the function:

bool openSimulator();

This function prepares everything needed for the client to communicate with the simulator.

## MainWindow Class

The MainWindow class manages:

- GUI creation 
- Simulation execution control 
- User interaction 
- Statistics display 
-	Reset logic 
-	Winner detection

## WorldWidget Class

The WorldWidget class is for visualizing the bug world.

The WorldWidget class manages:

Draw hex grid dynamically scaled 
Coloring: 
- Walls (#) 
-	Red bugs 
-	Black bugs 
-	Home bases 
-	Food
Render fading traces 
Display winner 

## World Visualization

•	A dynamically scaled hexagonal grid, which depends on the world file
•	Walls		gray
•	Food		green
•	Black Team:
  o	Black hexagon → Black bug 
  o	Blue hexagon → Black home base 
  o	Black dots → Trace history of black bugs
•	Red Team
  o	Red hexagon → Red bug 
  o	Pink hexagon → Red home base 
  o	Red dots → Trace history of red bugs

### 1.	Trace Lines with Fading
The last N positions of each bug are stored in the Trace length
These positions are drawn as small dots
The most recent position is fully opaque
Older positions gradually fade out

## Controls for simulation:

A toolbar at the bottom of the window provides interactive controls.

Start / Pause
•	Starts automatic stepping using a timer 
•	Mode indicator switches between: 
  o	Mode: AUTO 
  o	Mode: MANUAL 
•	AUTO mode repeatedly sends:
•	STEP ticks_per_frame

Reset
•	Stops simulation
•	Restarts the simulator process
•	Clears trace history
•	Reloads initial world state

Step N
Stops AUTO mode		N range: [1, 10000]

Trace Length Control
	Default trace length: 20		Range: [0, 200]
  
## Simulation Status Display

The bottom toolbar displays:
•  Current cycle number
•  Number of red bugs alive
•  Number of black bugs alive
•  Food collected by each team

Example:
Cycle: 120 | Red: 8 | Black: 5 | Food R:3 B:4 Mode: AUTO
if a team wins:
Cycle: 120 | Red: 0 | Black: 5 | Food R:3 B:4| Black wins! Mode: MANUAL
