#include <QtCore/QCoreApplication>
#include "Decoder.h"
#include "Player.h"
#include "Test.h"
#include <iostream>


const char* audio1 = "三葉の通学.mp3";
const char* audio2 = "泠鸢yousa - 09.乡情曲（翻唱）.flac";
const char* audio3 = "skycity1.mp3";



struct test_saver {
    FILE* fp;
    test_saver() {
        fp = fopen("out1.pcm", "wb");
    }

    ~test_saver() {
        fclose(fp);
    }
    void write(uchar* buf,int sz)
    {
        fwrite(buf, sz, 1, fp);
    }
}saver1;

/*
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    int volume = 100;
    Player player(volume);
    unsigned char buffer[1024];
    FIFO fifo([](int&, const unsigned char*, int)->bool {return false; }, [](int& freesize, int)->bool {return false; }, 1024);
    FILE* fp = fopen("output.pcm", "wb");
    Decoder d(&a);
    d.open(audio2);
    int sz = 0;
    while (true)
    {
        d.decode(fifo);
        if (!(sz = fifo[1024 - fifo.getFreesize()] >> buffer))
            break;
        fwrite(buffer, sz, 1, fp);
    }
    fclose(fp);
    return a.exec();
}
*/

int main(int argc, char* argv[])
{
    QCoreApplication a(argc, argv);
    FIFO buffer(1024);
    Decoder decoder;
    QTimer timer1;
    timer1.setInterval(1);
    QObject::connect(&timer1, &QTimer::timeout, [&]()
        {
            decoder.decode(buffer);
        });


    QTimer timer2;
    timer2.setInterval(2);
    QObject::connect(&timer2, &QTimer::timeout, [&]()
        {
            unsigned char buffer1[4096];
            if (timer1.isActive() || buffer.getFreesize())
            {
                int i = buffer[4096] >> buffer1;
                fwrite(buffer1, i, 1, saver1.fp);
            }
            else
            {
                timer2.stop();
                a.quit();
                return;
            }
        });
    QObject::connect(&decoder, &Decoder::decodeFin, &timer1, &QTimer::stop);
    decoder.open(audio3);
    timer1.start();
    timer2.start();
    return a.exec();
}