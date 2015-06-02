#ifndef SYSTEMCONFIG_H_
#define SYSTEMCONFIG_H_

#define CREATE_FILE_WITH_TIMESTAMP  1
#define CREATE_SUB_DIR_FOR_OUTPUT   1

#define FILE_LOG                    0       /*!< file_index for logger file */
#define FILE_VIDEO_TS               1       /*!< file_index for timestamp file */

#define DISPLAY_MSG_IN_CONSOLE      true
#define DISPLAY_WARNING_MESSAGES    true    //false //

#define ENABLE_PAUSE                false //true //

#define NUM_FRAMES_TO_IGNORE        5 // Ignore initial frames far camera stabilization // 5s if 1fps

#endif // SYSTEMCONFIG_H_
