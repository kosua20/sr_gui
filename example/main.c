#include <sr_gui.h>

#include "common/common.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#	include <windows.h>
#endif

#define MAX_STR_SIZE 1023

//#define TEST_NULL_INPUTS
//#define TEST_NULL_OUTPUTS

#ifdef TEST_NULL_INPUTS
#define IN(A) (NULL)
#else
#define IN(A) (A)
#endif

#ifdef TEST_NULL_OUTPUTS
#define OUT(A) (NULL)
#define RELEASE(ptr) ((void)(ptr))
#else
#define OUT(A) (A)
#define RELEASE(ptr) if(ptr){ free(ptr); }
#endif

int main(int argc, char** argv) {
	(void)argc;
	(void)argv;

	// Create dummy window for messages etc.
	createWindow(320, 240);

	sr_gui_init();

	if(1) {
		sr_gui_show_notification(IN("Notification title"), IN("This is a notification message which is longer than the title."));
	}

	if(1) {
		sr_gui_show_message(IN("Message title"), IN("This is a message text which is longer than the title."), IN(SR_GUI_MESSAGE_LEVEL_INFO));
		sr_gui_show_message(IN("Message title 2"), IN("This is a message text which is longer than the title, 2."), IN(SR_GUI_MESSAGE_LEVEL_ERROR));
	}

	if(1) {
		int res = sr_gui_ask_choice(IN("Question title"), IN("This is a question text which is longer than the title."), IN(SR_GUI_MESSAGE_LEVEL_WARN), IN("OK"), IN("Cancel"), IN("Nope"));
		if(res == SR_GUI_CANCELLED) {
			printf("Choice query canceled\n");
		} else {
			printf("Choice was: %s\n", res == SR_GUI_BUTTON0 ? "OK" : (res == SR_GUI_BUTTON1 ? "Cancel" : "Nope"));
		}
	}

	if(1) {
		int res = sr_gui_ask_choice(IN("Question title"), IN("This is a second question text with fewer choices."), IN(SR_GUI_MESSAGE_LEVEL_WARN), IN("OK"), IN("Cancel"), IN(NULL));
		if(res == SR_GUI_CANCELLED) {
			printf("Choice query canceled\n");
		} else {
			printf("Choice was: %s\n", res == SR_GUI_BUTTON0 ? "OK" : (res == SR_GUI_BUTTON1 ? "Cancel" : "Nope"));
		}
	}

	if(1) {
		char* content = "A default string";
		int res		  = sr_gui_ask_string(IN("String field title"), IN("Please input a string here"), OUT(&content));
		if(res == SR_GUI_VALIDATED) {
			printf("String was: %s\n", content);
		} else {
			printf("String query canceled.\n");
		}
		RELEASE(content);
	}

	if(1) {
		char* outPath = NULL;
		int res		  = sr_gui_ask_save_file(IN("Create file"), IN("~"), IN("jpg,png"), OUT(&outPath));
		if(res == SR_GUI_VALIDATED) {
			printf("File path was: %s\n", outPath);
		} else {
			printf("File path query canceled.\n");
		}
		RELEASE(outPath);
	}

	if(1) {
		char* outPath = NULL;
		int res		  = sr_gui_ask_directory(IN("Select directory"), IN("./"), OUT(&outPath));
		if(res == SR_GUI_VALIDATED) {
			printf("Directory path was: %s\n", outPath);
		} else {
			printf("Directory path query canceled.\n");
		}
		RELEASE(outPath);
	}

	if(1) {
		char* outPath = NULL;
		int res		  = sr_gui_ask_load_file(IN("Select path"), IN("./"), IN("txt"), OUT(&outPath));
		if(res == SR_GUI_VALIDATED) {
			printf("File path was: %s\n", outPath);
		} else {
			printf("File path query canceled.\n");
		}
		RELEASE(outPath);
	}

	if(1) {
		char** outPaths = NULL;
		int outCount	= 0;
		int res			= sr_gui_ask_load_files(IN("Select paths"), IN("./"), IN(NULL), OUT(&outPaths), OUT(&outCount));
		if(res == SR_GUI_VALIDATED) {
			printf("Multi paths: %d paths obtained\n", outCount);
			for(int i = 0; i < outCount; ++i) {
				printf("* %s\n", outPaths[i]);
			}
		} else {
			printf("Multi paths query canceled.\n");
		}
		for(int i = 0; i < outCount; ++i){
			RELEASE(outPaths[i]);
		}
		RELEASE(outPaths);
	}

	if(1) {
		unsigned char col[3];
		col[0]	= 0;
		col[1]	= 255;
		col[2]	= 128;
		int res = sr_gui_ask_color(OUT(col));
		if(res == SR_GUI_VALIDATED) {
			printf("Color was: %d,%d,%d\n", (int)col[0], (int)col[1], (int)col[2]);
		} else {
			printf("Color query canceled.\n");
		}
	}

	if(1){
		sr_gui_open_in_explorer(IN("../../../README.md"));
		sr_gui_open_in_browser(IN("https://github.com/kosua20/sr_gui"));
		sr_gui_open_in_default_app(IN("../../../LICENSE.md"));
	}

	if(1){
		char* outPath = NULL;
		int res = sr_gui_get_app_data_path(OUT(&outPath));
		if(res == SR_GUI_VALIDATED) {
			printf("Application data path was: '%s'\n", outPath);
		} else {
			printf("Application data path failed.\n");
		}
		RELEASE(outPath);
	}
	
	sr_gui_cleanup();

	return 0;
}
