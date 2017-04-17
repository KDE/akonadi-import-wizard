set(trojitaimporter_SRCS
   trojitaaddressbook.cpp  
   trojitaimportdata.cpp  
   trojitasettings.cpp
)

kcoreaddons_add_plugin(trojitaimporterplugin JSON trojitaimporter.json SOURCES
    ${trojitaimporter_SRCS} INSTALL_NAMESPACE importwizard)


target_link_libraries(trojitaimporterplugin
  libimportwizard
  KF5::XmlGui
  KF5::I18n
  KF5::Contacts
  KF5::MailImporterAkonadi
  KF5::MailTransport
  KF5::MailCommon
  KF5::IdentityManagement
)
