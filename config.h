#ifndef SV_CONFIG_H
#define SV_CONFIG_H

#define SV_PROGRAM_NAME "Source Viewer"

// macro for readString/writeString
#define PROJNAME            "PROJNAME"
#define CURPROJNAME         "CURPROJNAME"
#define PROJSTOREPATH       "PROJSTOREPATH"
#define SRCROOTPATH         "SRCROOTPATH"
#define CURTABINDEX         "CURTABINDEX"

// macro for readStringList/writeStringList
#define IGNOREFOLDERLIST    "IGNOREFOLDERLIST"
#define IGNOREFILELIST      "IGNOREFILELIST"
#define OPENEDFILELIST      "OPENEDFILELIST"
#define PROJECTNAMELIST     "PROJECTNAMELIST"

// macro for QSettings group
#define PROJECT_CONFIG      "project"
#define SETTINGS_CONFIG     "settings"

// macro for file extension
#define LAYOUT_SUFFIX       ".layout"
#define PROJECT_SUFFIX      ".project"
#define CONFIG_SUFFIX       ".config"
#define FILELIST_SUFFIX     ".filelist"

// macro for Qt Style Sheet
#define DEFAULT_QSS         "default.qss"
#define USER_QSS            "user.qss"

#endif // SV_CONFIG_H
