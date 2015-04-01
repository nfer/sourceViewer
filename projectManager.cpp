/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtWidgets>

#include "projectManager.h"

NewProjectDialog::NewProjectDialog(QWidget *parent)
    : QDialog(parent)
{
    mOKButton = createButton(tr("OK"), SLOT(browse()));
    mCancelButton = createButton(tr("Cancel"), SLOT(reject()));
    mBrowseButton = createButton(tr("Browse..."), SLOT(browse()));

    mProjName = new QLabel(tr("New Project Name..."));
    mProjStorePath = new QLabel(tr("Where do you want to store the project data file?"));

    mProjNameInput = new QLineEdit(tr("Unitled Project"));
    mProjNameInput->setFixedWidth(360);
    mProjStorePathInput = new QLineEdit(tr("Unitled Project Path"));
    mProjStorePathInput->setFixedWidth(360);

    QVBoxLayout *leftLayout = new QVBoxLayout();
    leftLayout->setSpacing(4);
    leftLayout->addWidget(mProjName);
    leftLayout->addWidget(mProjNameInput);
    leftLayout->addWidget(mProjStorePath);
    leftLayout->addWidget(mProjStorePathInput);
    leftLayout->insertSpacing(2, 15);
    QVBoxLayout *rightLayout = new QVBoxLayout();
    rightLayout->addWidget(mOKButton);
    rightLayout->addWidget(mCancelButton);
    rightLayout->addWidget(mBrowseButton);

    QHBoxLayout *mainLayout = new QHBoxLayout();
    mainLayout->addLayout(leftLayout);
    mainLayout->addLayout(rightLayout);
    mainLayout->insertSpacing(1, 20);
    mainLayout->setContentsMargins(12, 10, 12, 40);
    setLayout(mainLayout);

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    setWindowTitle(tr("New Project"));
}

void NewProjectDialog::browse()
{
}

void NewProjectDialog::find()
{
}

QPushButton *NewProjectDialog::createButton(const QString &text, const char *member)
{
    QPushButton *button = new QPushButton(text);
    connect(button, SIGNAL(clicked()), this, member);
    return button;
}
