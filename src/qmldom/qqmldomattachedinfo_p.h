/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtQml module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**/
#ifndef QMLDOMATTACHEDINFO_P_H
#define QMLDOMATTACHEDINFO_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qqmldom_global.h"
#include "qqmldomitem_p.h"

#include <memory>
#include <optional>

QT_BEGIN_NAMESPACE

namespace QQmlJS {
namespace Dom {
template<typename TreePtr>
class AttachedInfoLookupResult
{
public:
    TreePtr foundTree;
    Path lookupPath; // relative path used to reach result
    std::optional<Path> rootTreePath; // path of the root TreePath
    operator bool() { return bool(foundTree); }
    template<typename T>
    AttachedInfoLookupResult<std::shared_ptr<T>> as() const
    {
        AttachedInfoLookupResult<std::shared_ptr<T>> res;
        res.foundTree = std::static_pointer_cast<T>(foundTree);
        res.lookupPath = lookupPath;
        res.rootTreePath = rootTreePath;
        return res;
    }
};

class QMLDOM_EXPORT AttachedInfo : public OwningItem  {
    Q_GADGET
public:
    enum class PathType {
        Relative,
        Canonical
    };
    Q_ENUM(PathType)
    enum class FindOption { None = 0, SetRootTreePath = 0x1, Default = SetRootTreePath };
    Q_DECLARE_FLAGS(FindOptions, FindOption)
    Q_FLAG(FindOptions)

    constexpr static DomType kindValue = DomType::AttachedInfo;
    using Ptr = std::shared_ptr<AttachedInfo>;

    DomType kind() const override { return kindValue; }
    Path canonicalPath(const DomItem &self) const override {
        return self.m_ownerPath;
    }
    bool iterateDirectSubpaths(DomItem &self, function_ref<bool(Path, DomItem &)> visitor) override;

    AttachedInfo::Ptr makeCopy(const DomItem &self) const {
        return std::static_pointer_cast<AttachedInfo>(doCopy(self));
    }

    Ptr parent() const { return m_parent.lock(); }
    Path path() const { return m_path; }
    void setPath(Path p) { m_path = p; }

    AttachedInfo(Ptr parent = nullptr, Path p = Path()) : m_path(p), m_parent(parent) {}
    AttachedInfo(const AttachedInfo &o);

    static Ptr ensure(Ptr self, Path path, PathType pType = PathType::Relative);
    static Ptr find(Ptr self, Path p, PathType pType = PathType::Relative);
    static AttachedInfoLookupResult<Ptr>
    findAttachedInfo(const DomItem &item, QStringView treeFieldName,
                     FindOptions options = AttachedInfo::FindOption::None);
    static Ptr treePtr(const DomItem &item, QStringView fieldName) {
        return findAttachedInfo(item, fieldName, FindOption::None).foundTree;
    }

    DomItem itemAtPath(const DomItem &self, Path p, PathType pType = PathType::Relative) const {
        if (Ptr resPtr = find(self.ownerAs<AttachedInfo>(), p, pType)) {
            if (pType == PathType::Canonical)
                p = p.mid(m_path.length());
            Path resPath = self.canonicalPath();
            for (Path pEl : p) {
                resPath = resPath.field(Fields::subItems).key(pEl.toString());
            }
            return self.copy(resPtr, resPath);
        }
        return DomItem();
    }

    DomItem infoAtPath(const DomItem &self, Path p, PathType pType = PathType::Relative) const {
        return itemAtPath(self, p, pType).field(Fields::infoItem);
    }

    MutableDomItem ensureItemAtPath(const MutableDomItem &self, Path p, PathType pType = PathType::Relative) {
        if (Ptr resPtr = ensure(self.ownerAs<AttachedInfo>(), p, pType)) {
            if (pType == PathType::Canonical)
                p = p.mid(m_path.length());
            Path resPath = self.canonicalPath();
            for (Path pEl : p) {
                resPath = resPath.field(Fields::subItems).key(pEl.toString());
            }
            return MutableDomItem(self.base().copy(resPtr, resPath));
        }
        return MutableDomItem();
    }

    MutableDomItem ensureInfoAtPath(const MutableDomItem &self, Path p, PathType pType = PathType::Relative) {
        return ensureItemAtPath(self, p, pType).field(Fields::infoItem);
    }

    virtual AttachedInfo::Ptr instantiate(AttachedInfo::Ptr parent, Path p = Path()) const = 0;
    virtual DomItem infoItem(const DomItem &self) = 0;
    DomItem infoItem(const DomItem &self) const {
        return const_cast<AttachedInfo *>(this)->infoItem(self);
    }
    QMap<Path, Ptr> subItems() const {
        return m_subItems;
    }
    void setSubItems(QMap<Path, Ptr> v) {
        m_subItems = v;
    }
protected:
    Path m_path;
    std::weak_ptr<AttachedInfo> m_parent;
    QMap<Path, Ptr> m_subItems;
};

template <typename Info>
class QMLDOM_EXPORT AttachedInfoT : public AttachedInfo  {
public:
    using Ptr = std::shared_ptr<AttachedInfoT>;
    using InfoType = Info;

    AttachedInfoT(Ptr parent = nullptr, Path p = Path()) : AttachedInfo(parent, p) {}
    AttachedInfoT(const AttachedInfoT &o):
        AttachedInfo(o),
        m_info(o.m_info)
    {
        auto end = o.m_subItems.end();
        auto i = o.m_subItems.begin();
        while (i != end) {
            m_subItems.insert(i.key(), Ptr(
                                  new AttachedInfoT(*std::static_pointer_cast<AttachedInfoT>(i.value()).get())));
        }
    }

   static Ptr createTree(Path p = Path()) {
        return Ptr(new AttachedInfoT(nullptr, p));
    }

    static Ptr ensure(Ptr self, Path path, PathType pType = PathType::Relative){
        return std::static_pointer_cast<AttachedInfoT>(AttachedInfo::ensure(self, path, pType));
    }

    static Ptr find(Ptr self, Path p, PathType pType = PathType::Relative){
        return std::static_pointer_cast<AttachedInfoT>(AttachedInfo::find(self, p, pType));
    }

    static AttachedInfoLookupResult<Ptr>
    findAttachedInfo(const DomItem &item, QStringView fieldName, AttachedInfo::FindOptions options)
    {
        return AttachedInfo::findAttachedInfo(item, fieldName, options)
                .template as<AttachedInfoT>();
    }
    static Ptr treePtr(const DomItem &item, QStringView fieldName) {
        return std::static_pointer_cast<AttachedInfoT>(AttachedInfo::treePtr(item, fieldName));
    }
    static bool visitTree(Ptr base, function_ref<bool(Path, Ptr)>visitor, Path basePath = Path()) {
        if (base) {
            Path pNow = basePath.path(base->path());
            if (visitor(pNow, base)) {
                auto it = base->m_subItems.cbegin();
                auto end = base->m_subItems.cend();
                while (it != end) {
                    if (!visitTree(std::static_pointer_cast<AttachedInfoT>(it.value()), visitor, pNow))
                        return false;
                    ++it;
                }
            } else {
                return false;
            }
        }
        return true;
    }

    AttachedInfo::Ptr instantiate(AttachedInfo::Ptr parent, Path p = Path()) const override {
        return Ptr(new AttachedInfoT(std::static_pointer_cast<AttachedInfoT>(parent), p));
    }
    DomItem infoItem(const DomItem &self) override {
        return self.subWrapField(Fields::infoItem, m_info).item;
    }

    Ptr makeCopy(const DomItem &self) const {
        return std::static_pointer_cast<AttachedInfoT>(doCopy(self));
    }

    Ptr parent() const { return std::static_pointer_cast<AttachedInfoT>(AttachedInfo::parent()); }

    const Info &info() const { return m_info; }
    Info &info() { return m_info; }
protected:
    std::shared_ptr<OwningItem> doCopy(const DomItem &) const override {
        return Ptr(new AttachedInfoT(*this));
    }
private:
    Info m_info;
};

class QMLDOM_EXPORT FileLocations {
public:
    using Tree = std::shared_ptr<AttachedInfoT<FileLocations>>;
    constexpr static DomType kindValue = DomType::FileLocations;
    DomType kind() const {  return kindValue; }
    bool iterateDirectSubpaths(DomItem &self, function_ref<bool(Path, DomItem &)>);
    void ensureCommentLocations(QList<QString> keys);

    static Tree createTree(Path basePath);
    static Tree ensure(Tree base, Path basePath, AttachedInfo::PathType pType);

    // returns the path looked up and the found tree when looking for the info attached to item
    static AttachedInfoLookupResult<Tree>
    findAttachedInfo(const DomItem &item,
                     AttachedInfo::FindOptions options = AttachedInfo::FindOption::Default);
    // convenience: find FileLocations::Tree attached to the given item
    static FileLocations::Tree treePtr(const DomItem &);
    // convenience: find FileLocations* attached to the given item (if there is one)
    static const FileLocations *fileLocationsPtr(const DomItem &);


    static void updateFullLocation(Tree fLoc, SourceLocation loc);
    static void addRegion(Tree fLoc, QString locName, SourceLocation loc);
    static void addRegion(Tree fLoc, QStringView locName, SourceLocation loc);

    SourceLocation fullRegion;
    QMap<QString, SourceLocation> regions;
    QMap<QString, QList<SourceLocation>> preCommentLocations;
    QMap<QString, QList<SourceLocation>> postCommentLocations;
};

} // end namespace Dom
} // end namespace QQmlJS

QT_END_NAMESPACE
#endif // QMLDOMATTACHEDINFO_P_H
