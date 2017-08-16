set(trojitaimporter_SRCS
    trojitaaddressbook.cpp
    trojitaimportdata.cpp
    trojitasettings.cpp
    )

ecm_qt_declare_logging_category(trojitaimporter_SRCS HEADER trojitaplugin_debug.h
    IDENTIFIER TROJITAPLUGIN_LOG CATEGORY_NAME org.kde.pim.importwizard.trojitaplugin)
kcoreaddons_add_plugin(trojitaimporterplugin JSON trojitaimporter.json SOURCES
    ${trojitaimporter_SRCS} INSTALL_NAMESPACE importwizard)
set_target_properties(trojitaimporterplugin PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/importwizard/")


target_link_libraries(trojitaimporterplugin
    KPim::ImportWizard
    KF5::XmlGui
    KF5::I18n
    KF5::Contacts
    KF5::MailImporterAkonadi
    KF5::MailTransport
    KF5::MailCommon
    KF5::IdentityManagement
    )
