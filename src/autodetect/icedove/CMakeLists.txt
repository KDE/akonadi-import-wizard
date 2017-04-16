set(icedoveimporter_SRCS
   icedoveimportdata.cpp
   ../thunderbird/thunderbirdaddressbook.cpp  
   ../thunderbird/thunderbirdsettings.cpp
   ../thunderbird/addressbook/MorkParser.cpp

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
  KF5::MailCommon
)
