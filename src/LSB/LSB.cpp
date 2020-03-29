/*
 * Copyright (c) 2020 Alex Spataru <https://github.com/alex-spataru>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "LSB.h"

#include <QtMath>
#include <QBuffer>
#include <QMessageBox>
#include <QRandomGenerator>

/*
 * Define local variables
 */
static bool USE_GENERATED_IMAGES = true;
static QImage LSB_IMAGE =  LSB::generateImage(100, true);
static QImage SOURCE_IMAGE = LSB::generateImage(100, false);
static QImage LSB_IMAGE_DATA =  LSB::generateImage(100, false);

int set_bit(int num, int position, bool bit)
{
    int mask = 1 << position;

    if(bit)
        return num | mask;
    else
        return num & ~mask;
}

bool get_bit(int num, int position)
{
    bool bit = num & (1 << position);
    return bit;
}

bool LSB::useGeneratedImages()
{
    return USE_GENERATED_IMAGES;
}

void LSB::enableGeneratedImages(const bool enabled)
{
    USE_GENERATED_IMAGES = enabled;

    if(!enabled) {
        LSB_IMAGE = SOURCE_IMAGE;
        LSB_IMAGE_DATA = SOURCE_IMAGE;
    }

    else {
        LSB_IMAGE = generateImage(100, true);
        SOURCE_IMAGE = generateImage(0, false);
        LSB_IMAGE_DATA = generateImage(100, false);
    }
}

void LSB::setSourceImage(const QImage& image)
{
    SOURCE_IMAGE = image;

    if(!useGeneratedImages()) {
        LSB_IMAGE = SOURCE_IMAGE;
        LSB_IMAGE_DATA = SOURCE_IMAGE;
    }
}

QImage LSB::currentCompositeImage()
{
    return LSB_IMAGE;
}

QImage LSB::currentImageData()
{
    return LSB_IMAGE_DATA;
}

QImage LSB::generateImage(const int size, const bool random)
{
    // Create image & init. random generator
    QImage image(QSize(size, size), QImage::Format_RGB32);
    QRandomGenerator generator = QRandomGenerator::securelySeeded();

    // Initialize rgb values
    int r = 0;
    int g = 0;
    int b = 0;

    // Fill image pixels
    for (int i = 0; i < image.width(); ++i) {
        for (int j = 0; j < image.height(); ++j) {
            if (random) {
                r = generator.bounded(0, 100);
                g = generator.bounded(0, 255);
                b = generator.bounded(100, 255);
            }

            image.setPixel(i, j, qRgb(r, g, b));
        }
    }

    // Return image
    return image;
}

QImage LSB::encodeData(const QByteArray& data)
{
    // Append data size to start
    QByteArray injection;
    injection.append("$");
    injection.append(QString::number(data.length()));
    injection.append("$");
    injection.append(data);

    // Reset images (generate random image case)
    if(useGeneratedImages() || (SOURCE_IMAGE.width() == 0 && SOURCE_IMAGE.height() == 0)) {
        const int size = injection.length() * 4;
        LSB_IMAGE = generateImage(size, true);
        LSB_IMAGE_DATA = generateImage(size, false);
    }

    // Reset images using source image
    else {
        LSB_IMAGE = SOURCE_IMAGE;
        LSB_IMAGE_DATA = generateImage(qMin(LSB_IMAGE.width(), LSB_IMAGE.height()), false);
    }

    // Calculate the diagonal length using the Pythagorean theorem
    int cat = qMin(LSB_IMAGE.width(), LSB_IMAGE.height());
    int hyp = static_cast<int>(round(sqrt(2.0) * static_cast<double>(cat)));

    // Write data to image using LSB
    int bytesWritten = 0;
    for(int i = 0; i < hyp; i += 3) {
        // We have written all data, exit loop
        if(bytesWritten >= injection.length())
            break;

        // Get byte & pixel value
        char byte = injection.at(bytesWritten);
        QRgb pixel1 = LSB_IMAGE.pixel(i + 0, i + 0);
        QRgb pixel2 = LSB_IMAGE.pixel(i + 1, i + 1);
        QRgb pixel3 = LSB_IMAGE.pixel(i + 2, i + 2);

        // Get individual bits
        int bits[8];
        for(int i = 0; i < 8; ++i)
            bits[i] = ((1 << (i % 8)) & byte) >> (i % 8);

        // Write on LSBs of each byte of the image
        QRgb lsbPixel1 = qRgb(set_bit(qRed(pixel1),   0, bits[0]),
                              set_bit(qGreen(pixel1), 0, bits[1]),
                              set_bit(qBlue(pixel1),  0, bits[2]));
        QRgb lsbPixel2 = qRgb(set_bit(qRed(pixel2),   0, bits[3]),
                              set_bit(qGreen(pixel2), 0, bits[4]),
                              set_bit(qBlue(pixel2),  0, bits[5]));
        QRgb lsbPixel3 = qRgb(set_bit(qRed(pixel3),   0, bits[6]),
                              set_bit(qGreen(pixel3), 0, bits[7]),
                              set_bit(qBlue(pixel3),  0, 1));

        // Update pixels of image
        LSB_IMAGE.setPixel(i + 0, i + 0, lsbPixel1);
        LSB_IMAGE.setPixel(i + 1, i + 1, lsbPixel2);
        LSB_IMAGE.setPixel(i + 2, i + 2, lsbPixel3);
        LSB_IMAGE_DATA.setPixel(i + 0, i + 0, lsbPixel1);
        LSB_IMAGE_DATA.setPixel(i + 1, i + 1, lsbPixel2);
        LSB_IMAGE_DATA.setPixel(i + 2, i + 2, lsbPixel3);

        // Increment written bytes
        ++bytesWritten;
    }

    // Warn user if image was too small
    if(bytesWritten < injection.length()) {
        QMessageBox::critical(Q_NULLPTR,
                              QObject::tr("Error"),
                              QObject::tr("The image is too small to fit the requested data"));
        return QImage(0, 0, QImage::Format_RGB32);
    }

    // Return the obtained image
    return LSB_IMAGE;
}

QByteArray LSB::decodeData(const QImage& image)
{
    // Init. variables for obtaining data length
    int dataLenght = 0;
    int headerCount = 0;
    QString lengthString;

    // Calculate the diagonal length using the Pythagorean theorem
    int cat = qMin(image.width(), image.height());
    int hyp = static_cast<int>(round(sqrt(2.0) * static_cast<double>(cat)));

    // Decode data
    QByteArray data;
    for(int i = 0; i < hyp; i += 3) {
        // Get pixels
        QRgb pixel1 = image.pixel(i + 0, i + 0);
        QRgb pixel2 = image.pixel(i + 1, i + 1);
        QRgb pixel3 = image.pixel(i + 2, i + 2);

        // Get individual bits
        bool bits[8] = {
            get_bit(qRed(pixel1),   0),
            get_bit(qGreen(pixel1), 0),
            get_bit(qBlue(pixel1),  0),
            get_bit(qRed(pixel2),   0),
            get_bit(qGreen(pixel2), 0),
            get_bit(qBlue(pixel2),  0),
            get_bit(qRed(pixel3),   0),
            get_bit(qGreen(pixel3), 0)
        };

        // Get byte value from bits
        char byte = 0;
        for(int i = 0; i < 8; ++i)
            byte += (bits[i] << i);

        // Data length not calculated yet
        if(dataLenght <= 0 && headerCount < 2) {
            // Get start/end of header
            if(byte == '$')
                ++headerCount;

            // Append digit to length string
            else if(byte >= '0' && byte <= '9')
                lengthString.append(byte);
        }

        // If header length is equal to two
        else if(dataLenght <= 0 && headerCount == 2) {
            dataLenght = lengthString.toInt();
            data.append(byte);
        }

        // Append to data
        else if(data.length() < dataLenght)
            data.append(byte);
        else
            break;
    }

    // Update current LSB image
    LSB_IMAGE = image;

    // Regenerate data image
    LSB_IMAGE_DATA = image;
    for(int i = 0; i < image.width(); ++i) {
        if(i > 2 * dataLenght)
            break;

        for(int j = 0; j < image.height(); ++j) {
            QRgb pixel = LSB_IMAGE_DATA.pixel(i, j);
            LSB_IMAGE_DATA.setPixel(i, j, i == j ? pixel : qRgb(0, 0, 0));
        }
    }

    // Return data
    return data;
}

QByteArray LSB::imageToBinaryData(const QImage& image)
{
    // Create buffer to treat byte array as file
    QByteArray arr;
    QBuffer buffer(&arr);

    // If image is empty, return empty buffer
    if(image.width() <= 0 || image.height() <= 0)
        return arr;

    // Save image as PNG to buffer
    if(buffer.open(QIODevice::WriteOnly)) {
        image.save(&buffer, "PNG", 100);
        buffer.close();
    }

    // Return byte array
    return arr;
}

QByteArray LSB::decodeData(const QByteArray& rawImageData)
{
    // Copy the raw image data and load it into a memory buffer
    QByteArray data = rawImageData;
    QBuffer buffer(&data);

    // Open the buffer, load image data and decode image
    if(buffer.open(QIODevice::ReadOnly)) {
        QImage image;
        if(image.load(&buffer, "PNG"))
            return decodeData(image);

        buffer.close();
    }

    // Error, return empty byte array
    return QByteArray();
}
