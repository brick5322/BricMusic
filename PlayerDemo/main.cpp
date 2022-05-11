#include <QtCore/QCoreApplication>
#include <QTimer>
#include "Player.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    int volume = 100;
    Player player(volume);
    FILE* fp = fopen("./demo.pcm", "rb");
    QObject::connect(&player, &Player::getData, [&](unsigned char* buffer, int len) {
        if (feof(fp))
            a.quit();
        fread(buffer, len, 1, fp); });
    SDL_AudioSpec spec;
    spec.format = AUDIO_S16SYS;
    spec.channels = 2;
    spec.freq = 44100;
    player.resetContext(spec);
    return a.exec();
}
