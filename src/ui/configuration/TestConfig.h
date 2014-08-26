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

#ifndef TESTCONFIG_H
#define TESTCONFIG_H

#include "AP2ConfigWidget.h"
#include <QWidget>

namespace Ui {
class TestConfig;
}

class MotorDefinition{
public:
    MotorDefinition(int motorID, const QString& rotation);
    int motorId();
    const QString& rotation() const;

private:
    int m_motorId;
    QString m_rotation;
};

class VehicleDefintion{

private:
    int m_motorCount;
    QList<MotorDefinition> m_motorDefinitions;
};

class TestConfig : public AP2ConfigWidget
{
    Q_OBJECT

public:
    explicit TestConfig(QWidget *parent = 0);
    ~TestConfig();

private slots:
    void activeUASSet(UASInterface *uas);

private:
    Ui::TestConfig *ui;


};

#endif // TESTCONFIG_H
