set(balsaimporter_SRCS
    balsaaddressbook.cpp
    balsaimportdata.cpp
    balsasettings.cpp
    balsasettings.h
    balsaimportdata.h
    balsaaddressbook.h

    )
ecm_qt_declare_logging_category(balsaimporter_SRCS HEADER balsaplugin_debug.h
        IDENTIFIER BALSAPLUGIN_LOG CATEGORY_NAME org.kde.pim.importwizard.balsaplugin
        DESCRIPTION "importwizard (balsa plugin)"
        EXPORT IMPORTWIZARD
    )



kcoreaddons_add_plugin(balsaimporterplugin SOURCES ${balsaimporter_SRCS} INSTALL_NAMESPACE pim${QT_MAJOR_VERSION}/importwizard)
if (COMPILE_WITH_UNITY_CMAKE_SUPPORT)
    set_target_properties(balsaimporterplugin PROPERTIES UNITY_BUILD ON)
endif()
set_target_properties(balsaimporterplugin PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/importwizard/")

target_link_libraries(balsaimporterplugin
    KPim${KF_MAJOR_VERSION}::ImportWizard
    KF${KF_MAJOR_VERSION}::XmlGui
    KF${KF_MAJOR_VERSION}::I18n
    KF${KF_MAJOR_VERSION}::Contacts
    KPim${KF_MAJOR_VERSION}::MailImporterAkonadi
    KPim${KF_MAJOR_VERSION}::MailTransport
    KPim${KF_MAJOR_VERSION}::MailCommon
    KPim${KF_MAJOR_VERSION}::IdentityManagement
    KF5::MessageViewer
    )
