set(sylpheedimporter_SRCS
    sylpheedaddressbook.cpp
    sylpheedimportdata.cpp
    sylpheedsettings.cpp
    sylpheedsettingsutils.cpp
    )
ecm_qt_declare_logging_category(sylpheedimporter_SRCS HEADER sylpheedplugin_debug.h
    IDENTIFIER SYLPHEEDPLUGIN_LOG CATEGORY_NAME org.kde.pim.importwizard.sylpheedplugin)

kcoreaddons_add_plugin(sylpheedimporterplugin JSON sylpheedimporter.json SOURCES
    ${sylpheedimporter_SRCS} INSTALL_NAMESPACE importwizard)


target_link_libraries(sylpheedimporterplugin
    KPim::ImportWizard
    KF5::XmlGui
    KF5::I18n
    KF5::Contacts
    KF5::MailImporterAkonadi
    KF5::MailTransport
    KF5::IdentityManagement
    KF5::MailCommon
    )
