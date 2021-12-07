/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt Quick Dialogs module of the Qt Toolkit.
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
**
****************************************************************************/

#include "qquickfolderdialog_p.h"

#include <QtCore/qloggingcategory.h>
#include <QtQml/qqmlfile.h>

QT_BEGIN_NAMESPACE

Q_DECLARE_LOGGING_CATEGORY(lcDialogs)

/*!
    \qmltype FolderDialog
    \inherits Dialog
//!     \instantiates QQuickFolderDialog
    \inqmlmodule QtQuick.Dialogs
    \since 6.3
    \brief A native folder dialog.

    The FolderDialog type provides a QML API for native platform folder dialogs.

    \image qtquickdialogs-folderdialog-gtk.png

    To show a folder dialog, construct an instance of FolderDialog, set the
    desired properties, and call \l {Dialog::}{open()}. The \l currentFolder
    property can be used to determine the currently selected folder in the
    dialog. The \l selectedFolder property is updated only after the final
    selection has been made by accepting the dialog.

    \code
    MenuItem {
        text: "Open..."
        onTriggered: folderDialog.open()
    }

    FolderDialog {
        id: folderDialog
        currentFolder: viewer.folder
        folder: StandardPaths.standardLocations(StandardPaths.PicturesLocation)[0]
    }

    MyViewer {
        id: viewer
        folder: folderDialog.folder
    }
    \endcode

    \section2 Availability

    A native platform folder dialog is currently available on the following platforms:

    \list
    \li iOS
    \li Linux (when running with the GTK+ platform theme)
    \li macOS
    \li Windows
    \endlist

    \include includes/fallback.qdocinc

    \sa FileDialog, {QtCore::}{StandardPaths}
*/

QQuickFolderDialog::QQuickFolderDialog(QObject *parent)
    : QQuickAbstractDialog(QQuickDialogType::FolderDialog, parent),
      m_options(QFileDialogOptions::create())
{
    m_options->setFileMode(QFileDialogOptions::Directory);
    m_options->setAcceptMode(QFileDialogOptions::AcceptOpen);
    m_options->setInitialDirectory(QUrl::fromLocalFile(QDir::currentPath()));
}

/*!
    \qmlproperty url QtQuick.Dialogs::FolderDialog::currentFolder

    This property holds the folder that is currently being displayed in the dialog.

    \sa selectedFolder
*/
QUrl QQuickFolderDialog::currentFolder() const
{
    if (QPlatformFileDialogHelper *fileDialog = qobject_cast<QPlatformFileDialogHelper *>(handle()))
        return fileDialog->directory();
    return m_options->initialDirectory();
}

void QQuickFolderDialog::setCurrentFolder(const QUrl &folder)
{
    if (folder == m_options->initialDirectory())
        return;

    m_options->setInitialDirectory(folder);
    emit currentFolderChanged();
}

/*!
    \qmlproperty url QtQuick.Dialogs::FolderDialog::selectedFolder

    This property holds the last folder that was selected in the dialog.

    The value of this property is updated each time the user selects a folder
    in the dialog, and when the dialog is accepted. Alternatively, the
    \l {Dialog::}{accepted()} signal can be handled to get the final selection.

    \sa currentFolder, {Dialog::}{accepted()}
*/
QUrl QQuickFolderDialog::selectedFolder() const
{
    if (QPlatformFileDialogHelper *fileDialog = qobject_cast<QPlatformFileDialogHelper *>(handle())) {
        const QList<QUrl> selectedFiles = fileDialog->selectedFiles();
        if (!selectedFiles.isEmpty())
            return selectedFiles.first();
    }
    return QUrl();
}

void QQuickFolderDialog::setSelectedFolder(const QUrl &folder)
{
    if (folder == selectedFolder())
        return;

    if (QPlatformFileDialogHelper *fileDialog = qobject_cast<QPlatformFileDialogHelper *>(handle())) {
        fileDialog->selectFile(folder);
        emit selectedFolderChanged();
    }
}

/*!
    \qmlproperty flags QtQuick.Dialogs::FolderDialog::options

    This property holds the various options that affect the look and feel of the dialog.

    By default, all options are disabled.

    Options should be set before showing the dialog. Setting them while the dialog is
    visible is not guaranteed to have an immediate effect on the dialog (depending on
    the option and on the platform).

    Available options:
    \value FolderDialog.ShowDirsOnly Only show directories in the folder dialog. By default both folders and directories are shown.
    \value FolderDialog.DontResolveSymlinks Don't resolve symlinks in the folder dialog. By default symlinks are resolved.
    \value FolderDialog.ReadOnly Indicates that the dialog doesn't allow creating directories.
*/
QFileDialogOptions::FileDialogOptions QQuickFolderDialog::options() const
{
    return m_options->options();
}

void QQuickFolderDialog::setOptions(QFileDialogOptions::FileDialogOptions options)
{
    if (options == m_options->options())
        return;

    m_options->setOptions(options);
    emit optionsChanged();
}

void QQuickFolderDialog::resetOptions()
{
    setOptions({});
}

/*!
    \qmlproperty string QtQuick.Dialogs::FolderDialog::acceptLabel

    This property holds the label text shown on the button that accepts the dialog.

    When set to an empty string, the default label of the underlying platform is used.
    The default label is typically \uicontrol Open.

    The default value is an empty string.

    \sa rejectLabel
*/
QString QQuickFolderDialog::acceptLabel() const
{
    return m_options->labelText(QFileDialogOptions::Accept);
}

void QQuickFolderDialog::setAcceptLabel(const QString &label)
{
    if (label == m_options->labelText(QFileDialogOptions::Accept))
        return;

    m_options->setLabelText(QFileDialogOptions::Accept, label);
    emit acceptLabelChanged();
}

void QQuickFolderDialog::resetAcceptLabel()
{
    setAcceptLabel(QString());
}

/*!
    \qmlproperty string QtQuick.Dialogs::FolderDialog::rejectLabel

    This property holds the label text shown on the button that rejects the dialog.

    When set to an empty string, the default label of the underlying platform is used.
    The default label is typically \uicontrol Cancel.

    The default value is an empty string.

    \sa acceptLabel
*/
QString QQuickFolderDialog::rejectLabel() const
{
    return m_options->labelText(QFileDialogOptions::Reject);
}

void QQuickFolderDialog::setRejectLabel(const QString &label)
{
    if (label == m_options->labelText(QFileDialogOptions::Reject))
        return;

    m_options->setLabelText(QFileDialogOptions::Reject, label);
    emit rejectLabelChanged();
}

void QQuickFolderDialog::resetRejectLabel()
{
    setRejectLabel(QString());
}

bool QQuickFolderDialog::useNativeDialog() const
{
    if (!QQuickAbstractDialog::useNativeDialog())
        return false;

    if (m_options->testOption(QFileDialogOptions::DontUseNativeDialog)) {
        qCDebug(lcDialogs) << "  - the FolderDialog was told not to use a native dialog; not using native dialog";
        return false;
    }

    return true;
}

void QQuickFolderDialog::onCreate(QPlatformDialogHelper *dialog)
{
    if (QPlatformFileDialogHelper *fileDialog = qobject_cast<QPlatformFileDialogHelper *>(dialog)) {
        connect(fileDialog, &QPlatformFileDialogHelper::directoryEntered, this, &QQuickFolderDialog::currentFolderChanged);
        connect(fileDialog, &QPlatformFileDialogHelper::currentChanged, this, &QQuickFolderDialog::selectedFolderChanged);
        fileDialog->setOptions(m_options);
    }
}

void QQuickFolderDialog::onShow(QPlatformDialogHelper *dialog)
{
    m_options->setWindowTitle(title());
    if (QPlatformFileDialogHelper *fileDialog = qobject_cast<QPlatformFileDialogHelper *>(dialog)) {
        fileDialog->setOptions(m_options);

        const QUrl initialDir = m_options->initialDirectory();
        // If it's not valid, we shouldn't set it.
        if (m_firstShow && initialDir.isValid() && QDir(QQmlFile::urlToLocalFileOrQrc(initialDir)).exists())
            fileDialog->setDirectory(m_options->initialDirectory());
    }
    QQuickAbstractDialog::onShow(dialog);
}

QT_END_NAMESPACE