#include "utils/message.h"
#include <gtk/gtk.h>
#include "app.h"
char* showInputDialog(char* msg){
    GtkWidget* dialog=gtk_dialog_new_with_buttons("Input",GTK_WINDOW(app->window),GTK_DIALOG_DESTROY_WITH_PARENT,"OK",GTK_RESPONSE_ACCEPT,"Cancel",GTK_RESPONSE_CANCEL,NULL);
    GtkWidget* contentArea=gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GtkWidget* label=gtk_label_new(msg);
    GtkWidget* entry=gtk_entry_new();
    gtk_container_add(GTK_CONTAINER(contentArea),label);
    gtk_container_add(GTK_CONTAINER(contentArea),entry);
    gtk_widget_show_all(dialog);
    int res=gtk_dialog_run(GTK_DIALOG(dialog));
    char* text=(char*)gtk_entry_get_text(GTK_ENTRY(entry));
    char* output=(char*)malloc(strlen(text)+1);
    strcpy(output,text);
    gtk_widget_destroy(dialog);
    return ((res==-3 && strcmp(output,"")!=0)?output:0); //-3 is OK button ID
}
void showErrorDialog(char* msg){
    GtkWidget* message=gtk_message_dialog_new(GTK_WINDOW(app->window),GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_ERROR,GTK_BUTTONS_OK,"%s",msg);
    gtk_dialog_run(GTK_DIALOG(message));
    gtk_widget_destroy(message);
}
void showInfoDialog(char* msg){
    GtkWidget* message=gtk_message_dialog_new(GTK_WINDOW(app->window),GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_INFO,GTK_BUTTONS_OK,"%s",msg);
    gtk_dialog_run(GTK_DIALOG(message));
    gtk_widget_destroy(message);
}
int showYesNoDialog(char* msg){
    GtkWidget* dialog=gtk_dialog_new_with_buttons("Question",GTK_WINDOW(app->window),GTK_DIALOG_DESTROY_WITH_PARENT,"Yes",GTK_RESPONSE_YES,"No",GTK_RESPONSE_NO,NULL);
    GtkWidget* contentArea=gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GtkWidget* label=gtk_label_new(msg);
    gtk_container_add(GTK_CONTAINER(contentArea),label);
    gtk_widget_show_all(dialog);
    int res=gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
    return res;
}