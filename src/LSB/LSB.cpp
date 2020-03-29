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
static QImage SOURCE_IMAGE = QImage(0, 0, QImage::Format_RGB32);
static QImage LSB_IMAGE = QImage(100, 100, QImage::Format_RGB32);
static QImage LSB_IMAGE_DATA = QImage(100, 100, QImage::Format_RGB32);

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
      SOURCE_IMAGE = QImage(0, 0, QImage::Format_RGB32);
      LSB_IMAGE = QImage(100, 100, QImage::Format_RGB32);
      LSB_IMAGE_DATA = QImage(100, 100, QImage::Format_RGB32);
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

QImage LSB::encodeData(const QByteArray& data)
{
   // Reset images (generate random image case)
   if(useGeneratedImages() || (SOURCE_IMAGE.width() == 0 && SOURCE_IMAGE.height() == 0)) {
      // Resize image to data size
      const int size = data.length() * 8 + 120;
      LSB_IMAGE = QImage(QSize(size, size), QImage::Format_RGB32);
      LSB_IMAGE_DATA = QImage(QSize(size, size), QImage::Format_RGB32);

      // Fill image with random pixels
      QRandomGenerator generator = QRandomGenerator::securelySeeded();
      for(int i = 0; i < LSB_IMAGE.width(); ++i) {
         for(int j = 0; j < LSB_IMAGE.height(); ++j) {
            int r = generator.bounded(0, 100);
            int g = generator.bounded(0, 255);
            int b = generator.bounded(100, 255);
            LSB_IMAGE.setPixel(i, j, qRgb(r, g, b));
            LSB_IMAGE_DATA.setPixel(i, j, qRgb(0, 0, 0));
         }
      }
   }

   // Reset images using source image
   else {
      LSB_IMAGE = SOURCE_IMAGE;
      LSB_IMAGE_DATA = SOURCE_IMAGE;

      for(int i = 0; i < LSB_IMAGE.width(); ++i)
         for(int j = 0; j < LSB_IMAGE.height(); ++j)
            LSB_IMAGE_DATA.setPixel(i, j, qRgb(0, 0, 0));
   }

   // Append data size to start
   QByteArray injection;
   injection.append("$");
   injection.append(QString::number(data.length()));
   injection.append("$");
   injection.append(data);

   // Write data to image using LSB
   int bytesWritten = 0;
   int diagonalSize = qMin(LSB_IMAGE.width(), LSB_IMAGE.height());
   for(int i = 0; i < diagonalSize; ++i) {
       // We have written all data, exit loop
      if (bytesWritten >= injection.length())
          break;

      // Get byte & pixel value
      char byte = injection.at(bytesWritten);
      QRgb pixel1 = LSB_IMAGE.pixel(i + 0, i + 0);
      QRgb pixel2 = LSB_IMAGE.pixel(i + 1, i + 1);
      QRgb pixel3 = LSB_IMAGE.pixel(i + 2, i + 2);

      // Get individual bits
      int bits[8];
      for (int i = 0; i < 8; ++i)
          bits[i] = ((1 << (i % 8)) & byte) >> (i % 8);

      // Write on LSBs of each byte of the image
      QRgb lsbPixel1 = qRgb((qRed(pixel1)   & 0xFE) | bits[0],
                            (qGreen(pixel1) & 0xFE) | bits[1],
                            (qBlue(pixel1)  & 0xFE) | bits[2]);
      QRgb lsbPixel2 = qRgb((qRed(pixel2)   & 0xFE) | bits[3],
                            (qGreen(pixel2) & 0xFE) | bits[4],
                            (qBlue(pixel2)  & 0xFE) | bits[5]);
      QRgb lsbPixel3 = qRgb((qRed(pixel3)   & 0xFE) | bits[6],
                            (qGreen(pixel3) & 0xFE) | bits[7],
                            (qBlue(pixel3)));

      // Update pixels of image
      LSB_IMAGE.setPixel(i + 0, i + 0, lsbPixel1);
      LSB_IMAGE.setPixel(i + 1, i + 2, lsbPixel2);
      LSB_IMAGE.setPixel(i + 2, i + 2, lsbPixel3);
      LSB_IMAGE_DATA.setPixel(i + 0, i + 0, lsbPixel1);
      LSB_IMAGE_DATA.setPixel(i + 1, i + 2, lsbPixel2);
      LSB_IMAGE_DATA.setPixel(i + 2, i + 2, lsbPixel3);

      // Increment i & written bytes
      i += 3;
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

   // Decode data
   QByteArray data;
   for(int i = 0; i < image.width(); ++i) {
      for(int j = 0; j < image.height(); ++j) {
         // Get pixel and byte
         QRgb pixel = image.pixel(i, j);
         char byte = static_cast<char>(qRed(pixel));

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
         else if(dataLenght <= 0 && headerCount == 2)
            dataLenght = lengthString.toInt();

         // Append to data
         else if(i * image.height() + j < dataLenght)
            data.append(byte);
         else
            break;
      }
   }

   // Update current LSB image
   LSB_IMAGE = image;

   // Regenerate data image
   LSB_IMAGE_DATA = image;
   for(int i = 0; i < image.width(); ++i) {
      for(int j = 0; j < image.height(); ++j) {
         QRgb pixel = LSB_IMAGE_DATA.pixel(i, j);
         LSB_IMAGE_DATA.setPixel(i, j, qRgb(qRed(pixel), 0, 0));
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
