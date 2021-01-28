 #include <sr_gui.h>

#include "common/common.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_STR_SIZE 1023

int main(int argc, char** argv){
	(void)argc;
	(void)argv;

	// Create dummy window for messages etc.
	createWindow(320, 240);

	{
		sr_gui_show_notification("Notification title", "This is a notification message which is longer than the title.");
	}

	{
		sr_gui_show_message("Message title", "This is a message text which is longer than the title.", SR_GUI_MESSAGE_LEVEL_WARN);
	}

	{
		int res = sr_gui_ask_choice("Question title", "This is a question text which is longer than the title.", SR_GUI_MESSAGE_LEVEL_WARN , "OK", "Cancel", "Nope");
		printf("Choice was: %s\n", res == SR_GUI_BUTTON0 ? "OK" : (res == SR_GUI_BUTTON1 ? "Cancel" : "Nope"));
	}

	{
		char* content = "A default value";
		int res = sr_gui_ask_string("String field title", "Please input a string here", &content);
		if( res == SR_GUI_VALIDATED){
			printf("String was: %s\n", content);
		} else {
			printf("String query cancelled.\n");
		}
	}

	{
		char* outPath = NULL;
		int res = sr_gui_ask_save_file("Create file", "./", NULL, &outPath);
		if( res == SR_GUI_VALIDATED){
			printf("File path was: %s\n", outPath);
		} else {
			printf("File path query cancelled.\n");
		}
	}

	{
		char* outPath = NULL;
		int res = sr_gui_ask_directory("Select directory", "./", &outPath);
		if( res == SR_GUI_VALIDATED){
			printf("Directory path was: %s\n", outPath);
		} else {
			printf("Directory path query cancelled.\n");
		}
	}

	{
		char** outPaths = NULL;
		int outCount = 0;
		int res = sr_gui_ask_load_files("Select paths", "./", NULL, &outPaths, &outCount);
		if( res == SR_GUI_VALIDATED){
			printf("Multi paths: %d paths obtained\n", outCount);
			for(int i = 0; i < outCount ;++i){
				printf("* %s\n", outPaths[i]);
			}
		} else {
			printf("Multi paths query cancelled.\n");
		}
	}

	{
		unsigned char col[4];
		col[0] = 0; col[1] = 255; col[2] = 128; col[3] = 128;
		int res = sr_gui_ask_color(col, SR_GUI_ASK_ALPHA);
		if( res == SR_GUI_VALIDATED){
			printf("Color was: %d,%d,%d,%d\n", (int)col[0], (int)col[1], (int)col[2], (int)col[3]);
		} else {
			printf("Color query cancelled.\n");
		}

	}

	return 0;
}
