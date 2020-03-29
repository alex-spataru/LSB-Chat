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

#include <QFont>
#include <QObject>
#include <QPainter>
#include <QFontMetrics>
#include <QQuickImageProvider>

#include "LSB/LSB.h"
#include "Comms/NetworkComms.h"

class LsbImageProvider : public QQuickImageProvider
{
   public:
      LsbImageProvider() : QQuickImageProvider(QQuickImageProvider::Pixmap)
      {
         // Nothing to do
      }

      QPixmap requestPixmap(const QString& id, QSize* size, const QSize& requestedSize) override
      {
         Q_UNUSED(requestedSize)

         // Show composite image
         if(id == "composite")
            return QPixmap::fromImage(LSB::currentCompositeImage());

         // Show LSB data image
         else if(id == "data")
            return QPixmap::fromImage(LSB::currentImageData());

         // Generate pixmap with id as text
         else {
            QPixmap pixmap(size->width(), size->height());
            QPainter painter(&pixmap);

            // Set text font
            QFont font("Arial", size->height() / 18, QFont::Bold);
            painter.setFont(font);
            painter.setPen(Qt::white);

            // Draw text
            painter.drawText(QRectF(0, 0, size->width(), size->height()),
                             Qt::AlignCenter, "No valid image");

            // Return obtained pixmap
            return pixmap;
         }
      }
};

class QmlBridge : public QObject
{
      Q_OBJECT Q_PROPERTY(QString userName READ getUserName CONSTANT)
      Q_PROPERTY(QStringList sortTypes READ getSortTypes CONSTANT)
      Q_PROPERTY(QStringList peers READ getPeers NOTIFY peerCountChanged)
      Q_PROPERTY(QStringList lsbImagePaths READ getLsbImagePaths NOTIFY lsbDbUpdated)
      Q_PROPERTY(QStringList lsbImageDates READ getLsbImageDates NOTIFY lsbDbUpdated)
      Q_PROPERTY(QStringList lsbImageAuthorrs READ getLsbImageAuthors NOTIFY lsbDbUpdated)
      Q_PROPERTY(QString password READ getPassword WRITE setPassword NOTIFY passwordChanged)
      Q_PROPERTY(bool cryptoEnabled READ getCryptoEnabled WRITE setCryptoEnabled NOTIFY
                 cryptoEnabledChanged)

   signals:
      void lsbDbUpdated();
      void lsbImageChanged();
      void chatTextUpdated();
      void passwordChanged();
      void peerCountChanged();
      void cryptoEnabledChanged();
      void newParticipant(const QString& name);
      void participantLeft(const QString& name);
      void newMessage(const QString& user, const QString& message);

   public:
      QmlBridge();

      QStringList getPeers() const;
      QStringList getSortTypes() const;
      QStringList getLsbImagePaths() const;
      QStringList getLsbImageDates() const;
      QStringList getLsbImageAuthors() const;

      QString getUserName() const;
      QString getPassword() const;
      bool getCryptoEnabled() const;
      QString getLsbImagesDirectory() const;

   public slots:
      void init();
      void sendFile();
      void saveImages();
      void updateLsbImageDb();
      void openLsbImagesDirectory();
      void changeSortType(const int type);
      void sendMessage(const QString& text);
      void setPassword(const QString& password);
      void setCryptoEnabled(const bool enabled);
      void setLsbSearchQuery(const QString& query);
      void openMessage(const QString& filePath, const QString& password);

   private slots:
      void handleNewParticipant(const QString& name);
      void handleParticipantLeft(const QString& name);
      void handleMessages(const QString& name, const QByteArray& data);

   private:
      QString saveFile(const QString& name, const QByteArray& data, bool* ok);
      QString encodeData(const QByteArray& data, bool* ok, bool* continueSending);

   private:
      QString m_password;
      QStringList m_peers;
      bool m_cryptoEnabled;
      QStringList m_lsbImagePaths;
      QStringList m_lsbImageDates;
      QStringList m_lsbImageAuthors;

      NetworkComms m_comms;
};
