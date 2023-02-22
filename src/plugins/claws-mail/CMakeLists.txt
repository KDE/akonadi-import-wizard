set(clawsmailimporter_SRCS
    clawsmailaddressbook.cpp
    clawsmailimportdata.cpp
    clawsmailsettings.cpp
    ../sylpheed/sylpheedaddressbook.cpp
    ../sylpheed/sylpheedsettings.cpp
    ../sylpheed/sylpheedsettingsutils.cpp
    clawsmailimportdata.h
    clawsmailaddressbook.h
    clawsmailsettings.h

    )

ecm_qt_declare_logging_category(clawsmailimporter_SRCS HEADER sylpheedplugin_debug.h
        IDENTIFIER SYLPHEEDPLUGIN_LOG CATEGORY_NAME org.kde.pim.importwizard.sylpheedplugin
        DESCRIPTION "importwizard (sylpheed plugin)"
        EXPORT IMPORTWIZARD
    )

ecm_qt_declare_logging_category(clawsmailimporter_SRCS HEADER clawsmailplugin_debug.h
        IDENTIFIER CLAWSMAILPLUGIN_LOG CATEGORY_NAME org.kde.pim.importwizard.clawsmailplugin
        DESCRIPTION "importwizard (claws-mail plugin)"
        EXPORT IMPORTWIZARD
    )



kcoreaddons_add_plugin(clawsmailimporterplugin SOURCES ${clawsmailimporter_SRCS} INSTALL_NAMESPACE pim${QT_MAJOR_VERSION}/importwizard)
set_target_properties(clawsmailimporterplugin PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/importwizard/")
if (COMPILE_WITH_UNITY_CMAKE_SUPPORT)
    set_target_properties(clawsmailimporterplugin PROPERTIES UNITY_BUILD ON)
endif()

target_link_libraries(clawsmailimporterplugin
    KPim::ImportWizard
    KF${KF_MAJOR_VERSION}::XmlGui
    KF${KF_MAJOR_VERSION}::I18n
    KF${KF_MAJOR_VERSION}::Contacts
    KPim${KF_MAJOR_VERSION}::MailImporterAkonadi
    KPim${KF_MAJOR_VERSION}::MailTransport
    KPim${KF_MAJOR_VERSION}::IdentityManagement
    KPim${KF_MAJOR_VERSION}::MailCommon
    )
