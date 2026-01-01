/*
   SPDX-FileCopyrightText: 2012-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "evolutioncalendar.h"
using namespace Qt::Literals::StringLiterals;

#include "evolutionutil.h"

#include "evolutionv3plugin_debug.h"

#include <QDir>
#include <QDomDocument>
#include <QDomElement>
#include <QFile>

EvolutionCalendar::EvolutionCalendar() = default;

EvolutionCalendar::~EvolutionCalendar() = default;

void EvolutionCalendar::loadCalendar(const QString &filename)
{
    // Read gconf file
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        qCDebug(EVOLUTIONPLUGIN_LOG) << " We can't open file" << filename;
        return;
    }
    QDomDocument doc;
    if (!EvolutionUtil::loadInDomDocument(&file, doc)) {
        return;
    }
    QDomElement config = doc.documentElement();

    if (config.isNull()) {
        qCDebug(EVOLUTIONPLUGIN_LOG) << "No config found";
        return;
    }
    mCalendarPath = QDir::homePath() + QLatin1StringView("/.local/share/evolution/calendar/");
    for (QDomElement e = config.firstChildElement(); !e.isNull(); e = e.nextSiblingElement()) {
        const QString tag = e.tagName();
        if (tag == QLatin1StringView("entry")) {
            if (e.hasAttribute(u"name"_s)) {
                const QString attr = e.attribute(u"name"_s);
                if (attr == QLatin1StringView("sources")) {
                    readCalendar(e);
                } else {
                    qCDebug(EVOLUTIONPLUGIN_LOG) << " attr unknown " << attr;
                }
            }
        }
    }
}

void EvolutionCalendar::readCalendar(const QDomElement &calendar)
{
    for (QDomElement calendarConfig = calendar.firstChildElement(); !calendarConfig.isNull(); calendarConfig = calendarConfig.nextSiblingElement()) {
        if (calendarConfig.tagName() == QLatin1StringView("li")) {
            const QDomElement stringValue = calendarConfig.firstChildElement();
            extractCalendarInfo(stringValue.text());
        }
    }
}

void EvolutionCalendar::extractCalendarInfo(const QString &info)
{
    // qCDebug(IMPORTWIZARD_LOG)<<" info "<<info;
    // Read QDomElement
    QDomDocument cal;
    if (!EvolutionUtil::loadInDomDocument(info, cal)) {
        return;
    }
    QDomElement domElement = cal.documentElement();

    if (domElement.isNull()) {
        qCDebug(EVOLUTIONPLUGIN_LOG) << "Account not found";
        return;
    }
    QString base_uri;
    if (domElement.hasAttribute(u"base_uri"_s)) {
        base_uri = domElement.attribute(u"base_uri"_s);
    }
    if (base_uri == QLatin1StringView("local:")) {
        for (QDomElement e = domElement.firstChildElement(); !e.isNull(); e = e.nextSiblingElement()) {
            const QString tag = e.tagName();
            if (tag == QLatin1StringView("source")) {
                QString name;
                QMap<QString, QVariant> settings;
                if (e.hasAttribute(u"uid"_s)) { }
                if (e.hasAttribute(u"name"_s)) {
                    name = e.attribute(u"name"_s);
                    settings.insert(u"DisplayName"_s, name);
                }
                if (e.hasAttribute(u"relative_uri"_s)) {
                    const QString path = mCalendarPath + e.attribute(u"relative_uri"_s) + QLatin1StringView("/calendar.ics");
                    settings.insert(u"Path"_s, path);
                }
                if (e.hasAttribute(u"color_spec"_s)) {
                    // const QString color = e.attribute(u"color_spec"_s);
                    // Need id.
                    // TODO: Need to get id for collection to add color.
                }
                QDomElement propertiesElement = e.firstChildElement();
                if (!propertiesElement.isNull()) {
                    for (QDomElement property = propertiesElement.firstChildElement(); !property.isNull(); property = property.nextSiblingElement()) {
                        const QString propertyTag = property.tagName();
                        if (propertyTag == QLatin1StringView("property")) {
                            if (property.hasAttribute(u"name"_s)) {
                                const QString propertyName = property.attribute(u"name"_s);
                                if (propertyName == QLatin1StringView("custom-file-readonly")) {
                                    if (property.hasAttribute(u"value"_s)) {
                                        if (property.attribute(u"value"_s) == u'1') {
                                            settings.insert(u"ReadOnly"_s, true);
                                        }
                                    }
                                } else if (propertyName == QLatin1StringView("alarm")) {
                                    qCDebug(EVOLUTIONPLUGIN_LOG) << " need to implement alarm property";
                                } else {
                                    qCDebug(EVOLUTIONPLUGIN_LOG) << " property unknown :" << propertyName;
                                }
                            }
                        } else {
                            qCDebug(EVOLUTIONPLUGIN_LOG) << " tag unknown :" << propertyTag;
                        }
                    }
                }
                LibImportWizard::AbstractBase::createResource(u"akonadi_ical_resource"_s, name, settings);
            } else {
                qCDebug(EVOLUTIONPLUGIN_LOG) << " tag unknown :" << tag;
            }
        }
    } else if (base_uri == QLatin1StringView("webcal://")) {
        qCDebug(EVOLUTIONPLUGIN_LOG) << " need to implement webcal protocol";
    } else if (base_uri == QLatin1StringView("google://")) {
        qCDebug(EVOLUTIONPLUGIN_LOG) << " need to implement google protocol";
    } else if (base_uri == QLatin1StringView("caldav://")) {
        qCDebug(EVOLUTIONPLUGIN_LOG) << " need to implement caldav protocol";
    } else {
        qCDebug(EVOLUTIONPLUGIN_LOG) << " base_uri unknown" << base_uri;
    }
}
