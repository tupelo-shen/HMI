#include <termio.h>
#include <chrono>
#include <thread>
#include "HmiEvent.h"
#include "HmiMain.h"
#include "Simulator.h"

void init(void)
{

}
void hmiMain(HmiMain* p_hmi)
{
    std::cout << "HMI main thread is started!\n";
    p_hmi->Start();
}



int main(int argc, char* argv[])
{
    // 创建HMI_MAIN线程
    HmiMain* p_hmi = new HmiMain();
    std::thread hmi_main(hmiMain, p_hmi);
    hmi_main.detach();
    static Simulator sim;
    sim.gtkmain(argc, argv);

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

    return 0;
}
