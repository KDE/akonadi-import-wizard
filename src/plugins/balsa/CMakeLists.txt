set(balsaimporter_SRCS
   balsaaddressbook.cpp  
   balsaimportdata.cpp  
   balsasettings.cpp
)
ecm_qt_declare_logging_category(balsaimporter_SRCS HEADER balsaplugin_debug.h IDENTIFIER BALSAPLUGIN_LOG CATEGORY_NAME org.kde.pim.importwizard.balsaplugin)

kcoreaddons_add_plugin(balsaimporterplugin JSON balsaimporter.json SOURCES
    ${balsaimporter_SRCS} INSTALL_NAMESPACE importwizard)


target_link_libraries(balsaimporterplugin
  KPim::ImportWizard
  KF5::XmlGui
  KF5::I18n
  KF5::Contacts
  KF5::MailImporterAkonadi
  KF5::MailTransport
  KF5::MailCommon
  KF5::IdentityManagement
  KF5::MessageViewer
)
