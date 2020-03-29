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
#include <QRandomGenerator>

/*
 * Define local variables
 */
static QImage LSB_IMAGE = QImage(100, 100, QImage::Format_RGB32);
static QImage LSB_IMAGE_DATA = QImage(100, 100, QImage::Format_RGB32);

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
   // Calculate image border size
   const int size = static_cast<int>(round(sqrt(static_cast<double>(data.length()))));

   // Reset static images
   LSB_IMAGE = QImage(QSize(size, size), QImage::Format_RGB32);
   LSB_IMAGE_DATA = QImage(QSize(size, size), QImage::Format_RGB32);

   // Fill image with random pixels
   QRandomGenerator* generator = QRandomGenerator::system();
   for(int i = 0; i < LSB_IMAGE.width(); ++i) {
      for(int j = 0; j < LSB_IMAGE.height(); ++j) {
         int r = generator->bounded(0, 255);
         int g = generator->bounded(0, 255);
         int b = generator->bounded(0, 255);
         LSB_IMAGE.setPixel(i, j, qRgb(r, g, b));
         LSB_IMAGE_DATA.setPixel(i, j, qRgb(0, 0, 0));
      }
   }

   // Write data to image using LSB
   for(int i = 0; i < size; ++i) {
      for(int j = 0; j < size; ++j) {
         if(data.length() <= i * size + j)
            break;

         char byte = data.at(i * size + j);
         QRgb pixel = LSB_IMAGE.pixel(i, j);
         LSB_IMAGE_DATA.setPixel(i, j, qRgb(byte, 0, 0));
         LSB_IMAGE.setPixel(i, j, qRgb(byte, qGreen(pixel), qBlue(pixel)));
      }
   }

   // Return the obtained image
   return LSB_IMAGE;
}

QByteArray LSB::decodeData(const QImage& image)
{
   // Decode data
   QByteArray data;
   for(int i = 0; i < image.width(); ++i) {
      for(int j = 0; j < image.height(); ++j) {
         QRgb pixel = image.pixel(i, j);
         data.append(static_cast<char>(qRed(pixel)));
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
