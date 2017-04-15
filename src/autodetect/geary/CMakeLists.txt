set(gearyimporter_SRCS
   gearyaddressbook.cpp  
   gearyimportdata.cpp  
   gearysettings.cpp
)

kcoreaddons_add_plugin(gearyimporterplugin JSON gearyimporter.json SOURCES
    ${gearyimporter_SRCS} INSTALL_NAMESPACE importwizard)


target_link_libraries(gearyimporterplugin
  libimportwizard
  KF5::XmlGui
  KF5::I18n
  KF5::Contacts
  KF5::MailImporterAkonadi
  KF5::MailTransport
  KF5::IdentityManagement
  KF5::MailCommon
)
