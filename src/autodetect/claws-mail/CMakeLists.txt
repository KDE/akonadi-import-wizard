set(clawsmailimporter_SRCS
   clawsmailaddressbook.cpp  
   clawsmailimportdata.cpp  
   clawsmailsettings.cpp
   ../sylpheed/sylpheedaddressbook.cpp  
   ../sylpheed/sylpheedsettings.cpp
   ../sylpheed/sylpheedsettingsutils.cpp

)

kcoreaddons_add_plugin(clawsmailimporterplugin JSON clawsmailimporter.json SOURCES
    ${clawsmailimporter_SRCS} INSTALL_NAMESPACE importwizard)


target_link_libraries(clawsmailimporterplugin
  libimportwizard
  KF5::XmlGui
  KF5::I18n
  KF5::Contacts
  KF5::MailImporterAkonadi
  KF5::MailTransport
  KF5::IdentityManagement
  KF5::MailCommon
)
