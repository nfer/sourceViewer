#ifndef SV_CONFIG_H
#define SV_CONFIG_H

#include "stable.h"

#define SV_PROGRAM_NAME "Source Viewer"

// macro for readString/writeString
#define PROJNAME            "PROJNAME"
#define PROJSTOREPATH       "PROJSTOREPATH"
#define SRCROOTPATH         "SRCROOTPATH"

// macro for readStringList/writeStringList
#define IGNOREFOLDERLIST    "IGNOREFOLDERLIST"
#define IGNOREFILELIST      "IGNOREFILELIST"
#define OPENEDFILELIST      "OPENEDFILELIST"
#define PROJECTNAMELIST     "PROJECTNAMELIST"

// macro for QSettings group
#define PROJECT_CONFIG      "project"
#define SETTINGS_CONFIG     "settings"

// macro for file extension
#define PROJECT_SUFFIX      ".project"
#define CONFIG_SUFFIX       ".config"
#define FILELIST_SUFFIX     ".filelist"

#endif // SV_CONFIG_H
