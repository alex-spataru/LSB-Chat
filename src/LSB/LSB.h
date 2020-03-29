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

#ifndef LSB_H
#define LSB_H

#include <QImage>
#include <QObject>
#include <QByteArray>

class LSB
{
   public:
      static bool useGeneratedImages();
      static void enableGeneratedImages(const bool enabled);
      static void setSourceImage(const QImage& image);

      static QImage currentImageData();
      static QImage currentCompositeImage();
      static QImage generateImage(const int size, const bool random);

      static QImage encodeData(const QByteArray& data);
      static QByteArray decodeData(const QImage& image);
      static QByteArray decodeData(const QByteArray& rawImageData);

      static QByteArray imageToBinaryData(const QImage& image);
};

#endif
