# sr_gui

## Whishlist

Directory selector
int sr_ask_directory(const char* title, const char* startDir, char** outpath)

File load selector (with extensions)
int sr_ask_load_file(const char* title, const char* startDir, const char* exts, char** outpath)

File save selector (with extensions)
int sr_ask_save_file(const char* title, const char* startDir, const char* exts, char** outpath) do we need exts here ?

Basic OK message
void sr_show_message(const char* title, const char* message, int level)

Message with 2/N options + Info/Warn/Error level
int sr_ask_choice(const char* title, const char* message, int level, const char* button0, const char* button1, const char* button2)

Basic input (one field)
int sr_ask_string(const char* title, const char* message, char** result)

Desktop notification
void sr_show_notification(const char* title, const char* message)

Color picker
int sr_ask_color(const char* title, const unsigned char default[3], unsigned char result[3])

## Dependencies

"User32", "Comdlg32", "Comctl32", "runtimeobject"
On Windows notifications will work after the first run.

