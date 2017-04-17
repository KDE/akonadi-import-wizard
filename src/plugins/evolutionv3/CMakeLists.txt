set(evolutionv3importer_SRCS
   evolutionaddressbook.cpp  
   evolutionv3importdata.cpp  
   evolutionsettings.cpp
   evolutioncalendar.cpp
   evolutionutil.cpp
)

kcoreaddons_add_plugin(evolutionv3importerplugin JSON evolutionv3importer.json SOURCES
    ${evolutionv3importer_SRCS} INSTALL_NAMESPACE importwizard)


target_link_libraries(evolutionv3importerplugin
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
