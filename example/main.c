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

	//sr_gui_show_notification("Notification title", "This is a notification message which is longer than the title.");

	//sr_gui_show_message("Message title", "This is a message text which is longer than the title.", SR_GUI_MESSAGE_LEVEL_WARN);

//	int choice = sr_gui_ask_choice("Question title", "This is a question text which is longer than the title.", SR_GUI_MESSAGE_LEVEL_WARN , "OK", "Cancel", "Watuf");
//	printf("Choice was: %d\n", choice);

	char* content = "A default value";
	int res = sr_gui_ask_string("String field title", "Please input a string here", &content);
	if( res == SR_GUI_VALIDATED){
		printf("String was: %s\n", content);
	} else {
		printf("Cancelled.\n");
	}

//	unsigned char col[4];
//	col[0] = 0; col[1] = 255; col[2] = 128; col[3] = 128;
//	sr_gui_ask_color(col, SR_GUI_ASK_ALPHA);
//	printf("Color was: %d,%d,%d,%d\n", (int)col[0], (int)col[1], (int)col[2], (int)col[3]);


	//sleep(10);
	return 0;
}
