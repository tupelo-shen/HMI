/*
 * @Module Name：    Simulator.cpp 
 * @Function：
 * @Author:         Tupelo Shen
 * @Create Date:    2018-02-27
 */
#include <assert.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include "Simulator.h"

using namespace std;
using namespace boost;
using namespace boost::property_tree;

Options *Options::obj = 0;

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
    for( unsigned char idx = 0; idx < 5; idx++ )
    {
        static const char *keys[5] = { "COM.Sensor1", "COM.Sensor2", "COM.Hart", "COM.RS485", "COM.Bluetooth" };
        if( boost::optional<string> str = pt.get_optional<string>( keys[idx] ) )
        {
            uart[idx] = str.get();
        }
    }
    if( boost::optional<string> str = pt.get_optional<string>( "Flash.Program" ) )
    {
        progtamFlash = str.get();
    }
    if( boost::optional<string> str = pt.get_optional<string>( "Flash.UserData" ) )
    {
      dataFlash = str.get();
    }
    logFileName = "";
    if( boost::optional<string> str = pt.get_optional<string>( "Log.LogFile" ) )
    {
        logFileName = str.get();
    }
    if( boost::optional<string> str = pt.get_optional<string>( "Log.DisplayLog" ) )
    {
        logSwitch = str.get();
    }
    if( boost::optional<string> str = pt.get_optional<string>( "Log.DisplayImageFolder" ) )
    {
        logFolder = str.get();
    }
    if( boost::optional<string> str = pt.get_optional<string>( "Sensor.ch1" ) )
    {
        sensor_ch[0] = str.get();
    }
    if( boost::optional<string> str = pt.get_optional<string>( "Sensor.ch2" ) )
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
    assert( 0 != obj );
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
    assert( 0 != obj );
    return *obj;
}
//-----------------<< 创建主窗口 >>--------------------------------------
/*
 *      销毁对象
 * @params
 * @return Simulator
 */
static void destroy( GtkWidget * widget, gpointer data )
{
    gtk_main_quit();
}

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
    g_signal_connect(window, "destroy", G_CALLBACK( destroy ), NULL );
    setTitle(window);

    // GtkWidget * note = createSensorInputNote();
    // GtkWidget * dw = gtk_drawing_area_new();
    // this->dw = dw;
    // gtk_widget_show(dw);
    // gtk_widget_set_size_request( dw, disp.width(), disp.height() );

    // g_signal_connect( G_OBJECT(dw), "draw", G_CALLBACK( draw_callback ), this );
    // g_signal_connect(G_OBJECT(dw), "button_press_event", G_CALLBACK(callback_press_event), NULL);
    // g_signal_connect(G_OBJECT(dw), "button_release_event", G_CALLBACK(callback_press_event), NULL);
    // g_signal_connect(G_OBJECT(dw), "motion_notify_event", G_CALLBACK(callback_motion_event), NULL);

    // gtk_widget_set_events (dw,
    //         GDK_BUTTON_RELEASE_MASK
    //         | GDK_BUTTON_PRESS_MASK
    //         | GDK_POINTER_MOTION_MASK
    //         | GDK_POINTER_MOTION_HINT_MASK);

    // GtkWidget * hbox = gtk_box_new( GTK_ORIENTATION_HORIZONTAL, 0 );
    // gtk_widget_show( hbox );
    // gtk_container_add( GTK_CONTAINER( hbox ), dw );
    // gtk_container_add( GTK_CONTAINER( hbox ), note );

    // gtk_container_add( GTK_CONTAINER( window ), hbox );
    // if( fullScreenMode )
    // {
    //     gtk_window_fullscreen( GTK_WINDOW( window ) );
    // }
    gtk_widget_show(window);

  //g_timeout_add(100, update_draw, dw);

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

