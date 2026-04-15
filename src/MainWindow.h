#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QSpinBox>
#include <QLabel>
#include "Simulator.h"
#include "WorldWidget.h"
#include <QPushButton>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QString world_map,
               QString bug1,
               QString bug2,
               int ticks_per_frame,
               int fps,
               QWidget *parent,
               bool testMode = false);
    ~MainWindow();

    // Testing getters
    QString getStatsText() const { return statsLabel->text(); }
    QString getModeText() const { return modeLabel->text(); }
    bool isTimerActive() const { return timer.isActive(); }
    WorldWidget *getWorldWidget() const { return worldWidget; }

private slots:
    void updateSimulation();                   // Called by timer to update simulation state
    void toggleRun();                          // Start/pause simulation
    void manualStep();                         // Perform a manual step when in paused mode
    void resetSimulation();                    // Reset simulation to initial state
    void performStep(int ticks);               // Helper function to perform a given number of steps and update UI
    void updateStats(const WorldState &state); // Update stats label based on current world state

private:
    Simulator sim;
    WorldWidget *worldWidget;
    QTimer timer;

    QString worldPath, bug1Path, bug2Path;

    int ticks_per_frame;
    int fps;

    QPushButton *startButton;
    QPushButton *stepButton;
    QPushButton *resetButton;

    QSpinBox *traceSpin;
    QSpinBox *stepSpin;
    QLabel *statsLabel;
    QLabel *modeLabel;

    bool isTestMode;
};

#endif