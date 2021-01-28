#ifndef SR_GUI_H
#define SR_GUI_H

#ifdef __cplusplus
extern "C" {
#endif

#define SR_GUI_MESSAGE_LEVEL_INFO 0
#define SR_GUI_MESSAGE_LEVEL_WARN 1
#define SR_GUI_MESSAGE_LEVEL_ERROR 2

#define SR_GUI_VALIDATED 0
#define SR_GUI_CANCELLED 1
#define SR_GUI_BUTTON0 1000
#define SR_GUI_BUTTON1 (SR_GUI_BUTTON0+1)
#define SR_GUI_BUTTON2 (SR_GUI_BUTTON0+2)

void sr_gui_show_message(const char* title, const char* message, int level);

void sr_gui_show_notification(const char* title, const char* message);

int sr_gui_ask_directory(const char* title, const char* startPathr, char** outPath);

int sr_gui_ask_load_file(const char* title, const char* startDir, const char* exts, char** outpath);

int sr_gui_ask_save_file(const char* title, const char* startDir, const char* exts, char** outpath);

int sr_gui_ask_choice(const char* title, const char* message, int level, const char* button0, const char* button1, const char* button2);

int sr_gui_ask_string(const char* title, const char* message, char** result);

#define SR_GUI_ASK_ALPHA 1
#define SR_GUI_DONT_ASK_ALPHA 0

int sr_gui_ask_color(unsigned char color[4], int askAlpha);
	
#ifdef __cplusplus
}
#endif

#endif