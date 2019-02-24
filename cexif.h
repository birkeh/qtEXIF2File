/*!
 \file cexif.h

*/

#ifndef CEXIF_H
#define CEXIF_H

#include <QString>
#include <QVariant>
#include <QDateTime>

#include <QMetaType>
#include <QList>


/*!
 \brief

 \class cEXIFCompression cexif.h "cexif.h"
*/
class cEXIFCompression
{
public:
	cEXIFCompression(const qint32& iID, const QString& szCompression);

	qint32		m_iID;								/*!< TODO: describe */
	QString		m_szCompression;					/*!< TODO: describe */
};

Q_DECLARE_METATYPE(cEXIFCompression*)

/*!
 \brief

 \class cEXIFCompressionList cexif.h "cexif.h"
*/
class cEXIFCompressionList : public QList<cEXIFCompression*>
{
public:
	cEXIFCompressionList();

	/*!
	 \brief

	 \fn add
	 \param iID
	 \param szCompression
	 \return cEXIFCompression
	*/
	cEXIFCompression*	add(const qint32& iID, const QString& szCompression);
	/*!
	 \brief

	 \fn find
	 \param iID
	 \return cEXIFCompression
	*/
	cEXIFCompression*	find(const qint32& iID);

	/*!
	 \brief

	 \fn compression
	 \param iID
	 \return QString
	*/
	QString				compression(const qint32& iID);
};

Q_DECLARE_METATYPE(cEXIFCompressionList)

/*!
 \brief

 \class cEXIFLightSource cexif.h "cexif.h"
*/
class cEXIFLightSource
{
public:
	cEXIFLightSource(const qint32& iID, const QString& szLightSource);

	qint32		m_iID;					/*!< TODO: describe */
	QString		m_szLightSource;		/*!< TODO: describe */
};

Q_DECLARE_METATYPE(cEXIFLightSource*)

/*!
 \brief

 \class cEXIFLightSourceList cexif.h "cexif.h"
*/
class cEXIFLightSourceList : public QList<cEXIFLightSource*>
{
public:
	cEXIFLightSourceList();

	/*!
	 \brief

	 \fn add
	 \param iID
	 \param szLightSource
	 \return cEXIFLightSource
	*/
	cEXIFLightSource*	add(const qint32& iID, const QString& szLightSource);
	/*!
	 \brief

	 \fn find
	 \param iID
	 \return cEXIFLightSource
	*/
	cEXIFLightSource*	find(const qint32& iID);

	/*!
	 \brief

	 \fn lightSource
	 \param iID
	 \return QString
	*/
	QString			lightSource(const qint32& iID);
};

/*!
 \brief

 \class cEXIFFlash cexif.h "cexif.h"
*/
class cEXIFFlash
{
public:
	cEXIFFlash(const qint32& iID, const QString& szFlash);

	qint32		m_iID;					/*!< TODO: describe */
	QString		m_szFlash;				/*!< TODO: describe */
};

Q_DECLARE_METATYPE(cEXIFFlash*)

/*!
 \brief

 \class cEXIFFlashList cexif.h "cexif.h"
*/
class cEXIFFlashList : public QList<cEXIFFlash*>
{
public:
	cEXIFFlashList();

	/*!
	 \brief

	 \fn add
	 \param iID
	 \param szFlash
	 \return cEXIFFlash
	*/
	cEXIFFlash*		add(const qint32& iID, const QString& szFlash);
	/*!
	 \brief

	 \fn find
	 \param iID
	 \return cEXIFFlash
	*/
	cEXIFFlash*		find(const qint32& iID);

	/*!
	 \brief

	 \fn flash
	 \param iID
	 \return QString
	*/
	QString			flash(const qint32& iID);
};

/**
 * @brief
 *
 */
class cEXIFTag
{
public:
	cEXIFTag(const qint32& iTAGID, const QString& szTAGName, const qint32& iIFDID, const qint32& iTypeID, const QString& szDescription);

	qint32		m_iTAGID;					/*!< TODO: describe */
	QString		m_szTAGName;				/*!< TODO: describe */
	qint32		m_iIFDID;					/*!< TODO: describe */
	qint32		m_iTypeID;					/*!< TODO: describe */
	QString		m_szDescription;			/*!< TODO: describe */
};

Q_DECLARE_METATYPE(cEXIFTag*)

/*!
 \brief

 \class cEXIFTagList cexif.h "cexif.h"
*/
class cEXIFTagList : public QList<cEXIFTag*>
{
public:
	cEXIFTagList();

	/*!
	 \brief

	 \fn add
	 \param iTAGID
	 \param szTAGName
	 \param iIFDID
	 \param iTypeID
	 \param szDescription
	 \return cEXIFTag
	*/
	cEXIFTag*		add(const qint32& iTAGID, const QString& szTAGName, const qint32& iIFDID, const qint32& iTypeID, const QString& szDescription);
	/*!
	 \brief

	 \fn find
	 \param iTAGID
	 \param iIFDID
	 \return cEXIFTag
	*/
	cEXIFTag*		find(const qint32& iTAGID, const qint32& iIFDID);
};

/*!
 \brief

*/
class cEXIFValue
{
public:
	cEXIFValue(cEXIFTag* lpEXIFTag);

	/*!
	 \brief

	 \fn exifTag
	 \return cEXIFTag
	*/
	cEXIFTag*		exifTag();
	/*!
	 \brief

	 \fn setValue
	 \param szValue
	 \param iTypeId
	 \param iCount
	*/
	void			setValue(const QString& szValue, qint32 iTypeId, qint32 iCount = 1);
	/*!
	 \brief

	 \fn value
	 \return QVariant
	*/
	QVariant		value();
	/*!
	 \brief

	 \fn valueList
	 \return QList<QVariant>
	*/
	QList<QVariant>	valueList();

private:
	cEXIFTag*		m_lpEXIFTag;				/*!< TODO: describe */
	QList<QVariant>	m_valueList;				/*!< TODO: describe */

	/*!
	 \brief

	 \fn convertValue
	 \param szValue
	 \param iTypeId
	 \return QVariant
	*/
	QVariant		convertValue(const QString& szValue, qint32 iTypeId);
};

Q_DECLARE_METATYPE(cEXIFValue*)

/*!
 \brief

 \class cEXIFValueList cexif.h "cexif.h"
*/
class cEXIFValueList : public QList<cEXIFValue*>
{
public:
	cEXIFValueList();

	/*!
	 \brief

	 \fn add
	 \param lpEXIFTag
	 \return cEXIFValue
	*/
	cEXIFValue*		add(cEXIFTag* lpEXIFTag);
	/*!
	 \brief

	 \fn find
	 \param lpEXIFTag
	 \return cEXIFValue
	*/
	cEXIFValue*		find(cEXIFTag* lpEXIFTag);
};

/*!
 \brief

 \class cEXIF cexif.h "cexif.h"
*/
class cEXIF
{
public:
	cEXIF();

	/*!
	 \brief

	 \fn fromFile
	 \param szFileName
	 \return bool
	*/
	bool					fromFile(const QString& szFileName);

	/*!
	 \brief

	 \fn imageWidth
	 \return qint32
	*/
	qint32					imageWidth();
	/*!
	 \brief

	 \fn imageHeight
	 \return qint32
	*/
	qint32					imageHeight();
	/*!
	 \brief

	 \fn imageOrientation
	 \return qint16
	*/
	qint16					imageOrientation();
	/*!
	 \brief

	 \fn cameraMake
	 \return QString
	*/
	QString					cameraMake();
	/*!
	 \brief

	 \fn cameraModel
	 \return QString
	*/
	QString					cameraModel();
	/*!
	 \brief

	 \fn dateTime
	 \return QDateTime
	*/
	QDateTime				dateTime();
	/*!
	 \brief

	 \fn fNumber
	 \return QString
	*/
	QString					fNumber();
	/*!
	 \brief

	 \fn iso
	 \return qint32
	*/
	qint32					iso();
	/*!
	 \brief

	 \fn flash
	 \return QString
	*/
	QString					flash();
	/*!
	 \brief

	 \fn flashID
	 \return qint32
	*/
	qint32					flashID();
	/*!
	 \brief

	 \fn focalLength
	 \return qreal
	*/
	qreal					focalLength();
	/*!
	 \brief

	 \fn lensMake
	 \return QString
	*/
	QString					lensMake();
	/*!
	 \brief

	 \fn lensModel
	 \return QString
	*/
	QString					lensModel();
	/*!
	 \brief

	 \fn exposureTime
	 \return QString
	*/
	QString					exposureTime();
	/*!
	 \brief

	 \fn exposureBias
	 \return qint32
	*/
	qint32					exposureBias();
	/*!
	 \brief

	 \fn exifVersion
	 \return QString
	*/
	QString					exifVersion();
	/*!
	 \brief

	 \fn dateTimeOriginal
	 \return QDateTime
	*/
	QDateTime				dateTimeOriginal();
	/*!
	 \brief

	 \fn dateTimeDigitized
	 \return QDateTime
	*/
	QDateTime				dateTimeDigitized();
	/*!
	 \brief

	 \fn whiteBalance
	 \return qint32
	*/
	qint32					whiteBalance();
	/*!
	 \brief

	 \fn focalLength35
	 \return qreal
	*/
	qreal					focalLength35();
	/*!
	 \brief

	 \fn gps
	 \return QString
	*/
	QString					gps();
	/*!
	 \brief

	 \fn fileName
	 \return QString
	*/
	QString					fileName();
	/*!
	 \brief

	 \fn previewList
	 \return QList<QImage>
	*/
	QList<QImage>			previewList();
	/*!
	 \brief

	 \fn thumbnail
	 \return QImage
	*/
	QImage					thumbnail();

private:
	cEXIFValueList			m_exifValueList;				/*!< TODO: describe */
	qint32					m_iWidth;						/*!< TODO: describe */
	qint32					m_iHeight;						/*!< TODO: describe */
	QString					m_szFileName;					/*!< TODO: describe */

	cEXIFCompressionList	m_exifCompressionList;			/*!< TODO: describe */
	cEXIFLightSourceList	m_exifLightSourceList;			/*!< TODO: describe */
	cEXIFFlashList			m_exifFlashList;				/*!< TODO: describe */
	cEXIFTagList			m_exifTagList;					/*!< TODO: describe */

	/*!
	 \brief

	 \fn getTag
	 \param iTAGID
	 \param iIFDID
	 \return QVariant
	*/
	QVariant				getTag(qint32 iTAGID, qint32 iIFDID);
	/*!
	 \brief

	 \fn getTagList
	 \param iTAGID
	 \param iIFDID
	 \return QList<QVariant>
	*/
	QList<QVariant>			getTagList(qint32 iTAGID, qint32 iIFDID);
};

#endif // CEXIF_H
