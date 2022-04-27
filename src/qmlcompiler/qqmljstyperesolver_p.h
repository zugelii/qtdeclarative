/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the tools applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:GPL-EXCEPT$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QQMLJSTYPERESOLVER_P_H
#define QQMLJSTYPERESOLVER_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

#include <private/qtqmlcompilerexports_p.h>

#include <private/qqmlirbuilder_p.h>
#include <private/qqmljsast_p.h>
#include "qqmljsimporter_p.h"
#include "qqmljslogger_p.h"
#include "qqmljsregistercontent_p.h"
#include "qqmljsscope_p.h"
#include "qqmljsscopesbyid_p.h"

QT_BEGIN_NAMESPACE

class QQmlJSImportVisitor;
class Q_QMLCOMPILER_PRIVATE_EXPORT QQmlJSTypeResolver
{
public:
    enum BaseOrExtension { Base, Extension };
    enum ParentMode { UseDocumentParent, UseParentProperty };

    QQmlJSTypeResolver(QQmlJSImporter *importer);

    // Note: must be called after the construction to read the QML program
    void init(QQmlJSImportVisitor *visitor, QQmlJS::AST::Node *program);

    QQmlJSScope::ConstPtr voidType() const { return m_voidType; }
    QQmlJSScope::ConstPtr emptyListType() const { return m_emptyListType; }
    QQmlJSScope::ConstPtr nullType() const { return m_nullType; }
    QQmlJSScope::ConstPtr realType() const { return m_realType; }
    QQmlJSScope::ConstPtr floatType() const { return m_floatType; }
    QQmlJSScope::ConstPtr intType() const { return m_intType; }
    QQmlJSScope::ConstPtr boolType() const { return m_boolType; }
    QQmlJSScope::ConstPtr stringType() const { return m_stringType; }
    QQmlJSScope::ConstPtr stringListType() const { return m_stringListType; }
    QQmlJSScope::ConstPtr urlType() const { return m_urlType; }
    QQmlJSScope::ConstPtr dateTimeType() const { return m_dateTimeType; }
    QQmlJSScope::ConstPtr variantListType() const { return m_variantListType; }
    QQmlJSScope::ConstPtr varType() const { return m_varType; }
    QQmlJSScope::ConstPtr jsValueType() const { return m_jsValueType; }
    QQmlJSScope::ConstPtr jsPrimitiveType() const { return m_jsPrimitiveType; }
    QQmlJSScope::ConstPtr listPropertyType() const { return m_listPropertyType; }
    QQmlJSScope::ConstPtr metaObjectType() const { return m_metaObjectType; }
    QQmlJSScope::ConstPtr jsGlobalObject() const { return m_jsGlobalObject; }

    QQmlJSScope::ConstPtr scopeForLocation(const QV4::CompiledData::Location &location) const;
    QQmlJSScope::ConstPtr scopeForId(
            const QString &id, const QQmlJSScope::ConstPtr &referrer) const;

    bool isPrefix(const QString &name) const
    {
        return m_imports.contains(name) && !m_imports[name].scope;
    }

    enum ListMode { UseListReference, UseQObjectList };
    QQmlJSScope::ConstPtr listType(const QQmlJSScope::ConstPtr &elementType, ListMode mode) const;
    QQmlJSScope::ConstPtr typeForName(const QString &name) const { return m_imports[name].scope; }
    QQmlJSScope::ConstPtr typeFromAST(QQmlJS::AST::Type *type) const;
    QQmlJSScope::ConstPtr typeForConst(QV4::ReturnedValue rv) const;
    QQmlJSRegisterContent typeForBinaryOperation(QSOperator::Op oper,
                                                 const QQmlJSRegisterContent &left,
                                                 const QQmlJSRegisterContent &right) const;

    enum class UnaryOperator { Plus, Minus, Increment, Decrement };

    QQmlJSRegisterContent typeForArithmeticUnaryOperation(
            UnaryOperator oper, const QQmlJSRegisterContent &operand) const;

    bool isPrimitive(const QQmlJSRegisterContent &type) const;
    bool isNumeric(const QQmlJSRegisterContent &type) const;

    bool canConvertFromTo(const QQmlJSScope::ConstPtr &from, const QQmlJSScope::ConstPtr &to) const;
    bool canConvertFromTo(const QQmlJSRegisterContent &from, const QQmlJSRegisterContent &to) const;
    QQmlJSRegisterContent merge(const QQmlJSRegisterContent &a,
                                const QQmlJSRegisterContent &b) const;

    enum class ComponentIsGeneric { No, Yes };
    QQmlJSScope::ConstPtr
    genericType(const QQmlJSScope::ConstPtr &type,
                ComponentIsGeneric allowComponent = ComponentIsGeneric::No) const;

    QQmlJSRegisterContent globalType(const QQmlJSScope::ConstPtr &type) const;
    QQmlJSRegisterContent scopedType(const QQmlJSScope::ConstPtr &scope, const QString &name) const;
    QQmlJSRegisterContent memberType(const QQmlJSRegisterContent &type, const QString &name) const;
    QQmlJSRegisterContent valueType(const QQmlJSRegisterContent &listType) const;
    QQmlJSRegisterContent returnType(const QQmlJSScope::ConstPtr &type,
                                     QQmlJSRegisterContent::ContentVariant variant) const;

    bool registerIsStoredIn(const QQmlJSRegisterContent &reg,
                            const QQmlJSScope::ConstPtr &type) const;
    bool registerContains(const QQmlJSRegisterContent &reg,
                          const QQmlJSScope::ConstPtr &type) const;
    QQmlJSScope::ConstPtr containedType(const QQmlJSRegisterContent &container) const;
    QString containedTypeName(const QQmlJSRegisterContent &container) const;

    QQmlJSRegisterContent tracked(const QQmlJSRegisterContent &type) const;
    QQmlJSRegisterContent original(const QQmlJSRegisterContent &type) const;

    QQmlJSScope::ConstPtr trackedContainedType(const QQmlJSRegisterContent &container) const;
    QQmlJSScope::ConstPtr originalContainedType(const QQmlJSRegisterContent &container) const;

    void adjustTrackedType(const QQmlJSScope::ConstPtr &tracked,
                           const QQmlJSScope::ConstPtr &conversion) const;
    void adjustTrackedType(const QQmlJSScope::ConstPtr &tracked,
                           const QList<QQmlJSScope::ConstPtr> &conversions) const;
    void generalizeType(const QQmlJSScope::ConstPtr &type) const;

    void setParentMode(ParentMode mode) { m_parentMode = mode; }
    ParentMode parentMode() const { return m_parentMode; }

    QQmlJSScope::ConstPtr storedType(const QQmlJSScope::ConstPtr &type) const;
    QQmlJSScope::ConstPtr originalType(const QQmlJSScope::ConstPtr &type) const;
    QQmlJSScope::ConstPtr trackedType(const QQmlJSScope::ConstPtr &type) const;
    QQmlJSScope::ConstPtr comparableType(const QQmlJSScope::ConstPtr &type) const;

    const QQmlJSScopesById &objectsById() const { return m_objectsById; }
    const QHash<QQmlJS::SourceLocation, QQmlJSMetaSignalHandler> &signalHandlers() const
    {
        return m_signalHandlers;
    }

    bool equals(const QQmlJSScope::ConstPtr &a, const QQmlJSScope::ConstPtr &b) const;

    QQmlJSRegisterContent convert(
            const QQmlJSRegisterContent &from, const QQmlJSRegisterContent &to) const;

    QQmlJSScope::ConstPtr merge(const QQmlJSScope::ConstPtr &a,
                                const QQmlJSScope::ConstPtr &b) const;

    bool canHoldUndefined(const QQmlJSRegisterContent &content) const;
    bool isNumeric(const QQmlJSScope::ConstPtr &type) const;

protected:

    QQmlJSRegisterContent memberType(const QQmlJSScope::ConstPtr &type, const QString &name) const;
    QQmlJSRegisterContent memberEnumType(const QQmlJSScope::ConstPtr &type,
                                         const QString &name) const;
    bool isPrimitive(const QQmlJSScope::ConstPtr &type) const;
    bool checkEnums(const QQmlJSScope::ConstPtr &scope, const QString &name,
                    QQmlJSRegisterContent *result, BaseOrExtension mode) const;
    bool canPrimitivelyConvertFromTo(
            const QQmlJSScope::ConstPtr &from, const QQmlJSScope::ConstPtr &to) const;
    QQmlJSRegisterContent lengthProperty(bool isWritable, const QQmlJSScope::ConstPtr &scope) const;
    void trackListPropertyType(const QQmlJSScope::ConstPtr &trackedListElementType) const;
    QQmlJSRegisterContent transformed(
            const QQmlJSRegisterContent &origin,
            QQmlJSScope::ConstPtr (QQmlJSTypeResolver::*op)(const QQmlJSScope::ConstPtr &) const) const;

    QQmlJSScope::ConstPtr m_voidType;
    QQmlJSScope::ConstPtr m_emptyListType;
    QQmlJSScope::ConstPtr m_nullType;
    QQmlJSScope::ConstPtr m_numberPrototype;
    QQmlJSScope::ConstPtr m_realType;
    QQmlJSScope::ConstPtr m_floatType;
    QQmlJSScope::ConstPtr m_intType;
    QQmlJSScope::ConstPtr m_boolType;
    QQmlJSScope::ConstPtr m_stringType;
    QQmlJSScope::ConstPtr m_stringListType;
    QQmlJSScope::ConstPtr m_urlType;
    QQmlJSScope::ConstPtr m_dateTimeType;
    QQmlJSScope::ConstPtr m_variantListType;
    QQmlJSScope::ConstPtr m_varType;
    QQmlJSScope::ConstPtr m_jsValueType;
    QQmlJSScope::ConstPtr m_jsPrimitiveType;
    QQmlJSScope::ConstPtr m_listPropertyType;
    QQmlJSScope::ConstPtr m_metaObjectType;
    QQmlJSScope::ConstPtr m_jsGlobalObject;

    QQmlJSScopesById m_objectsById;
    QHash<QV4::CompiledData::Location, QQmlJSScope::ConstPtr> m_objectsByLocation;
    QQmlJSImporter::ImportedTypes m_imports;
    QHash<QQmlJS::SourceLocation, QQmlJSMetaSignalHandler> m_signalHandlers;

    ParentMode m_parentMode = UseParentProperty;
    QQmlJSLogger *m_logger = nullptr;

    struct TrackedType
    {
        // The type originally found via type analysis.
        QQmlJSScope::ConstPtr original;

        // Any later replacement used to overwrite the contents of the clone.
        QQmlJSScope::ConstPtr replacement;

        // A clone of original, used to track the type,
        // contents possibly overwritten by replacement.
        QQmlJSScope::Ptr clone;
    };

    struct TypeTracker
    {
        QHash<QQmlJSScope::ConstPtr, QQmlJSScope::Ptr> listTypes;
        QHash<QQmlJSScope::ConstPtr, TrackedType> trackedTypes;
    };

    std::unique_ptr<TypeTracker> m_typeTracker;
};

QT_END_NAMESPACE

#endif // QQMLJSTYPERESOLVER_P_H
