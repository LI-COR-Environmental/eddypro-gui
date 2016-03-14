/*
 * Copyright (C) 2008  Remko Troncon
 * Licensed under the MIT license.
 * See COPYING for license details.
 */

#include "QtTestUtil/TestRegistry.h"

#include <QtTest/QtTest>

namespace QtTestUtil {

TestRegistry* TestRegistry::getInstance() {
    static TestRegistry registry;
    return &registry;
}

void TestRegistry::registerTest(QObject* test) {
    tests_ += test;

    static int n = 0;
    ++n;
    qDebug() << "n" << n << tests_.last()->objectName();
}

int TestRegistry::runTests(int argc, char* argv[]) {
    int result = 0;
    int exec = 0;
    foreach(QObject* test, tests_) {
        ++exec;
        result |= QTest::qExec(test, argc, argv);
        qDebug() << "exec: " << exec;
    }
    return result;
}

}
