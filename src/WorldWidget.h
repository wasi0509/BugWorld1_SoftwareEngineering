#ifndef WORLDWIDGET_H
#define WORLDWIDGET_H

#include <QWidget>
#include <deque>
#include "Simulator.h"

class WorldWidget : public QWidget
{
    Q_OBJECT

public:
    explicit WorldWidget(QWidget *parent = nullptr); // Constructor
    void setWorldState(const WorldState &state);     // Update world state and add to trace history
    void setTraceLength(int length);                 // Set how many past frames to keep in trace history
    void setWinner(const QString &text);             // Set winner text to display on the widget
    void clearTraces();                              // Clear all trace history and update display

    // Testing getters
    int getRedTraceSize() const { return redTraces.size(); }
    int getBlackTraceSize() const { return blackTraces.size(); }
    int getTraceLength() const { return traceLength; }
    QString getWinnerText() const { return winnerText; }

protected:
    void paintEvent(QPaintEvent *event) override; // Important part: Override paint event to draw the world and traces

private:
    WorldState world;

    std::deque<std::vector<QPair<int, int>>> redTraces;
    std::deque<std::vector<QPair<int, int>>> blackTraces;
    int traceLength = 20;
    double hexSize = 15.0;

    QPointF hexToPixel(int row, int col) const;
    QString winnerText;
};

#endif