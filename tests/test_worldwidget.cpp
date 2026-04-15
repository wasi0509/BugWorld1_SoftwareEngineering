#include <QtTest>
#include "../src/WorldWidget.h"

class TestWorldWidget : public QObject
{
    Q_OBJECT

private slots:
    void testTraceLengthLimit(); // Ensure that trace history does not exceed the specified length
    void testClearTraces();      // Verify that clearTraces properly clears all trace history
    void testWinnerText();       // Verify that setWinner correctly updates the winner text and that it can be cleared
};

void TestWorldWidget::testTraceLengthLimit() // Ensure that trace history does not exceed the specified length
{
    WorldWidget widget;
    widget.setTraceLength(3);

    WorldState state;
    state.rows = 1;
    state.cols = 1;
    state.mapData = {"R."};

    for (int i = 0; i < 10; ++i) // Simulate 10 updates with the same state
        widget.setWorldState(state);

    // Trace size not exceed 3
    QCOMPARE(widget.getRedTraceSize(), 3);
    QCOMPARE(widget.getBlackTraceSize(), 3);
}

void TestWorldWidget::testClearTraces()
{
    WorldWidget widget;
    widget.setTraceLength(5);

    WorldState state;
    state.rows = 1;
    state.cols = 1;
    state.mapData = {"R"};

    widget.setWorldState(state);

    QVERIFY(widget.getRedTraceSize() > 0);

    widget.clearTraces();

    QCOMPARE(widget.getRedTraceSize(), 0);
    QCOMPARE(widget.getBlackTraceSize(), 0);
}

void TestWorldWidget::testWinnerText()
{
    WorldWidget widget;

    widget.setWinner("Red Wins!");
    QCOMPARE(widget.getWinnerText(), QString("Red Wins!"));

    widget.setWinner("");
    QCOMPARE(widget.getWinnerText(), QString(""));
}

QTEST_MAIN(TestWorldWidget)
#include "test_worldwidget.moc"