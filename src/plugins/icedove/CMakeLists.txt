set(icedoveimporter_SRCS
    icedoveimportdata.cpp
    ../thunderbird/thunderbirdaddressbook.cpp
    ../thunderbird/thunderbirdsettings.cpp
    ../thunderbird/addressbook/MorkParser.cpp

    )

ecm_qt_declare_logging_category(icedoveimporter_SRCS HEADER thunderbirdplugin_debug.h
    IDENTIFIER THUNDERBIRDPLUGIN_LOG CATEGORY_NAME org.kde.pim.importwizard.thunderbirdplugin)
kcoreaddons_add_plugin(icedoveimporterplugin JSON icedoveimporter.json SOURCES
    ${icedoveimporter_SRCS} INSTALL_NAMESPACE importwizard)


target_link_libraries(icedoveimporterplugin
    KPim::ImportWizard
    KF5::XmlGui
    KF5::I18n
    KF5::Contacts
    KF5::MailImporterAkonadi
    KF5::MailTransport
    KF5::IdentityManagement
    KF5::MailCommon
    )
