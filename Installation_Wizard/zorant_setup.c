#include <gtk/gtk.h>
#include <glib.h>
#include <stdio.h>
#include <string.h>  // For strcmp
#include <unistd.h>
#include <stdlib.h>

void on_next_button_clicked(GtkWidget *widget, gpointer data);
void on_back_button_clicked(GtkWidget *widget, gpointer data);

GtkWidget* create_intro_page(GtkStack *stack) {
    GtkWidget *page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    GtkWidget *label = gtk_label_new("Welcome to Zora NT Installation Wizard!\nClick Next to begin.");
    GtkWidget *next_button = gtk_button_new_with_label("Next");

    g_signal_connect(next_button, "clicked", G_CALLBACK(on_next_button_clicked), stack);

    gtk_box_pack_start(GTK_BOX(page), label, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(page), next_button, FALSE, FALSE, 0);

    gtk_widget_set_name(page, "intro_page");  // Set the name for easy comparison
    return page;
}

// Declare callback function
void on_accept_checkbox_toggled(GtkToggleButton *checkbox, GtkWidget *next_button);

// Define other functions, including create_license_page
GtkWidget* create_license_page(GtkStack *stack) {
    GtkWidget *page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    GtkWidget *label = gtk_label_new("Please read the following license agreement:");
    
    GtkWidget *text_view = gtk_text_view_new();
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
    const char *license_text = 
    "========================================\n"
    "               ZoraNT License Agreement\n"
    "                   Version 1.1\n"
    "========================================\n\n"
    "1. License Grant\n"
    "-----------------\n"
    "You are granted the following rights under the Meisei Open-Source License:\n\n"
    "- Modification: You may modify the source code of ZoraNT.\n"
    "- Distribution: You may distribute the source code or modified versions of ZoraNT, provided that the license terms are met.\n"
    "- Private Use: You may use ZoraNT and any modifications made to it for personal, non-commercial purposes.\n"
    "- Commercial Use: You may use ZoraNT or modified versions for commercial purposes.\n"
    "- Patent Use: You may use, distribute, or modify the source code without concern of patent claims related to the code or derived works.\n\n"
    "2. Conditions\n"
    "-------------\n"
    "To exercise the above rights, you must comply with the following conditions:\n\n"
    "- Source Code Disclosure: When distributing ZoraNT or modified versions, you must provide access to the source code.\n"
    "- License and Copyright Notice: You must include the original license and copyright notice in all copies or substantial portions of ZoraNT.\n"
    "- Network Use is Distribution: If ZoraNT is used in a network-based environment, you must provide users with access to the source code.\n"
    "- Same License: Any modifications you make to ZoraNT must be licensed under this license or a compatible open-source license.\n"
    "- State Changes: You must document any changes made to the original code, making clear what modifications have been implemented.\n\n"
    "3. Additional Terms\n"
    "-------------------\n"
    "- No Patents on Code: You may not file patents on the code or any derived works. The copyright and license notice must not be removed from the program.\n"
    "- No Trademark Use: You may not use the name 'ZoraNT' or the name of Meisei Technologies in advertising or promoting derived works without explicit written permission.\n\n"
    "4. Enforcement\n"
    "--------------\n"
    "Failure to comply with the terms of this license may result in legal action. Meisei Technologies and Tomoko Saito reserve the right to enforce the license and seek remedies, including compensation or recall of the software.\n\n"
    "5. Disclaimer\n"
    "-------------\n"
    "The software is provided 'as is,' without any warranties of any kind. Meisei Technologies and Tomoko Saito are not responsible for any claims, damages, or other liabilities arising from the use of ZoraNT.\n\n"
    "6. Contact Information\n"
    "----------------------\n"
    "For any questions or additional information about the license or software, please contact:\n"
    "  Tomoko Saito\n"
    "  Meisei Technologies\n"
    "  Contact Information Link: https://miyamii.carrd.co/\n\n"
    "Signed:\n"
    "  Tomoko Saito\n"
    "  Meisei Technologies\n"
    "  Date: 2024/11/18\n"
    "========================================";
    gtk_text_buffer_set_text(buffer, license_text, -1);
    gtk_text_view_set_editable(GTK_TEXT_VIEW(text_view), FALSE);
    
    GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(scrolled_window), text_view);

    GtkWidget *accept_checkbox = gtk_check_button_new_with_label("I accept the terms of the license agreement.");
    GtkWidget *next_button = gtk_button_new_with_label("Next");
    GtkWidget *back_button = gtk_button_new_with_label("Back");

    gtk_widget_set_sensitive(next_button, FALSE);
    g_signal_connect(accept_checkbox, "toggled", G_CALLBACK(on_accept_checkbox_toggled), next_button);

    g_signal_connect(next_button, "clicked", G_CALLBACK(on_next_button_clicked), stack);
    g_signal_connect(back_button, "clicked", G_CALLBACK(on_back_button_clicked), stack);

    gtk_box_pack_start(GTK_BOX(page), label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(page), scrolled_window, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(page), accept_checkbox, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(page), back_button, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(page), next_button, FALSE, FALSE, 0);

    return page;
}

// Declare the global progress bar
GtkWidget *progress_bar;

// Forward declaration of the function
GtkWidget* create_installation_page(GtkStack *stack, const char *install_dir, const char *repo_url, GtkWidget *progress_bar);

// Define the missing callback function for checkbox toggling
void on_accept_checkbox_toggled(GtkToggleButton *checkbox, GtkWidget *next_button) {
    gboolean is_checked = gtk_toggle_button_get_active(checkbox);
    gtk_widget_set_sensitive(next_button, is_checked);
}

// Handle the "Next" button click in the directory selection page
void on_next_directory_button_clicked(GtkButton *button, GtkStack *stack) {
    GtkWidget *page = gtk_stack_get_visible_child(stack);
    GtkWidget *file_chooser = GTK_WIDGET(g_object_get_data(G_OBJECT(page), "file_chooser"));

    const char *selected_directory = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(file_chooser));
    
    if (selected_directory != NULL) {
        g_print("Selected directory: %s\n", selected_directory);

        // Store the selected directory in the stack's data
        g_object_set_data(G_OBJECT(stack), "selected_directory", g_strdup(selected_directory));

        // Create the installation page and pass the selected directory
        GtkWidget *installation_page = create_installation_page(stack, selected_directory, "https://github.com/Plutomaster28/ZoraNT-Initial-Setup-Service", progress_bar);

        // Add the installation page to the stack
        gtk_stack_add_titled(GTK_STACK(stack), installation_page, "installation_page", "Installation");

        // Transition to the installation page
        gtk_stack_set_visible_child(GTK_STACK(stack), installation_page);
    } else {
        g_print("No directory selected\n");
    }
}

// Create the directory selection page
GtkWidget* create_directory_page(GtkStack *stack) {
    GtkWidget *page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    GtkWidget *label = gtk_label_new("Select the directory to install Zora NT:");
    GtkWidget *file_chooser = gtk_file_chooser_button_new("Select Folder", GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER);
    GtkWidget *next_button = gtk_button_new_with_label("Next");
    GtkWidget *back_button = gtk_button_new_with_label("Back");

    // Set default directory
    gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(file_chooser), g_get_home_dir());

    g_signal_connect(next_button, "clicked", G_CALLBACK(on_next_directory_button_clicked), stack);
    g_signal_connect(back_button, "clicked", G_CALLBACK(on_back_button_clicked), stack);

    // Save file chooser to retrieve later
    g_object_set_data(G_OBJECT(page), "file_chooser", file_chooser);

    gtk_box_pack_start(GTK_BOX(page), label, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(page), file_chooser, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(page), back_button, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(page), next_button, FALSE, FALSE, 0);

    gtk_widget_set_name(page, "directory_page");
    return page;
}

// Update the progress bar with the current progress
static void update_progress(GtkWidget *progress_bar, double fraction) {
    gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progress_bar), fraction);
    gtk_progress_bar_pulse(GTK_PROGRESS_BAR(progress_bar));  // Optional: Use pulse to show activity
}

// Function to run a system command and update progress
int run_command(const char *command) {
    FILE *fp = popen(command, "r");
    if (!fp) {
        perror("popen failed");
        return 0; // Command failed
    }

    char line[256];
    while (fgets(line, sizeof(line), fp) != NULL) {
        printf("%s", line);
    }

    fclose(fp);
    return 1; // Command succeeded
}

// Function to ping the repository (checking availability)
int ping_repository(const char *repo_url) {
    char command[512];
    snprintf(command, sizeof(command), "ping -c 1 %s", repo_url);

    if (run_command(command)) {
        printf("Repository is reachable.\n");
        return 1;
    } else {
        printf("Repository is not reachable.\n");
        return 0;
    }
}

// Function to download/clone the repository
int clone_repository(const char *repo_url, const char *target_dir) {
    char command[512];
    snprintf(command, sizeof(command), "git clone %s %s", repo_url, target_dir);

    if (run_command(command)) {
        printf("Cloning successful.\n");
        return 1;
    } else {
        printf("Failed to clone the repository.\n");
        return 0;
    }
}

// Function to handle zip files (if the repository is a ZIP file)
int unzip_repository(const char *zip_path, const char *target_dir) {
    char command[512];
    snprintf(command, sizeof(command), "unzip -q %s -d %s", zip_path, target_dir);

    if (run_command(command)) {
        printf("Unzipping successful.\n");
        return 1;
    } else {
        printf("Failed to unzip the repository.\n");
        return 0;
    }
}

// Function to delete a file (in this case, the zip file)
void delete_file(const char *file_path) {
    char command[512];
    snprintf(command, sizeof(command), "rm %s", file_path);
    run_command(command);
    printf("Deleted file: %s\n", file_path);
}

// Function to execute the installation with progress bar updates
void perform_installation(GtkWidget *progress_bar, const char *repo_url, const char *install_dir) {
    // Step 1: Ping repository
    if (!ping_repository(repo_url)) {
        fprintf(stderr, "Error: Repository not reachable.\n");
        return;
    }
    update_progress(progress_bar, 0.2);  // Update progress bar after pinging

    // Step 2: Clone the repository
    if (!clone_repository(repo_url, install_dir)) {
        fprintf(stderr, "Error: Cloning the repository failed.\n");
        return;
    }
    update_progress(progress_bar, 0.5);  // Update progress bar after cloning

    // Step 3: Check if the directory is compressed (let's assume it is a .zip)
    char zip_path[512];
    snprintf(zip_path, sizeof(zip_path), "%s.zip", install_dir);
    if (access(zip_path, F_OK) == 0) { // If ZIP file exists
        if (!unzip_repository(zip_path, install_dir)) {
            fprintf(stderr, "Error: Unzipping the repository failed.\n");
            return;
        }

        // Step 4: Delete the ZIP file after unzipping
        delete_file(zip_path);
    }
    update_progress(progress_bar, 0.8);  // Update progress bar after unzipping

    // Final Step: Installation complete
    update_progress(progress_bar, 1.0);  // Set the progress to 100%
    printf("Installation completed successfully.\n");
}

// Update the installation page function to use the global progress bar
GtkWidget* create_installation_page(GtkStack *stack, const char *install_dir, const char *repo_url, GtkWidget *progress_bar) {
    GtkWidget *page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    GtkWidget *label = gtk_label_new("Installation in progress...\nPlease wait while Zora NT installs.");

    // Don't create a new progress bar; use the global progress bar
    gtk_box_pack_start(GTK_BOX(page), label, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(page), progress_bar, FALSE, FALSE, 0);

    // Perform installation with the existing progress bar
    perform_installation(progress_bar, repo_url, install_dir);

    return page;
}

GtkWidget* create_complete_page(GtkStack *stack) {
    GtkWidget *page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    GtkWidget *label = gtk_label_new("Installation Complete! Click Finish to exit.");
    GtkWidget *finish_button = gtk_button_new_with_label("Finish");

    g_signal_connect(finish_button, "clicked", G_CALLBACK(gtk_main_quit), NULL);

    gtk_box_pack_start(GTK_BOX(page), label, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(page), finish_button, FALSE, FALSE, 0);

    gtk_widget_set_name(page, "complete_page");
    return page;
}

void on_next_button_clicked(GtkWidget *widget, gpointer data) {
    GtkStack *stack = GTK_STACK(data);
    const char *current_name = gtk_stack_get_visible_child_name(stack);

    g_print("Next button clicked, current page: %s\n", current_name);

    if (strcmp(current_name, "intro_page") == 0) {
        gtk_stack_set_visible_child_name(stack, "license_page");
    } else if (strcmp(current_name, "license_page") == 0) {
        gtk_stack_set_visible_child_name(stack, "directory_page");
    } else if (strcmp(current_name, "directory_page") == 0) {
        gtk_stack_set_visible_child_name(stack, "installation_page");  // Go to installation page
    } else if (strcmp(current_name, "installation_page") == 0) {
        gtk_stack_set_visible_child_name(stack, "complete_page");
    }
}

void on_back_button_clicked(GtkWidget *widget, gpointer data) {
    GtkStack *stack = GTK_STACK(data);
    const char *current_name = gtk_stack_get_visible_child_name(stack);

    g_print("Back button clicked, current page: %s\n", current_name);

    if (strcmp(current_name, "license_page") == 0) {
        gtk_stack_set_visible_child_name(stack, "intro_page");
    } else if (strcmp(current_name, "directory_page") == 0) {
        gtk_stack_set_visible_child_name(stack, "license_page");
    }
}

// Function to get the selected directory (or default to Downloads)
const char* get_selected_directory(GtkStack *stack) {
    const char *selected_directory = g_object_get_data(G_OBJECT(stack), "selected_directory");

    // If no directory is selected, use the default Downloads directory
    if (selected_directory == NULL) {
        const gchar *home_dir = g_get_home_dir();  // Get the user's home directory (const gchar*)
        gchar *downloads_dir = g_build_filename(home_dir, "Downloads", NULL);  // Build path to Downloads folder

        // Debugging: Print the default directory
        g_print("No directory selected, using default Downloads directory: %s\n", downloads_dir);
        
        return downloads_dir;  // Return the default Downloads directory
    }

    // Debugging: Print the selected directory
    g_print("Selected directory: %s\n", selected_directory);

    return selected_directory;
}

// Modify main function
int main(int argc, char *argv[]) {
    GtkWidget *window;
    GtkWidget *stack;
    GtkWidget *stack_switcher;
    GtkWidget *progress_bar;
    const char *repo_url = "https://github.com/Plutomaster28/ZoraNT-Initial-Setup-Service"; // Default or user-defined

    // Initialize GTK
    gtk_init(&argc, &argv);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Zora NT Installation Wizard");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    stack = gtk_stack_new();
    gtk_stack_set_transition_type(GTK_STACK(stack), GTK_STACK_TRANSITION_TYPE_SLIDE_UP);
    gtk_stack_set_transition_duration(GTK_STACK(stack), 500);

    // Create all pages
    GtkWidget *intro_page = create_intro_page(GTK_STACK(stack));
    GtkWidget *license_page = create_license_page(GTK_STACK(stack));
    GtkWidget *directory_page = create_directory_page(GTK_STACK(stack));

    // Get the selected directory (or default to Downloads)
    const char *install_dir = get_selected_directory(GTK_STACK(stack));

    // If no directory is selected, handle the case
    if (install_dir == NULL) {
        // Handle case where the user didn't select a directory or canceled
        g_print("No installation directory selected. Exiting...\n");
        gtk_main_quit();
        return 1;
    }

    // Initialize progress bar
    progress_bar = gtk_progress_bar_new();  

    // Create installation page
    GtkWidget *installation_page = create_installation_page(GTK_STACK(stack), install_dir, repo_url, progress_bar);
    GtkWidget *complete_page = create_complete_page(GTK_STACK(stack));

    // Add pages to the stack
    gtk_stack_add_titled(GTK_STACK(stack), intro_page, "intro_page", "Intro");
    gtk_stack_add_titled(GTK_STACK(stack), license_page, "license_page", "License");
    gtk_stack_add_titled(GTK_STACK(stack), directory_page, "directory_page", "Directory");
    gtk_stack_add_titled(GTK_STACK(stack), installation_page, "installation_page", "Installation");
    gtk_stack_add_titled(GTK_STACK(stack), complete_page, "complete_page", "Complete");

    // Create stack switcher to navigate between pages (optional)
    stack_switcher = gtk_stack_switcher_new();
    gtk_stack_switcher_set_stack(GTK_STACK_SWITCHER(stack_switcher), GTK_STACK(stack));

    GtkWidget *main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_box_pack_start(GTK_BOX(main_box), stack_switcher, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(main_box), stack, TRUE, TRUE, 0);

    gtk_container_add(GTK_CONTAINER(window), main_box);
    gtk_widget_show_all(window);

    gtk_main();
    return 0;
}