

#include "sr_gui.h"
#include "sr_gui_internal.h"

#include <stdio.h>
#include <math.h>
#include <string.h>

void sr_gui_init(){
	// Nothing.
}

void sr_gui_cleanup(){
	// Nothing.
}

void sr_gui_show_message(const char* title, const char* message, int level){

	if(level == SR_GUI_MESSAGE_LEVEL_ERROR){

	} else if(level == SR_GUI_MESSAGE_LEVEL_WARN){

	}

}

void sr_gui_show_notification(const char* title, const char* message){

}

int sr_gui_ask_directory(const char* title, const char* startDir, char** outPath){
	*outPath = NULL;


	// Copy to result, allocating enough space.
//	*outPath = (char*)SR_GUI_MALLOC(sizeof(char) * (size+1));
//	if(*outPath == NULL){
//		return SR_GUI_CANCELLED;
//	}
//	SR_GUI_MEMCPY(*outPath, buffer, sizeof(char) * (size+1));
	return SR_GUI_VALIDATED;
}

int sr_gui_ask_load_files(const char* title, const char* startDir, const char* exts, char*** outPaths, int* outCount){
	*outCount = 0;
	*outPaths = NULL;

	return SR_GUI_VALIDATED;
}

int sr_gui_ask_save_file(const char* title, const char* startDir, const char* exts, char** outPath){
	*outPath = NULL;


	return SR_GUI_VALIDATED;
}

int sr_gui_ask_choice(const char* title, const char* message, int level, const char* button0, const char* button1, const char* button2){

	if(level == SR_GUI_MESSAGE_LEVEL_ERROR){

	} else if(level == SR_GUI_MESSAGE_LEVEL_WARN){

	}

	return SR_GUI_BUTTON0;
}

int sr_gui_ask_string(const char* title, const char* message, char** result){

	return SR_GUI_VALIDATED;
}


int sr_gui_ask_color(unsigned char color[3]){

	return SR_GUI_VALIDATED;
}

