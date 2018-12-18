#include <gtk/gtk.h>
#include <SDL/SDL.h>

void  set_pixel_value(GdkPixbuf *pixbuf, int x, int y, int color)
{
     guchar *pixel;
     guchar *p;
     gint channel;
     gint rowstride;

     if(!pixbuf)   return ;

     channel = gdk_pixbuf_get_n_channels(pixbuf);
     rowstride = gdk_pixbuf_get_rowstride(pixbuf);
     pixel = gdk_pixbuf_get_pixels(pixbuf);
     p = pixel+y*rowstride+x*channel;

     p[0] = (color & 0xFF000000) >>24;
     p[1] = (color & 0x00FF0000) >>16;
     p[2] = (color & 0x0000FF00) >>8;
}

// 输出矩形框到pixbuf中
void  PutRect(GdkPixbuf*pixbuf, SDL_Rect rect, gint color)
{
    gint i;
    gint j;
    gint x;
    gint y;
    gint w;
    gint h;

    if (pixbuf == NULL)
        return;
    
    w = gdk_pixbuf_get_width(pixbuf);
    h = gdk_pixbuf_get_height(pixbuf);

    // 画上、下两条线
    for(i = 0; i < 2; ++i)
    {
        if (i == 0) 
            y = rect.y;     // 矩形的上边的y坐标
        else 
            y += rect.h;    // 矩形的下边的y坐标

        x = rect.x;
        for(j = 0; j < rect.w; ++j)
        {
            if ( x+j < w && y < h)
                set_pixel_value(pixbuf, x+j, y, color);
        }
    }

    // 画左、右两条线
    for(i = 0; i < 2; ++i)
    {
        if (i == 0)    
            x = rect.x;
        else   
            x += rect.w;

        y = rect.y;
        for(j = 0; j < rect.h; ++j)
        {
            if (x < w && y+j < h)
                set_pixel_value(pixbuf, x, y+j, color);
        }
    }
}

int main (int argc, char *argv[])
{
    GtkWidget   *win = NULL;
    // GtkWidget   *vbox = NULL;
    GtkWidget   *image;
    GdkPixbuf   *pixbuf;
    SDL_Rect    rect;

    /* 初始化GTK+ */
    gtk_init (&argc, &argv);

    /* 创建主窗口 */
    win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_container_set_border_width (GTK_CONTAINER(win), 8);
    // gtk_widget_set_usize(win, 400, 300);
    gtk_widget_set_size_request(win, 400, 300);
    gtk_window_set_title(GTK_WINDOW(win), "Test draw rectangle to pixbuf");
    gtk_window_set_position(GTK_WINDOW(win), GTK_WIN_POS_CENTER);
    g_signal_connect(win, "destroy", gtk_main_quit, NULL);

    // vbox = gtk_vbox_new(FALSE, 50);
    GtkWidget * vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 50);
    gtk_container_add(GTK_CONTAINER(win), vbox);

    pixbuf = gdk_pixbuf_new_from_file("chrismas.jpg", NULL);

    rect.x = 20;
    rect.y = 20;
    rect.w = 150;
    rect.h = 150;

    // 画一个黄色的矩形到pixbuf中
    PutRect(pixbuf, rect, 0xFFFF0000);
    image = gtk_image_new_from_pixbuf(pixbuf);
    g_object_unref(pixbuf);
    gtk_box_pack_start(GTK_BOX(vbox), image, TRUE, TRUE, 10);

    gtk_widget_show_all (win);

    gtk_main ();
    return 0;
}