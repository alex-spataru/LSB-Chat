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

#include "QmlBridge.h"
#include "LSB/Crypto.h"

#include <QDir>
#include <QUrl>
#include <QFileDialog>
#include <QMessageBox>
#include <QJsonObject>
#include <QRandomGenerator>
#include <QDesktopServices>

/*
 * Set maximum transfer limit to 1 MB
 */
static qint64 MAX_TRANSFER_SIZE = 1 * 1024;

/**
 * @brief GET_JSON_DATA
 * @param type
 * @param filename
 * @param base64
 * @param encrypted
 * @return
 *
 * Generates a binary JSON representation of the given data
 */
static QByteArray GET_JSON_DATA(const QString& type,
                                const QString& filename,
                                const QString& base64,
                                const bool encrypted)
{
    // Generate JSON object
    QJsonObject jsonObject;
    jsonObject.insert("MessageType", type);
    jsonObject.insert("Encrypted", QJsonValue(encrypted));
    jsonObject.insert("Length", QJsonValue(base64.length()));
    jsonObject.insert("FileName", filename);
    jsonObject.insert("Base64", QJsonValue(base64));

    // Return byte array
    return QJsonDocument(jsonObject).toJson(QJsonDocument::Compact);
}

QmlBridge::QmlBridge()
{
    setCryptoEnabled(false);
    connect(&m_comms, SIGNAL(newParticipant(QString)),
            this,     SIGNAL(newParticipant(QString)));
    connect(&m_comms, SIGNAL(participantLeft(QString)),
            this,     SIGNAL(participantLeft(QString)));
    connect(&m_comms, SIGNAL(newMessage(QString, QByteArray)),
            this,       SLOT(handleMessages(QString, QByteArray)));
    connect(this,     SIGNAL(newParticipant(QString)),
            this,       SLOT(handleNewParticipant(QString)));
    connect(this,     SIGNAL(participantLeft(QString)),
            this,       SLOT(handleParticipantLeft(QString)));
}

QStringList QmlBridge::getPeers() const
{
    return m_peers;
}

QImage QmlBridge::userImage() const
{
    return m_userImage;
}

QString QmlBridge::getUserName() const
{
    return m_comms.username();
}

QString QmlBridge::getPassword() const
{
    return m_password;
}

bool QmlBridge::getCryptoEnabled() const
{
    return m_cryptoEnabled;
}

bool QmlBridge::getGenerateImagesEnabled() const
{
    return LSB::useGeneratedImages();
}

void QmlBridge::init()
{
    newParticipant(getUserName());
    enableGeneratedImages(true);
}

void QmlBridge::sendFile()
{
    // Get file path
    const QString path = QFileDialog::getOpenFileName(Q_NULLPTR,
                                                      tr("Choose file to share"),
                                                      QDir::homePath());

    // File path empty, abort
    if(path.isEmpty())
        return;

    // Load file contents
    QFile file(path);
    QByteArray fileData;
    if(file.open(QFile::ReadOnly)) {
        // File is too large, abort
        if(file.size() > MAX_TRANSFER_SIZE) {
            QMessageBox::warning(Q_NULLPTR,
                                 tr("File too large"),
                                 tr("The file is too large to be sent, sorry"));
            file.close();
            return;
        }

        // Read data
        fileData = file.readAll();
        file.close();
    }

    // Check if file is empty
    if(fileData.isEmpty()) {
        QMessageBox::warning(Q_NULLPTR,
                             tr("Empty file"),
                             tr("The file is empty, please check it before sending it!"));
        return;
    }

    // Get filename (e.g. if filepath is "~/Desktop/image.jpg", we only need "image.jpg")
    QFileInfo fileInfo(path);
    QString fileName = fileInfo.fileName();

    // Encrypt file (if required) and encode it with Base64
    bool encryptionOk;
    bool allowSendingData;
    QString base64 = encodeData(fileData, &encryptionOk, &allowSendingData);

    // Abort if user denied sending data
    if(!allowSendingData)
        return;

    // Generate JSON binary data, load data into image and send image data
    QByteArray json = GET_JSON_DATA("File",fileName, base64, encryptionOk);
    QImage lsbImage = LSB::encodeData(json);
    m_comms.sendBinaryData(LSB::imageToBinaryData(lsbImage));

    // Generate message
    QUrl url = QUrl::fromLocalFile(path);
    QString message = tr("Sent file \"%1\", <a href=\"%2\">click here to open it</a>.")
                      .arg(fileName)
                      .arg(url.toString());

    // Emit signal
    emit lsbImageChanged();
    emit newMessage(getUserName(), message);
}

void QmlBridge::saveImages()
{
    // Get folder where to save images
    const QString path = QFileDialog::getExistingDirectory(Q_NULLPTR,
                         tr("Select Directory to Save Files"),
                         QDir::homePath());

    // File path empty, abort
    if(path.isEmpty())
        return;

    // Get LSB images
    QImage data = LSB::currentImageData();
    QImage composite = LSB::currentCompositeImage();

    // Save both LSB images as PNG
    bool ok = true;
    ok &= data.save(QDir(path).filePath("LSB_Data.png"), "PNG", 100);
    ok &= composite.save(QDir(path).filePath("LSB_Composite.png"), "PNG", 100);

    // Check if images where saved correctly
    if(!ok) {
        QMessageBox::critical(Q_NULLPTR,
                              tr("Saving Error"),
                              tr("There was an error while exporting the images, wrong permissions?"));
        return;
    }

    // Get file URLs
    QUrl dataUrl = QUrl::fromLocalFile(QDir(path).filePath("LSB_Data.png"));
    QUrl compositeUrl = QUrl::fromLocalFile(QDir(path).filePath("LSB_Composite.png"));

    // Open files
    QDesktopServices::openUrl(dataUrl);
    QDesktopServices::openUrl(compositeUrl);
}

void QmlBridge::extractInformation()
{
    // Select image
    const QString filePath = QFileDialog::getOpenFileName(Q_NULLPTR,
                             tr("Select Image"),
                             QDir::homePath(),
                             tr("Images (*.png)"));

    // File path empty, abort
    if(filePath.isEmpty())
        return;

    // Read image data
    QFile file(filePath);
    if (file.open(QFile::ReadOnly)) {
        handleMessages("LSB Reader (Local)", file.readAll());
        file.close();
    }

    // Error opening file
    else
        QMessageBox::warning(Q_NULLPTR,
                             tr("Error loading image"),
                             tr("Cannot open file for reading, wrong permisions?"));
}

void QmlBridge::selectLsbImageSource()
{
    // Select image
    const QString filePath = QFileDialog::getOpenFileName(Q_NULLPTR,
                             tr("Select Image"),
                             QDir::homePath(),
                             tr("Images (*.png *.xpm *.jpg *.jpeg)"));

    // File path empty, abort
    if(filePath.isEmpty())
        return;

    // Change user image
    QImage image;
    if (image.load(filePath)) {
        m_userImage = image;
        LSB::setSourceImage(userImage());
        enableGeneratedImages(false);
    }
}

void QmlBridge::sendMessage(const QString& text)
{
    // Text is empty abort
    if(text.isEmpty())
        return;

    // Check text size
    if(text.length() > MAX_TRANSFER_SIZE) {
        QMessageBox::warning(Q_NULLPTR,
                             tr("Message too large"),
                             tr("The message is too large to be sent, sorry"));
        return;
    }

    // Encrypt the text (if required) and encode it with Base64
    bool encryptionOk;
    bool allowSendingData;
    QString base64 = encodeData(text.toUtf8(), &encryptionOk, &allowSendingData);

    // Abort if user denied sending data
    if(!allowSendingData)
        return;

    // Generate JSON binary data, load data into image and send image data
    QByteArray json = GET_JSON_DATA("Text", "", base64, encryptionOk);
    QImage lsbImage = LSB::encodeData(json);
    m_comms.sendBinaryData(LSB::imageToBinaryData(lsbImage));

    // Emit signal
    emit lsbImageChanged();
    emit newMessage(getUserName(), text);
}

void QmlBridge::setPassword(const QString& password)
{
    m_password = password;
    emit passwordChanged();
}

void QmlBridge::setCryptoEnabled(const bool enabled)
{
    m_cryptoEnabled = enabled;
    emit cryptoEnabledChanged();
}

void QmlBridge::enableGeneratedImages(const bool enabled)
{
    LSB::enableGeneratedImages(enabled);

    if(!enabled)
        LSB::setSourceImage(userImage());

    emit lsbImageChanged();
    emit lsbImageSourceChanged();
}

void QmlBridge::handleNewParticipant(const QString& name)
{
    if(!getPeers().contains(name)) {
        m_peers.append(name);
        emit peerCountChanged();
    }
}

void QmlBridge::handleParticipantLeft(const QString& name)
{
    if(getPeers().contains(name)) {
        m_peers.removeAt(m_peers.indexOf(name));
        emit peerCountChanged();
    }
}

void QmlBridge::handleMessages(const QString& name, const QByteArray& data)
{
    // Ignore empty packets
    if(data.isEmpty())
        return;

    // Decode LSB and obtain JSON document
    QByteArray jsonData = LSB::decodeData(data);
    QJsonDocument document = QJsonDocument::fromJson(jsonData);

    // Update the LSB image
    emit lsbImageChanged();

    // Get data from JSON object
    const int length = document.object().value("Length").toInt();
    const bool encrypted = document.object().value("Encrypted").toBool();
    const QString fileName = document.object().value("FileName").toString();
    const QString messageType = document.object().value("MessageType").toString();
    const QString base64 = document.object().value("Base64").toString();

    // Cancel if size does not match
    if(length != base64.length() || base64.isEmpty())
        return;

    // Convert from Base64 to normal data
    QByteArray msgData = QByteArray::fromBase64(base64.toUtf8());

    // Decrypt data if possible
    if(encrypted) {
        // User has no password set, abort
        if (getPassword().isEmpty())
            return;

        // Try to decrypt data with current password
        CryptoError error;
        QByteArray decrypted = Crypto::decryptData(msgData, getPassword().toUtf8(), &error);

        // No error, replace msgData with decrypted data
        if (error == kNoError)
            msgData = decrypted;
    }

    // Data is a message -> display it on the chat room
    if(messageType == "Text")
        emit newMessage(name, QString::fromUtf8(msgData));

    // Data is a file -> save it to downloads and generate message
    else if(messageType == "File") {
        // Try to save the file
        bool ok;
        QString filePath = saveFile(fileName, msgData, &ok);
        QUrl url = QUrl::fromLocalFile(filePath);

        // File saved correctly, generate message with link to file
        if(ok) {
            QString message = tr("Sent file \"%1\", <a href=\"%2\">click here to open it</a>.")
                              .arg(fileName)
                              .arg(url.toString());

            emit newMessage(name, message);
        }
    }
}

QString QmlBridge::saveFile(const QString& name, const QByteArray& data, bool* ok)
{
    // Check arguments
    Q_ASSERT(ok);

    // Data or name is invalid
    if(name.isEmpty() || data.isEmpty()) {
        *ok = false;
        return "";
    }

    // Save file to downloads folder
    QDir downloadsPath(QDir::homePath() + "/Downloads/" + qAppName() + "/");

    // Create downloads folder if it does not exist
    if(!downloadsPath.exists())
        downloadsPath.mkpath(".");

    // Create file for writting
    QFile file(downloadsPath.filePath(name));
    if(file.open(QFile::WriteOnly)) {
        file.write(data);
        *ok = true;
        file.close();
        return file.fileName();
    }

    // Error
    *ok = false;
    return "";
}

QString QmlBridge::encodeData(const QByteArray& data, bool* cryptoOk, bool* continueSending)
{
    // Check arguments
    Q_ASSERT(cryptoOk);
    Q_ASSERT(continueSending);

    // Try to encrypt the data, if an error occurs, ask the user what to do
    if(getCryptoEnabled()) {
        // Try to encrypt the data
        CryptoError error;
        QByteArray encryptedData = Crypto::encryptData(data, getPassword().toUtf8(), &error);

        // Password empty, ask user if he/she wants to contine
        if(error == kPasswordEmpty) {
            int ret = QMessageBox::question(Q_NULLPTR,
                                            tr("Empty Password"),
                                            tr("The password is empty, so the data will not be " \
                                               "encrypted, do you want to continue?"),
                                            QMessageBox::Yes | QMessageBox::No);
            if(ret == QMessageBox::No) {
                *cryptoOk = false;
                *continueSending = false;
            }

            else {
                *cryptoOk = false;
                encryptedData = data;
                *continueSending = true;
            }
        }

        // Unknown error
        else if(error == kUnknownError) {
            int ret = QMessageBox::question(Q_NULLPTR,
                                            tr("Encryption error"),
                                            tr("There was an error while encrypting the data. " \
                                               "Would you like to send the unencrypted data?"),
                                            QMessageBox::Yes | QMessageBox::No);
            if(ret == QMessageBox::No) {
                *cryptoOk = false;
                *continueSending = false;
            }

            else {
                *cryptoOk = false;
                encryptedData = data;
                *continueSending = true;
            }
        }

        // Encryption ok, set encryption flag to true
        else {
            *cryptoOk = true;
            *continueSending = true;
        }

        // Return obtained data
        return QString::fromUtf8(encryptedData.toBase64());
    }

    // Encryption disabled, send original data
    *cryptoOk = false;
    *continueSending = true;
    return QString::fromUtf8(data.toBase64());
}
