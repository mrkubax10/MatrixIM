#include "screens/login_screen.h"
#include "app.h"
#include <stdio.h>
void loginscreen_buttonLogin_clicked(GtkWidget* widget,gpointer userData){
    
}
void loginscreen_selectHomeserver_clicked(GtkWidget* widget,gpointer userData){
    
}
void loginscreen_init(){
    GtkWidget* fixedContainer=gtk_fixed_new();
    GtkWidget* homeserverContainer=gtk_flow_box_new();
    GtkWidget* entryHomeserver=gtk_entry_new();
    GtkWidget* buttonSelectHomeserver=gtk_button_new_with_label("Select");

    gtk_flow_box_set_min_children_per_line(GTK_FLOW_BOX(homeserverContainer),2);

    //g_signal_connect(buttonLogin,"clicked",G_CALLBACK(loginscreen_buttonLogin_clicked),0);

    gtk_container_add(GTK_CONTAINER(app->window),fixedContainer);
    gtk_fixed_put(GTK_FIXED(fixedContainer),homeserverContainer,2,2);
    gtk_flow_box_insert(GTK_FLOW_BOX(homeserverContainer),entryHomeserver,-1);
    gtk_flow_box_insert(GTK_FLOW_BOX(homeserverContainer),buttonSelectHomeserver,-1);
}