#include <sr_gui.h>

#include "common/common.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#	include <windows.h>
#endif

#define MAX_STR_SIZE 1023

int main(int argc, char** argv) {
	(void)argc;
	(void)argv;

	// Create dummy window for messages etc.
	createWindow(320, 240);

	sr_gui_init();

	if(1) {
		sr_gui_show_notification("Notification title", "This is a notification message which is longer than the title.");
	}

	if(1) {
		sr_gui_show_message("Message title", "This is a message text which is longer than the title.", SR_GUI_MESSAGE_LEVEL_INFO);
		sr_gui_show_message("Message title 2", "This is a message text which is longer than the title, 2.", SR_GUI_MESSAGE_LEVEL_ERROR);
	}

	if(1) {
		int res = sr_gui_ask_choice("Question title", "This is a question text which is longer than the title.", SR_GUI_MESSAGE_LEVEL_WARN, "OK", "Cancel", "Nope");
		if(res == SR_GUI_CANCELLED) {
			printf("Choice query canceled\n");
		} else {
			printf("Choice was: %s\n", res == SR_GUI_BUTTON0 ? "OK" : (res == SR_GUI_BUTTON1 ? "Cancel" : "Nope"));
		}
	}

	if(1) {
		int res = sr_gui_ask_choice("Question title", "This is a second question text with fewer choices.", SR_GUI_MESSAGE_LEVEL_WARN, "OK", "Cancel", NULL);
		if(res == SR_GUI_CANCELLED) {
			printf("Choice query canceled\n");
		} else {
			printf("Choice was: %s\n", res == SR_GUI_BUTTON0 ? "OK" : (res == SR_GUI_BUTTON1 ? "Cancel" : "Nope"));
		}
	}

	if(1) {
		char* content = "A default value";
		int res		  = sr_gui_ask_string("String field title", "Please input a string here", &content);
		if(res == SR_GUI_VALIDATED) {
			printf("String was: %s\n", content);
		} else {
			printf("String query canceled.\n");
		}
	}

	if(1) {
		char* outPath = NULL;
		int res		  = sr_gui_ask_save_file("Create file", "~", "jpg,png", &outPath);
		if(res == SR_GUI_VALIDATED) {
			printf("File path was: %s\n", outPath);
		} else {
			printf("File path query canceled.\n");
		}
	}

	if(1) {
		char* outPath = NULL;
		int res		  = sr_gui_ask_directory("Select directory", "./", &outPath);
		if(res == SR_GUI_VALIDATED) {
			printf("Directory path was: %s\n", outPath);
		} else {
			printf("Directory path query canceled.\n");
		}
	}

	if(1) {
		char** outPaths = NULL;
		int outCount	= 0;
		int res			= sr_gui_ask_load_files("Select paths", "./", NULL, &outPaths, &outCount);
		if(res == SR_GUI_VALIDATED) {
			printf("Multi paths: %d paths obtained\n", outCount);
			for(int i = 0; i < outCount; ++i) {
				printf("* %s\n", outPaths[i]);
			}
		} else {
			printf("Multi paths query canceled.\n");
		}
	}

	if(1) {
		unsigned char col[3];
		col[0]	= 0;
		col[1]	= 255;
		col[2]	= 128;
		int res = sr_gui_ask_color(col);
		if(res == SR_GUI_VALIDATED) {
			printf("Color was: %d,%d,%d\n", (int)col[0], (int)col[1], (int)col[2]);
		} else {
			printf("Color query canceled.\n");
		}
	}

	if(1){
		sr_gui_open_in_explorer("../../../README.md");
	}

	if(1){
		sr_gui_open_in_browser("https://github.com/kosua20/sr_gui");
	}

	if(1){
		sr_gui_open_in_default_app("../../../LICENSE.md");
	}
	
	sr_gui_cleanup();

	return 0;
}
