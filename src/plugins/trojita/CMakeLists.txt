set(trojitaimporter_SRCS
    trojitaaddressbook.cpp
    trojitaimportdata.cpp
    trojitasettings.cpp
    trojitasettings.h
    trojitaimportdata.h
    trojitaaddressbook.h
    )

ecm_qt_declare_logging_category(trojitaimporter_SRCS HEADER trojitaplugin_debug.h
        IDENTIFIER TROJITAPLUGIN_LOG CATEGORY_NAME org.kde.pim.importwizard.trojitaplugin
        DESCRIPTION "importwizard (trojita plugin)"
        EXPORT IMPORTWIZARD
    )

kcoreaddons_add_plugin(trojitaimporterplugin SOURCES ${trojitaimporter_SRCS} INSTALL_NAMESPACE pim${QT_MAJOR_VERSION}/importwizard)
set_target_properties(trojitaimporterplugin PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/importwizard/")
if (COMPILE_WITH_UNITY_CMAKE_SUPPORT)
    set_target_properties(trojitaimporterplugin PROPERTIES UNITY_BUILD ON)
endif()


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
