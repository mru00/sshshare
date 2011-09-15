#include <gtk/gtk.h>
#include <iostream>
#include <fstream>

#include "sshshare.hxx"
#include "scp.hxx"
#include "shares.hxx"
#include "scp.hxx"
#include "config.hxx"

using namespace std;


auto_ptr<shares_t> shares_ptr;
GtkListStore* list_store_users, *list_store_shares;
GtkWidget* list_view_users, *list_view_shares;


static void cb_apply (GtkWidget *wid, GtkWidget *win)
{
    GtkTreeSelection *selection;
    GtkTreeModel     *model;
    int index;
    gchar* name;
    GtkTreeIter i1;

    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(list_view_shares));
    if (gtk_tree_selection_get_selected (selection, &model, &i1))
    {
        gtk_tree_model_get (model, &i1, 0, &index, 1, &name, -1);
        share_t& share = shares_ptr->share()[index];

        create_share(name, share.users());
        g_free(name);
    }
}


static void cb_info (GtkWidget *wid, GtkWidget *win)
{
    GtkWidget *dialog = NULL;

    dialog = gtk_message_dialog_new (GTK_WINDOW (win), GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_CLOSE, "Hello World!");
    gtk_window_set_position (GTK_WINDOW (dialog), GTK_WIN_POS_CENTER);
    gtk_dialog_run (GTK_DIALOG (dialog));
    gtk_widget_destroy (dialog);
}

static void populate_shares_list(GtkListStore* store, auto_ptr<shares_t>& shares)
{
    gtk_list_store_clear(store);
    GtkTreeIter iter;
    int i = 0;
    for (shares_t::share_const_iterator it (shares->share ().begin ());
            it != shares->share ().end ();
            ++it, ++i)
    {
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter, 0, i, 1, it->name().c_str(), -1);
    }
}

static void populate_users_list(GtkListStore* store, users_t& users)
{

    gtk_list_store_clear(store);

    GtkTreeIter iter;
    int i  =0;
    for (users_t::user_const_iterator it (users.user().begin ());
            it != users.user().end ();
            ++it, ++i)
    {
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter, 0, i, 1, it->name().c_str(), 2, it->password().c_str(), -1);
    }
}

static void share_selection_changed_cb (GtkTreeSelection *selection, gpointer data)
{
    GtkTreeIter iter;
    GtkTreeModel *model;
    int index;
    gchar* name;

    if (gtk_tree_selection_get_selected (selection, &model, &iter))
    {
        gtk_tree_model_get (model, &iter, 0, &index, 1, &name, -1);

        share_t& share = shares_ptr->share()[index];

        populate_users_list(list_store_users, share.users());


        g_free(name);
    }
}

static void setup_selection(GtkTreeView* view, GCallback cb)
{
    GtkTreeSelection* select = gtk_tree_view_get_selection(view);
    gtk_tree_selection_set_mode(select, GTK_SELECTION_SINGLE);
    g_signal_connect(G_OBJECT(select), "changed", cb, NULL);
}

static GtkWidget* create_list_shares(GtkWidget* win)
{
    GtkWidget *button = NULL;
    GtkWidget *vbox = gtk_vbox_new (FALSE, 2);
    GtkWidget *hbox = gtk_hbutton_box_new();

    button = gtk_button_new_from_stock (GTK_STOCK_ADD);
    g_signal_connect (G_OBJECT (button), "clicked", G_CALLBACK (cb_info), (gpointer) win);
    gtk_box_pack_start (GTK_BOX (hbox), button, FALSE, FALSE, 0);

    button = gtk_button_new_from_stock (GTK_STOCK_REMOVE);
    g_signal_connect (G_OBJECT (button), "clicked", G_CALLBACK (cb_info), (gpointer) win);
    gtk_box_pack_start (GTK_BOX (hbox), button, FALSE, FALSE, 0);


    gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, FALSE, 0);

    GtkListStore* store = gtk_list_store_new(2, G_TYPE_INT, G_TYPE_STRING);
    GtkWidget* list = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
    GtkCellRenderer* renderer = gtk_cell_renderer_text_new();
    GtkTreeViewColumn* column = gtk_tree_view_column_new_with_attributes("Name", renderer, "text", 1, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(list), column);

    list_store_shares = store;
    list_view_shares = list;

    setup_selection(GTK_TREE_VIEW(list), G_CALLBACK(share_selection_changed_cb));

    gtk_box_pack_start (GTK_BOX (vbox), list, TRUE, TRUE, 0);
    return vbox;
}

static void user_cell_edited_callback (GtkCellRendererText *cell,
                                       gchar               *path_string,
                                       gchar               *new_text,
                                       gpointer             user_data)
{
    GtkTreeSelection *selection;
    GtkTreeModel     *model;
    int index;
    GtkTreeIter i1;
    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(list_view_shares));
    if (gtk_tree_selection_get_selected (selection, &model, &i1))
    {
        gtk_tree_model_get (model, &i1, 0, &index, -1);
        share_t& share = shares_ptr->share()[index];


        GtkTreeIter i2;
        selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(list_view_users));
        if (gtk_tree_selection_get_selected (selection, &model, &i2))
        {
            gtk_tree_model_get (model, &i2, 0, &index, -1);

            user_t& user = share.users().user()[index];
            guint column_number = GPOINTER_TO_UINT(g_object_get_data(G_OBJECT(cell), "my_column_num"));
            if (column_number == 0)
                user.name(new_text);
            else if (column_number == 1)
                user.password(new_text);
            populate_users_list(list_store_users, share.users());
        }
    }


}

static GtkWidget* create_users_box(GtkWidget* win)
{
    GtkWidget *button = NULL;
    GtkWidget *vbox = gtk_vbox_new (FALSE, 0);
    GtkWidget *hbox = gtk_hbutton_box_new();

    button = gtk_button_new_from_stock (GTK_STOCK_ADD);
    g_signal_connect (G_OBJECT (button), "clicked", G_CALLBACK (cb_info), (gpointer) win);
    gtk_box_pack_start (GTK_BOX (hbox), button, TRUE, TRUE, 0);

    button = gtk_button_new_from_stock (GTK_STOCK_REMOVE);
    g_signal_connect (G_OBJECT (button), "clicked", G_CALLBACK (cb_info), (gpointer) win);
    gtk_box_pack_start (GTK_BOX (hbox), button, TRUE, TRUE, 0);


    gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, FALSE, 0);

    GtkListStore* store = gtk_list_store_new(3, G_TYPE_INT, G_TYPE_STRING, G_TYPE_STRING);
    GtkWidget* list = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));

    GtkCellRenderer* renderer = gtk_cell_renderer_text_new();
    g_object_set(renderer, "editable", TRUE, NULL);
    g_signal_connect(renderer, "edited", (GCallback) user_cell_edited_callback, NULL);
    g_object_set_data(G_OBJECT(renderer), "my_column_num", GUINT_TO_POINTER(0));


    GtkTreeViewColumn* column = gtk_tree_view_column_new_with_attributes("username", renderer, "text", 1, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(list), column);


    renderer = gtk_cell_renderer_text_new();
    g_object_set(renderer, "editable", TRUE, NULL);
    g_signal_connect(renderer, "edited", (GCallback) user_cell_edited_callback, NULL);
    g_object_set_data(G_OBJECT(renderer), "my_column_num", GUINT_TO_POINTER(1));

    column = gtk_tree_view_column_new_with_attributes("password", renderer, "text", 2, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(list), column);

    list_store_users = store;
    list_view_users = list;

    gtk_box_pack_start (GTK_BOX (vbox), list, TRUE, TRUE, 0);
    return vbox;
}

static GtkWidget* create_win()
{
    GtkWidget *win = NULL;

    /* Create the main window */
    win = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_container_set_border_width (GTK_CONTAINER (win), 8);
    gtk_window_set_title (GTK_WINDOW (win), "ssh share");
    gtk_window_set_position (GTK_WINDOW (win), GTK_WIN_POS_CENTER);
    gtk_widget_realize (win);
    g_signal_connect (win, "destroy", gtk_main_quit, NULL);

    return win;
}
static GtkWidget* create_right_box(GtkWidget* win)
{

    GtkWidget* vbox = gtk_vbox_new(FALSE, 6);

    gtk_box_pack_start (GTK_BOX (vbox), create_users_box(win), TRUE, TRUE, 0);

    GtkWidget* button = gtk_button_new_from_stock(GTK_STOCK_APPLY);
    g_signal_connect (G_OBJECT (button), "clicked", G_CALLBACK (cb_apply), (gpointer) win);
    gtk_box_pack_start(GTK_BOX(vbox), button, TRUE, TRUE, 0);


    return vbox;

}


static GtkWidget* create_ui()
{
    GtkWidget *button = NULL;
    GtkWidget *win = create_win();
    GtkWidget *vbox = NULL;
    GtkWidget* paned = NULL;

    /* Create a vertical box with buttons */
    vbox = gtk_vbox_new (FALSE, 6);
    gtk_container_add (GTK_CONTAINER (win), vbox);

    paned = gtk_hpaned_new();

    gtk_paned_add1(GTK_PANED(paned), create_list_shares(win));
    gtk_paned_add2(GTK_PANED(paned), create_right_box(win));

    gtk_box_pack_start (GTK_BOX (vbox), paned, TRUE, TRUE, 0);



    button = gtk_button_new_from_stock (GTK_STOCK_CLOSE);
    g_signal_connect (button, "clicked", gtk_main_quit, NULL);
    gtk_box_pack_start (GTK_BOX (vbox), button, FALSE, FALSE, 0);

    return win;
}

static void on_progress(int prog)
{
    printf("progress: %d\n", prog);
}

static void on_status_change(int status, const char* description)
{
    printf("status: %d / %s\n", status, description);
    fflush (stdout);
}


int main (int argc, char *argv[])
{

    /* Initialize GTK+ */
    g_log_set_handler ("Gtk", G_LOG_LEVEL_WARNING, (GLogFunc) gtk_false, NULL);
    gtk_init (&argc, &argv);
    g_log_set_handler ("Gtk", G_LOG_LEVEL_WARNING, g_log_default_handler, NULL);



    get_file("asdf", on_progress, on_status_change);

    ScpProcess scp1(Config::makePath("shares/sharedata.xml"), ".");
    scp1.run();

    shares_ptr = shares("sharedata.xml");

    GtkWidget *win = create_ui();
    populate_shares_list(list_store_shares, shares_ptr);


    /* Enter the main loop */
    gtk_widget_show_all (win);
    gtk_main ();


    // Serialize the object model to XML.
    //
    xml_schema::namespace_infomap map;
    map[""].name = "";
    map[""].schema = "sshshare.xsd";

    std::ofstream ofs ("sharedata.xml");
    shares (ofs, *shares_ptr, map);

    ScpProcess("sharedata.xml", Config::makePath("shares/sharedata.xml")).run();

    return 0;
}
