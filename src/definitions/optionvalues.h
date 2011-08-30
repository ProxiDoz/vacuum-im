#ifndef DEF_OPTIONVALUES_H
#define DEF_OPTIONVALUES_H

// AccountManager
#define OPV_ACCOUNT_ROOT                                "accounts"
#define OPV_ACCOUNT_ITEM                                "accounts.account"
#define OPV_ACCOUNT_NAME                                "accounts.account.name"
#define OPV_ACCOUNT_ACTIVE                              "accounts.account.active"
#define OPV_ACCOUNT_STREAMJID                           "accounts.account.streamJid"
#define OPV_ACCOUNT_PASSWORD                            "accounts.account.password"
#define OPV_ACCOUNT_REQUIREENCRYPTION                   "accounts.account.require-encryption"
// ConnectionManager
#define OPV_ACCOUNT_CONNECTION_ITEM                     "accounts.account.connection"
#define OPV_ACCOUNT_CONNECTION_TYPE                     "accounts.account.connection-type"
// DefaultConnection
#define OPV_ACCOUNT_CONNECTION_HOST                     "accounts.account.connection.host"
#define OPV_ACCOUNT_CONNECTION_PORT                     "accounts.account.connection.port"
#define OPV_ACCOUNT_CONNECTION_PROXY                    "accounts.account.connection.proxy"
#define OPV_ACCOUNT_CONNECTION_USELEGACYSSL             "accounts.account.connection.use-legacy-ssl"
// Registration
#define OPV_ACCOUNT_REGISTER                            "accounts.account.register-on-server"
// StatusChanger
#define OPV_ACCOUNT_AUTOCONNECT                         "accounts.account.auto-connect"
#define OPV_ACCOUNT_AUTORECONNECT                       "accounts.account.auto-reconnect"
#define OPV_ACCOUNT_STATUS_ISMAIN                       "accounts.account.status.is-main"
#define OPV_ACCOUNT_STATUS_LASTONLINE                   "accounts.account.status.last-online"
// MessageArchiver
#define OPV_ACCOUNT_ARCHIVEREPLICATION                  "accounts.account.archive-replication"
// BookMarks
#define OPV_ACCOUNT_IGNOREAUTOJOIN                      "accounts.account.ignore-autojoin"

// Console
#define OPV_CONSOLE_ROOT                                "console"
#define OPV_CONSOLE_CONTEXT_ITEM                        "console.context"
#define OPV_CONSOLE_CONTEXT_NAME                        "console.context.name"
#define OPV_CONSOLE_CONTEXT_STREAMJID                   "console.context.streamjid"
#define OPV_CONSOLE_CONTEXT_CONDITIONS                  "console.context.conditions"
#define OPV_CONSOLE_CONTEXT_HIGHLIGHTXML                "console.context.highlight-xml"
#define OPV_CONSOLE_CONTEXT_WORDWRAP                    "console.context.word-wrap"

// DataStreamsManager
#define OPV_DATASTREAMS_ROOT                            "datastreams"
#define OPV_DATASTREAMS_SPROFILE_ITEM                   "datastreams.settings-profile"
#define OPV_DATASTREAMS_SPROFILE_NAME                   "datastreams.settings-profile.name"
#define OPV_DATASTREAMS_METHOD_ITEM                     "datastreams.settings-profile.method"
// InBandStream
#define OPV_DATASTREAMS_METHOD_BLOCKSIZE                "datastreams.settings-profile.method.block-size"
#define OPV_DATASTREAMS_METHOD_MAXBLOCKSIZE             "datastreams.settings-profile.method.max-block-size"
#define OPV_DATASTREAMS_METHOD_STANZATYPE               "datastreams.settings-profile.method.stanza-type"
// SocksStreams
#define OPV_DATASTREAMS_SOCKSLISTENPORT                 "datastreams.socks-listen-port"
#define OPV_DATASTREAMS_METHOD_DISABLEDIRECT            "datastreams.settings-profile.method.disable-direct-connections"
#define OPV_DATASTREAMS_METHOD_FORWARDHOST              "datastreams.settings-profile.method.forward-host"
#define OPV_DATASTREAMS_METHOD_FORWARDPORT              "datastreams.settings-profile.method.forward-port"
#define OPV_DATASTREAMS_METHOD_USEACCOUNTSTREAMPROXY    "datastreams.settings-profile.method.use-account-stream-proxy"
#define OPV_DATASTREAMS_METHOD_STREAMPROXYLIST          "datastreams.settings-profile.method.stream-proxy-list"
#define OPV_DATASTREAMS_METHOD_USEACCOUNTNETPROXY       "datastreams.settings-profile.method.use-account-network-proxy"
#define OPV_DATASTREAMS_METHOD_NETWORKPROXY             "datastreams.settings-profile.method.network-proxy"

// FileStreamsManager
#define OPV_FILESTREAMS_DEFAULTDIR                      "filestreams.default-dir"
#define OPV_FILESTREAMS_GROUPBYSENDER                   "filestreams.group-by-sender"
#define OPV_FILESTREAMS_DEFAULTMETHOD                   "filestreams.default-method"
#define OPV_FILESTREAMS_ACCEPTABLEMETHODS               "filestreams.acceptable-methods"
// FileTransfer
#define OPV_FILETRANSFER_ROOT                           "filestreams.filetransfer"
#define OPV_FILETRANSFER_AUTORECEIVE                    "filestreams.filetransfer.autoreceive"
#define OPV_FILETRANSFER_HIDEONSTART                    "filestreams.filetransfer.hide-dialog-on-start"
#define OPV_FILETRANSFER_REMOVEONFINISH                 "filestreams.filetransfer.remove-stream-on-finish"

// MainWindow
#define OPV_MAINWINDOW_SHOW                             "mainwindow.show"
#define OPV_MAINWINDOW_ALIGN                            "mainwindow.align"

// MessageWidgets
#define OPV_MESSAGES_ROOT                               "messages"
#define OPV_MESSAGES_SHOWSTATUS                         "messages.show-status"
#define OPV_MESSAGES_ARCHIVESTATUS                      "messages.archive-status"
#define OPV_MESSAGES_SHOWINFOWIDGET                     "messages.show-info-widget"
#define OPV_MESSAGES_INFOWIDGETMAXSTATUSCHARS           "messages.info-widget-max-status-chars"
#define OPV_MESSAGES_EDITORAUTORESIZE                   "messages.editor-auto-resize"
#define OPV_MESSAGES_EDITORMINIMUMLINES                 "messages.editor-minimum-lines"
#define OPV_MESSAGES_CLEANCHATTIMEOUT                   "messages.clean-chat-timeout"
#define OPV_MESSAGES_TABWINDOWS_ROOT                    "messages.tab-windows"
#define OPV_MESSAGES_TABWINDOWS_ENABLE                  "messages.tab-windows.enable"
#define OPV_MESSAGES_TABWINDOWS_DEFAULT                 "messages.tab-windows.default"
#define OPV_MESSAGES_TABWINDOW_ITEM                     "messages.tab-windows.window"
#define OPV_MESSAGES_TABWINDOW_NAME                     "messages.tab-windows.window.name"
#define OPV_MESSAGES_TABWINDOW_TABSCLOSABLE             "messages.tab-windows.window.tabs-closable"
#define OPV_MESSAGES_TABWINDOW_TABSBOTTOM               "messages.tab-windows.window.tabs-bottom"
#define OPV_MESSAGES_TABWINDOW_SHOWINDICES              "messages.tab-windows.window.show-indices"
#define OPV_MESSAGES_TABWINDOW_REMOVETABSONCLOSE        "messages.tab-windows.window.remove-tabs-on-close"
// Emoticons
#define OPV_MESSAGES_EMOTICONS                          "messages.emoticons"
// ChatStates
#define OPV_MESSAGES_CHATSTATESENABLED                  "messages.chatstates-enabled"
// ChatMessageHandler
#define OPV_MESSAGES_LOAD_HISTORY                       "messages.load-chat-history"
//MessageStyles
#define OPV_MESSAGES_SHOWDATESEPARATORS                 "messages.show-date-separators"

// MultiUserChat
#define OPV_MUC_GROUPCHAT_SHOWENTERS                    "muc.groupchat.show-enters"
#define OPV_MUC_GROUPCHAT_SHOWSTATUS                    "muc.groupchat.show-status"
#define OPV_MUC_GROUPCHAT_ARCHIVESTATUS                 "muc.groupchat.archive-status"

// MessageArchive
#define OPV_HISTORY_COLLECTION_SIZE                     "history.collection.size"
#define OPV_HISTORY_COLLECTION_MAXSIZE                  "history.collection.max-size"
#define OPV_HISTORY_COLLECTION_TIMEOUT                  "history.collection.timeout"
#define OPV_HISTORY_COLLECTION_MINTIMEOUT               "history.collection.min-timeout"
#define OPV_HISTORY_COLLECTION_MAXTIMEOUT               "history.collection.max-timeout"
#define OPV_HISTORY_COLLECTION_MINMESSAGES              "history.collection.min-messages"

// MessageStyles
#define OPV_MESSAGESTYLE_ROOT                           "message-styles"
#define OPV_MESSAGESTYLE_MTYPE_ITEM                     "message-styles.message-type"
#define OPV_MESSAGESTYLE_CONTEXT_ITEM                   "message-styles.message-type.context"
#define OPV_MESSAGESTYLE_STYLE_TYPE                     "message-styles.message-type.context.style-type"
#define OPV_MESSAGESTYLE_STYLE_ITEM                     "message-styles.message-type.context.style"
// AdiumMessageStyle
#define OPV_MESSAGESTYLE_STYLE_ID                       "message-styles.message-type.context.style.style-id"
#define OPV_MESSAGESTYLE_STYLE_VARIANT                  "message-styles.message-type.context.style.variant"
#define OPV_MESSAGESTYLE_STYLE_FONTFAMILY               "message-styles.message-type.context.style.font-family"
#define OPV_MESSAGESTYLE_STYLE_FONTSIZE                 "message-styles.message-type.context.style.font-size"
#define OPV_MESSAGESTYLE_STYLE_BGCOLOR                  "message-styles.message-type.context.style.bg-color"
#define OPV_MESSAGESTYLE_STYLE_BGIMAGEFILE              "message-styles.message-type.context.style.bg-image-file"
#define OPV_MESSAGESTYLE_STYLE_BGIMAGELAYOUT            "message-styles.message-type.context.style.bg-image-layout"

// OptionsManager
#define OPV_MISC_ROOT                                   "misc"
#define OPV_MISC_AUTOSTART                              "misc.autostart"
#define OPV_MISC_SHAREOSVERSION                         "misc.share-os-version"

// Notifications
#define OPV_NOTIFICATIONS_ROOT                          "notifications"
#define OPV_NOTIFICATIONS_EXPANDGROUP                   "notifications.expand-groups"
#define OPV_NOTIFICATIONS_NOSOUNDIFDND                  "notifications.no-sound-if-dnd"
#define OPV_NOTIFICATIONS_POPUPTIMEOUT                  "notifications.popup-timeout"
#define OPV_NOTIFICATIONS_SOUNDCOMMAND                  "notifications.sound-command"
#define OPV_NOTIFICATIONS_TYPEKINDS_ITEM                "notifications.type-kinds.type"
#define OPV_NOTIFICATIONS_KINDENABLED_ITEM              "notifications.kind-enabled.kind"

// ConnectionManager
#define OPV_PROXY_ROOT                                  "proxy"
#define OPV_PROXY_DEFAULT                               "proxy.default"
#define OPV_PROXY_ITEM                                  "proxy.proxy"
#define OPV_PROXY_NAME                                  "proxy.proxy.name"
#define OPV_PROXY_TYPE                                  "proxy.proxy.type"
#define OPV_PROXY_HOST                                  "proxy.proxy.host"
#define OPV_PROXY_PORT                                  "proxy.proxy.port"
#define OPV_PROXY_USER                                  "proxy.proxy.user"
#define OPV_PROXY_PASS                                  "proxy.proxy.pass"

// RostersView
#define OPV_ROSTER_ROOT                                 "roster"
#define OPV_ROSTER_SHOWOFFLINE                          "roster.show-offline"
#define OPV_ROSTER_SHOWRESOURCE                         "roster.show-resource"
#define OPV_ROSTER_SHOWSTATUSTEXT                       "roster.show-status-text"
#define OPV_ROSTER_SORTBYSTATUS                         "roster.sort-by-status"
// RosterChanger
#define OPV_ROSTER_AUTOSUBSCRIBE                        "roster.auto-subscribe"
#define OPV_ROSTER_AUTOUNSUBSCRIBE                      "roster.auto-unsubscribe"
// Avatars
#define OPV_AVATARS_SHOW                                "roster.avatars.show"
#define OPV_AVATARS_SHOWEMPTY                           "roster.avatars.show-empty"
#define OPV_AVATARS_CUSTOM_ITEM                         "roster.avatars.custom"
// RosterSearch
#define OPV_ROSTER_SEARCH_ENABLED                       "roster.search.enabled"
#define OPV_ROSTER_SEARCH_FIELDEBANLED                  "roster.search.field-enabled"

//ShortcutManager
#define OPV_SHORTCUTS                                   "shortcuts"

// StatusChanger
#define OPV_STATUSES_ROOT                               "statuses"
#define OPV_STATUSES_MAINSTATUS                         "statuses.main-status"
#define OPV_STATUSES_MODIFY                             "statuses.modify-status"
#define OPV_STATUS_ITEM                                 "statuses.status"
#define OPV_STATUS_NAME                                 "statuses.status.name"
#define OPV_STATUS_SHOW                                 "statuses.status.show"
#define OPV_STATUS_TEXT                                 "statuses.status.text"
#define OPV_STATUS_PRIORITY                             "statuses.status.priority"
// AutoStatus
#define OPV_AUTOSTARTUS_ROOT                            "statuses.autostatus"
#define OPV_AUTOSTARTUS_RULE_ITEM                       "statuses.autostatus.rule"
#define OPV_AUTOSTARTUS_RULE_ENABLED                    "statuses.autostatus.rule.enabled"
#define OPV_AUTOSTARTUS_RULE_TIME                       "statuses.autostatus.rule.time"
#define OPV_AUTOSTARTUS_RULE_SHOW                       "statuses.autostatus.rule.show"
#define OPV_AUTOSTARTUS_RULE_TEXT                       "statuses.autostatus.rule.text"

// StatusIcons
#define OPV_STATUSICONS                                 "statusicons"
#define OPV_STATUSICONS_DEFAULT                         "statusicons.default-iconset"
#define OPV_STATUSICONS_RULES_ROOT                      "statusicons.rules"
#define OPV_STATUSICONS_RULE_ITEM                       "statusicons.rules.rule"
#define OPV_STATUSICONS_RULE_PATTERN                    "statusicons.rules.rule.pattern"
#define OPV_STATUSICONS_RULE_ICONSET                    "statusicons.rules.rule.iconset"

#endif // DEF_OPTIONVALUES_H
