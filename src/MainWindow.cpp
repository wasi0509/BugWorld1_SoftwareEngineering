#include "MainWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QDebug>

// MainWindow constructor initializes the UI and sets up connections
MainWindow::MainWindow(QString world_map,
                       QString bug1,
                       QString bug2,
                       int ticks_per_frame,
                       int fps,
                       QWidget *parent,
                       bool testMode) : QMainWindow(parent),
                                        sim(world_map.toStdString(),
                                            bug1.toStdString(),
                                            bug2.toStdString()),
                                        worldPath(world_map),
                                        bug1Path(bug1),
                                        bug2Path(bug2),
                                        ticks_per_frame(ticks_per_frame),
                                        fps(fps),
                                        isTestMode(testMode)
{
    // Only open simulator if not in test mode
    if (!isTestMode)
    {
        sim.openSimulator();
    }

    QWidget *central = new QWidget;
    setCentralWidget(central);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    central->setLayout(mainLayout);

    worldWidget = new WorldWidget;
    mainLayout->addWidget(worldWidget);

    // Control panel with buttons and stats
    QHBoxLayout *controls = new QHBoxLayout;

    stepSpin = new QSpinBox;
    stepSpin->setRange(1, 10000);
    stepSpin->setValue(1);

    // Initialize buttons
    startButton = new QPushButton("Start");
    stepButton = new QPushButton("Step");
    resetButton = new QPushButton("Reset");

    // Add controls to layout
    controls->addWidget(startButton);
    controls->addWidget(resetButton);
    controls->addWidget(new QLabel("Step N:"));
    controls->addWidget(stepButton);
    controls->addWidget(stepSpin);

    // Trace length control
    traceSpin = new QSpinBox;
    traceSpin->setRange(0, 200);
    traceSpin->setValue(20);
    controls->addWidget(new QLabel("Trace Length:"));
    controls->addWidget(traceSpin);

    // Stats and mode labels
    statsLabel = new QLabel("Stats");
    modeLabel = new QLabel("Mode: MANUAL");

    controls->addWidget(statsLabel);
    controls->addWidget(modeLabel);

    mainLayout->addLayout(controls);

    // Connect signals and slots
    connect(&timer, &QTimer::timeout, this, &MainWindow::updateSimulation);
    connect(startButton, &QPushButton::clicked, this, &MainWindow::toggleRun);
    connect(stepButton, &QPushButton::clicked, this, &MainWindow::manualStep);
    connect(resetButton, &QPushButton::clicked, this, &MainWindow::resetSimulation);
    connect(traceSpin, QOverload<int>::of(&QSpinBox::valueChanged), worldWidget, &WorldWidget::setTraceLength);

    // Set timer interval based on fps
    timer.setInterval(1000 / fps);

    // Fetch initial state from simulator and display it
    sim.sendCommand("FETCH");
    std::string response = sim.readResponse();
    WorldState initial = sim.parseResponse(response);
    worldWidget->setWorldState(initial);

    // Update stats label with initial state
    QString stats = QString("Cycle: %1 | Red: %2 | Black: %3 | Food R:%4 B:%5")
                        .arg(initial.cycle)
                        .arg(initial.redAlive)
                        .arg(initial.blackAlive)
                        .arg(initial.redFood)
                        .arg(initial.blackFood);
    statsLabel->setText(stats);
}

// perform a given number of steps and update the UI
void MainWindow::performStep(int ticks)
{
    if (isTestMode)
        return;
    sim.sendCommand("STEP " + std::to_string(ticks));
    std::string response = sim.readResponse();
    WorldState state = sim.parseResponse(response);

    worldWidget->setWorldState(state);
    updateStats(state);

    // Stop automatically if one team died
    if (state.redAlive == 0 || state.blackAlive == 0)
    {
        timer.stop();
        startButton->setText("Start");

        QString winner =
            state.redAlive > 0 ? "Red Wins!" : state.blackAlive > 0 ? "Black Wins!"
                                                                    : "Draw!";

        statsLabel->setText(statsLabel->text() + " | " + winner);
        worldWidget->setWinner(winner);
    }
}

// Update stats label based on current world state
void MainWindow::updateStats(const WorldState &state)
{
    QString stats = QString("Cycle: %1 | Red: %2 | Black: %3 | Food R:%4 B:%5")
                        .arg(state.cycle)
                        .arg(state.redAlive)
                        .arg(state.blackAlive)
                        .arg(state.redFood)
                        .arg(state.blackFood);

    statsLabel->setText(stats);
}

//  Destructor
MainWindow::~MainWindow()
{
    if (!isTestMode)
    {
        sim.sendCommand("QUIT");
    }
    timer.stop();
}

// Called by timer to update simulation state
void MainWindow::updateSimulation()
{
    performStep(ticks_per_frame);
}

// Start/pause simulation
void MainWindow::toggleRun()
{
    if (timer.isActive())
    {
        timer.stop();
        startButton->setText("Start");
        modeLabel->setText("Mode: MANUAL");
    }
    else
    {
        timer.start(1000 / fps);
        startButton->setText("Pause");
        modeLabel->setText("Mode: AUTO");
    }
}

// Perform a manual step when in paused mode
void MainWindow::manualStep()
{

    if (timer.isActive())
    {
        timer.stop();
        startButton->setText("Start");
        modeLabel->setText("Mode: MANUAL");
    }

    int n = stepSpin->value();
    performStep(n);
}

// Reset simulation to initial state
void MainWindow::resetSimulation()
{
    timer.stop();
    worldWidget->setWinner("");
    startButton->setText("Start");

    if (!isTestMode)
    {
        sim.sendCommand("QUIT");

        sim = Simulator(worldPath.toStdString(),
                        bug1Path.toStdString(),
                        bug2Path.toStdString());

        sim.openSimulator();
        worldWidget->clearTraces();
        sim.sendCommand("FETCH");
        std::string response = sim.readResponse();
        WorldState initial = sim.parseResponse(response);
        worldWidget->setWorldState(initial);

        QString stats = QString("Cycle: %1 | Red: %2 | Black: %3 | Food R:%4 B:%5")
                            .arg(initial.cycle)
                            .arg(initial.redAlive)
                            .arg(initial.blackAlive)
                            .arg(initial.redFood)
                            .arg(initial.blackFood);

        statsLabel->setText(stats);
        updateStats(initial);
    }
}
