/* *****************************************************************************
Copyright (c) 2016-2017, The Regents of the University of California (Regents).
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of the FreeBSD Project.

REGENTS SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
THE SOFTWARE AND ACCOMPANYING DOCUMENTATION, IF ANY, PROVIDED HEREUNDER IS 
PROVIDED "AS IS". REGENTS HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, 
UPDATES, ENHANCEMENTS, OR MODIFICATIONS.

*************************************************************************** */

// Written: fmckenna

#include "InputWidgetEDP.h"
#include <QPushButton>
#include <QScrollArea>
#include <QJsonArray>
#include <QJsonObject>
#include <QLabel>
#include <QFrame>
#include <sectiontitle.h>
#include <QSpacerItem>

#include <QDebug>


InputWidgetEDP::InputWidgetEDP(QWidget *parent) : SimCenterWidget(parent)
{
    verticalLayout = new QVBoxLayout();
    this->setLayout(verticalLayout);
    this->makeEDP();
}

InputWidgetEDP::~InputWidgetEDP()
{

}

void
InputWidgetEDP::makeEDP(void)
{
    // title & add button
    QHBoxLayout *titleLayout = new QHBoxLayout();

    SectionTitle *title=new SectionTitle();
    title->setText(tr("Quantities of Interest"));
    title->setMinimumWidth(250);
    QSpacerItem *spacer1 = new QSpacerItem(50,10);
    QSpacerItem *spacer2 = new QSpacerItem(20,10);

    QPushButton *addEDP = new QPushButton();
    addEDP->setMinimumWidth(75);
    addEDP->setMaximumWidth(75);
    addEDP->setText(tr("Add"));
    connect(addEDP,SIGNAL(clicked()),this,SLOT(addEDP()));

    QPushButton *removeEDP = new QPushButton();
    removeEDP->setMinimumWidth(75);
    removeEDP->setMaximumWidth(75);
    removeEDP->setText(tr("Remove"));
    connect(removeEDP,SIGNAL(clicked()),this,SLOT(removeEDP()));

    titleLayout->addWidget(title);
    titleLayout->addItem(spacer1);
    titleLayout->addWidget(addEDP);
    titleLayout->addItem(spacer2);
    titleLayout->addWidget(removeEDP);
    titleLayout->addStretch();

    verticalLayout->addLayout(titleLayout);

    QScrollArea *sa = new QScrollArea;
    sa->setWidgetResizable(true);
    sa->setLineWidth(0);
    sa->setFrameShape(QFrame::NoFrame);

    edp = new QFrame();
    edp->setFrameShape(QFrame::NoFrame);
    edp->setLineWidth(0);

    edpLayout = new QVBoxLayout;

    //setLayout(layout);
    edp->setLayout(edpLayout);
    edpLayout->addStretch();
    this->addEDP();

    sa->setWidget(edp);
    verticalLayout->addWidget(sa);

    verticalLayout->setSpacing(0);
    verticalLayout->setMargin(0);
}

void InputWidgetEDP::addEDP(void)
{
    EDP *theEDP = new EDP();
    theEDPs.append(theEDP);
    edpLayout->insertWidget(edpLayout->count()-1, theEDP);
}


int InputWidgetEDP::getNumEDP(void)
{
    return theEDPs.length();
}

void InputWidgetEDP::clear(void)
{
    // loop over random variables, removing from layout & deleting
    for (int i = 0; i <theEDPs.size(); ++i) {
        EDP *theEDP = theEDPs.at(i);
        edpLayout->removeWidget(theEDP);
        delete theEDP;
    }
    theEDPs.clear();
}


void InputWidgetEDP::removeEDP(void)
{
    // find the ones selected & remove them
    int numEDPs = theEDPs.size();
    for (int i = numEDPs-1; i >= 0; i--) {
        EDP *theEDP = theEDPs.at(i);
        if (theEDP->isSelectedForRemoval()) {
            theEDP->close();
            edpLayout->removeWidget(theEDP);
            theEDPs.remove(i);
            //theEDP->setParent(0);
            delete theEDP;
        }
    }
}


bool
InputWidgetEDP::outputToJSON(QJsonObject &jsonObject)
{
    bool result = true;

    // create an JSON array and add each one that succesfully writes itself
    QJsonArray edpArray;
    for (int i = 0; i <theEDPs.size(); ++i) {
        QJsonObject edp;
        if (theEDPs.at(i)->outputToJSON(edp) == true)
            edpArray.append(edp);
        else
            result = false;
    }
    jsonObject["EDP"]=edpArray;
    return result;
}


bool
InputWidgetEDP::inputFromJSON(QJsonObject &rvObject)
{
    bool result = true;

    // clear existing
    this->clear();

    // go get the array, and for each component create one, get it to read & then add
    QJsonArray rvArray = rvObject["EDP"].toArray();
    foreach (const QJsonValue &rvValue, rvArray) {
        QJsonObject rvObject = rvValue.toObject();
        EDP *theEDP = new EDP();
        if (theEDP->inputFromJSON(rvObject) == true) {
            theEDPs.append(theEDP);
            edpLayout->insertWidget(edpLayout->count()-1, theEDP);
        } else
            result = false;
    }

    return result;
}

int
InputWidgetEDP::processResults(double *data) {
    for (int i = 0; i <theEDPs.size(); ++i) {
        theEDPs.at(i)->setResults(&data[i*4]);
    }
    return 0;
}
