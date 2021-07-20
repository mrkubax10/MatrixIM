#include "screens/login_screen.h"
#include "app.h"
#include <stdio.h>
LoginScreen* loginScreen;
LoginScreen* LoginScreen_new(){
    LoginScreen* output=(LoginScreen*)malloc(sizeof(LoginScreen));
    return output;
}

void loginscreen_buttonLogin_clicked(GtkWidget* widget,gpointer userData){
    
}
void loginscreen_init(){
    loginScreen=LoginScreen_new();
    loginScreen->fixedContainer=gtk_fixed_new();
    loginScreen->homeserverContainer=gtk_flow_box_new();
    loginScreen->labelHomeserver=gtk_label_new("Homeserver (IP/Port): ");
    loginScreen->entryHomeserver=gtk_entry_new();
    loginScreen->entryPort=gtk_entry_new();
    loginScreen->userDataContainer=gtk_list_box_new();
    loginScreen->entryUsernameContainer=gtk_flow_box_new();
    loginScreen->entryUsername=gtk_entry_new();
    loginScreen->labelUsername=gtk_label_new("Username: ");
    loginScreen->entryPasswordContainer=gtk_flow_box_new();
    loginScreen->entryPassword=gtk_entry_new();
    loginScreen->labelPassword=gtk_label_new("Password: ");
    loginScreen->loginRegisterContainer=gtk_flow_box_new();
    loginScreen->buttonLogin=gtk_button_new_with_label("Login");
    loginScreen->buttonRegister=gtk_button_new_with_label("Register");

    gtk_flow_box_set_min_children_per_line(GTK_FLOW_BOX(loginScreen->homeserverContainer),3);
    gtk_flow_box_set_min_children_per_line(GTK_FLOW_BOX(loginScreen->entryUsernameContainer),2);
    gtk_flow_box_set_min_children_per_line(GTK_FLOW_BOX(loginScreen->entryPasswordContainer),2);
    gtk_flow_box_set_min_children_per_line(GTK_FLOW_BOX(loginScreen->entryPasswordContainer),2);

    g_signal_connect(loginScreen->buttonLogin,"clicked",G_CALLBACK(loginscreen_buttonLogin_clicked),0);

    gtk_container_add(GTK_CONTAINER(app->window),loginScreen->fixedContainer);
    gtk_fixed_put(GTK_FIXED(loginScreen->fixedContainer),loginScreen->homeserverContainer,2,2);
    gtk_flow_box_insert(GTK_FLOW_BOX(loginScreen->homeserverContainer),loginScreen->labelHomeserver,-1);
    gtk_flow_box_insert(GTK_FLOW_BOX(loginScreen->homeserverContainer),loginScreen->entryHomeserver,-1);
    gtk_flow_box_insert(GTK_FLOW_BOX(loginScreen->homeserverContainer),loginScreen->entryPort,-1);
    gtk_fixed_put(GTK_FIXED(loginScreen->fixedContainer),loginScreen->userDataContainer,2,50);
    gtk_list_box_insert(GTK_LIST_BOX(loginScreen->userDataContainer),loginScreen->entryUsernameContainer,-1);
    gtk_flow_box_insert(GTK_FLOW_BOX(loginScreen->entryUsernameContainer),loginScreen->labelUsername,-1);
    gtk_flow_box_insert(GTK_FLOW_BOX(loginScreen->entryUsernameContainer),loginScreen->entryUsername,-1);
    gtk_list_box_insert(GTK_LIST_BOX(loginScreen->userDataContainer),loginScreen->entryPasswordContainer,-1);
    gtk_flow_box_insert(GTK_FLOW_BOX(loginScreen->entryPasswordContainer),loginScreen->labelPassword,-1);
    gtk_flow_box_insert(GTK_FLOW_BOX(loginScreen->entryPasswordContainer),loginScreen->entryPassword,-1);
    gtk_list_box_insert(GTK_LIST_BOX(loginScreen->userDataContainer),loginScreen->loginRegisterContainer,-1);
    gtk_flow_box_insert(GTK_FLOW_BOX(loginScreen->loginRegisterContainer),loginScreen->buttonLogin,-1);
    gtk_flow_box_insert(GTK_FLOW_BOX(loginScreen->loginRegisterContainer),loginScreen->buttonRegister,-1);
}