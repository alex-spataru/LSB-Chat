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
static QImage SOURCE_IMAGE;
static QImage IMG_COMPOSITE;
static QImage IMG_DIFFERENTIAL;
static bool USE_GENERATED_IMAGES = true;

/*
 * Define image format to use when reading/exporting image data to binary data
 */
static const char* IMAGE_FORMAT = "PNG";

/**
 * @brief set_bit
 * @param num
 * @param position
 * @param bit
 * @return
 *
 * Changes the bit of @a num at the n-th @a position with the given @a bitValue
 */
int set_bit(int num, int position, bool bitValue)
{
    int mask = 1 << position;

    if(bitValue)
        return num | mask;
    else
        return num & ~mask;
}

/**
 * @brief get_bit
 * @param num
 * @param position
 * @return
 *
 * Returns the value of the bit at the n-th @a position of num
 */
bool get_bit(int num, int position)
{
    bool bit = num & (1 << position);
    return bit;
}

/**
 * @brief LSB::useGeneratedImages
 * @return
 *
 * Returns @c true if the LSB algorithm shall be executed on an auto-generated image
 */
bool LSB::useGeneratedImages()
{
    return USE_GENERATED_IMAGES;
}

/**
 * @brief LSB::enableGeneratedImages
 * @param enabled
 *
 * If @a enabled is set to @c true, LSB module will generate an image filled with random pixels
 * for each time the user executes the LSB-Write algorithm.
 *
 * If @a enabled is set to @c false, LSB module will execute the LSB-Write algorithm over an
 * existing image selected by the user.
 */
void LSB::enableGeneratedImages(const bool enabled)
{
    USE_GENERATED_IMAGES = enabled;

    if(!enabled) {
        IMG_COMPOSITE = SOURCE_IMAGE;
        IMG_DIFFERENTIAL = SOURCE_IMAGE;
    }

    else {
        SOURCE_IMAGE = generateImage(0, false);
        IMG_COMPOSITE = generateImage(1000, true);
        IMG_DIFFERENTIAL = generateImage(1000, false);
    }
}

/**
 * @brief LSB::setSourceImage
 * @param image
 *
 * Sets the image that the LSB module will use to write data over.
 *
 * @note This will only have a visible effect if the user has called @fn enableGeneratedImages
 *       with argument @a enabled set to @c false).
 */
void LSB::setSourceImage(const QImage& image)
{
    SOURCE_IMAGE = image;

    if(!useGeneratedImages()) {
        IMG_COMPOSITE = SOURCE_IMAGE;
        IMG_DIFFERENTIAL = SOURCE_IMAGE;
    }
}

/**
 * @brief LSB::currentCompositeImage
 * @return
 *
 * Returns the resultant image after executing the LSB algorithm, or the LSB-touched image
 * received when extracting information with the LSB-Read algorithm.
 */
QImage LSB::currentCompositeImage()
{
    if (IMG_COMPOSITE.isNull())
        IMG_COMPOSITE = generateImage(100, false);

    return IMG_COMPOSITE;
}

/**
 * @brief LSB::currentImageData
 * @return
 *
 * Returns an image with black background, which only shows the pixels that have been modified
 * by the LSB-Write algorithm.
 */
QImage LSB::currentImageData()
{
    if (IMG_DIFFERENTIAL.isNull())
        IMG_DIFFERENTIAL = generateImage(100, false);

    return IMG_DIFFERENTIAL;
}

/**
 * @brief LSB::generateImage
 * @param size
 * @param random
 * @return
 *
 * Generates a new rectangular image of the given @a size. If @a random is set to @c true, then
 * the image will be filled with randomly-generated pixels. Otherwise, the image will be filled
 * with black pixels.
 */
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
        if (random) {
            r = generator.bounded(0x20, 0xdd);
            g = generator.bounded(0x20, 0x90);
            b = generator.bounded(0x20, 0xff);
        }

        for (int j = 0; j < image.height(); ++j)
            image.setPixel(j, i, qRgb(r, g, b));
    }

    // Return image
    return image;
}

/**
 * @brief LSB::encodeData
 * @param data
 * @return
 *
 * Encodes the given @a data in an image using the LSB-Write algorithm, the data is encoded in
 * the following manner:
 *
 * 1) Only the hypothenuse will contain data.
 * 2) The hypothenuse will be calculated considering a rectangular image. The sides are equal
 *    to the smallest side of the image.
 * 3) Data will start with the following format @c{$DATA_LENGTH$}
 * 4) After data length header is written, the given @a data is written over the image
 */
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
        const int size = qMin(10 * 10000.0, injection.length() * 3.2);
        IMG_COMPOSITE = generateImage(size, true);
        IMG_DIFFERENTIAL = generateImage(size, false);
    }

    // Reset images using source image
    else {
        IMG_COMPOSITE = SOURCE_IMAGE;
        IMG_DIFFERENTIAL = generateImage(qMin(IMG_COMPOSITE.width(), IMG_COMPOSITE.height()), false);
    }

    // Calculate the diagonal length using the Pythagorean theorem
    int cat = qMin(IMG_COMPOSITE.width(), IMG_COMPOSITE.height());
    int hyp = static_cast<int>(round(sqrt(2.0) * static_cast<double>(cat)));

    // Write data to image using LSB
    int bytesWritten = 0;
    for(int i = 0; i < hyp; i += 3) {
        // We have written all data, exit loop
        if(bytesWritten >= injection.length())
            break;

        // Get byte & pixel value
        char byte = injection.at(bytesWritten);
        QRgb pixel1 = IMG_COMPOSITE.pixel(i + 0, i + 0);
        QRgb pixel2 = IMG_COMPOSITE.pixel(i + 1, i + 1);
        QRgb pixel3 = IMG_COMPOSITE.pixel(i + 2, i + 2);

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
        IMG_COMPOSITE.setPixel(i + 0, i + 0, lsbPixel1);
        IMG_COMPOSITE.setPixel(i + 1, i + 1, lsbPixel2);
        IMG_COMPOSITE.setPixel(i + 2, i + 2, lsbPixel3);
        IMG_DIFFERENTIAL.setPixel(i + 0, i + 0, lsbPixel1);
        IMG_DIFFERENTIAL.setPixel(i + 1, i + 1, lsbPixel2);
        IMG_DIFFERENTIAL.setPixel(i + 2, i + 2, lsbPixel3);

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
    return IMG_COMPOSITE;
}

/**
 * @brief LSB::decodeData
 * @param image
 * @return
 *
 * Decodes and returns the data contained in the given @a image. If the data is invalid, or
 * the image is invalid, an empty byte array will be returned.
 */
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
    IMG_COMPOSITE = image;

    // Regenerate data image
    IMG_DIFFERENTIAL = generateImage(cat, false);
    for(int i = 0; i < cat; ++i)
        IMG_DIFFERENTIAL.setPixel(i, i, IMG_COMPOSITE.pixel(i, i));

    // Return data
    return data;
}

/**
 * @brief LSB::imageToBinaryData
 * @param image
 * @return
 *
 * Converts the given image to a byte array by exporting the image data using the PNG format.
 * The PNG format was choosen because - unlike JPEG - the format is looseless.
 */
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
        image.save(&buffer, IMAGE_FORMAT, 100);
        buffer.close();
    }

    // Return byte array
    return arr;
}

/**
 * @brief LSB::decodeData
 * @param rawImageData
 * @return
 *
 * Creates an image from the given @a rawimageData and attempts to decode the information contained
 * in the image using the LSB-Read algorithm.
 *
 * @note The image format must be PNG, otherwise, the conversion will fail.
 */
QByteArray LSB::decodeData(const QByteArray& rawImageData)
{
    // Copy the raw image data and load it into a memory buffer
    QByteArray data = rawImageData;
    QBuffer buffer(&data);

    // Open the buffer, load image data and decode image
    if(buffer.open(QIODevice::ReadOnly)) {
        QImage image;
        if(image.load(&buffer, IMAGE_FORMAT))
            return decodeData(image);

        buffer.close();
    }

    // Error, return empty byte array
    return QByteArray();
}
