/*!
 \file cpicture.h

*/

#ifndef CPICTURE_H
#define CPICTURE_H


#include <QObject>
#include <QList>
#include <QDateTime>

#include <QMetaType>


/*!
 \brief

 \class cPicture cpicture.h "cpicture.h"
*/
class cPicture : public QObject
{
	Q_OBJECT
public:
	/*!
	 \brief

	 \fn cPicture
	 \param parent
	*/
	explicit cPicture(QObject *parent = nullptr);

	/*!
	 \brief

	 \fn fromFile
	 \param szFileName
	 \return bool
	*/
	bool					fromFile(const QString& szFileName);

	/*!
	 \brief

	 \fn setImageWidth
	 \param imageWidth
	*/
	void					setImageWidth(const qint32& imageWidth);
	/*!
	 \brief

	 \fn imageWidth
	 \return qint32
	*/
	qint32					imageWidth();

	/*!
	 \brief

	 \fn setImageHeight
	 \param imageHeight
	*/
	void					setImageHeight(const qint32& imageHeight);
	/*!
	 \brief

	 \fn imageHeight
	 \return qint32
	*/
	qint32					imageHeight();

	/*!
	 \brief

	 \fn setImageOrientation
	 \param imageOrientation
	*/
	void					setImageOrientation(const qint16& imageOrientation);
	/*!
	 \brief

	 \fn imageOrientation
	 \return qint16
	*/
	qint16					imageOrientation();

	/*!
	 \brief

	 \fn setCameraMake
	 \param cameraMake
	*/
	void					setCameraMake(const QString& cameraMake);
	/*!
	 \brief

	 \fn cameraMake
	 \return QString
	*/
	QString					cameraMake();

	/*!
	 \brief

	 \fn setCameraModel
	 \param cameraModel
	*/
	void					setCameraModel(const QString& cameraModel);
	/*!
	 \brief

	 \fn cameraModel
	 \return QString
	*/
	QString					cameraModel();

	/*!
	 \brief

	 \fn setDateTime
	 \param dateTime
	*/
	void					setDateTime(const QDateTime& dateTime);
	/*!
	 \brief

	 \fn dateTime
	 \return QDateTime
	*/
	QDateTime				dateTime();

	/*!
	 \brief

	 \fn setFNumber
	 \param fNumber
	*/
	void					setFNumber(const QString& fNumber);
	/*!
	 \brief

	 \fn fNumber
	 \return QString
	*/
	QString					fNumber();

	/*!
	 \brief

	 \fn setISO
	 \param iso
	*/
	void					setISO(const qint32& iso);
	/*!
	 \brief

	 \fn iso
	 \return qint32
	*/
	qint32					iso();

	/*!
	 \brief

	 \fn setFlash
	 \param flash
	*/
	void					setFlash(const QString& flash);
	/*!
	 \brief

	 \fn flash
	 \return QString
	*/
	QString					flash();

	/*!
	 \brief

	 \fn setFlashID
	 \param flashID
	*/
	void					setFlashID(const qint32& flashID);
	/*!
	 \brief

	 \fn flashID
	 \return qint32
	*/
	qint32					flashID();

	/*!
	 \brief

	 \fn setFocalLength
	 \param focalLength
	*/
	void					setFocalLength(const qreal& focalLength);
	/*!
	 \brief

	 \fn focalLength
	 \return qreal
	*/
	qreal					focalLength();

	/*!
	 \brief

	 \fn setLensMake
	 \param lensMake
	*/
	void					setLensMake(const QString& lensMake);
	/*!
	 \brief

	 \fn lensMake
	 \return QString
	*/
	QString					lensMake();

	/*!
	 \brief

	 \fn setLensModel
	 \param lensModel
	*/
	void					setLensModel(const QString& lensModel);
	/*!
	 \brief

	 \fn lensModel
	 \return QString
	*/
	QString					lensModel();

	/*!
	 \brief

	 \fn setExposureTime
	 \param exposureTime
	*/
	void					setExposureTime(const QString& exposureTime);
	/*!
	 \brief

	 \fn exposureTime
	 \return QString
	*/
	QString					exposureTime();

	/*!
	 \brief

	 \fn setExposureBias
	 \param exposureBias
	*/
	void					setExposureBias(const qint32& exposureBias);
	/*!
	 \brief

	 \fn exposureBias
	 \return qint32
	*/
	qint32					exposureBias();

	/*!
	 \brief

	 \fn setExifVersion
	 \param exifVersion
	*/
	void					setExifVersion(const QString& exifVersion);
	/*!
	 \brief

	 \fn exifVersion
	 \return QString
	*/
	QString					exifVersion();

	/*!
	 \brief

	 \fn setDateTimeOriginal
	 \param dateTimeOriginal
	*/
	void					setDateTimeOriginal(const QDateTime& dateTimeOriginal);
	/*!
	 \brief

	 \fn dateTimeOriginal
	 \return QDateTime
	*/
	QDateTime				dateTimeOriginal();

	/*!
	 \brief

	 \fn setDateTimeDigitized
	 \param dateTimeDigitized
	*/
	void					setDateTimeDigitized(const QDateTime& dateTimeDigitized);
	/*!
	 \brief

	 \fn dateTimeDigitized
	 \return QDateTime
	*/
	QDateTime				dateTimeDigitized();

	/*!
	 \brief

	 \fn setWhiteBalance
	 \param whiteBalance
	*/
	void					setWhiteBalance(const qint32& whiteBalance);
	/*!
	 \brief

	 \fn whiteBalance
	 \return qint32
	*/
	qint32					whiteBalance();

	/*!
	 \brief

	 \fn setFocalLength35
	 \param focalLength35
	*/
	void					setFocalLength35(const qreal& focalLength35);
	/*!
	 \brief

	 \fn focalLength35
	 \return qreal
	*/
	qreal					focalLength35();

	/*!
	 \brief

	 \fn setGPS
	 \param gps
	*/
	void					setGPS(const QString& gps);
	/*!
	 \brief

	 \fn gps
	 \return QString
	*/
	QString					gps();

	/*!
	 \brief

	 \fn setFileName
	 \param fileName
	*/
	void					setFileName(const QString& fileName);
	/*!
	 \brief

	 \fn fileName
	 \return QString
	*/
	QString					fileName();

	/*!
	 \brief

	 \fn setFilePath
	 \param filePath
	*/
	void					setFilePath(const QString& filePath);
	/*!
	 \brief

	 \fn filePath
	 \return QString
	*/
	QString					filePath();

	/*!
	 \brief

	 \fn setFileSize
	 \param fileSize
	*/
	void					setFileSize(const qint64& fileSize);
	/*!
	 \brief

	 \fn fileSize
	 \return qint64
	*/
	qint64					fileSize();

	/*!
	 \brief

	 \fn setThumbnail
	 \param thumbnail
	*/
	void					setThumbnail(const QImage& thumbnail);
	/*!
	 \brief

	 \fn thumbnail
	 \return QImage
	*/
	QImage					thumbnail();

signals:

public slots:

private:
	QString					m_szFileName;			/*!< TODO: describe */
	QString					m_szFilePath;			/*!< TODO: describe */
	qint64					m_iFileSize;			/*!< TODO: describe */
	qint32					m_imageWidth;			/*!< TODO: describe */
	qint32					m_imageHeight;			/*!< TODO: describe */
	qint16					m_imageOrientation;		/*!< TODO: describe */
	QString					m_cameraMake;			/*!< TODO: describe */
	QString					m_cameraModel;			/*!< TODO: describe */
	QDateTime				m_dateTime;				/*!< TODO: describe */
	QString					m_fNumber;				/*!< TODO: describe */
	qint32					m_iso;					/*!< TODO: describe */
	QString					m_flash;				/*!< TODO: describe */
	qint32					m_flashID;				/*!< TODO: describe */
	qreal					m_focalLength;			/*!< TODO: describe */
	QString					m_lensMake;				/*!< TODO: describe */
	QString					m_lensModel;			/*!< TODO: describe */
	QString					m_exposureTime;			/*!< TODO: describe */
	qint32					m_exposureBias;			/*!< TODO: describe */
	QString					m_exifVersion;			/*!< TODO: describe */
	QDateTime				m_dateTimeOriginal;		/*!< TODO: describe */
	QDateTime				m_dateTimeDigitized;	/*!< TODO: describe */
	qint32					m_whiteBalance;			/*!< TODO: describe */
	qreal					m_focalLength35;		/*!< TODO: describe */
	QString					m_gps;					/*!< TODO: describe */
};

Q_DECLARE_METATYPE(cPicture*)

#endif // CPICTURE_H
