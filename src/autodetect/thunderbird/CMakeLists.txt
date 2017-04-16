set(thunderbirdimporter_SRCS
   thunderbirdaddressbook.cpp  
   thunderbirdimportdata.cpp  
   thunderbirdsettings.cpp
   addressbook/MorkParser.cpp
)

kcoreaddons_add_plugin(thunderbirdimporterplugin JSON thunderbirdimporter.json SOURCES
    ${thunderbirdimporter_SRCS} INSTALL_NAMESPACE importwizard)


target_link_libraries(thunderbirdimporterplugin
  libimportwizard
  KF5::XmlGui
  KF5::I18n
  KF5::Contacts
  KF5::MailImporterAkonadi
  KF5::MailTransport
  KF5::IdentityManagement
  KF5::MailCommon
)
