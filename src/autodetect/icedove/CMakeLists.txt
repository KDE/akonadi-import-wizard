set(icedoveimporter_SRCS
   icedoveaddressbook.cpp  
   icedoveimportdata.cpp  
   icedovesettings.cpp
)

kcoreaddons_add_plugin(icedoveimporterplugin JSON icedoveimporter.json SOURCES
    ${icedoveimporter_SRCS} INSTALL_NAMESPACE importwizard)


target_link_libraries(icedoveimporterplugin
  libimportwizard
  KF5::XmlGui
  KF5::I18n
  KF5::Contacts
  KF5::MailImporterAkonadi
  KF5::MailTransport
  KF5::IdentityManagement
)
