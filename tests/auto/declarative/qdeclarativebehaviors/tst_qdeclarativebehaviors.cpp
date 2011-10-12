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
#include <qsignalspy.h>
#include <QtDeclarative/qdeclarativeengine.h>
#include <QtDeclarative/qdeclarativecomponent.h>
#include <QtDeclarative/qsgview.h>
#include <private/qsgrectangle_p.h>
#include <private/qsgtext_p.h>
#include <private/qdeclarativebehavior_p.h>
#include <private/qdeclarativeanimation_p.h>
#include <private/qsgitem_p.h>
#include "../shared/util.h"
#include "../../../shared/util.h"

class tst_qdeclarativebehaviors : public QObject
{
    Q_OBJECT
public:
    tst_qdeclarativebehaviors() {}

private slots:
    void simpleBehavior();
    void scriptTriggered();
    void cppTriggered();
    void loop();
    void colorBehavior();
    void parentBehavior();
    void replaceBinding();
    //void transitionOverrides();
    void group();
    void emptyBehavior();
    void explicitSelection();
    void nonSelectingBehavior();
    void reassignedAnimation();
    void disabled();
    void dontStart();
    void startup();
    void groupedPropertyCrash();
    void runningTrue();
    void sameValue();
    void delayedRegistration();
};

void tst_qdeclarativebehaviors::simpleBehavior()
{
    QDeclarativeEngine engine;
    QDeclarativeComponent c(&engine, QUrl::fromLocalFile(TESTDATA("simple.qml")));
    QSGRectangle *rect = qobject_cast<QSGRectangle*>(c.create());
    QTRY_VERIFY(rect);
    QTRY_VERIFY(qobject_cast<QDeclarativeBehavior*>(rect->findChild<QDeclarativeBehavior*>("MyBehavior"))->animation());

    QSGItemPrivate::get(rect)->setState("moved");
    QTRY_VERIFY(qobject_cast<QSGRectangle*>(rect->findChild<QSGRectangle*>("MyRect"))->x() > 0);
    QTRY_VERIFY(qobject_cast<QSGRectangle*>(rect->findChild<QSGRectangle*>("MyRect"))->x() < 200);
    //i.e. the behavior has been triggered

    delete rect;
}

void tst_qdeclarativebehaviors::scriptTriggered()
{
    QDeclarativeEngine engine;
    QDeclarativeComponent c(&engine, QUrl::fromLocalFile(TESTDATA("scripttrigger.qml")));
    QSGRectangle *rect = qobject_cast<QSGRectangle*>(c.create());
    QTRY_VERIFY(rect);

    rect->setColor(QColor("red"));
    QTRY_VERIFY(qobject_cast<QSGRectangle*>(rect->findChild<QSGRectangle*>("MyRect"))->x() > 0);
    QTRY_VERIFY(qobject_cast<QSGRectangle*>(rect->findChild<QSGRectangle*>("MyRect"))->x() < 200);
    //i.e. the behavior has been triggered

    delete rect;
}

void tst_qdeclarativebehaviors::cppTriggered()
{
    QDeclarativeEngine engine;
    QDeclarativeComponent c(&engine, QUrl::fromLocalFile(TESTDATA("cpptrigger.qml")));
    QSGRectangle *rect = qobject_cast<QSGRectangle*>(c.create());
    QTRY_VERIFY(rect);

    QSGRectangle *innerRect = qobject_cast<QSGRectangle*>(rect->findChild<QSGRectangle*>("MyRect"));
    QTRY_VERIFY(innerRect);

    innerRect->setProperty("x", 200);
    QTRY_VERIFY(innerRect->x() > 0);
    QEXPECT_FAIL("", "QTBUG-21001", Continue);
    QTRY_VERIFY(innerRect->x() < 200);  //i.e. the behavior has been triggered

    delete rect;
}

void tst_qdeclarativebehaviors::loop()
{
    QDeclarativeEngine engine;
    QDeclarativeComponent c(&engine, QUrl::fromLocalFile(TESTDATA("loop.qml")));
    QSGRectangle *rect = qobject_cast<QSGRectangle*>(c.create());
    QTRY_VERIFY(rect);

    //don't crash
    QSGItemPrivate::get(rect)->setState("moved");

    delete rect;
}

void tst_qdeclarativebehaviors::colorBehavior()
{
    QDeclarativeEngine engine;
    QDeclarativeComponent c(&engine, QUrl::fromLocalFile(TESTDATA("color.qml")));
    QSGRectangle *rect = qobject_cast<QSGRectangle*>(c.create());
    QTRY_VERIFY(rect);

    QSGItemPrivate::get(rect)->setState("red");
    QTRY_VERIFY(qobject_cast<QSGRectangle*>(rect->findChild<QSGRectangle*>("MyRect"))->color() != QColor("red"));
    QTRY_VERIFY(qobject_cast<QSGRectangle*>(rect->findChild<QSGRectangle*>("MyRect"))->color() != QColor("green"));
    //i.e. the behavior has been triggered

    delete rect;
}

void tst_qdeclarativebehaviors::parentBehavior()
{
    QDeclarativeEngine engine;
    QDeclarativeComponent c(&engine, QUrl::fromLocalFile(TESTDATA("parent.qml")));
    QSGRectangle *rect = qobject_cast<QSGRectangle*>(c.create());
    QTRY_VERIFY(rect);

    QSGItemPrivate::get(rect)->setState("reparented");
    QTRY_VERIFY(rect->findChild<QSGRectangle*>("MyRect")->parentItem() != rect->findChild<QSGItem*>("NewParent"));
    QTRY_VERIFY(rect->findChild<QSGRectangle*>("MyRect")->parentItem() == rect->findChild<QSGItem*>("NewParent"));

    delete rect;
}

void tst_qdeclarativebehaviors::replaceBinding()
{
    QDeclarativeEngine engine;
    QDeclarativeComponent c(&engine, QUrl::fromLocalFile(TESTDATA("binding.qml")));
    QSGRectangle *rect = qobject_cast<QSGRectangle*>(c.create());
    QTRY_VERIFY(rect);

    QSGItemPrivate::get(rect)->setState("moved");
    QSGRectangle *innerRect = qobject_cast<QSGRectangle*>(rect->findChild<QSGRectangle*>("MyRect"));
    QTRY_VERIFY(innerRect);
    QTRY_VERIFY(innerRect->x() > 0);
    QTRY_VERIFY(innerRect->x() < 200);
    //i.e. the behavior has been triggered
    QTRY_COMPARE(innerRect->x(), (qreal)200);
    rect->setProperty("basex", 10);
    QTRY_COMPARE(innerRect->x(), (qreal)200);
    rect->setProperty("movedx", 210);
    QTRY_COMPARE(innerRect->x(), (qreal)210);

    QSGItemPrivate::get(rect)->setState("");
    QTRY_VERIFY(innerRect->x() > 10);
    QTRY_VERIFY(innerRect->x() < 210);  //i.e. the behavior has been triggered
    QTRY_COMPARE(innerRect->x(), (qreal)10);
    rect->setProperty("movedx", 200);
    QTRY_COMPARE(innerRect->x(), (qreal)10);
    rect->setProperty("basex", 20);
    QTRY_COMPARE(innerRect->x(), (qreal)20);

    delete rect;
}

void tst_qdeclarativebehaviors::group()
{
    /* XXX TODO Create a test element for this case.
    {
        QDeclarativeEngine engine;
        QDeclarativeComponent c(&engine, QUrl::fromLocalFile(TESTDATA("groupProperty.qml")));
        QSGRectangle *rect = qobject_cast<QSGRectangle*>(c.create());
        qDebug() << c.errorString();
        QTRY_VERIFY(rect);

        QSGItemPrivate::get(rect)->setState("moved");
        //QTest::qWait(200);
        QTRY_VERIFY(qobject_cast<QSGRectangle*>(rect->findChild<QSGRectangle*>("MyRect"))->x() > 0);
        QTRY_VERIFY(qobject_cast<QSGRectangle*>(rect->findChild<QSGRectangle*>("MyRect"))->x() < 200);
        //i.e. the behavior has been triggered

        delete rect;
    }
    */

    {
        QDeclarativeEngine engine;
        QDeclarativeComponent c(&engine, QUrl::fromLocalFile(TESTDATA("groupProperty2.qml")));
        QSGRectangle *rect = qobject_cast<QSGRectangle*>(c.create());
        QTRY_VERIFY(rect);

        QSGItemPrivate::get(rect)->setState("moved");
        QTRY_VERIFY(qobject_cast<QSGRectangle*>(rect->findChild<QSGRectangle*>("MyRect"))->border()->width() > 0);
        QTRY_VERIFY(qobject_cast<QSGRectangle*>(rect->findChild<QSGRectangle*>("MyRect"))->border()->width() < 4);
        //i.e. the behavior has been triggered

        delete rect;
    }
}

void tst_qdeclarativebehaviors::emptyBehavior()
{
    QDeclarativeEngine engine;
    QDeclarativeComponent c(&engine, QUrl::fromLocalFile(TESTDATA("empty.qml")));
    QSGRectangle *rect = qobject_cast<QSGRectangle*>(c.create());
    QVERIFY(rect);

    QSGItemPrivate::get(rect)->setState("moved");
    qreal x = qobject_cast<QSGRectangle*>(rect->findChild<QSGRectangle*>("MyRect"))->x();
    QCOMPARE(x, qreal(200));    //should change immediately

    delete rect;
}

void tst_qdeclarativebehaviors::explicitSelection()
{
    QDeclarativeEngine engine;
    QDeclarativeComponent c(&engine, QUrl::fromLocalFile(TESTDATA("explicit.qml")));
    QSGRectangle *rect = qobject_cast<QSGRectangle*>(c.create());
    QVERIFY(rect);

    QSGItemPrivate::get(rect)->setState("moved");
    QTRY_VERIFY(qobject_cast<QSGRectangle*>(rect->findChild<QSGRectangle*>("MyRect"))->x() > 0);
    QTRY_VERIFY(qobject_cast<QSGRectangle*>(rect->findChild<QSGRectangle*>("MyRect"))->x() < 200);
    //i.e. the behavior has been triggered

    delete rect;
}

void tst_qdeclarativebehaviors::nonSelectingBehavior()
{
    QDeclarativeEngine engine;
    QDeclarativeComponent c(&engine, QUrl::fromLocalFile(TESTDATA("nonSelecting2.qml")));
    QSGRectangle *rect = qobject_cast<QSGRectangle*>(c.create());
    QVERIFY(rect);

    QSGItemPrivate::get(rect)->setState("moved");
    qreal x = qobject_cast<QSGRectangle*>(rect->findChild<QSGRectangle*>("MyRect"))->x();
    QCOMPARE(x, qreal(200));    //should change immediately

    delete rect;
}

void tst_qdeclarativebehaviors::reassignedAnimation()
{
    QDeclarativeEngine engine;
    QDeclarativeComponent c(&engine, QUrl::fromLocalFile(TESTDATA("reassignedAnimation.qml")));
    QString warning = QUrl::fromLocalFile(TESTDATA("reassignedAnimation.qml")).toString() + ":9:9: QML Behavior: Cannot change the animation assigned to a Behavior.";
    QTest::ignoreMessage(QtWarningMsg, qPrintable(warning));
    QSGRectangle *rect = qobject_cast<QSGRectangle*>(c.create());
    QVERIFY(rect);
    QCOMPARE(qobject_cast<QDeclarativeNumberAnimation*>(
                 rect->findChild<QDeclarativeBehavior*>("MyBehavior")->animation())->duration(), 200);

    delete rect;
}

void tst_qdeclarativebehaviors::disabled()
{
    QDeclarativeEngine engine;
    QDeclarativeComponent c(&engine, QUrl::fromLocalFile(TESTDATA("disabled.qml")));
    QSGRectangle *rect = qobject_cast<QSGRectangle*>(c.create());
    QVERIFY(rect);
    QCOMPARE(rect->findChild<QDeclarativeBehavior*>("MyBehavior")->enabled(), false);

    QSGItemPrivate::get(rect)->setState("moved");
    qreal x = qobject_cast<QSGRectangle*>(rect->findChild<QSGRectangle*>("MyRect"))->x();
    QCOMPARE(x, qreal(200));    //should change immediately

    delete rect;
}

void tst_qdeclarativebehaviors::dontStart()
{
    QDeclarativeEngine engine;

    QDeclarativeComponent c(&engine, QUrl::fromLocalFile(TESTDATA("dontStart.qml")));

    QString warning = c.url().toString() + ":13:13: QML NumberAnimation: setRunning() cannot be used on non-root animation nodes.";
    QTest::ignoreMessage(QtWarningMsg, qPrintable(warning));
    QSGRectangle *rect = qobject_cast<QSGRectangle*>(c.create());
    QVERIFY(rect);

    QDeclarativeAbstractAnimation *myAnim = rect->findChild<QDeclarativeAbstractAnimation*>("MyAnim");
    QVERIFY(myAnim && myAnim->qtAnimation());
    QVERIFY(myAnim->qtAnimation()->state() == QAbstractAnimation::Stopped);

    delete rect;
}

void tst_qdeclarativebehaviors::startup()
{
    {
        QDeclarativeEngine engine;
        QDeclarativeComponent c(&engine, QUrl::fromLocalFile(TESTDATA("startup.qml")));
        QSGRectangle *rect = qobject_cast<QSGRectangle*>(c.create());
        QVERIFY(rect);

        QSGRectangle *innerRect = rect->findChild<QSGRectangle*>("innerRect");
        QVERIFY(innerRect);

        QCOMPARE(innerRect->x(), qreal(100));    //should be set immediately

        delete rect;
    }

    {
        QDeclarativeEngine engine;
        QDeclarativeComponent c(&engine, QUrl::fromLocalFile(TESTDATA("startup2.qml")));
        QSGRectangle *rect = qobject_cast<QSGRectangle*>(c.create());
        QVERIFY(rect);

        QSGRectangle *innerRect = rect->findChild<QSGRectangle*>("innerRect");
        QVERIFY(innerRect);

        QSGText *text = rect->findChild<QSGText*>();
        QVERIFY(text);

        QCOMPARE(innerRect->x(), text->width());    //should be set immediately

        delete rect;
    }
}

//QTBUG-10799
void tst_qdeclarativebehaviors::groupedPropertyCrash()
{
    QDeclarativeEngine engine;
    QDeclarativeComponent c(&engine, QUrl::fromLocalFile(TESTDATA("groupedPropertyCrash.qml")));
    QSGRectangle *rect = qobject_cast<QSGRectangle*>(c.create());
    QVERIFY(rect);  //don't crash
}

//QTBUG-5491
void tst_qdeclarativebehaviors::runningTrue()
{
    QDeclarativeEngine engine;
    QDeclarativeComponent c(&engine, QUrl::fromLocalFile(TESTDATA("runningTrue.qml")));
    QSGRectangle *rect = qobject_cast<QSGRectangle*>(c.create());
    QVERIFY(rect);

    QDeclarativeAbstractAnimation *animation = rect->findChild<QDeclarativeAbstractAnimation*>("rotAnim");
    QVERIFY(animation);

    QSignalSpy runningSpy(animation, SIGNAL(runningChanged(bool)));
    rect->setProperty("myValue", 180);
    QTRY_VERIFY(runningSpy.count() > 0);
}

//QTBUG-12295
void tst_qdeclarativebehaviors::sameValue()
{
    QDeclarativeEngine engine;
    QDeclarativeComponent c(&engine, QUrl::fromLocalFile(TESTDATA("qtbug12295.qml")));
    QSGRectangle *rect = qobject_cast<QSGRectangle*>(c.create());
    QVERIFY(rect);

    QSGRectangle *target = rect->findChild<QSGRectangle*>("myRect");
    QVERIFY(target);

    target->setX(100);
    QCOMPARE(target->x(), qreal(100));

    target->setProperty("x", 0);
#ifdef QT_BUILD_INTERNAL
    QEXPECT_FAIL("", "QTBUG-12295 - Behaviour on x {} does not work consistently.", Continue);
#endif
    QTRY_VERIFY(target->x() != qreal(0) && target->x() != qreal(100));
    QTRY_VERIFY(target->x() == qreal(0));   //make sure Behavior has finished.

    target->setX(100);
    QCOMPARE(target->x(), qreal(100));

    //this is the main point of the test -- the behavior needs to be triggered again
    //even though we set 0 twice in a row.
    target->setProperty("x", 0);
    QTRY_VERIFY(target->x() != qreal(0) && target->x() != qreal(100));
}

//QTBUG-18362
void tst_qdeclarativebehaviors::delayedRegistration()
{
    QDeclarativeEngine engine;

    QDeclarativeComponent c(&engine, QUrl::fromLocalFile(TESTDATA("delayedRegistration.qml")));
    QSGRectangle *rect = qobject_cast<QSGRectangle*>(c.create());
    QVERIFY(rect != 0);

    QSGItem *innerRect = rect->property("myItem").value<QSGItem*>();
    QVERIFY(innerRect != 0);

    QCOMPARE(innerRect->property("x").toInt(), int(0));

    QTRY_COMPARE(innerRect->property("x").toInt(), int(100));
}

QTEST_MAIN(tst_qdeclarativebehaviors)

#include "tst_qdeclarativebehaviors.moc"
