#include <QtTest>
#include "../src/Simulator.h"

class TestSimulator : public QObject
{
    Q_OBJECT

private slots:

    void testParseBasicResponse();     // Verify that parseResponse correctly parses a well-formed response string
    void testParseEmptyResponse();     // Verify that parseResponse handles an empty response string without crashing and returns default values
    void testParseMalformedResponse(); // Verify that parseResponse handles a malformed response string gracefully, defaulting to 0 or empty values where appropriate
};

void TestSimulator::testParseBasicResponse()
{
    Simulator sim("dummy", "dummy", "dummy");

    std::string response =
        "CYCLE 10\n"
        "MAP 2 3\n"
        "ROW . R B\n"
        "ROW # . .\n"
        "STATS 3 2 5 1\n"
        "END\n";

    WorldState state = sim.parseResponse(response);

    QCOMPARE(state.cycle, 10);     // Check cycle count
    QCOMPARE(state.rows, 2);       // Check row count
    QCOMPARE(state.cols, 3);       // Check column count
    QCOMPARE(state.redAlive, 3);   // Check red alive count
    QCOMPARE(state.blackAlive, 2); // Check black alive count
    QCOMPARE(state.redFood, 5);    // Check red food count
    QCOMPARE(state.blackFood, 1);  // Check black food count

    QCOMPARE((int)state.mapData.size(), 2);
}

void TestSimulator::testParseEmptyResponse()
{
    Simulator sim("d", "d", "d");
    WorldState state = sim.parseResponse("");

    // Expect all default values since response is empty
    QCOMPARE(state.cycle, 0); //  default to 0
    QCOMPARE(state.rows, 0);  //  default to 0
    QCOMPARE(state.cols, 0);  //  default to 0
    QVERIFY(state.mapData.empty());
}

void TestSimulator::testParseMalformedResponse()
{
    Simulator sim("d", "d", "d");

    std::string response =
        "CYCLE abc\n"
        "MAP X Y\n"
        "ROW ???\n"
        "STATS x y z w\n"
        "END\n";

    WorldState state = sim.parseResponse(response);

    // handle malformed values, defaulting to 0 or empty
    QCOMPARE(state.cycle, 0);
    QCOMPARE(state.rows, 0);
    QCOMPARE(state.cols, 0);
    QCOMPARE(state.redAlive, 0);
    QCOMPARE(state.blackAlive, 0);
}

QTEST_MAIN(TestSimulator)
#include "test_simulator.moc"