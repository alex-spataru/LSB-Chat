#include <QtTest>
#include <QByteArray>
#include <QCoreApplication>

#include "LSB/LSB.h"
#include "LSB/Crypto.h"

class Tests : public QObject
{
    Q_OBJECT

private slots:
    void testLSB()
    {
        // Enable auto-image generation
        LSB::enableGeneratedImages(true);

        // Set data to encode
        const QByteArray data = "The quick brown fox jumped over the lazy dog";

        // Encode data
        const QImage lsbImage = LSB::encodeData(data);

        // Validate image size
        QVERIFY(lsbImage.width() > 0);
        QVERIFY(lsbImage.height() > 0);

        // Decode data from image
        const QByteArray decoded = LSB::decodeData(lsbImage);

        // Validate that input and output data is the same
        QVERIFY(data == decoded);
    }

    void testCrypto()
    {
        // Define original data
        const QByteArray data = "The quick brown fox jumped over the lazy dog";

        // Define test keys
        const QByteArray key1 = "123";
        const QByteArray key2 = "132";
        const QByteArray key3 = "1234567890abcdefghijklmnopqrstuvwxyz!#$%&/()=?";

        // Init. error variable
        CryptoError error, error1, error2, error3;

        // Checar que el error se cambie cuando metemos una contraseña incorrecta
        Crypto::encryptData(data, "", &error);
        QVERIFY(error == kPasswordEmpty);

        // Hacer encriptaciones para tres llaves diferentes
        const QByteArray cipher1 = Crypto::encryptData(data, key1, &error1);
        const QByteArray cipher2 = Crypto::encryptData(data, key2, &error2);
        const QByteArray cipher3 = Crypto::encryptData(data, key3, &error3);

        // Checar que no haya errores de cifrado
        QVERIFY(error1 == kNoError);
        QVERIFY(error2 == kNoError);
        QVERIFY(error3 == kNoError);

        // Checar que los byte arrays cifrados sean diferentes entre si y que sean dif. a datos
        // originales
        QVERIFY(cipher1 != data);
        QVERIFY(cipher2 != data);
        QVERIFY(cipher3 != data);
        QVERIFY(cipher1 != cipher2);
        QVERIFY(cipher1 != cipher3);
        QVERIFY(cipher2 != cipher3);

        // Descifrar los archivos con las llaves adecuadas
        const QByteArray decipher1 = Crypto::decryptData(cipher1, key1, &error1);
        const QByteArray decipher2 = Crypto::decryptData(cipher2, key2, &error2);
        const QByteArray decipher3 = Crypto::decryptData(cipher3, key3, &error3);

        // Checar que no haya errores de descifrado
        QVERIFY(error1 == kNoError);
        QVERIFY(error2 == kNoError);
        QVERIFY(error3 == kNoError);

        // Checar que todos los archivos descifrados sean iguales a los datos originales
        QVERIFY(decipher1 == data);
        QVERIFY(decipher2 == data);
        QVERIFY(decipher3 == data);

        // Verificar que al usar la llave inadecuada el decif. falla
        const QByteArray badDecipher12 = Crypto::decryptData(cipher1, key2, &error);
        const QByteArray badDecipher13 = Crypto::decryptData(cipher1, key3, &error);
        const QByteArray badDecipher21 = Crypto::decryptData(cipher2, key1, &error);
        const QByteArray badDecipher23 = Crypto::decryptData(cipher2, key3, &error);
        const QByteArray badDecipher31 = Crypto::decryptData(cipher3, key1, &error);
        const QByteArray badDecipher32 = Crypto::decryptData(cipher3, key2, &error);

        // Checar que no haya errores de descifrado
        QVERIFY(badDecipher12 != data);
        QVERIFY(badDecipher13 != data);
        QVERIFY(badDecipher21 != data);
        QVERIFY(badDecipher23 != data);
        QVERIFY(badDecipher31 != data);
        QVERIFY(badDecipher32 != data);
    }
};

QTEST_MAIN(Tests)
#include "TestMain.moc"
