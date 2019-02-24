/*!
 \file cpicture.cpp

*/

#include "cexif.h"
#include "cpicture.h"

#include <QFileInfo>


cPicture::cPicture(QObject *parent) :
	QObject(parent),
	m_szFileName(""),
	m_szFilePath(""),
	m_iFileSize(0),
	m_imageWidth(0),
	m_imageHeight(0),
	m_cameraMake(""),
	m_cameraModel(""),
	m_fNumber(""),
	m_iso(0),
	m_flash(""),
	m_flashID(0),
	m_focalLength(0.0),
	m_lensMake(""),
	m_lensModel(""),
	m_exposureTime(""),
	m_exposureBias(0),
	m_exifVersion(""),
	m_whiteBalance(0),
	m_focalLength35(0.0),
	m_gps("")
{
}

bool cPicture::fromFile(const QString& szFileName)
{
	cEXIF		exif;
	QFileInfo	fileInfo(szFileName);

	if(!exif.fromFile(szFileName))
		return(false);

	m_szFileName			= fileInfo.fileName();
	m_szFilePath			= fileInfo.absolutePath();
	m_iFileSize				= fileInfo.size();
	m_imageWidth			= exif.imageWidth();
	m_imageHeight			= exif.imageHeight();
	m_imageOrientation		= exif.imageOrientation();
	m_cameraMake			= exif.cameraMake();
	m_cameraModel			= exif.cameraModel();
	m_dateTime				= exif.dateTime();
	m_fNumber				= exif.fNumber();
	m_iso					= exif.iso();
	m_flash					= exif.flash();
	m_flashID				= exif.flashID();
	m_focalLength			= exif.focalLength();
	m_lensMake				= exif.lensMake();
	m_lensModel				= exif.lensModel();
	m_exposureTime			= exif.exposureTime();
	m_exposureBias			= exif.exposureBias();
	m_exifVersion			= exif.exifVersion();
	m_dateTimeOriginal		= exif.dateTimeOriginal();
	m_dateTimeDigitized		= exif.dateTimeDigitized();
	m_whiteBalance			= exif.whiteBalance();
	m_focalLength35			= exif.focalLength35();
	m_gps					= exif.gps();

	return(true);
}

void cPicture::setImageWidth(const qint32& imageWidth)
{
	m_imageWidth	= imageWidth;
}

qint32 cPicture::imageWidth()
{
	return(m_imageWidth);
}

void cPicture::setImageHeight(const qint32& imageHeight)
{
	m_imageHeight	= imageHeight;
}

qint32 cPicture::imageHeight()
{
	return(m_imageHeight);
}

void cPicture::setImageOrientation(const qint16& imageOrientation)
{
	m_imageOrientation	= imageOrientation;
}

qint16 cPicture::imageOrientation()
{
	return(m_imageOrientation);
}

void cPicture::setCameraMake(const QString& cameraMake)
{
	m_cameraMake	= cameraMake;
}

QString cPicture::cameraMake()
{
	return(m_cameraMake);
}

void cPicture::setCameraModel(const QString& cameraModel)
{
	m_cameraModel	= cameraModel;
}

QString cPicture::cameraModel()
{
	return(m_cameraModel);
}

void cPicture::setDateTime(const QDateTime& dateTime)
{
	m_dateTime	 = dateTime;
}

QDateTime cPicture::dateTime()
{
	return(m_dateTime);
}

void cPicture::setFNumber(const QString& fNumber)
{
	m_fNumber	= fNumber;
}

QString cPicture::fNumber()
{
	return(m_fNumber);
}

void cPicture::setISO(const qint32& iso)
{
	m_iso	= iso;
}

qint32 cPicture::iso()
{
	return(m_iso);
}

void cPicture::setFlash(const QString& flash)
{
	m_flash	= flash;
}

QString cPicture::flash()
{
	return(m_flash);
}

void cPicture::setFlashID(const qint32& flashID)
{
	m_flashID	= flashID;
}

qint32 cPicture::flashID()
{
	return(m_flashID);
}

void cPicture::setFocalLength(const qreal& focalLength)
{
	m_focalLength	= focalLength;
}

qreal cPicture::focalLength()
{
	return(m_focalLength);
}

void cPicture::setLensMake(const QString& lensMake)
{
	m_lensMake	= lensMake;
}

QString cPicture::lensMake()
{
	return(m_lensMake);
}

void cPicture::setLensModel(const QString& lensModel)
{
	m_lensModel	= lensModel;
}

QString cPicture::lensModel()
{
	return(m_lensModel);
}

void cPicture::setExposureTime(const QString& exposureTime)
{
	m_exposureTime	= exposureTime;
}

QString cPicture::exposureTime()
{
	return(m_exposureTime);
}

void cPicture::setExposureBias(const qint32& exposureBias)
{
	m_exposureBias	= exposureBias;
}

qint32 cPicture::exposureBias()
{
	return(m_exposureBias);
}

void cPicture::setExifVersion(const QString& exifVersion)
{
	m_exifVersion	= exifVersion;
}

QString cPicture::exifVersion()
{
	return(m_exifVersion);
}

void cPicture::setDateTimeOriginal(const QDateTime& dateTimeOriginal)
{
	m_dateTimeOriginal	= dateTimeOriginal;
}

QDateTime cPicture::dateTimeOriginal()
{
	return(m_dateTimeOriginal);
}

void cPicture::setDateTimeDigitized(const QDateTime& dateTimeDigitized)
{
	m_dateTimeDigitized	= dateTimeDigitized;
}

QDateTime cPicture::dateTimeDigitized()
{
	return(m_dateTimeDigitized);
}

void cPicture::setWhiteBalance(const qint32& whiteBalance)
{
	m_whiteBalance	= whiteBalance;
}

qint32 cPicture::whiteBalance()
{
	return(m_whiteBalance);
}

void cPicture::setFocalLength35(const qreal& focalLength35)
{
	m_focalLength35	= focalLength35;
}

qreal cPicture::focalLength35()
{
	return(m_focalLength35);
}

void cPicture::setGPS(const QString& gps)
{
	m_gps	= gps;
}

QString cPicture::gps()
{
	return(m_gps);
}

void cPicture::setFileName(const QString& fileName)
{
	m_szFileName	= fileName;
}

QString cPicture::fileName()
{
	return(m_szFileName);
}

void cPicture::setFilePath(const QString& filePath)
{
	m_szFilePath	= filePath;
}

QString cPicture::filePath()
{
	return(m_szFilePath);
}

void cPicture::setFileSize(const qint64& fileSize)
{
	m_iFileSize	= fileSize;
}

qint64 cPicture::fileSize()
{
	return(m_iFileSize);
}
