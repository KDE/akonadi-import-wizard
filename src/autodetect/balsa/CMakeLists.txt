set(balsaimporter_SRCS
   balsaaddressbook.cpp  
   balsaimportdata.cpp  
   balsasettings.cpp
)

kcoreaddons_add_plugin(balsaimporterplugin JSON balsaimporter.json SOURCES
    ${balsaimporter_SRCS} INSTALL_NAMESPACE importwizard)


target_link_libraries(balsaimporterplugin
  libimportwizard
  KF5::XmlGui
  KF5::I18n
  KF5::Contacts
  KF5::MailImporterAkonadi
  KF5::MailTransport
  KF5::MailCommon
  KF5::IdentityManagement
  KF5::MessageViewer
)
