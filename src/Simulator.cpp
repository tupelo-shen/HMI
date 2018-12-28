/*
 * @Module Name：    Simulator.cpp 
 * @Function：
 * @Author:         Tupelo Shen
 * @Create Date:    2018-02-27
 */
#include <assert.h>
#include <stdlib.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <vector>
#include <cairo.h>
#include "Simulator.h"
#include "GDC_Driver.h"

using namespace std;
using namespace boost;
using namespace boost::property_tree;

Options *Options::obj = 0;
// ------------------<< local class ConstCString>>-----
class ConstCString
{
    private:
        const char * str;
        ConstCString(void)
        {
            str = 0;
        }

    public:
        ConstCString(ConstCString & obj)
        {
            this->str = obj.str;
        }
        ConstCString(const char * str)
        {
            this->str = str;
        }
        ~ConstCString(void)
        {
            this->str = 0;
        }
        inline int strcmp(const char * str) const;
        inline bool operator == (const char * str) const;
        inline signed long select(const char * list[], signed long size) const;
};

/*
 *          inline函数： 比较两个C字符串
 * @params
 *          str：指向要比较const字符串的指针
 * @return
 *          0，相等；1，比要比较的字符串小；2，比要比较的字符串大
 */
inline int ConstCString::strcmp(const char * str) const
{
    const char * base = this->str;
    while((*base == *str) && (*base != '\0'))
    {
        base ++;
        str ++;
    }
    if(*base < *str) return 1;
    if(*base > *str) return -1;
    return 0;
}

/*
 *      inline函数： 判断2个C字符串是否相等
 *
 * @params
 *          str：指向要比较const字符串的指针
 * @return
 *          true，相等；
 *          false，不相等；
 */
inline bool ConstCString::operator == (const char * str) const
{
    if(0 == strcmp(str))
    {
        return true;
    }
    return false;
}

/*
 *      inline函数： 从字符串列表中，选出一个与字符串相等的项
 *
 * @params
 *          list[]， 字符串列表
 *          size，   字符串列表的大小
 * @return
 *          成功返回列表中对应项的索引idx；
 *          不成功，返回-1；
 */
inline signed long ConstCString::select(const char * list[], signed long size) const
{
    for(signed long idx = 0; idx < size; idx ++)
    {
        if(*this == list[idx])
        {
            return idx;
        }
    }
    return -1;
}
//-------------------<< class Options >>-------------------------
/*
 *      构造函数
 * @params
 * @return
 */
Options::Options(const char *f_name)
{
    // 初始化文件名称
    Options::iniFileName = f_name;

    // 载入初始化文件
    ptree pt;
    read_ini(f_name, pt);
    for(unsigned char idx = 0; idx < 5; idx++)
    {
        static const char *keys[5] = { "COM.Sensor1", "COM.Sensor2", "COM.Hart", "COM.RS485", "COM.Bluetooth" };
        if(boost::optional<string> str = pt.get_optional<string>(keys[idx]))
        {
            uart[idx] = str.get();
        }
    }
    if(boost::optional<string> str = pt.get_optional<string>("Flash.Program"))
    {
        progtamFlash = str.get();
    }
    if(boost::optional<string> str = pt.get_optional<string>("Flash.UserData"))
    {
      dataFlash = str.get();
    }
    logFileName = "";
    if(boost::optional<string> str = pt.get_optional<string>("Log.LogFile"))
    {
        logFileName = str.get();
    }
    if(boost::optional<string> str = pt.get_optional<string>("Log.DisplayLog"))
    {
        logSwitch = str.get();
    }
    if(boost::optional<string> str = pt.get_optional<string>("Log.DisplayImageFolder"))
    {
        logFolder = str.get();
    }
    if(boost::optional<string> str = pt.get_optional<string>("Sensor.ch1"))
    {
        sensor_ch[0] = str.get();
    }
    if(boost::optional<string> str = pt.get_optional<string>("Sensor.ch2"))
    {
        sensor_ch[1] = str.get();
    }
    obj = this;
}

/*
 *      析构函数
 * @params
 * @return
 */
Options::~Options(void)
{
    obj = 0;
}

/*
 *      得到全局对象
 * @params
 * @return Simulator
 */
Options & Options::refObject(void)
{
    assert(0 != obj);
    return *obj;
}

//-------------------<< class Simulator >>-------------------------
Simulator* Simulator::obj = 0;

/*
 *      构造函数
 * @params
 * @return
 */
Simulator::Simulator(void) 
{
    obj = this;
    fullScreenMode = false;
}
/*
 *      构造函数-带参数
 * @params
 *          Simulator
 * @return
 */
Simulator::Simulator(Simulator& obj) : fullScreenMode(obj.fullScreenMode)
{
}
/*
 *      析构函数
 * @params
 * @return
 */
Simulator::~Simulator(void)
{
    obj = 0;
}

/*
 *      得到全局对象
 * @params
 * @return Simulator
 */
Simulator& Simulator::refObject(void)
{
    assert(0 != obj);
    return *obj;
}
// ----------------<< 本文件的局部函数 >>--------------------------------
typedef struct
{
    GtkWidget*      widget;
    const char*     label;
} RadioLinkWidget;

/*
 *      回调函数 ： 描述事件的回调函数
 *
 * @params
 *          widget， 描画事件的窗口句柄
 *          cr，
 *          data，        
 * @return
 *          成功，返回FALSE；
 */
static gboolean draw_callback(GtkWidget* widget, cairo_t* cr, gpointer data)
{
    Simulator& app = *(Simulator *)data;

    if(app.getBmp()->get24BitRawData2() != NULL)
    {
        GdkPixbuf* pixbuf = 0;
        pixbuf = gdk_pixbuf_new_from_data(app.getBmp()->get24BitRawData2(),         /* 8位/样本压缩格式的图像数据 */
                                GDK_COLORSPACE_RGB,                                 /* 用于图像数据的色彩空间 */
                                FALSE,                                              /* 数据是否具有不透明度通道 */
                                8,                                                  /* RGB分量使用8位表示 */
                                app.getBmp()->width()*2, app.getBmp()->height()*2,  /* 图像的宽度和高度，必须大于0 */
                                3 * app.getBmp()->width()*2,                        /* Number of bytes between lines (ie stride) */  
                                NULL, NULL);                                        /* 回调函数 */
        if(0 != pixbuf)
        {
            gdk_cairo_set_source_pixbuf(cr, pixbuf, 0, 0);
            cairo_paint(cr);
        }
    }

    return FALSE;
}

// modbus数据结构
typedef struct
{
    unsigned short addr;
    unsigned short data;
    const char * lavel;
    const char * value;
} ModbusData;
static void create_model(GtkWidget * treeview)
{
    GtkListStore * store = gtk_list_store_new(4, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
    gtk_tree_view_set_model(GTK_TREE_VIEW(treeview), GTK_TREE_MODEL(store));
    g_object_unref(store);
}

static void set_data(GtkWidget * treeview, vector< ModbusData > modbus)
{
    GtkListStore * store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(treeview)));
    gtk_list_store_clear(store);
    GtkTreeIter iter;
    for(unsigned long idx = 0; idx < modbus.size() ; idx ++)
    {
        char addr[5];
        char data[6];
        sprintf(addr, "%04x",  modbus[idx].addr);
        sprintf(data, "%04xh", modbus[idx].data);
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter, 0, addr, 1, data, 2, modbus[idx].lavel, 3, modbus[idx].value, -1);
    }
}

static void append_column_to_treeview(GtkWidget * treeview, const char * title, const int order)
{
    GtkCellRenderer * renderer = gtk_cell_renderer_text_new();
    GtkTreeViewColumn * column = gtk_tree_view_column_new_with_attributes(title, renderer, "text",  order, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
}

static void create_view(GtkWidget *treeview)
{
    append_column_to_treeview(treeview, "Address", 0);
    append_column_to_treeview(treeview, "Data", 1);
    append_column_to_treeview(treeview, "Label", 2);
    append_column_to_treeview(treeview, "value", 3);
}
//-----------------<< 创建主窗口 >>--------------------------------------
static GtkWidget * createModbusInput(void)
{
    static const char * label[] = { "Coil", "Input", "Hold", "File", "Ident"  };
    GtkWidget * note = gtk_notebook_new();
    for(unsigned long idx = 0; idx < (sizeof(label) / sizeof(const char *)); idx ++)
    {
        GtkWidget * treeView = gtk_tree_view_new();
        static const ModbusData db_init[] =
        {
            { 0x0000, 0x0000, "pH",   "7.00 [pH]" },
            { 0x0001, 0x000a, "",     ""          },
            { 0x0002, 0x000b, "ORP",  "-1 [mv]"   },
            { 0x0003, 0x000c, "",     ""          },
            { 0x0004, 0x000d, "TEMP", "25 [deg]"  },
            { 0x0005, 0xffff, "",     ""          },
            { 0x0006, 0x0000, "pH",   "7.00 [pH]" },
            { 0x0007, 0x000a, "",     ""          },
            { 0x0008, 0x000b, "ORP",  "-1 [mv]"   },
            { 0x0009, 0x000c, "",     ""          },
            { 0x000A, 0x000d, "TEMP", "25 [deg]"  },
            { 0x000B, 0xffff, "",     ""          },
            { 0x000C, 0x0000, "pH",   "7.00 [pH]" },
            { 0x000D, 0x000a, "",     ""          },
            { 0x000E, 0x000b, "ORP",  "-1 [mv]"   },
            { 0x000F, 0x000c, "",     ""          },
            { 0x0010, 0x000d, "TEMP", "25 [deg]"  },
            { 0x0011, 0xffff, "",     ""          },
            { 0x0012, 0x0000, "pH",   "7.00 [pH]" },
            { 0x0013, 0x000a, "",     ""          },
            { 0x0014, 0x000b, "ORP",  "-1 [mv]"   },
            { 0x0015, 0x000c, "",     ""          },
            { 0x0016, 0x000d, "TEMP", "25 [deg]"  },
            { 0x0017, 0xffff, "",     ""          }
        };
        vector< ModbusData > DataBase;
        for(unsigned long idx = 0; idx < (sizeof(db_init) / sizeof(ModbusData)) ; idx ++)
        {
            DataBase.push_back(db_init[idx]);
        }
        create_model(treeView);
        set_data(treeView, DataBase);
        create_view(treeView);

        gtk_widget_show(treeView);
        GtkWidget * scrollWindow = gtk_scrolled_window_new(NULL, NULL);
        gtk_widget_show(scrollWindow);
        gtk_widget_set_size_request(scrollWindow, 200,300);
        gtk_container_add(GTK_CONTAINER(scrollWindow), treeView);
        gtk_notebook_append_page(GTK_NOTEBOOK(note), scrollWindow, gtk_label_new(label[idx]));
    }
    return note;
}
static GtkWidget * createInputValue(const char * label, const char * unit)
{
    GtkWidget * hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    GtkWidget * tx   = gtk_entry_new();
    GtkWidget * lb1  = gtk_label_new(label);
    GtkWidget * lb2  = gtk_label_new(unit);
    gtk_widget_show(hbox);
    gtk_widget_show(tx);
    gtk_widget_show(lb1);
    gtk_widget_show(lb2);
    gtk_container_add(GTK_CONTAINER(hbox), lb1);
    gtk_container_add(GTK_CONTAINER(hbox), tx);
    gtk_container_add(GTK_CONTAINER(hbox), lb2);
    gtk_label_set_justify(GTK_LABEL(lb1), GTK_JUSTIFY_LEFT);
    gtk_widget_set_size_request(lb1, 150, 30);
    return hbox;
}

static GtkWidget * createInputPanel(const char * labels[][2], unsigned int size)
{
    GtkWidget * vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    for(unsigned int idx = 0; idx < size; idx ++)
    {
        gtk_container_add(GTK_CONTAINER(vbox), createInputValue(labels[idx][0], labels[idx][1]));
    }
    return vbox;
}

static GtkWidget * createSensorPanels(vector< RadioLinkWidget > & widgetList)
{
    GtkWidget * hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    static const char * label[] = { "pH", "SC", "ISC", "DO", "DO70G" };
    widgetList.reserve((sizeof(label) / sizeof(const char *)));
    for(unsigned long idx = 0; idx < (sizeof(label) / sizeof(const char *)); idx ++)
    {
        static const char * pH_labels[5][2] =
        {
            { "Input 1(15,14)",         "mV"    },
            { "Input 2(13,14)",         "mV"    },
            { "Temp. input(11,12)",     "Ohm"   },
            { "Input 1 impedance",      "Ohm"   },
            { "Input 2 impedance",      "Ohm"   }
        };
        static const char * SC_labels[3][2] =
        {
            { "Sensor(13,15)",          "Ohm"   },
            { "Temp. input(11,12)",     "Ohm"   },
            { "Polarization",           "%"     }
        };
        static const char * DO_labels[4][2] =
        {
            { "Galvanic input(13,15)",  "uA"    },
            { "Galvanic imp.(16,15)",   "Ohm"   },
            { "Polaro input(17,15)",    "uA"    },
            { "Temp. input(11,12)",     "Ohm"   }
        };
        RadioLinkWidget panel;
        panel.label = label[idx];
        ConstCString str(label[idx]);
        switch(str.select(label, (sizeof(label) / sizeof(const char *))))
        {
            case 0:
                panel.widget = createInputPanel(pH_labels, (sizeof(pH_labels) / (sizeof(const char *)*2)));
                gtk_widget_show(panel.widget);
                break;
            case 1:
                panel.widget = createInputPanel(SC_labels, (sizeof(SC_labels) / (sizeof(const char *)*2)));
                gtk_widget_hide(panel.widget);
                break;
            case 2:
                panel.widget = createInputPanel(SC_labels, (sizeof(SC_labels) / (sizeof(const char *)*2))-1);
                gtk_widget_hide(panel.widget);
                break;
            case 3:
                panel.widget = createInputPanel(DO_labels, (sizeof(DO_labels) / (sizeof(const char *)*2)));
                gtk_widget_hide(panel.widget);
                break;
            case 4:
                panel.widget = createInputPanel(DO_labels, (sizeof(DO_labels) / (sizeof(const char *)*2)));
                gtk_widget_hide(panel.widget);
                break;
            default:
                break;
        }
        gtk_container_add(GTK_CONTAINER(hbox), panel.widget);
        widgetList.push_back(panel);
    }
    gtk_widget_show(hbox);
    return hbox;
};

static void radioButton_toggled(GtkWidget * button, GtkWidget * widget)
{
    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(button)))
    {   /* On */
        gtk_widget_show(widget);
    }
    else
    {   /* Off */
        gtk_widget_hide(widget);
    }
}

static GtkWidget * createRadioBottun(vector< RadioLinkWidget > & widgetList)
{
    GtkWidget * hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_show(hbox);

    if(0 < widgetList.size())
    {
        GtkWidget * rbt = gtk_radio_button_new_with_label(NULL, widgetList[0].label);
        g_signal_connect(GTK_TOGGLE_BUTTON(rbt), "toggled", G_CALLBACK(radioButton_toggled), widgetList[0].widget);
        gtk_widget_show(rbt);
        gtk_container_add(GTK_CONTAINER(hbox), rbt);
        for(unsigned int idx = 1; idx < widgetList.size(); idx ++)
        {
            rbt = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(rbt), widgetList[idx].label);
            g_signal_connect(GTK_TOGGLE_BUTTON(rbt), "toggled", G_CALLBACK(radioButton_toggled), widgetList[idx].widget);
            gtk_widget_show(rbt);
            gtk_container_add(GTK_CONTAINER(hbox), rbt);
        }
    }
    return hbox;
}

static GtkWidget * createSensorInputPanel(void)
{
    GtkWidget * vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    vector< RadioLinkWidget > widgetList;
    GtkWidget * inpPanel = createSensorPanels(widgetList);
    gtk_container_add(GTK_CONTAINER(vbox), createRadioBottun(widgetList));
    gtk_container_add(GTK_CONTAINER(vbox), inpPanel);
    return vbox;
}

static GtkWidget * createSensorInput(void)
{
    // 创建一个竖直box
    GtkWidget * vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_show(vbox);

    vector<RadioLinkWidget> widgetList;
    static const char * label[] = { "Sensor Input", "Modbus Map" };
    widgetList.reserve((sizeof(label) / sizeof(const char *)));
    for(unsigned long idx = 0; idx < (sizeof(label) / sizeof(const char *)); idx ++)
    {
        RadioLinkWidget panel;
        panel.label = label[idx];
        ConstCString str(label[idx]);
        switch(str.select(label, (sizeof(label) / sizeof(const char *))))
        {
            case 0: // Sensor Input Panel
                panel.widget = createSensorInputPanel();
                gtk_widget_show(panel.widget);
                break;
            case 1: // Modbus Input Panel
                panel.widget = createModbusInput();
                gtk_widget_hide(panel.widget);
                break;
            default:
                break;
        }
        widgetList.push_back(panel);
    }

    gtk_container_add(GTK_CONTAINER(vbox), createRadioBottun(widgetList));
    for(unsigned int idx = 0; idx < widgetList.size(); idx ++)
    {
        gtk_container_add(GTK_CONTAINER(vbox), widgetList[idx].widget);
    }
    return vbox;
}

static GtkWidget* createSensorInputNote(void)
{
    GtkWidget * note = gtk_notebook_new();  //笔记本控件，可以让用户标签式地切换多个界面。
    gtk_widget_show(note);
    for(unsigned long idx1 = 0; idx1 < 2; idx1 ++)
    {
        GtkWidget * sub_note = gtk_notebook_new();
        gtk_widget_show(sub_note);
        static const char * label[2] = { "Sensor 1", "Sensor 2" };
        gtk_notebook_append_page(GTK_NOTEBOOK(note), sub_note, gtk_label_new(label[idx1]));
        for(unsigned long idx2 = 0; idx2 < 4; idx2 ++)
        {
            static const char*  label[2][4] =
            {
                { "CH 1-1", "CH 1-2", "CH 1-3", "CH 1-4" },
                { "CH 2-1", "CH 2-2", "CH 2-3", "CH 2-4" }
            };
            gtk_notebook_append_page(GTK_NOTEBOOK(sub_note), createSensorInput(), gtk_label_new(label[idx1][idx2]));
        }
    }
    return note;
}
/*
 *      销毁对象
 * @params
 * @return Simulator
 */
static void destroy(GtkWidget * widget, gpointer data)
{
    gtk_main_quit();
}
//-----------------<< 事件处理函数 >>--------------------------------------
extern pf_touch_event_func_t touchpanel_callback;

static int pre_x, pre_y;
static int cur_x, cur_y;

typedef struct
{
    GtkWidget *widget;
    GdkEventButton *event;
}event_long_button;

static bool is_long_touched = false;    // 保存是否为长按事件

/*
 *      鼠标按下事件回调函数
 *
 * @params
 *          widget， 响应该回调函数的窗口句柄
 *          event，  事件
 * @return
 *          成功返回列表中对应项的索引idx；
 *          不成功，返回-1；
 */
gboolean callback_press_event(GtkWidget *widget, GdkEventButton* event)
{
    static guint                srcid;
    pf_touch_event_t            event_type;
    pf_point_t                  point;
    pf_drag_flick_direction_t   dir = pf_drag_flick_none;
    switch(event->type) 
    {
        case GDK_BUTTON_PRESS:
            printf("touched ");
            static event_long_button long_button_event;
            long_button_event.widget = widget;
            long_button_event.event = event;
            event_type = pf_touch_event_touch;
            pre_x = event->x;
            pre_y = event->y;
            // srcid = g_timeout_add(getLongtapDetTime() < 500 ? 1000 : getLongtapDetTime(), callback_long_press_event, &long_button_event);
            break;
        case GDK_BUTTON_RELEASE:
            printf("released ");
            if(!is_long_touched) {
                g_source_remove(srcid);
            }
            is_long_touched = false;
            event_type = pf_touch_event_release;
            break;
        default:
            break;
    }

    point.x = (unsigned short) event->x * 320 / gtk_widget_get_allocated_width (widget);
    point.y = (unsigned short) event->y * 240 / gtk_widget_get_allocated_height (widget);
    printf("x = %d, y = %d \n", point.x, point.y);
    touchpanel_callback(event_type, point, 0, dir);
    return FALSE;
}
/*
 *      鼠标移动事件
 * @params
 * @return
 */
// gboolean callback_motion_event(GtkWidget *widget, GdkEventMotion *event)
// {
//     static unsigned int         pre_time = 0;
//     GdkModifierType             state;
//     pf_touch_event_t            event_type;
//     pf_point_t                  point;
//     pf_drag_flick_direction_t   dir = pf_drag_flick_none;

//     gdk_window_get_pointer(event->window, &cur_x, &cur_y, &state);
// }
/*
 *      创建主窗口title
 * @params
 * @return
 */
static void setTitle(GtkWidget* window)
{
    char rev[4] = {'1','1','1'};
    char build_time[15] = {'2','0','1','8','-','1','2','-','0','6'};
    char title[512];
    sprintf(title, "HMI - Revision %d.%02d.%02d build:%s", 
        rev[1],rev[2], rev[3], build_time);
    gtk_window_set_title(GTK_WINDOW(window), title);
}
/*
 *      创建主窗口
 * @params
 * @return
 */
int Simulator::gtkmain(int argc, char* argv[])
{
    gtk_init(&argc, &argv);
    
    GtkWidget* window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    g_signal_connect(window, "destroy", G_CALLBACK(destroy), NULL);
    setTitle(window);

    GtkWidget * note = createSensorInputNote();
    GtkWidget * dw = gtk_drawing_area_new();    // 创建一个绘图区
    this->dw = dw;
    gtk_widget_show(dw);
    gtk_widget_set_size_request(dw, disp.width(), disp.height());

    g_signal_connect(G_OBJECT(dw), "draw", G_CALLBACK(draw_callback), this);
    g_signal_connect(G_OBJECT(dw), "button_press_event", G_CALLBACK(callback_press_event), NULL);
    g_signal_connect(G_OBJECT(dw), "button_release_event", G_CALLBACK(callback_press_event), NULL);
    // g_signal_connect(G_OBJECT(dw), "motion_notify_event", G_CALLBACK(callback_motion_event), NULL);

    gtk_widget_set_events (dw,
            GDK_BUTTON_RELEASE_MASK
            | GDK_BUTTON_PRESS_MASK
            | GDK_POINTER_MOTION_MASK
            | GDK_POINTER_MOTION_HINT_MASK);

    GtkWidget * hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_show(hbox);
    gtk_container_add(GTK_CONTAINER(hbox), dw);
    gtk_container_add(GTK_CONTAINER(hbox), note);

    gtk_container_add(GTK_CONTAINER(window), hbox);
    if(fullScreenMode)
    {
        gtk_window_fullscreen(GTK_WINDOW(window));
    }
    gtk_widget_show(window);

    // g_timeout_add(100, update_draw, dw);

    gtk_main();
    return 0;       
}
/*
 *      描述
 * @params
 * @return
 */
void Simulator::initBmp()
{
    disp.init();
}

