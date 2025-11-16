#include <gtk/gtk.h>
#include <pty.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <regex.h>

typedef struct {
    GtkTextBuffer *buffer;
    int master_fd;
} TerminalData;

// ฟังก์ชันลบ ANSI/CSI/OSC escape sequences
void strip_escape_sequences(char *str) {
    regex_t regex;
    // ลบ CSI sequences (\033[...A-Z]) และ OSC sequences (\033]...BEL)
    regcomp(&regex, "\033\\[[0-9;?]*[A-Za-z]|\\033\\].*?\007", REG_EXTENDED);
    regmatch_t match;
    while(regexec(&regex, str, 1, &match, 0) == 0) {
        memmove(str + match.rm_so, str + match.rm_eo, strlen(str + match.rm_eo) + 1);
    }
    regfree(&regex);
}

// อ่าน output จาก bash และแสดงใน GtkTextView
gboolean read_from_pty(GIOChannel *source, GIOCondition condition, gpointer user_data) {
    TerminalData *term = (TerminalData *)user_data;
    char buf[512];
    int n = read(term->master_fd, buf, sizeof(buf)-1);
    if(n > 0) {
        buf[n] = '\0';
        strip_escape_sequences(buf); // ลบ escape sequences
        GtkTextIter end;
        gtk_text_buffer_get_end_iter(term->buffer, &end);
        gtk_text_buffer_insert(term->buffer, &end, buf, -1);
    }
    return TRUE;
}

// ส่ง input จาก GtkEntry ไป bash
void send_input(GtkEntry *entry, gpointer user_data) {
    TerminalData *term = (TerminalData *)user_data;
    const gchar *input = gtk_entry_get_text(entry);
    if(input && strlen(input) > 0) {
        write(term->master_fd, input, strlen(input));
        write(term->master_fd, "\n", 1);
        gtk_entry_set_text(entry, "");
    }
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Mini GTK Terminal");
    gtk_window_set_default_size(GTK_WINDOW(window), 600, 400);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    GtkWidget *text_view = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(text_view), FALSE);
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(text_view), TRUE);
    gtk_box_pack_start(GTK_BOX(vbox), text_view, TRUE, TRUE, 0);

    GtkWidget *entry = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(vbox), entry, FALSE, FALSE, 0);

    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));

    // สร้าง pseudo-terminal และ fork bash แบบ interactive
    int master_fd;
    pid_t pid = forkpty(&master_fd, NULL, NULL, NULL);
    if(pid == 0) {
        execlp("bash", "bash", "--norc", "--noprofile", NULL); // interactive แต่ไม่โหลด config
        perror("execlp failed");
        exit(1);
    }

    TerminalData term;
    term.buffer = buffer;
    term.master_fd = master_fd;

    // ตั้ง master_fd เป็น non-blocking
    int flags = fcntl(master_fd, F_GETFL, 0);
    fcntl(master_fd, F_SETFL, flags | O_NONBLOCK);

    // ใช้ GIOChannel ให้ GTK เรียก callback เมื่อ master_fd มีข้อมูล
    GIOChannel *channel = g_io_channel_unix_new(master_fd);
    g_io_add_watch(channel, G_IO_IN | G_IO_HUP | G_IO_ERR, read_from_pty, &term);

    // ส่ง input เมื่อกด enter
    g_signal_connect(entry, "activate", G_CALLBACK(send_input), &term);

    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}
