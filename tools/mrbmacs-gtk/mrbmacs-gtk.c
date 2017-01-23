#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mruby.h"
#include "mruby/variable.h"
#include "mruby/data.h"

#include <locale.h>
#include <gtk/gtk.h>

#include "mrbmacs-frame.h"

const char init_file_name[] = ".mrbmacsrc";
mrb_state *mrb;

static gboolean
mrbmacs_keypress(GtkWidget *widget, GdkEventKey *event, gpointer data)
{
  mrb_funcall(mrb, *(mrb_value *)data, "key_press", 2, mrb_fixnum_value(event->state),
    mrb_fixnum_value(event->keyval));
  return false;
}

static mrb_value
mrb_mrbmacs_editloop(mrb_state *mrb, mrb_value self)
{
  mrb_value frame_obj;
  mrb_value prefix_key;
  struct mrb_mrbmacs_frame_data *frame;
  fprintf(stderr, "editloop\n");

  prefix_key = mrb_str_new_lit(mrb, "");
  mrb_iv_set(mrb, self, mrb_intern_lit(mrb, "@prefix_key"), prefix_key);
  
  frame_obj = mrb_iv_get(mrb, self, mrb_intern_lit(mrb, "@frame"));
  frame = (struct mrb_mrbmacs_frame_data *)DATA_PTR(frame_obj);
  g_signal_connect(G_OBJECT((GtkWidget *)DATA_PTR(frame->view_win)), "key-press-event", G_CALLBACK(mrbmacs_keypress), &self);
  
  gtk_main();
  return self;
}

char *
get_init_file_path(mrb_state *mrb)
{
  char *path = NULL;
  char *home = getenv("HOME");
  int len, n;
  size_t size;
  
  if (home != NULL) {
    len = snprintf(NULL, 0, "%s/%s", home, init_file_name);
    if (len >= 0) {
      size = len + 1;
      path = (char *)mrb_malloc_simple(mrb, size);
      if (path != NULL) {
        n = snprintf(path, size, "%s/%s", home, init_file_name);
        if (n != len) {
          mrb_free(mrb, path);
          path = NULL;
        }
      }
    }
  }
  return path;
}

int
main(int argc, char **argv)
{
  struct RClass *mrbmacs_class;
  mrb_value mrbmacs;
  char *fname = NULL;
  char *init_path = NULL;
  
  mrb = mrb_open();

  setlocale(LC_CTYPE, "");

  gtk_init(&argc, &argv);
  if (mrb == NULL) {
    fputs("Invalid mrb_state, exiting scimre\n", stderr);
    return EXIT_FAILURE;
  }
  mrbmacs_gtk_init(mrb);
  
  init_path = get_init_file_path(mrb);
  if (argc > 1) {
    fname = argv[1];
  } 
  mrbmacs_class = mrb_class_get_under(mrb, mrb_module_get(mrb, "Mrbmacs"), "Application");
  mrb_define_method(mrb, mrbmacs_class, "editloop", mrb_mrbmacs_editloop, MRB_ARGS_NONE());
  mrbmacs = mrb_funcall(mrb, mrb_obj_value(mrbmacs_class), "new", 1, mrb_str_new_cstr(mrb, init_path));

  if (argc < 2) {
    mrb_funcall(mrb, mrbmacs, "run", 0);
  } else {
    mrb_funcall(mrb, mrbmacs, "run", 1, mrb_str_new_cstr(mrb, fname));
  }
  return 0;
}
