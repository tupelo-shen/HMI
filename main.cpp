#include <termio.h>
#include <chrono>
#include <thread>
#include "HmiEvent.h"
#include "HmiMain.h"
#include "GDC_Sim.h"
#include "Simulator.h"
#include "HMIControl.h"

void hmiMain(HmiMain* p_hmi)
{
    std::cout << "HMI main thread is started!\n";
    p_hmi->Start();
}

void KeyboardScan(HmiMain* p_hmi)
{
    while(true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        struct termios new_settings;
        struct termios stored_settings;

        // tty访问设置
        tcgetattr(0, &stored_settings);
        new_settings = stored_settings;
        new_settings.c_lflag &= (~ICANON);
        new_settings.c_cc[VTIME] = 0;
        tcgetattr(0, &stored_settings);
        new_settings.c_cc[VMIN] = 1;
        tcsetattr(0, TCSANOW, &new_settings);

        int input = getchar();

        tcsetattr(0, TCSANOW, &stored_settings);

        if(input != EOF)
        {
            HmiEvent ev(HmiEvent::HMI_EV_KEYDOWN, (unsigned long)input);
            p_hmi->SetReady(true);
            p_hmi->AddEventQueue(ev);
        }
    }
}

static HMIControl hmi_control;

int main(int argc, char* argv[])
{
    // int sig = analyzeOptions( argc, argv );
    int sig = 1;
    Options opts( "Simulator.ini" );
    if( opts.logFileName != "" )
    {
        freopen( opts.logFileName.c_str(), "w", stdout );
        freopen( opts.logFileName.c_str(), "w", stderr );
    }

    static Simulator sim;

    switch(sig)
    {
        case 1:
        {
            // Simulator mode
            sim.initBmp();
            setSim(&sim);
            static HmiMain *p_hmi = new HmiMain();
            hmi_control.setHMI(p_hmi); 
            std::thread hmi_main(hmiMain, p_hmi);
            std::thread kb_scan(KeyboardScan, p_hmi);
            sim.gtkmain(argc, argv);
            hmi_main.join();
            kb_scan.join();
        }
        break;
        case 2:
        {}
        break;
    }
    // 创建HMI_MAIN线程
    // HmiMain* p_hmi = new HmiMain();
    // std::thread hmi_main(hmiMain, p_hmi);
    // hmi_main.detach();
    
    // sim.gtkmain(argc, argv);

    // while(true)
    // {
    //     std::this_thread::sleep_for(std::chrono::milliseconds(100));

    //     struct termios new_settings;
    //     struct termios stored_settings;

    //     // tty访问设置
    //     tcgetattr(0, &stored_settings);
    //     new_settings = stored_settings;
    //     new_settings.c_lflag &= (~ICANON);
    //     new_settings.c_cc[VTIME] = 0;
    //     tcgetattr(0, &stored_settings);
    //     new_settings.c_cc[VMIN] = 1;
    //     tcsetattr(0, TCSANOW, &new_settings);

    //     int input = getchar();

    //     tcsetattr(0, TCSANOW, &stored_settings);

    //     if(input != EOF)
    //     {
    //         HmiEvent ev(HmiEvent::HMI_EV_KEYDOWN, (unsigned long)input);
    //         p_hmi->SetReady(true);
    //         p_hmi->AddEventQueue(ev);
    //     }
    // }

    return 0;
}

// #include <boost/filesystem.hpp>

// int main(void)
// {
//     boost::filesystem::path path("/test/test1");
//     boost::filesystem::path old_cpath = boost::filesystem::current_path();
//     boost::filesystem::path parent_path = old_cpath.parent_path();
//     boost::filesystem::path file_path = old_cpath/"file"; //path支持重载/运算符
    
//     return 0;
// }
