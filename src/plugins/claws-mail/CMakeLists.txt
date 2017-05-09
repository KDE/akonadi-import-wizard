set(clawsmailimporter_SRCS
    clawsmailaddressbook.cpp
    clawsmailimportdata.cpp
    clawsmailsettings.cpp
    ../sylpheed/sylpheedaddressbook.cpp
    ../sylpheed/sylpheedsettings.cpp
    ../sylpheed/sylpheedsettingsutils.cpp

    )

ecm_qt_declare_logging_category(clawsmailimporter_SRCS HEADER sylpheedplugin_debug.h
    IDENTIFIER SYLPHEEDPLUGIN_LOG CATEGORY_NAME org.kde.pim.importwizard.sylpheedplugin)
ecm_qt_declare_logging_category(clawsmailimporter_SRCS HEADER clawsmailplugin_debug.h
    IDENTIFIER CLAWSMAILPLUGIN_LOG CATEGORY_NAME org.kde.pim.importwizard.clawsmailplugin)
kcoreaddons_add_plugin(clawsmailimporterplugin JSON clawsmailimporter.json SOURCES
    ${clawsmailimporter_SRCS} INSTALL_NAMESPACE importwizard)


target_link_libraries(clawsmailimporterplugin
    KPim::ImportWizard
    KF5::XmlGui
    KF5::I18n
    KF5::Contacts
    KF5::MailImporterAkonadi
    KF5::MailTransport
    KF5::IdentityManagement
    KF5::MailCommon
    )
