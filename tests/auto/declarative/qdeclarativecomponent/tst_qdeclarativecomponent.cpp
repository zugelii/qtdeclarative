/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/
#include <qtest.h>
#include <QDebug>

#include <QtDeclarative/qdeclarativeengine.h>
#include <QtDeclarative/qdeclarativecomponent.h>
#include <QtDeclarative/qsgitem.h>
#include <QtDeclarative/qdeclarativeproperty.h>
#include <QtDeclarative/qdeclarativeincubator.h>
#include <qcolor.h>
#include "../shared/util.h"
#include "../../../shared/util.h"

class MyIC : public QObject, public QDeclarativeIncubationController
{
    Q_OBJECT
public:
    MyIC() { startTimer(5); }
protected:
    virtual void timerEvent(QTimerEvent*) {
        incubateFor(5);
    }
};

class tst_qdeclarativecomponent : public QObject
{
    Q_OBJECT
public:
    tst_qdeclarativecomponent() { engine.setIncubationController(&ic); }

private slots:
    void null();
    void loadEmptyUrl();
    void qmlCreateObject();
    void qmlCreateObjectWithProperties();
    void qmlIncubateObject();

private:
    QDeclarativeEngine engine;
    MyIC ic;
};

void tst_qdeclarativecomponent::null()
{
    {
        QDeclarativeComponent c;
        QVERIFY(c.isNull());
    }

    {
        QDeclarativeComponent c(&engine);
        QVERIFY(c.isNull());
    }
}


void tst_qdeclarativecomponent::loadEmptyUrl()
{
    QDeclarativeComponent c(&engine);
    c.loadUrl(QUrl());

    QVERIFY(c.isError());
    QCOMPARE(c.errors().count(), 1);
    QDeclarativeError error = c.errors().first();
    QCOMPARE(error.url(), QUrl());
    QCOMPARE(error.line(), -1);
    QCOMPARE(error.column(), -1);
    QCOMPARE(error.description(), QLatin1String("Invalid empty URL"));
}

void tst_qdeclarativecomponent::qmlIncubateObject()
{
    QDeclarativeComponent component(&engine, QUrl::fromLocalFile(TESTDATA("incubateObject.qml")));
    QObject *object = component.create();
    QVERIFY(object != 0);
    QCOMPARE(object->property("test1").toBool(), true);
    QCOMPARE(object->property("test2").toBool(), false);

    QTRY_VERIFY(object->property("test2").toBool() == true);

    delete object;
}

void tst_qdeclarativecomponent::qmlCreateObject()
{
    QDeclarativeEngine engine;
    QDeclarativeComponent component(&engine, QUrl::fromLocalFile(TESTDATA("createObject.qml")));
    QObject *object = component.create();
    QVERIFY(object != 0);

    QObject *testObject1 = object->property("qobject").value<QObject*>();
    QVERIFY(testObject1);
    QVERIFY(testObject1->parent() == object);

    QObject *testObject2 = object->property("declarativeitem").value<QObject*>();
    QVERIFY(testObject2);
    QVERIFY(testObject2->parent() == object);
    QCOMPARE(testObject2->metaObject()->className(), "QSGItem");
}

void tst_qdeclarativecomponent::qmlCreateObjectWithProperties()
{
    QDeclarativeEngine engine;
    QDeclarativeComponent component(&engine, QUrl::fromLocalFile(TESTDATA("createObjectWithScript.qml")));
    QVERIFY2(component.errorString().isEmpty(), component.errorString().toUtf8());
    QObject *object = component.create();
    QVERIFY(object != 0);

    QObject *testObject1 = object->property("declarativerectangle").value<QObject*>();
    QVERIFY(testObject1);
    QVERIFY(testObject1->parent() == object);
    QCOMPARE(testObject1->property("x").value<int>(), 17);
    QCOMPARE(testObject1->property("y").value<int>(), 17);
    QCOMPARE(testObject1->property("color").value<QColor>(), QColor(255,255,255));
    QCOMPARE(QDeclarativeProperty::read(testObject1,"border.width").toInt(), 3);
    QCOMPARE(QDeclarativeProperty::read(testObject1,"innerRect.border.width").toInt(), 20);
    delete testObject1;

    QObject *testObject2 = object->property("declarativeitem").value<QObject*>();
    QVERIFY(testObject2);
    QVERIFY(testObject2->parent() == object);
    //QCOMPARE(testObject2->metaObject()->className(), "QDeclarativeItem_QML_2");
    QCOMPARE(testObject2->property("x").value<int>(), 17);
    QCOMPARE(testObject2->property("y").value<int>(), 17);
    QCOMPARE(testObject2->property("testBool").value<bool>(), true);
    QCOMPARE(testObject2->property("testInt").value<int>(), 17);
    QCOMPARE(testObject2->property("testObject").value<QObject*>(), object);
    delete testObject2;

    QObject *testBindingObj = object->property("bindingTestObject").value<QObject*>();
    QVERIFY(testBindingObj);
    QCOMPARE(testBindingObj->parent(), object);
    QCOMPARE(testBindingObj->property("testValue").value<int>(), 300);
    object->setProperty("width", 150);
    QCOMPARE(testBindingObj->property("testValue").value<int>(), 150 * 3);
    delete testBindingObj;

    QObject *testBindingThisObj = object->property("bindingThisTestObject").value<QObject*>();
    QVERIFY(testBindingThisObj);
    QCOMPARE(testBindingThisObj->parent(), object);
    QCOMPARE(testBindingThisObj->property("testValue").value<int>(), 900);
    testBindingThisObj->setProperty("width", 200);
    QCOMPARE(testBindingThisObj->property("testValue").value<int>(), 200 * 3);
    delete testBindingThisObj;
}

QTEST_MAIN(tst_qdeclarativecomponent)

#include "tst_qdeclarativecomponent.moc"
