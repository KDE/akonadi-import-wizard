set(evolutionv3importer_SRCS
    evolutionaddressbook.cpp
    evolutionv3importdata.cpp
    evolutionsettings.cpp
    evolutioncalendar.cpp
    evolutionutil.cpp
    evolutionsettings.h
    evolutionv3importdata.h
    evolutionutil.h
    evolutioncalendar.h
    evolutionaddressbook.h
    )
ecm_qt_declare_logging_category(evolutionv3importer_SRCS HEADER evolutionv3plugin_debug.h
        IDENTIFIER EVOLUTIONPLUGIN_LOG CATEGORY_NAME org.kde.pim.importwizard.evolutionplugin
        DESCRIPTION "importwizard (thunderbird plugin)"
        EXPORT IMPORTWIZARD
    )


kcoreaddons_add_plugin(evolutionv3importerplugin SOURCES ${evolutionv3importer_SRCS} INSTALL_NAMESPACE pim${QT_MAJOR_VERSION}/importwizard)
if (COMPILE_WITH_UNITY_CMAKE_SUPPORT)
    set_target_properties(evolutionv3importerplugin PROPERTIES UNITY_BUILD ON)
endif()

set_target_properties(evolutionv3importerplugin PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/importwizard/")

target_link_libraries(evolutionv3importerplugin
    KPim::ImportWizard
    KF${KF_MAJOR_VERSION}::XmlGui
    KF${KF_MAJOR_VERSION}::I18n
    KF${KF_MAJOR_VERSION}::Contacts
    KPim${KF_MAJOR_VERSION}::MailImporterAkonadi
    KF5::MailTransport
    KF5::MailCommon
    KF5::IdentityManagement
    KF5::MessageViewer
    )
