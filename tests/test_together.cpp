#include <QtTest>
#include "../src/MainWindow.h"

class TestTogether : public QObject
{
    Q_OBJECT

private slots:
    void testInitialState(); // Verify that MainWindow initializes correctly in test mode
    void testToggleRun();    // Verify that toggling run mode starts and stops the timer as expected
};

void TestTogether::testInitialState()
{
    MainWindow window("dummy.world",
                      "dummy.bug",
                      "dummy.bug",
                      1, 1, nullptr, true);

    QCOMPARE(window.getModeText(), QString("Mode: MANUAL"));
    QVERIFY(!window.isTimerActive());
    QVERIFY(window.getWorldWidget() != nullptr);
}

void TestTogether::testToggleRun()
{
    MainWindow window("dummy.world",
                      "dummy.bug",
                      "dummy.bug",
                      1, 1, nullptr, true);

    // Start
    QMetaObject::invokeMethod(&window, "toggleRun");

    QVERIFY(window.isTimerActive());
    QCOMPARE(window.getModeText(), QString("Mode: AUTO"));

    // Stop
    QMetaObject::invokeMethod(&window, "toggleRun");

    QVERIFY(!window.isTimerActive());
    QCOMPARE(window.getModeText(), QString("Mode: MANUAL"));
}

QTEST_MAIN(TestTogether)
#include "test_together.moc"