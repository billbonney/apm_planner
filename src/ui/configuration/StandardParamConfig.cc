/*===================================================================
APM_PLANNER Open Source Ground Control Station

(c) 2013 APM_PLANNER PROJECT <http://www.diydrones.com>

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
#include "QsLog.h"
#include "StandardParamConfig.h"
#include "ParamWidget.h"
#include "QGCMouseWheelEventFilter.h"

StandardParamConfig::StandardParamConfig(QWidget *parent) : AP2ConfigWidget(parent),
    m_findMode(false)
{
    ui.setupUi(this);
    initConnections();
    connect(ui.searchLineEdit, SIGNAL(textEdited(QString)), this, SLOT(findParameterWidget(QString)));
    connect(ui.clearButton, SIGNAL(clicked()), this, SLOT(clearSearch()));
}

StandardParamConfig::~StandardParamConfig()
{
}

void StandardParamConfig::addRange(QString title,QString description,QString param,double min,double max,double increment)
{
    ParamWidget *widget = new ParamWidget(param,ui.scrollAreaWidgetContents);
    connect(widget,SIGNAL(doubleValueChanged(QString,double)),this,SLOT(doubleValueChanged(QString,double)));
    connect(widget,SIGNAL(intValueChanged(QString,int)),this,SLOT(intValueChanged(QString,int)));
    m_paramToWidgetMap[param] = widget;
    widget->setupDouble(title + "(" + param + ")",description,0,min,max,increment);
    ui.verticalLayout->addWidget(widget);
    widget->installEventFilter(QGCMouseWheelEventFilter::getFilter());
    widget->show();
}

void StandardParamConfig::addCombo(QString title,QString description,QString param,QList<QPair<int,QString> > valuelist)
{
    ParamWidget *widget = new ParamWidget(param,ui.scrollAreaWidgetContents);
    connect(widget,SIGNAL(doubleValueChanged(QString,double)),this,SLOT(doubleValueChanged(QString,double)));
    connect(widget,SIGNAL(intValueChanged(QString,int)),this,SLOT(intValueChanged(QString,int)));
    m_paramToWidgetMap[param] = widget;
    widget->setupCombo(title + "(" + param + ")",description,valuelist);
    ui.verticalLayout->addWidget(widget);
    widget->installEventFilter(QGCMouseWheelEventFilter::getFilter());
    widget->show();
}
void StandardParamConfig::parameterChanged(int uas, int component, QString parameterName, QVariant value)
{
    Q_UNUSED(uas);
    Q_UNUSED(component);

    if (m_paramToWidgetMap.contains(parameterName))
    {
        if (value.type() == QVariant::Double || value.type() == QMetaType::Float)
        {
            m_paramToWidgetMap[parameterName]->setValue(value.toDouble());
        }
        else
        {
            m_paramToWidgetMap[parameterName]->setValue(value.toInt());
        }
    }
}
void StandardParamConfig::doubleValueChanged(QString param,double value)
{
    if (!m_uas)
    {
        this->showNullMAVErrorMessageBox();
    }
    m_uas->getParamManager()->setParameter(1,param,value);
}

void StandardParamConfig::intValueChanged(QString param,int value)
{
    if (!m_uas)
    {
        this->showNullMAVErrorMessageBox();
    }
    m_uas->getParamManager()->setParameter(1,param,value);
}

void StandardParamConfig::hideParameterWidgets(bool hide)
{
    QLOG_DEBUG() << "StandardParamConfig::hideParameterWidgets : " << m_paramToWidgetMap.count();
    int count = 0;
    foreach(const QString paramKey, m_paramToWidgetMap.keys()){

        ParamWidget *paramWidget = m_paramToWidgetMap.value(paramKey);
        QLOG_DEBUG() << "Hide Widget " << paramKey << tr("(%1)").arg(count) << m_paramToWidgetMap.value(paramKey);
        if (hide)
            ui.verticalLayout->removeWidget(paramWidget);
        count++;
    }

}

void StandardParamConfig::findParameterWidget(const QString &searchString)
{
    QLOG_DEBUG() << "StandardParamConfig::findParameterWidget" << searchString;
    if (searchString.length() < 3)
        return; // Require 3 or more characters to work
    if(!m_findMode){
        m_findMode = true;
        hideParameterWidgets(true);

    }

//    QRegExp expression(searchString);
//    QMap<QString, ParamWidget* >::const_iterator i = m_paramToWidgetMap.find(searchString);
//    while (i != m_paramToWidgetMap.end() && i.key().contains(searchString)){
//        QLOG_DEBUG() << "Key" << i.key() << ": " << i.value();
//        ++i;
//    }

}

void StandardParamConfig::clearSearch()
{
    QLOG_DEBUG() << "StandardParamConfig::clearSearch";
    hideParameterWidgets(false);
    m_findMode = false;
}

