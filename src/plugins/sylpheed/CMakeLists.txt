set(sylpheedimporter_SRCS
    sylpheedaddressbook.cpp
    sylpheedimportdata.cpp
    sylpheedsettings.cpp
    sylpheedsettingsutils.cpp
    sylpheedimportdata.h
    sylpheedsettings.h
    sylpheedaddressbook.h
    sylpheedsettingsutils.h
)
ecm_qt_declare_logging_category(sylpheedimporter_SRCS HEADER sylpheedplugin_debug.h
        IDENTIFIER SYLPHEEDPLUGIN_LOG CATEGORY_NAME org.kde.pim.importwizard.sylpheedplugin
        DESCRIPTION "importwizard (sylpheed plugin)"
        EXPORT IMPORTWIZARD
)

kcoreaddons_add_plugin(sylpheedimporterplugin SOURCES ${sylpheedimporter_SRCS} INSTALL_NAMESPACE pim6/importwizard)
set_target_properties(
    sylpheedimporterplugin
    PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY
            "${CMAKE_BINARY_DIR}/bin/importwizard/"
)
if(COMPILE_WITH_UNITY_CMAKE_SUPPORT)
    set_target_properties(
        sylpheedimporterplugin
        PROPERTIES
            UNITY_BUILD
                ON
    )
endif()

target_link_libraries(
    sylpheedimporterplugin
    KPim6::ImportWizard
    KF6::XmlGui
    KF6::I18n
    KF6::Contacts
    KPim6::MailImporterAkonadi
    KPim6::MailTransport
    KPim6::IdentityManagementCore
    KPim6::MailCommon
)
