/*===================================================================
APM_PLANNER Open Source Ground Control Station

(c) 2014 APM_PLANNER PROJECT <http://www.diydrones.com>
(c) Bill Bonney <billbonney@communistech.com>

This file is part of the APM_PLANNER project

    APM_PLANNER is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    APM_PLANNER is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with APM_PLANNER. If not, see <http://www.gnu.org/licenses/>.

======================================================================*/

#include "TestConfig.h"
#include "ui_TestConfig.h"

#include "QGCCore.h"

TestConfig::TestConfig(QWidget *parent) :
    AP2ConfigWidget(parent),
    ui(new Ui::TestConfig)
{
    ui->setupUi(this);
}

TestConfig::~TestConfig()
{
    delete ui;
}

void TestConfig::activeUASSet(UASInterface *uas)
{
    if(m_uas){
        disconnect(m_uas,SIGNAL(parameterChanged(int,int,QString,QVariant)),
                   this,SLOT(parameterChanged(int,int,QString,QVariant)));
    }
    m_uas = uas;
    if(m_uas){
        connect(m_uas,SIGNAL(parameterChanged(int,int,QString,QVariant)),
                   this,SLOT(parameterChanged(int,int,QString,QVariant)));

    }
}
