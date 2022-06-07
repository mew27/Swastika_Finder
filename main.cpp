#include <QFile>
#include <QElapsedTimer>
#include <iostream>

#include <QImage>
#include <QPainter>
#include <QList>

#include <QGuiApplication>

char getBit(const QByteArray& pi_byte, long long bitpos) {

    if (bitpos == 0)
        return 1;
    else if (bitpos == 1)
        return 1;
    else {
        int bytepos = (bitpos - 2) / 4 + 2;
        int sbitpos = (bitpos - 2) % 4;

        char byte = 0;

        if (pi_byte[bytepos] >= 97)
            byte = pi_byte[bytepos] - 87;
        else
            byte = pi_byte[bytepos] - 48;

        return (byte >> (3 - sbitpos)) & 0x1;
    }
}

char swastikArray[] = {
    1, 0, 1, 1, 1,
    1, 0, 1, 0, 0,
    1, 1, 1, 1, 1,
    0, 0, 1, 0, 1,
    1, 1, 1, 0, 1
};

bool checkSwastika(const QByteArray& pi_byte, long long int bit) {
    int i = 0;
    //qDebug() << "ba length = " << ba.length();
    while (i < 25) {
        if (getBit(pi_byte, bit + i) != swastikArray[i])
            return false;
        i++;
    }

    return true;
}

QList<long long int> getSwastika(const QByteArray& pi_byte, long long int maxnum) {
    QList<long long int> swastika_list;

    QElapsedTimer timer;
    timer.start();

    #pragma omp parallel for
    for (long long int counter = 0; counter < maxnum; counter++) {

        if (checkSwastika(pi_byte, counter)) {
            std::cout << "Svastica found, at bit " << counter << std::endl;
            swastika_list.append(counter);
        }

    }
    std::cout << "Total time " << timer.elapsed() / 1000 << "s" << std::endl;
    std::cout << "Total Swastikas = " << swastika_list.length() << std::endl;

    return swastika_list;
}

int main(int argc, char *argv[])
{
    QGuiApplication a(argc, argv);

    QFile file("/home/paolo/Scaricati/pi_hex_1b/pi_hex_1b.txt");

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        std::cout << "Errore !" << std::endl;
        exit(1);
    }

    QByteArray pi_byte(file.readAll());
    file.close();

    const QList<long long int> swastika_list = getSwastika(pi_byte, 4000000000L);

    QRgb whiteVal = qRgb(255, 255, 255), redVal = qRgb(255, 0, 0);
    QBrush redBrush(redVal), whiteBrush(whiteVal);
    int pixel_size = 25;
    int numrows = 15;
    int numcols = 25;
    int header_size = 20;

    for (auto swastika_pos : swastika_list) {
        QImage image(numcols * pixel_size, header_size + numrows * pixel_size, QImage::Format_RGB32);
        image.fill(0);

        QPainter painter(&image);

        QFile output_image("/home/paolo/Swastika_PI/Pics/Outputpic_" + QString::number(swastika_pos) + ".png");

        painter.setBrush(whiteBrush);

        painter.setFont(QFont("Arial", 20, QFont::Bold, false));
        painter.setPen(Qt::white);
        painter.drawText(0, 20, "Bit " + QString::number(swastika_pos));
        painter.setPen(Qt::black);

        if (!output_image.open(QIODevice::WriteOnly)){
            std::cout << "Error opening" << std::endl;
            exit(1);
        }

        int counter = 0;
        for (int i = 0; i < numrows; i++) {
            for (int j = 0; j <numcols; j++) {
                if (j > 10 && j <= 15) {
                    if (getBit(pi_byte, swastika_pos - 5 * 5 + counter)) {
                        if (counter >= 25 && counter < 50)
                            painter.setBrush(redBrush);
                        else
                            painter.setBrush(whiteBrush);
                        painter.drawRect(j * pixel_size, header_size + i*pixel_size, pixel_size, pixel_size);
                    }
                    counter++;
                }
            }
        }


        image.save(&output_image, "PNG");
        output_image.close();
    }

}
