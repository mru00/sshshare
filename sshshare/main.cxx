#include <gtk/gtk.h>
#include <iostream>
#include <fstream>
#include <boost/filesystem.hpp>


#include "sshshare.hxx"
#include "scp.hxx"
#include "shares.hxx"
#include "scp.hxx"
#include "config.hxx"
#include "password.hxx"


using namespace std;
namespace fs = boost::filesystem;


auto_ptr<shares_t> shares_ptr;
GtkListStore* list_store_users, *list_store_shares;
GtkWidget* list_view_users, *list_view_shares, *label_href, *label_sftp;
GtkTextBuffer* text_template_buffer;
GtkWidget* dialog_comm;



static void set_label_href(const string& share)
{
    gtk_label_set_markup (GTK_LABEL(label_href), string("<a href=\"" + Config::makeHttpUrl(share) + "\">"+Config::makeHttpUrl(share)+"</a>").c_str());
    gtk_label_set_markup (GTK_LABEL(label_sftp), string("<a href=\"" + Config::makeSftpUrl(share) + "\">"+Config::makeSftpUrl(share)+"</a>").c_str());
}

struct list_store_set_share {
    typedef shares_t::share_sequence seq_type;
    typedef share_t val_type; int i;
    list_store_set_share() : i(0) { gtk_list_store_clear(list_store_shares); }
    void operator ()(const share_t& share) {
        GtkTreeIter iter;
        gtk_list_store_append(list_store_shares, &iter);
        gtk_list_store_set(list_store_shares, &iter, 0, i++, 1, share.name().c_str(), -1);
    }
};

struct list_store_set_user {
    typedef users_t::user_sequence seq_type;
    typedef user_t val_type; int i;
    list_store_set_user() : i(0) { gtk_list_store_clear(list_store_users); }
    void operator ()(const user_t& user) {
        GtkTreeIter iter;
        gtk_list_store_append(list_store_users, &iter);
        gtk_list_store_set(list_store_users, &iter, 0, i++, 1, user.name().c_str(), 2, user.password().c_str(), -1);
    }
};

template<typename setter> static void pop_list(const typename setter::seq_type& sequence)
{
    for_each(sequence.begin(), sequence.end(), setter());
}

static void update_template_buffer()
{
    string share_name;
    string user_name;
    string password;

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

            share_name = share.name();

            string template_text;
            template_text += Config::makeHttpUrl(share.name()) + "\n";
            template_text += "\n";
            template_text += "username: " + user.name() + "\n";
            template_text += "password: " + user.password() + "\n";

            gtk_text_buffer_set_text(text_template_buffer, template_text.c_str(), -1);
        }
        else
        {
            gtk_text_buffer_set_text(text_template_buffer, "\n\n\n\n", -1);
        }
    }
    else
    {
        gtk_text_buffer_set_text(text_template_buffer, "\n\n\n\n", -1);
    }
}

static void dialog_warn(GtkWidget* win, const string& message)
{
    cerr << "Warning message: " << message << endl;

    GtkWidget* dialog = gtk_message_dialog_new (GTK_WINDOW(win),
                        GTK_DIALOG_MODAL,
                        GTK_MESSAGE_ERROR,
                        GTK_BUTTONS_CLOSE,
                        "%s", message.c_str());
    gtk_dialog_run (GTK_DIALOG (dialog));
    gtk_widget_destroy (dialog);
}

static bool validName(const string& str)
{
    string::const_iterator it = str.begin();

    if (it == str.end()) return false;
    if (!isalpha(*it++)) return false;
    for (; it != str.end(); it ++)
    {
        if (!isalnum(*it) && *it != '_') return false;
    }
    return true;
}

struct compare_name {
    string new_name;
    compare_name(const string& name) : new_name(name) {}
    bool operator ()(const user_t& user) { return user.name() == new_name; }
};

static void cb_add_user(GtkWidget *, GtkWidget *win)
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

        string new_name;
        for (int i = 0; ;i++) {
            stringstream ss;
            ss << "user" << i;
            new_name = ss.str();
            const users_t::user_sequence& user=share.users().user();
            if (find_if(user.begin(), user.end(), compare_name(new_name)) == user.end()) break;
        }

        auto_ptr<user_t> new_user ( new user_t(new_name, Password::generate(8)));
        share.users().user().push_back(new_user);
        pop_list<list_store_set_user>(share.users().user());
        g_free(name);
    }
    else
    {
        dialog_warn(win, "select share first!");
    }

}

static void cb_new_share(GtkWidget *, GtkWidget *win)
{

    GtkWidget *dialog = gtk_dialog_new_with_buttons ("New Share",
                        GTK_WINDOW(win),
                        GTK_DIALOG_MODAL ,
                        GTK_STOCK_OK,
                        GTK_RESPONSE_ACCEPT,
                        GTK_STOCK_CANCEL,
                        GTK_RESPONSE_REJECT,
                        NULL);

    GtkWidget* content_area = gtk_dialog_get_content_area (GTK_DIALOG (dialog));
    GtkWidget* label = gtk_label_new ("Name:");
    GtkWidget* entry = gtk_entry_new();
    GtkWidget* hbox = gtk_hbox_new(FALSE, 6);

    gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
    gtk_box_pack_start (GTK_BOX (hbox), entry, TRUE, FALSE, 0);

    gtk_container_add (GTK_CONTAINER (content_area), hbox);
    gtk_widget_show_all (GTK_WIDGET(dialog));

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT)
    {
        const gchar* name = gtk_entry_get_text(GTK_ENTRY(entry));

        if (!validName(name))
        {
            dialog_warn(win, "Invalid Name");
        }
        else
        {
            auto_ptr<users_t> users ( new users_t ());
            auto_ptr<share_t> share (new share_t(name, users));

            shares_ptr->share().push_back(share);
            pop_list<list_store_set_share>(shares_ptr->share());
        }
    }
    gtk_widget_destroy(dialog);
}

static void cb_apply (GtkWidget *, GtkWidget *win)
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

        if (share.users().user().size() == 0)
        {
            dialog_warn(win, "No users specified; cannot continue");
        }
        else
        {
            try
            {
                create_share(name, share.users().user());
            }
            catch (ProcessException& e)
            {
                dialog_warn(win,e.what());
            }
        }
        g_free(name);
    }
    else
    {
        dialog_warn(win,"select share first");
    }
}


static void cb_info (GtkWidget *, GtkWidget *win)
{
    GtkWidget *dialog = NULL;

    dialog = gtk_message_dialog_new (GTK_WINDOW (win), GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_CLOSE, "Hello World!");
    gtk_window_set_position (GTK_WINDOW (dialog), GTK_WIN_POS_CENTER);
    gtk_dialog_run (GTK_DIALOG (dialog));
    gtk_widget_destroy (dialog);
}



static void share_selection_changed_cb (GtkTreeSelection *selection, gpointer )
{
    GtkTreeIter iter;
    GtkTreeModel *model;
    int index;
    gchar* name;

    if (gtk_tree_selection_get_selected (selection, &model, &iter))
    {
        gtk_tree_model_get (model, &iter, 0, &index, 1, &name, -1);

        if (validName(name))
        {
            share_t& share = shares_ptr->share()[index];
            pop_list<list_store_set_user>(share.users().user());
            set_label_href(name);
        }

        update_template_buffer();

        g_free(name);
    }
}

static void user_selection_changed_cb (GtkTreeSelection *selection, gpointer )
{
    GtkTreeIter iter;
    GtkTreeModel *model;

    if (gtk_tree_selection_get_selected (selection, &model, &iter))
    {
        update_template_buffer();
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
    g_signal_connect (G_OBJECT (button), "clicked", G_CALLBACK (cb_new_share), (gpointer) win);
    gtk_box_pack_start (GTK_BOX (hbox), button, FALSE, FALSE, 0);

    button = gtk_button_new_from_stock (GTK_STOCK_REMOVE);
    g_signal_connect (G_OBJECT (button), "clicked", G_CALLBACK (cb_info), (gpointer) win);
    gtk_box_pack_start (GTK_BOX (hbox), button, FALSE, FALSE, 0);


    gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, FALSE, 0);

    GtkWidget* scrollwindow = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrollwindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);


    GtkListStore* store = gtk_list_store_new(2, G_TYPE_INT, G_TYPE_STRING);
    GtkWidget* list = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
    GtkCellRenderer* renderer = gtk_cell_renderer_text_new();
    GtkTreeViewColumn* column = gtk_tree_view_column_new_with_attributes("Name", renderer, "text", 1, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(list), column);

    list_store_shares = store;
    list_view_shares = list;

    setup_selection(GTK_TREE_VIEW(list), G_CALLBACK(share_selection_changed_cb));

    gtk_container_add(GTK_CONTAINER(scrollwindow), list);
    gtk_box_pack_start (GTK_BOX (vbox), scrollwindow, TRUE, TRUE, 0);
    return vbox;
}

static void user_cell_edited_callback (GtkCellRendererText *cell,
                                       gchar               *,
                                       gchar               *new_text,
                                       gpointer             )
{
    if (!validName(new_text)) return;
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
            pop_list<list_store_set_user>(share.users().user());
        }
    }
}

static GtkWidget* create_users_box(GtkWidget* win)
{
    GtkWidget *button = NULL;
    GtkWidget *vbox = gtk_vbox_new (FALSE, 0);
    GtkWidget *hbox = gtk_hbutton_box_new();

    button = gtk_button_new_from_stock (GTK_STOCK_ADD);
    g_signal_connect (G_OBJECT (button), "clicked", G_CALLBACK (cb_add_user), (gpointer) win);
    gtk_box_pack_start (GTK_BOX (hbox), button, TRUE, TRUE, 0);

    button = gtk_button_new_from_stock (GTK_STOCK_REMOVE);
    g_signal_connect (G_OBJECT (button), "clicked", G_CALLBACK (cb_info), (gpointer) win);
    gtk_box_pack_start (GTK_BOX (hbox), button, TRUE, TRUE, 0);


    gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, FALSE, 0);

    GtkListStore* store = gtk_list_store_new(3, G_TYPE_INT, G_TYPE_STRING, G_TYPE_STRING);
    GtkWidget* list = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));


    GtkWidget* scrollwindow = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrollwindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);

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

    setup_selection(GTK_TREE_VIEW(list), G_CALLBACK(user_selection_changed_cb));

    list_store_users = store;
    list_view_users = list;

    gtk_container_add(GTK_CONTAINER(scrollwindow), list);
    gtk_box_pack_start (GTK_BOX (vbox), scrollwindow, TRUE, TRUE, 0);
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
    gtk_box_pack_start(GTK_BOX(vbox), button, FALSE, TRUE, 0);

    label_href = gtk_label_new("");
    label_sftp = gtk_label_new("");

    gtk_box_pack_start(GTK_BOX(vbox), label_href, FALSE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), label_sftp, FALSE, TRUE, 0);
    text_template_buffer = gtk_text_buffer_new(NULL);
    gtk_text_buffer_set_text(GTK_TEXT_BUFFER(text_template_buffer), "\n\n\n\n", -1);

    GtkWidget* text_template = gtk_text_view_new_with_buffer(text_template_buffer);
    gtk_text_view_set_editable(GTK_TEXT_VIEW(text_template), FALSE);
    gtk_box_pack_start(GTK_BOX(vbox), text_template, FALSE, TRUE, 0);


    set_label_href("");
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

static void create_dialog_comm()
{
    dialog_comm = gtk_message_dialog_new(NULL, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_INFO,GTK_BUTTONS_NONE,
                                         "%s", "Communicating with server");
}

int main (int argc, char *argv[])
{

    /* Initialize GTK+ */
    g_log_set_handler ("Gtk", G_LOG_LEVEL_WARNING, (GLogFunc) gtk_false, NULL);
    gtk_init (&argc, &argv);
    g_log_set_handler ("Gtk", G_LOG_LEVEL_WARNING, g_log_default_handler, NULL);

    create_dialog_comm();

    try
    {
        gtk_widget_show_all(dialog_comm);
        ScpProcess(Config::makePath("shares/" + Config::xmlfilename), ".").run();
        gtk_widget_hide(dialog_comm);

        if (fs::exists(fs::status(Config::xmlfilename)))
        {
            try
            {
                shares_ptr = shares(Config::xmlfilename);
            }
            catch (xml_schema::parsing ex)
            {
                stringstream str;
                str << "Failed to parse file: " << endl;
                str << ex << endl << endl;
                str << "This is a terminal error." << endl;

                cerr << str.str() << endl;

                dialog_warn(NULL, str.str());

                exit(EXIT_FAILURE);
            }
        }
        else
        {
            dialog_warn(NULL, "Failed to load file, starting a fresh one.");
            shares_ptr.reset(new shares_t());
        }
    }
    catch (ProcessException& e)
    {
        cerr << "Exception: " << e.what() << endl;
        dialog_warn(NULL, "Failed to communicate with server. Please check console output. Terminating.");
        gtk_widget_hide(dialog_comm);
        exit(1);
    }


    GtkWidget *win = create_ui();
    pop_list<list_store_set_share>(shares_ptr->share());


    /* Enter the main loop */
    gtk_widget_show_all (win);
    gtk_main ();

    // Serialize the object model to XML.
    //
    xml_schema::namespace_infomap map;
    map[""].name = "";
    map[""].schema = Config::getDataNamespace();

    std::ofstream ofs (Config::xmlfilename.c_str());
    shares (ofs, *shares_ptr, map);

    try
    {
        gtk_widget_show_all(dialog_comm);
        ScpProcess("sharedata.xml", Config::makePath("shares/sharedata.xml")).run();
    }
    catch (ProcessException& e)
    {
        cerr << "Exception: " << e.what() << endl;
        dialog_warn(NULL, "Failed to communicate with server. Please check console output.");
    }
    gtk_widget_hide(dialog_comm);

    return 0;
}


