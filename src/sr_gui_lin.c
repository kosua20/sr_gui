

#include "sr_gui.h"
#include "sr_gui_internal.h"

#include <notify.h>
#include <gtk/gtk.h>

void _sr_gui_flush_gtk_events(){
	while (gtk_events_pending()){
		gtk_main_iteration();
	}
}

void sr_gui_init(){
	notify_init(SR_GUI_APP_NAME);
}

void sr_gui_cleanup(){
	notify_uninit();
}

void sr_gui_show_message(const char* title, const char* message, int level){
	if(!gtk_init_check(NULL, NULL)){
		return;
	}

	GtkMessageType levelGtk = GTK_MESSAGE_INFO;
	if(level == SR_GUI_MESSAGE_LEVEL_ERROR){
		levelGtk = GTK_MESSAGE_ERROR;
	} else if(level == SR_GUI_MESSAGE_LEVEL_WARN){
		levelGtk = GTK_MESSAGE_WARNING;
	}

	GtkMessageDialog* dialog = (GtkMessageDialog*)gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, levelGtk, GTK_BUTTONS_OK, title);
	gtk_message_dialog_format_secondary_text(dialog, message);

	int res = gtk_dialog_run(dialog);
	(void)res;
	gtk_widget_destroy(dialog);
}

void sr_gui_show_notification(const char* title, const char* message){
	NotifyNotification* notif = notify_notification_new(title, message, NULL);
	notify_notification_set_timeout(notif, NOTIFY_EXPIRES_DEFAULT);
	notify_notification_set_urgency(notif, NOTIFY_URGENCY_NORMAL);
	GError error;
	gboolean res = notify_notification_show(notif, &error);
	(void)res;
}

int sr_gui_ask_directory(const char* title, const char* startDir, char** outPath){
	*outPath = NULL;
	if(!gtk_init_check(NULL, NULL)){
		return SR_GUI_CANCELLED;
	}

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
	if(!gtk_init_check(NULL, NULL)){
		return SR_GUI_CANCELLED;
	}

	return SR_GUI_VALIDATED;
}

int sr_gui_ask_save_file(const char* title, const char* startDir, const char* exts, char** outPath){
	*outPath = NULL;
	if(!gtk_init_check(NULL, NULL)){
		return SR_GUI_CANCELLED;
	}

	return SR_GUI_VALIDATED;
}

int sr_gui_ask_choice(const char* title, const char* message, int level, const char* button0, const char* button1, const char* button2){
	if(!gtk_init_check(NULL, NULL)){
		return SR_GUI_CANCELLED;
	}

	if(level == SR_GUI_MESSAGE_LEVEL_ERROR){

	} else if(level == SR_GUI_MESSAGE_LEVEL_WARN){

	}

	GtkWidget* dialog = gtk_dialog_new_with_buttons (title, NULL, GTK_DIALOG_MODAL, button0, SR_GUI_BUTTON0, button1, SR_GUI_BUTTON1, button2, SR_GUI_BUTTON2, NULL);
	gtk_dialog_set_default_response((GtkDialog*)dialog, SR_GUI_BUTTON0);

	int res = gtk_dialog_run(GTK_DIALOG(picker));
	gtk_widget_destroy(picker);

	if(res != SR_GUI_BUTTON0 && res != SR_GUI_BUTTON1 && res != SR_GUI_BUTTON2){
		return SR_GUI_CANCELLED;
	}
	return res;
}

int sr_gui_ask_string(const char* title, const char* message, char** result){
	if(!gtk_init_check(NULL, NULL)){
		return SR_GUI_CANCELLED;
	}

	return SR_GUI_VALIDATED;
}


int sr_gui_ask_color(unsigned char color[3]){
	if(!gtk_init_check(NULL, NULL)){
		return SR_GUI_CANCELLED;
	}

	GdkRGBA colorGtk;
	colorGtk.red = ((float)color[0])/255.0f;
	colorGtk.green = ((float)color[1])/255.0f;
	colorGtk.blue = ((float)color[2])/255.0f;
	colorGtk.alpha = 1.0f;

	GtkColorChooser* picker = (GtkColorChooser*)gtk_color_chooser_dialog_new (NULL, NULL);
	gtk_color_chooser_set_rgba(picker, &colorGtk);
	gtk_color_chooser_set_use_alpha(picker, 0)

	int res = gtk_dialog_run(GTK_DIALOG(picker));
	if(res != GTK_RESPONSE_ACCEPT){
		gtk_widget_destroy(picker);
		return SR_GUI_CANCELLED;
	}

	gtk_color_chooser_get_rgba(picker, &colorGtk);
	color[0] = (unsigned char)(fmin(fmax(255.0f * colorGtk.red, 0.0f), 255.0f));
	color[1] = (unsigned char)(fmin(fmax(255.0f * colorGtk.green, 0.0f), 255.0f));
	color[2] = (unsigned char)(fmin(fmax(255.0f * colorGtk.blue, 0.0f), 255.0f));
	gtk_widget_destroy(picker);

	return SR_GUI_VALIDATED;
}

