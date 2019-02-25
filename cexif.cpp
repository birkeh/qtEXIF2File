/*!
 \file cexif.cpp

*/

#include "cexif.h"

#include <QDebug>
#include <QFileInfo>

#include <exiv2\exiv2.hpp>


cEXIF::cEXIF() :
	m_iWidth(0),
	m_iHeight(0),
	m_szFileName("")
{
}

bool cEXIF::fromFile(const QString& szFileName)
{
	if(!QFile::exists(szFileName))
		return(false);

	m_exifValueList.clear();

	m_szFileName	= "";

	Exiv2::Image::UniquePtr	image;

	try
	{
		image	= Exiv2::ImageFactory::open(szFileName.toLocal8Bit().toStdString());
	}
	catch (Exiv2::AnyError& e)
	{
		qDebug() << e.what();
//		std::cout << "Caught Exiv2 exception '" << e << "'\n";
		return(false);
	}

	if(!image.get())
		return(false);

	image->readMetadata();

	Exiv2::ExifData&				exifData	= image->exifData();

	m_iWidth		= image->pixelWidth();
	m_iHeight		= image->pixelHeight();
	m_szFileName	= szFileName;

	if(!exifData.empty())
	{
		Exiv2::ExifData::const_iterator	end			= exifData.end();
		for(Exiv2::ExifData::const_iterator i = exifData.begin(); i != end; ++i)
		{
			cEXIFTag*	lpTag	= m_exifTagList.find(i->tag(), i->ifdId());

			if(lpTag)
			{
				cEXIFValue*	lpValue	= m_exifValueList.add(lpTag);
				if(lpValue)
					lpValue->setValue(QString::fromStdString(i->value().toString()), i->typeId(), i->count());
			}
		}
	}

	return(true);
}

qint32 cEXIF::imageWidth()
{
	if(m_iWidth)
		return(m_iWidth);

	return(getTag(0x0100, 1).value<qint32>());
}

qint32 cEXIF::imageHeight()
{
	if(m_iHeight)
		return(m_iHeight);

	return(getTag(0x0101, 1).value<qint32>());
}

qint16 cEXIF::imageOrientation()
{
	return(getTag(0x0112, 1).value<qint16>());
}

QString cEXIF::cameraMake()
{
	return(getTag(0x010f, 1).value<QString>());
}

QString cEXIF::cameraModel()
{
	return(getTag(0x0110, 1).value<QString>());
}

QDateTime cEXIF::dateTime()
{
	QDateTime	dateTime	= QDateTime::fromString(getTag(0x0132, 1).value<QString>(), "yyyy:MM:dd hh:mm:ss");

	return(dateTime);
}

QString cEXIF::fNumber()
{
	return(getTag(0x829d, 5).value<QString>());
}

qint32 cEXIF::iso()
{
	return(getTag(0x8827, 5).value<qint32>());
}

QString cEXIF::flash()
{
	return(m_exifFlashList.flash(getTag(0x9209, 5).value<qint32>()));
}

qint32 cEXIF::flashID()
{
	return(getTag(0x9209, 5).value<qint32>());
}

qreal cEXIF::focalLength()
{
	return(getTag(0x920a, 5).value<qreal>());
}

QString cEXIF::lensMake()
{
	return(getTag(0xa433, 5).value<QString>());
}

QString cEXIF::lensModel()
{
	return(getTag(0xa434, 5).value<QString>());
}

QString cEXIF::exposureTime()
{
	qreal	value	= getTag(0x829a, 5).value<qreal>();

	if(value == 0.0)
		return("error");

	if(value >= 1.0)
		return(QString("%1 sec").arg(value));

	return(QString("1/%1").arg(1/value));
}

qint32 cEXIF::exposureBias()
{
	return(getTag(0x9204, 5).value<qint32>());
}

QString cEXIF::exifVersion()
{
	return(getTag(0x9000, 5).value<QString>());
}

QDateTime cEXIF::dateTimeOriginal()
{
	return(QDateTime::fromString(getTag(0x9003, 5).value<QString>(), "yyyy:MM:dd hh:mm:ss"));
}

QDateTime cEXIF::dateTimeDigitized()
{
	return(QDateTime::fromString(getTag(0x9004, 5).value<QString>(), "yyyy:MM:dd hh:mm:ss"));
}

qint32 cEXIF::whiteBalance()
{
	return(getTag(0xa403, 5).value<qint32>());
}

qreal cEXIF::focalLength35()
{
	return(getTag(0xa405, 5).value<qreal>());
}

QString cEXIF::gps()
{
	QList<QVariant> nList	= getTagList(0x0002, 6);
	QList<QVariant> eList	= getTagList(0x0004, 6);

	if(nList.count() != 3)
		return("");
	if(eList.count() != 3)
		return("");

	QString szGPS	= QString("%1 %2° %3' %4\" %5 %6° %7' %8\"").arg(getTag(0x0001, 6).value<QString>()).arg(nList[0].value<QString>()).arg(nList[1].value<QString>()).arg(nList[2].value<QString>()).arg(getTag(0x0001, 6).value<QString>()).arg(eList[0].value<QString>()).arg(eList[1].value<QString>()).arg(eList[2].value<QString>());
	return(szGPS);
}

QString cEXIF::fileName()
{
	return(m_szFileName);
}

QVariant cEXIF::getTag(qint32 iTAGID, qint32 iIFDID)
{
	cEXIFTag*	lpTag	= m_exifTagList.find(iTAGID, iIFDID);

	if(!lpTag)
		return(QVariant());

	cEXIFValue*	lpValue	= m_exifValueList.find(lpTag);

	if(!lpValue)
		return(QVariant());

	return(lpValue->value());
}

QList<QVariant> cEXIF::getTagList(qint32 iTAGID, qint32 iIFDID)
{
	cEXIFTag*	lpTag	= m_exifTagList.find(iTAGID, iIFDID);

	if(!lpTag)
		return(QList<QVariant>());

	cEXIFValue*	lpValue	= m_exifValueList.find(lpTag);

	if(!lpValue)
		return(QList<QVariant>());

	return(lpValue->valueList());
}

cEXIFCompression::cEXIFCompression(const qint32& iID, const QString& szCompression) :
	m_iID(iID),
	m_szCompression(szCompression)
{
}

cEXIFCompressionList::cEXIFCompressionList()
{
	add(0x0001, QObject::tr("Uncompressed"));
	add(0x0002, QObject::tr("CCITT 1D"));
	add(0x0003, QObject::tr("T4/Group 3 Fax"));
	add(0x0004, QObject::tr("T6/Group 4 Fax"));
	add(0x0005, QObject::tr("LZW"));
	add(0x0006, QObject::tr("JPEG (old-style)"));
	add(0x0007, QObject::tr("JPEG"));
	add(0x0008, QObject::tr("Adobe Deflate"));
	add(0x0009, QObject::tr("JBIG B&W"));
	add(0x000A, QObject::tr("JBIG Color"));
	add(0x0063, QObject::tr("JPEG"));
	add(0x0106, QObject::tr("Kodak 262"));
	add(0x7FFE, QObject::tr("Next"));
	add(0x7FFF, QObject::tr("Sony ARW Compressed"));
	add(0x8001, QObject::tr("Packed RAW"));
	add(0x8002, QObject::tr("Samsung SRW Compressed"));
	add(0x8003, QObject::tr("CCIRLEW"));
	add(0x8004, QObject::tr("Samsung SRW Compressed 2"));
	add(0x8005, QObject::tr("PackBits"));
	add(0x8029, QObject::tr("Thunderscan"));
	add(0x8063, QObject::tr("Kodak KDC Compressed"));
	add(0x807F, QObject::tr("IT8CTPAD"));
	add(0x8080, QObject::tr("IT8LW"));
	add(0x8081, QObject::tr("IT8MP"));
	add(0x8082, QObject::tr("IT8BL"));
	add(0x808C, QObject::tr("PixarFilm"));
	add(0x808D, QObject::tr("PixarLog"));
	add(0x80B2, QObject::tr("Deflate"));
	add(0x80B3, QObject::tr("DCS"));
	add(0x80EB, QObject::tr("Aperio JPEG 2000 YCbCr"));
	add(0x80ED, QObject::tr("Aperio JPEG 2000 RGB"));
	add(0x8765, QObject::tr("JBIG"));
	add(0x8774, QObject::tr("SGILog"));
	add(0x8775, QObject::tr("SGILog24"));
	add(0x8798, QObject::tr("JPEG 2000"));
	add(0x8799, QObject::tr("Nikon NEF Compressed"));
	add(0x879B, QObject::tr("JBIG2 TIFF FX"));
	add(0x879E, QObject::tr("Microsoft Document Imaging (MDI) Binary Level Codec"));
	add(0x879F, QObject::tr("Microsoft Document Imaging (MDI) Progressive Transform Codec"));
	add(0x87A0, QObject::tr("Microsoft Document Imaging (MDI) Vector"));
	add(0x8847, QObject::tr("ESRI Lerc"));
	add(0x884C, QObject::tr("Lossy JPEG"));
	add(0x886D, QObject::tr("LZMA2"));
	add(0x886E, QObject::tr("Zstd"));
	add(0x886F, QObject::tr("WebP"));
	add(0x8875, QObject::tr("PNG"));
	add(0x8876, QObject::tr("JPEG XR"));
	add(0xFDE8, QObject::tr("Kodak DCR Compressed"));
	add(0xFFFF, QObject::tr("Pentax PEF Compressed"));
}

cEXIFCompression* cEXIFCompressionList::add(const qint32& iID, const QString& szCompression)
{
	cEXIFCompression*	lpNew	= find(iID);

	if(lpNew)
		return(nullptr);

	lpNew	= new cEXIFCompression(iID, szCompression);
	append(lpNew);
	return(lpNew);
}

cEXIFCompression* cEXIFCompressionList::find(const qint32& iID)
{
	for(int x = 0;x < count();x++)
	{
		cEXIFCompression*	lpCompression= at(x);

		if(lpCompression->m_iID == iID)
			return(lpCompression);
	}
	return(nullptr);
}

QString cEXIFCompressionList::compression(const qint32& iID)
{
	cEXIFCompression* lpCompression	= find(iID);
	if(!lpCompression)
		return(QObject::tr("unknown"));
	return(lpCompression->m_szCompression);
}

cEXIFLightSource::cEXIFLightSource(const qint32& iID, const QString& szLightSource) :
	m_iID(iID),
	m_szLightSource(szLightSource)
{
}

cEXIFLightSourceList::cEXIFLightSourceList()
{
	add(0x0001, QObject::tr("Daylight"));
	add(0x0002, QObject::tr("Fluorescent"));
	add(0x0003, QObject::tr("Tungsten (Incandescent)"));
	add(0x0004, QObject::tr("Flash"));
	add(0x0009, QObject::tr("Fine Weather"));
	add(0x000A, QObject::tr("Cloudy"));
	add(0x000B, QObject::tr("Shade"));
	add(0x000C, QObject::tr("Daylight Fluorescent"));
	add(0x000D, QObject::tr("Day White Fluorescent"));
	add(0x000E, QObject::tr("Cool White Fluorescent"));
	add(0x000F, QObject::tr("White Fluorescent"));
	add(0x0010, QObject::tr("Warm White Fluorescent"));
	add(0x0011, QObject::tr("Standard Light A"));
	add(0x0012, QObject::tr("Standard Light B"));
	add(0x0013, QObject::tr("Standard Light C"));
	add(0x0014, QObject::tr("D55"));
	add(0x0015, QObject::tr("D65"));
	add(0x0016, QObject::tr("D75"));
	add(0x0017, QObject::tr("D50"));
	add(0x0018, QObject::tr("ISO Studio Tungsten"));
	add(0x00FF, QObject::tr("Other"));
}

cEXIFLightSource* cEXIFLightSourceList::add(const qint32& iID, const QString& szLightSource)
{
	cEXIFLightSource*	lpNew	= find(iID);

	if(lpNew)
		return(nullptr);

	lpNew	= new cEXIFLightSource(iID, szLightSource);
	append(lpNew);
	return(lpNew);
}

cEXIFLightSource* cEXIFLightSourceList::find(const qint32& iID)
{
	for(int x = 0;x < count();x++)
	{
		cEXIFLightSource*	lpLightSource	= at(x);

		if(lpLightSource->m_iID == iID)
			return(lpLightSource);
	}
	return(nullptr);
}

QString cEXIFLightSourceList::lightSource(const qint32& iID)
{
	cEXIFLightSource* lpLightSource	= find(iID);
	if(!lpLightSource)
		return(QObject::tr("unknown"));
	return(lpLightSource->m_szLightSource);
}

cEXIFFlash::cEXIFFlash(const qint32& iID, const QString& szFlash) :
	m_iID(iID),
	m_szFlash(szFlash)
{
}

cEXIFFlashList::cEXIFFlashList()
{
	add(0x00, QObject::tr("No Flash"));
	add(0x01, QObject::tr("Fired"));
	add(0x05, QObject::tr("Fired, Return not detected"));
	add(0x07, QObject::tr("Fired, Return detected"));
	add(0x08, QObject::tr("On, Did not fire"));
	add(0x09, QObject::tr("On, Fired"));
	add(0x0d, QObject::tr("On, Return not detected"));
	add(0x0f, QObject::tr("On, Return detected"));
	add(0x10, QObject::tr("Off, Did not fire"));
	add(0x14, QObject::tr("Off, Did not fire, Return not detected"));
	add(0x18, QObject::tr("Auto, Did not fire"));
	add(0x19, QObject::tr("Auto, Fired"));
	add(0x1d, QObject::tr("Auto, Fired, Return not detected"));
	add(0x1f, QObject::tr("Auto, Fired, Return detected"));
	add(0x20, QObject::tr("No Flash function"));
	add(0x30, QObject::tr("Off, No flash function"));
	add(0x41, QObject::tr("Fired, Red-eye reduction"));
	add(0x45, QObject::tr("Fired, Red-eye reduction, Return not detected"));
	add(0x47, QObject::tr("Fired, Red-eye reduction, Return detected"));
	add(0x49, QObject::tr("On, Red-eye reduction"));
	add(0x4d, QObject::tr("On, Red-eye reduction, Return not detected"));
	add(0x4f, QObject::tr("On, Red-eye reduction, Return detected"));
	add(0x50, QObject::tr("Off, Red-eye reduction"));
	add(0x58, QObject::tr("Auto, Did not fire, Red-eye reduction"));
	add(0x59, QObject::tr("Auto, Fired, Red-eye reduction"));
	add(0x5d, QObject::tr("Auto, Fired, Red-eye reduction, Return not detected"));
	add(0x5f, QObject::tr("Auto, Fired, Red-eye reduction, Return detected"));
}

cEXIFFlash* cEXIFFlashList::add(const qint32& iID, const QString& szFlash)
{
	cEXIFFlash*	lpNew	= find(iID);

	if(lpNew)
		return(nullptr);

	lpNew	= new cEXIFFlash(iID, szFlash);
	append(lpNew);
	return(lpNew);
}

cEXIFFlash* cEXIFFlashList::find(const qint32& iID)
{
	for(int x = 0;x < count();x++)
	{
		cEXIFFlash*	lpFlash	= at(x);

		if(lpFlash->m_iID == iID)
			return(lpFlash);
	}
	return(nullptr);
}

QString cEXIFFlashList::flash(const qint32& iID)
{
	cEXIFFlash* lpFlash	= find(iID);
	if(!lpFlash)
		return(QObject::tr("unknown"));
	return(lpFlash->m_szFlash);
}

cEXIFTag::cEXIFTag(const qint32& iTAGID, const QString& szTAGName, const qint32& iIFDID, const qint32& iTypeID, const QString& szDescription) :
	m_iTAGID(iTAGID),
	m_szTAGName(szTAGName),
	m_iIFDID(iIFDID),
	m_iTypeID(iTypeID),
	m_szDescription(szDescription)
{
}

cEXIFTagList::cEXIFTagList()
{
	add(0x000b, QObject::tr("ProcessingSoftware"), 1, 2, QObject::tr("The name and version of the software used to post-process the picture."));
	add(0x00fe, QObject::tr("NewSubfileType"), 1, 4, QObject::tr("A general indication of the kind of data contained in this subfile."));
	add(0x00ff, QObject::tr("SubfileType"), 1, 3, QObject::tr("A general indication of the kind of data contained in this subfile. This field is deprecated. The NewSubfileType field should be used instead."));
	add(0x0100, QObject::tr("ImageWidth"), 1, 4, QObject::tr("The number of columns of image data, equal to the number of pixels per row. In JPEG compressed data a JPEG marker is used instead of this tag."));
	add(0x0101, QObject::tr("ImageHeight"), 1, 4, QObject::tr("The number of rows of image data. In JPEG compressed data a JPEG marker is used instead of this tag."));
	add(0x0102, QObject::tr("BitsPerSample"), 1, 3, QObject::tr("The number of bits per image component. In this standard each component of the image is 8 bits, so the value for this tag is 8. See also <SamplesPerPixel>. In JPEG compressed data a JPEG marker is used instead of this tag."));
	add(0x0103, QObject::tr("Compression"), 1, 3, QObject::tr("The compression scheme used for the image data. When a primary image is JPEG compressed, this designation is not necessary and is omitted. When thumbnails use JPEG compression, this tag value is set to 6."));
	add(0x0106, QObject::tr("PhotometricInterpretation"), 1, 3, QObject::tr("The pixel composition. In JPEG compressed data a JPEG marker is used instead of this tag."));
	add(0x0107, QObject::tr("Thresholding"), 1, 3, QObject::tr("For black and white TIFF files that represent shades of gray, the technique used to convert from gray to black and white pixels."));
	add(0x0108, QObject::tr("CellWidth"), 1, 3, QObject::tr("The width of the dithering or halftoning matrix used to create a dithered or halftoned bilevel file."));
	add(0x0109, QObject::tr("CellLength"), 1, 3, QObject::tr("The length of the dithering or halftoning matrix used to create a dithered or halftoned bilevel file."));
	add(0x010a, QObject::tr("FillOrder"), 1, 3, QObject::tr("The logical order of bits within a byte"));
	add(0x010d, QObject::tr("DocumentName"), 1, 2, QObject::tr("The name of the document from which this image was scanned"));
	add(0x010e, QObject::tr("ImageDescription"), 1, 2, QObject::tr("A character string giving the title of the image. It may be a comment such as \"1988 company picnic\" or the like. Two-bytes character codes cannot be used. When a 2-bytes code is necessary, the Exif Private tag <UserComment> is to be used."));
	add(0x010f, QObject::tr("Make"), 1, 2, QObject::tr("The manufacturer of the recording equipment. This is the manufacturer of the DSC, scanner, video digitizer or other equipment that generated the image. When the field is left blank, it is treated as unknown."));
	add(0x0110, QObject::tr("Model"), 1, 2, QObject::tr("The model name or model number of the equipment. This is the model name or number of the DSC, scanner, video digitizer or other equipment that generated the image. When the field is left blank, it is treated as unknown."));
	add(0x0111, QObject::tr("StripOffsets"), 1, 4, QObject::tr("For each strip, the byte offset of that strip. It is recommended that this be selected so the number of strip bytes does not exceed 64 Kbytes. With JPEG compressed data this designation is not needed and is omitted. See also <RowsPerStrip> and <StripByteCounts>."));
	add(0x0112, QObject::tr("Orientation"), 1, 3, QObject::tr("The image orientation viewed in terms of rows and columns."));
	add(0x0115, QObject::tr("SamplesPerPixel"), 1, 3, QObject::tr("The number of components per pixel. Since this standard applies to RGB and YCbCr images, the value set for this tag is 3. In JPEG compressed data a JPEG marker is used instead of this tag."));
	add(0x0116, QObject::tr("RowsPerStrip"), 1, 4, QObject::tr("The number of rows per strip. This is the number of rows in the image of one strip when an image is divided into strips. With JPEG compressed data this designation is not needed and is omitted. See also <StripOffsets> and <StripByteCounts>."));
	add(0x0117, QObject::tr("StripByteCounts"), 1, 4, QObject::tr("The total number of bytes in each strip. With JPEG compressed data this designation is not needed and is omitted."));
	add(0x011a, QObject::tr("XResolution"), 1, 5, QObject::tr("The number of pixels per <ResolutionUnit> in the <ImageWidth> direction. When the image resolution is unknown, 72 [dpi] is designated."));
	add(0x011b, QObject::tr("YResolution"), 1, 5, QObject::tr("The number of pixels per <ResolutionUnit> in the <ImageLength> direction. The same value as <XResolution> is designated."));
	add(0x011c, QObject::tr("PlanarConfiguration"), 1, 3, QObject::tr("Indicates whether pixel components are recorded in a chunky or planar format. In JPEG compressed files a JPEG marker is used instead of this tag. If this field does not exist, the TIFF default of 1 (chunky) is assumed."));
	add(0x0122, QObject::tr("GrayResponseUnit"), 1, 3, QObject::tr("The precision of the information contained in the GrayResponseCurve."));
	add(0x0123, QObject::tr("GrayResponseCurve"), 1, 3, QObject::tr("For grayscale data, the optical density of each possible pixel value."));
	add(0x0124, QObject::tr("T4Options"), 1, 4, QObject::tr("T.4-encoding options."));
	add(0x0125, QObject::tr("T6Options"), 1, 4, QObject::tr("T.6-encoding options."));
	add(0x0128, QObject::tr("ResolutionUnit"), 1, 3, QObject::tr("The unit for measuring <XResolution> and <YResolution>. The same unit is used for both <XResolution> and <YResolution>. If the image resolution is unknown, 2 (inches) is designated."));
	add(0x0129, QObject::tr("PageNumber"), 1, 3, QObject::tr("The page number of the page from which this image was scanned."));
	add(0x012d, QObject::tr("TransferFunction"), 1, 3, QObject::tr("A transfer function for the image, described in tabular style. Normally this tag is not necessary, since color space is specified in the color space information tag (<ColorSpace>)."));
	add(0x0131, QObject::tr("Software"), 1, 2, QObject::tr("This tag records the name and version of the software or firmware of the camera or image input device used to generate the image. The detailed format is not specified, but it is recommended that the example shown below be followed. When the field is left blank, it is treated as unknown."));
	add(0x0132, QObject::tr("DateTime"), 1, 2, QObject::tr("The date and time of image creation. In Exif standard, it is the date and time the file was changed."));
	add(0x013b, QObject::tr("Artist"), 1, 2, QObject::tr("This tag records the name of the camera owner, photographer or image creator. The detailed format is not specified, but it is recommended that the information be written as in the example below for ease of Interoperability. When the field is left blank, it is treated as unknown. Ex.) \"Camera owner, John Smith; Photographer, Michael Brown; Image creator, Ken James\""));
	add(0x013c, QObject::tr("HostComputer"), 1, 2, QObject::tr("This tag records information about the host computer used to generate the image."));
	add(0x013d, QObject::tr("Predictor"), 1, 3, QObject::tr("A predictor is a mathematical operator that is applied to the image data before an encoding scheme is applied."));
	add(0x013e, QObject::tr("WhitePoint"), 1, 5, QObject::tr("The chromaticity of the white point of the image. Normally this tag is not necessary, since color space is specified in the colorspace information tag (<ColorSpace>)."));
	add(0x013f, QObject::tr("PrimaryChromaticities"), 1, 5, QObject::tr("The chromaticity of the three primary colors of the image. Normally this tag is not necessary, since colorspace is specified in the colorspace information tag (<ColorSpace>)."));
	add(0x0140, QObject::tr("ColorMap"), 1, 3, QObject::tr("A color map for palette color images. This field defines a Red-Green-Blue color map (often called a lookup table) for palette-color images. In a palette-color image, a pixel value is used to index into an RGB lookup table."));
	add(0x0141, QObject::tr("HalftoneHints"), 1, 3, QObject::tr("The purpose of the HalftoneHints field is to convey to the halftone function the range of gray levels within a colorimetrically-specified image that should retain tonal detail."));
	add(0x0142, QObject::tr("TileWidth"), 1, 3, QObject::tr("The tile width in pixels. This is the number of columns in each tile."));
	add(0x0143, QObject::tr("TileLength"), 1, 3, QObject::tr("The tile length (height) in pixels. This is the number of rows in each tile."));
	add(0x0144, QObject::tr("TileOffsets"), 1, 3, QObject::tr("For each tile, the byte offset of that tile, as compressed and stored on disk. The offset is specified with respect to the beginning of the TIFF file. Note that this implies that each tile has a location independent of the locations of other tiles."));
	add(0x0145, QObject::tr("TileByteCounts"), 1, 3, QObject::tr("For each tile, the number of (compressed) bytes in that tile. See TileOffsets for a description of how the byte counts are ordered."));
	add(0x014a, QObject::tr("SubIFDs"), 1, 4, QObject::tr("Defined by Adobe Corporation to enable TIFF Trees within a TIFF file."));
	add(0x014c, QObject::tr("InkSet"), 1, 3, QObject::tr("The set of inks used in a separated (PhotometricInterpretation=5) image."));
	add(0x014d, QObject::tr("InkNames"), 1, 2, QObject::tr("The name of each ink used in a separated (PhotometricInterpretation=5) image."));
	add(0x014e, QObject::tr("NumberOfInks"), 1, 3, QObject::tr("The number of inks. Usually equal to SamplesPerPixel, unless there are extra samples."));
	add(0x0150, QObject::tr("DotRange"), 1, 1, QObject::tr("The component values that correspond to a 0% dot and 100% dot."));
	add(0x0151, QObject::tr("TargetPrinter"), 1, 2, QObject::tr("A description of the printing environment for which this separation is intended."));
	add(0x0152, QObject::tr("ExtraSamples"), 1, 3, QObject::tr("Specifies that each pixel has m extra components whose interpretation is defined by one of the values listed below."));
	add(0x0153, QObject::tr("SampleFormat"), 1, 3, QObject::tr("This field specifies how to interpret each data sample in a pixel."));
	add(0x0154, QObject::tr("SMinSampleValue"), 1, 3, QObject::tr("This field specifies the minimum sample value."));
	add(0x0155, QObject::tr("SMaxSampleValue"), 1, 3, QObject::tr("This field specifies the maximum sample value."));
	add(0x0156, QObject::tr("TransferRange"), 1, 3, QObject::tr("Expands the range of the TransferFunction"));
	add(0x0157, QObject::tr("ClipPath"), 1, 1, QObject::tr("A TIFF ClipPath is intended to mirror the essentials of PostScript's path creation functionality."));
	add(0x0158, QObject::tr("XClipPathUnits"), 1, 8, QObject::tr("The number of units that span the width of the image, in terms of integer ClipPath coordinates."));
	add(0x0159, QObject::tr("YClipPathUnits"), 1, 8, QObject::tr("The number of units that span the height of the image, in terms of integer ClipPath coordinates."));
	add(0x015a, QObject::tr("Indexed"), 1, 3, QObject::tr("Indexed images are images where the 'pixels' do not represent color values, but rather an index (usually 8-bit) into a separate color table, the ColorMap."));
	add(0x015b, QObject::tr("JPEGTables"), 1, 7, QObject::tr("This optional tag may be used to encode the JPEG quantization and Huffman tables for subsequent use by the JPEG decompression process."));
	add(0x015f, QObject::tr("OPIProxy"), 1, 3, QObject::tr("OPIProxy gives information concerning whether this image is a low-resolution proxy of a high-resolution image (Adobe OPI)."));
	add(0x0200, QObject::tr("JPEGProc"), 1, 4, QObject::tr("This field indicates the process used to produce the compressed data"));
	add(0x0201, QObject::tr("JPEGInterchangeFormat"), 1, 4, QObject::tr("The offset to the start byte (SOI) of JPEG compressed thumbnail data. This is not used for primary image JPEG data."));
	add(0x0202, QObject::tr("JPEGInterchangeFormatLength"), 1, 4, QObject::tr("The number of bytes of JPEG compressed thumbnail data. This is not used for primary image JPEG data. JPEG thumbnails are not divided but are recorded as a continuous JPEG bitstream from SOI to EOI. Appn and COM markers should not be recorded. Compressed thumbnails must be recorded in no more than 64 Kbytes, including all other data to be recorded in APP1."));
	add(0x0203, QObject::tr("JPEGRestartInterval"), 1, 3, QObject::tr("This Field indicates the length of the restart interval used in the compressed image data."));
	add(0x0205, QObject::tr("JPEGLosslessPredictors"), 1, 3, QObject::tr("This Field points to a list of lossless predictor-selection values, one per component."));
	add(0x0206, QObject::tr("JPEGPointTransforms"), 1, 3, QObject::tr("This Field points to a list of point transform values, one per component."));
	add(0x0207, QObject::tr("JPEGQTables"), 1, 4, QObject::tr("This Field points to a list of offsets to the quantization tables, one per component."));
	add(0x0208, QObject::tr("JPEGDCTables"), 1, 4, QObject::tr("This Field points to a list of offsets to the DC Huffman tables or the lossless Huffman tables, one per component."));
	add(0x0209, QObject::tr("JPEGACTables"), 1, 4, QObject::tr("This Field points to a list of offsets to the Huffman AC tables, one per component."));
	add(0x0211, QObject::tr("YCbCrCoefficients"), 1, 5, QObject::tr("The matrix coefficients for transformation from RGB to YCbCr image data. No default is given in TIFF; but here the value given in Appendix E, \"Color Space Guidelines\"), is used as the default. The color space is declared in a color space information tag, with the default being the value that gives the optimal image characteristics Interoperability this condition."));
	add(0x0212, QObject::tr("YCbCrSubSampling"), 1, 3, QObject::tr("The sampling ratio of chrominance components in relation to the luminance component. In JPEG compressed data a JPEG marker is used instead of this tag."));
	add(0x0213, QObject::tr("YCbCrPositioning"), 1, 3, QObject::tr("The position of chrominance components in relation to the luminance component. This field is designated only for JPEG compressed data or uncompressed YCbCr data. The TIFF default is 1 (centered); but when Y:Cb:Cr = 4:2:2 it is recommended in this standard that 2 (co-sited) be used to record data, in order to improve the image quality when viewed on TV systems. When this field does not exist, the reader shall assume the TIFF default. In the case of Y:Cb:Cr = 4:2:0, the TIFF default (centered) is recommended. If the reader does not have the capability of supporting both kinds of <YCbCrPositioning>, it shall follow the TIFF default regardless of the value in this field. It is preferable that readers be able to support both centered and co-sited positioning."));
	add(0x0214, QObject::tr("ReferenceBlackWhite"), 1, 5, QObject::tr("The reference black point value and reference white point value. No defaults are given in TIFF, but the values below are given as defaults here. The color space is declared in a color space information tag, with the default being the value that gives the optimal image characteristics Interoperability these conditions."));
	add(0x02bc, QObject::tr("XMLPacket"), 1, 1, QObject::tr("XMP Metadata (Adobe technote 9-14-02)"));
	add(0x4746, QObject::tr("Rating"), 1, 3, QObject::tr("Rating tag used by Windows"));
	add(0x4749, QObject::tr("RatingPercent"), 1, 3, QObject::tr("Rating tag used by Windows, value in percent"));
	add(0x800d, QObject::tr("ImageID"), 1, 2, QObject::tr("ImageID is the full pathname of the original, high-resolution image, or any other identifying string that uniquely identifies the original image (Adobe OPI)."));
	add(0x828d, QObject::tr("CFARepeatPatternDim"), 1, 3, QObject::tr("Contains two values representing the minimum rows and columns to define the repeating patterns of the color filter array"));
	add(0x828e, QObject::tr("CFAPattern"), 1, 1, QObject::tr("Indicates the color filter array (CFA) geometric pattern of the image sensor when a one-chip color area sensor is used. It does not apply to all sensing methods"));
	add(0x828f, QObject::tr("BatteryLevel"), 1, 5, QObject::tr("Contains a value of the battery level as a fraction or string"));
	add(0x8298, QObject::tr("Copyright"), 1, 2, QObject::tr("Copyright information. In this standard the tag is used to indicate both the photographer and editor copyrights. It is the copyright notice of the person or organization claiming rights to the image. The Interoperability copyright statement including date and rights should be written in this field; e.g., \"Copyright, John Smith, 19xx. All rights reserved.\". In this standard the field records both the photographer and editor copyrights, with each recorded in a separate part of the statement. When there is a clear distinction between the photographer and editor copyrights, these are to be written in the order of photographer followed by editor copyright, separated by NULL (in this case since the statement also ends with a NULL, there are two NULL codes). When only the photographer copyright is given, it is terminated by one NULL code. When only the editor copyright is given, the photographer copyright part consists of one space followed by a terminating NULL code, then the editor copyright is given. When the field is left blank, it is treated as unknown."));
	add(0x829a, QObject::tr("ExposureTime"), 1, 5, QObject::tr("Exposure time, given in seconds."));
	add(0x829d, QObject::tr("FNumber"), 1, 5, QObject::tr("The F number."));
	add(0x83bb, QObject::tr("IPTCNAA"), 1, 4, QObject::tr("Contains an IPTC/NAA record"));
	add(0x8649, QObject::tr("ImageResources"), 1, 1, QObject::tr("Contains information embedded by the Adobe Photoshop application"));
	add(0x8769, QObject::tr("ExifTag"), 1, 4, QObject::tr("A pointer to the Exif IFD. Interoperability, Exif IFD has the same structure as that of the IFD specified in TIFF. ordinarily, however, it does not contain image data as in the case of TIFF."));
	add(0x8773, QObject::tr("InterColorProfile"), 1, 7, QObject::tr("Contains an InterColor Consortium (ICC) format color space characterization/profile"));
	add(0x8822, QObject::tr("ExposureProgram"), 1, 3, QObject::tr("The class of the program used by the camera to set exposure when the picture is taken."));
	add(0x8824, QObject::tr("SpectralSensitivity"), 1, 2, QObject::tr("Indicates the spectral sensitivity of each channel of the camera used."));
	add(0x8825, QObject::tr("GPSTag"), 1, 4, QObject::tr("A pointer to the GPS Info IFD. The Interoperability structure of the GPS Info IFD, like that of Exif IFD, has no image data."));
	add(0x8827, QObject::tr("ISOSpeedRatings"), 1, 3, QObject::tr("Indicates the ISO Speed and ISO Latitude of the camera or input device as specified in ISO 12232."));
	add(0x8828, QObject::tr("OECF"), 1, 7, QObject::tr("Indicates the Opto-Electric Conversion Function (OECF) specified in ISO 14524."));
	add(0x8829, QObject::tr("Interlace"), 1, 3, QObject::tr("Indicates the field number of multifield images."));
	add(0x882a, QObject::tr("TimeZoneOffset"), 1, 8, QObject::tr("This optional tag encodes the time zone of the camera clock (relative to Greenwich Mean Time) used to create the DataTimeOriginal tag-value when the picture was taken. It may also contain the time zone offset of the clock used to create the DateTime tag-value when the image was modified."));
	add(0x882b, QObject::tr("SelfTimerMode"), 1, 3, QObject::tr("Number of seconds image capture was delayed from button press."));
	add(0x9003, QObject::tr("DateTimeOriginal"), 1, 2, QObject::tr("The date and time when the original image data was generated."));
	add(0x9102, QObject::tr("CompressedBitsPerPixel"), 1, 5, QObject::tr("Specific to compressed data; states the compressed bits per pixel."));
	add(0x9201, QObject::tr("ShutterSpeedValue"), 1, 10, QObject::tr("Shutter speed."));
	add(0x9202, QObject::tr("ApertureValue"), 1, 5, QObject::tr("The lens aperture."));
	add(0x9203, QObject::tr("BrightnessValue"), 1, 10, QObject::tr("The value of brightness."));
	add(0x9204, QObject::tr("ExposureBiasValue"), 1, 10, QObject::tr("The exposure bias."));
	add(0x9205, QObject::tr("MaxApertureValue"), 1, 5, QObject::tr("The smallest F number of the lens."));
	add(0x9206, QObject::tr("SubjectDistance"), 1, 10, QObject::tr("The distance to the subject, given in meters."));
	add(0x9207, QObject::tr("MeteringMode"), 1, 3, QObject::tr("The metering mode."));
	add(0x9208, QObject::tr("LightSource"), 1, 3, QObject::tr("The kind of light source."));
	add(0x9209, QObject::tr("Flash"), 1, 3, QObject::tr("Indicates the status of flash when the image was shot."));
	add(0x920a, QObject::tr("FocalLength"), 1, 5, QObject::tr("The actual focal length of the lens, in mm."));
	add(0x920b, QObject::tr("FlashEnergy"), 1, 5, QObject::tr("Amount of flash energy (BCPS)."));
	add(0x920c, QObject::tr("SpatialFrequencyResponse"), 1, 7, QObject::tr("SFR of the camera."));
	add(0x920d, QObject::tr("Noise"), 1, 7, QObject::tr("Noise measurement values."));
	add(0x920e, QObject::tr("FocalPlaneXResolution"), 1, 5, QObject::tr("Number of pixels per FocalPlaneResolutionUnit (37392) in ImageWidth direction for main image."));
	add(0x920f, QObject::tr("FocalPlaneYResolution"), 1, 5, QObject::tr("Number of pixels per FocalPlaneResolutionUnit (37392) in ImageLength direction for main image."));
	add(0x9210, QObject::tr("FocalPlaneResolutionUnit"), 1, 3, QObject::tr("Unit of measurement for FocalPlaneXResolution(37390) and FocalPlaneYResolution(37391)."));
	add(0x9211, QObject::tr("ImageNumber"), 1, 4, QObject::tr("Number assigned to an image, e.g., in a chained image burst."));
	add(0x9212, QObject::tr("SecurityClassification"), 1, 2, QObject::tr("Security classification assigned to the image."));
	add(0x9213, QObject::tr("ImageHistory"), 1, 2, QObject::tr("Record of what has been done to the image."));
	add(0x9214, QObject::tr("SubjectLocation"), 1, 3, QObject::tr("Indicates the location and area of the main subject in the overall scene."));
	add(0x9215, QObject::tr("ExposureIndex"), 1, 5, QObject::tr("Encodes the camera exposure index setting when image was captured."));
	add(0x9216, QObject::tr("TIFFEPStandardID"), 1, 1, QObject::tr("Contains four ASCII characters representing the TIFF/EP standard version of a TIFF/EP file, eg '1', '0', '0', '0'"));
	add(0x9217, QObject::tr("SensingMethod"), 1, 3, QObject::tr("Type of image sensor."));
	add(0x9c9b, QObject::tr("XPTitle"), 1, 1, QObject::tr("Title tag used by Windows, encoded in UCS2"));
	add(0x9c9c, QObject::tr("XPComment"), 1, 1, QObject::tr("Comment tag used by Windows, encoded in UCS2"));
	add(0x9c9d, QObject::tr("XPAuthor"), 1, 1, QObject::tr("Author tag used by Windows, encoded in UCS2"));
	add(0x9c9e, QObject::tr("XPKeywords"), 1, 1, QObject::tr("Keywords tag used by Windows, encoded in UCS2"));
	add(0x9c9f, QObject::tr("XPSubject"), 1, 1, QObject::tr("Subject tag used by Windows, encoded in UCS2"));
	add(0xc4a5, QObject::tr("PrintImageMatching"), 1, 7, QObject::tr("Print Image Matching, description needed."));
	add(0xc612, QObject::tr("DNGVersion"), 1, 1, QObject::tr("This tag encodes the DNG four-tier version number. For files compliant with version 1.1.0.0 of the DNG specification, this tag should contain the bytes: 1, 1, 0, 0."));
	add(0xc613, QObject::tr("DNGBackwardVersion"), 1, 1, QObject::tr("This tag specifies the oldest version of the Digital Negative specification for which a file is compatible. Readers shouldnot attempt to read a file if this tag specifies a version number that is higher than the version number of the specification the reader was based on. In addition to checking the version tags, readers should, for all tags, check the types, counts, and values, to verify it is able to correctly read the file."));
	add(0xc614, QObject::tr("UniqueCameraModel"), 1, 2, QObject::tr("Defines a unique, non-localized name for the camera model that created the image in the raw file. This name should include the manufacturer's name to avoid conflicts, and should not be localized, even if the camera name itself is localized for different markets (see LocalizedCameraModel). This string may be used by reader software to index into per-model preferences and replacement profiles."));
	add(0xc615, QObject::tr("LocalizedCameraModel"), 1, 1, QObject::tr("Similar to the UniqueCameraModel field, except the name can be localized for different markets to match the localization of the camera name."));
	add(0xc616, QObject::tr("CFAPlaneColor"), 1, 1, QObject::tr("Provides a mapping between the values in the CFAPattern tag and the plane numbers in LinearRaw space. This is a required tag for non-RGB CFA images."));
	add(0xc617, QObject::tr("CFALayout"), 1, 3, QObject::tr("Describes the spatial layout of the CFA."));
	add(0xc618, QObject::tr("LinearizationTable"), 1, 3, QObject::tr("Describes a lookup table that maps stored values into linear values. This tag is typically used to increase compression ratios by storing the raw data in a non-linear, more visually uniform space with fewer total encoding levels. If SamplesPerPixel is not equal to one, this single table applies to all the samples for each pixel."));
	add(0xc619, QObject::tr("BlackLevelRepeatDim"), 1, 3, QObject::tr("Specifies repeat pattern size for the BlackLevel tag."));
	add(0xc61a, QObject::tr("BlackLevel"), 1, 5, QObject::tr("Specifies the zero light (a.k.a. thermal black or black current) encoding level, as a repeating pattern. The origin of this pattern is the top-left corner of the ActiveArea rectangle. The values are stored in row-column-sample scan order."));
	add(0xc61b, QObject::tr("BlackLevelDeltaH"), 1, 10, QObject::tr("If the zero light encoding level is a function of the image column, BlackLevelDeltaH specifies the difference between the zero light encoding level for each column and the baseline zero light encoding level. If SamplesPerPixel is not equal to one, this single table applies to all the samples for each pixel."));
	add(0xc61c, QObject::tr("BlackLevelDeltaV"), 1, 10, QObject::tr("If the zero light encoding level is a function of the image row, this tag specifies the difference between the zero light encoding level for each row and the baseline zero light encoding level. If SamplesPerPixel is not equal to one, this single table applies to all the samples for each pixel."));
	add(0xc61d, QObject::tr("WhiteLevel"), 1, 3, QObject::tr("This tag specifies the fully saturated encoding level for the raw sample values. Saturation is caused either by the sensor itself becoming highly non-linear in response, or by the camera's analog to digital converter clipping."));
	add(0xc61e, QObject::tr("DefaultScale"), 1, 5, QObject::tr("DefaultScale is required for cameras with non-square pixels. It specifies the default scale factors for each direction to convert the image to square pixels. Typically these factors are selected to approximately preserve total pixel count. For CFA images that use CFALayout equal to 2, 3, 4, or 5, such as the Fujifilm SuperCCD, these two values should usually differ by a factor of 2.0."));
	add(0xc61f, QObject::tr("DefaultCropOrigin"), 1, 3, QObject::tr("Raw images often store extra pixels around the edges of the final image. These extra pixels help prevent interpolation artifacts near the edges of the final image. DefaultCropOrigin specifies the origin of the final image area, in raw image coordinates (i.e., before the DefaultScale has been applied), relative to the top-left corner of the ActiveArea rectangle."));
	add(0xc620, QObject::tr("DefaultCropSize"), 1, 3, QObject::tr("Raw images often store extra pixels around the edges of the final image. These extra pixels help prevent interpolation artifacts near the edges of the final image. DefaultCropSize specifies the size of the final image area, in raw image coordinates (i.e., before the DefaultScale has been applied)."));
	add(0xc621, QObject::tr("ColorMatrix1"), 1, 10, QObject::tr("ColorMatrix1 defines a transformation matrix that converts XYZ values to reference camera native color space values, under the first calibration illuminant. The matrix values are stored in row scan order. The ColorMatrix1 tag is required for all non-monochrome DNG files."));
	add(0xc622, QObject::tr("ColorMatrix2"), 1, 10, QObject::tr("ColorMatrix2 defines a transformation matrix that converts XYZ values to reference camera native color space values, under the second calibration illuminant. The matrix values are stored in row scan order."));
	add(0xc623, QObject::tr("CameraCalibration1"), 1, 10, QObject::tr("CameraCalibration1 defines a calibration matrix that transforms reference camera native space values to individual camera native space values under the first calibration illuminant. The matrix is stored in row scan order. This matrix is stored separately from the matrix specified by the ColorMatrix1 tag to allow raw converters to swap in replacement color matrices based on UniqueCameraModel tag, while still taking advantage of any per-individual camera calibration performed by the camera manufacturer."));
	add(0xc624, QObject::tr("CameraCalibration2"), 1, 10, QObject::tr("CameraCalibration2 defines a calibration matrix that transforms reference camera native space values to individual camera native space values under the second calibration illuminant. The matrix is stored in row scan order. This matrix is stored separately from the matrix specified by the ColorMatrix2 tag to allow raw converters to swap in replacement color matrices based on UniqueCameraModel tag, while still taking advantage of any per-individual camera calibration performed by the camera manufacturer."));
	add(0xc625, QObject::tr("ReductionMatrix1"), 1, 10, QObject::tr("ReductionMatrix1 defines a dimensionality reduction matrix for use as the first stage in converting color camera native space values to XYZ values, under the first calibration illuminant. This tag may only be used if ColorPlanes is greater than 3. The matrix is stored in row scan order."));
	add(0xc626, QObject::tr("ReductionMatrix2"), 1, 10, QObject::tr("ReductionMatrix2 defines a dimensionality reduction matrix for use as the first stage in converting color camera native space values to XYZ values, under the second calibration illuminant. This tag may only be used if ColorPlanes is greater than 3. The matrix is stored in row scan order."));
	add(0xc627, QObject::tr("AnalogBalance"), 1, 5, QObject::tr("Normally the stored raw values are not white balanced, since any digital white balancing will reduce the dynamic range of the final image if the user decides to later adjust the white balance; however, if camera hardware is capable of white balancing the color channels before the signal is digitized, it can improve the dynamic range of the final image. AnalogBalance defines the gain, either analog (recommended) or digital (not recommended) that has been applied the stored raw values."));
	add(0xc628, QObject::tr("AsShotNeutral"), 1, 3, QObject::tr("Specifies the selected white balance at time of capture, encoded as the coordinates of a perfectly neutral color in linear reference space values. The inclusion of this tag precludes the inclusion of the AsShotWhiteXY tag."));
	add(0xc629, QObject::tr("AsShotWhiteXY"), 1, 5, QObject::tr("Specifies the selected white balance at time of capture, encoded as x-y chromaticity coordinates. The inclusion of this tag precludes the inclusion of the AsShotNeutral tag."));
	add(0xc62a, QObject::tr("BaselineExposure"), 1, 10, QObject::tr("Camera models vary in the trade-off they make between highlight headroom and shadow noise. Some leave a significant amount of highlight headroom during a normal exposure. This allows significant negative exposure compensation to be applied during raw conversion, but also means normal exposures will contain more shadow noise. Other models leave less headroom during normal exposures. This allows for less negative exposure compensation, but results in lower shadow noise for normal exposures. Because of these differences, a raw converter needs to vary the zero point of its exposure compensation control from model to model. BaselineExposure specifies by how much (in EV units) to move the zero point. Positive values result in brighter default results, while negative values result in darker default results."));
	add(0xc62b, QObject::tr("BaselineNoise"), 1, 5, QObject::tr("Specifies the relative noise level of the camera model at a baseline ISO value of 100, compared to a reference camera model. Since noise levels tend to vary approximately with the square root of the ISO value, a raw converter can use this value, combined with the current ISO, to estimate the relative noise level of the current image."));
	add(0xc62c, QObject::tr("BaselineSharpness"), 1, 5, QObject::tr("Specifies the relative amount of sharpening required for this camera model, compared to a reference camera model. Camera models vary in the strengths of their anti-aliasing filters. Cameras with weak or no filters require less sharpening than cameras with strong anti-aliasing filters."));
	add(0xc62d, QObject::tr("BayerGreenSplit"), 1, 4, QObject::tr("Only applies to CFA images using a Bayer pattern filter array. This tag specifies, in arbitrary units, how closely the values of the green pixels in the blue/green rows track the values of the green pixels in the red/green rows. A value of zero means the two kinds of green pixels track closely, while a non-zero value means they sometimes diverge. The useful range for this tag is from 0 (no divergence) to about 5000 (quite large divergence)."));
	add(0xc62e, QObject::tr("LinearResponseLimit"), 1, 5, QObject::tr("Some sensors have an unpredictable non-linearity in their response as they near the upper limit of their encoding range. This non-linearity results in color shifts in the highlight areas of the resulting image unless the raw converter compensates for this effect. LinearResponseLimit specifies the fraction of the encoding range above which the response may become significantly non-linear."));
	add(0xc62f, QObject::tr("CameraSerialNumber"), 1, 2, QObject::tr("CameraSerialNumber contains the serial number of the camera or camera body that captured the image."));
	add(0xc630, QObject::tr("LensInfo"), 1, 5, QObject::tr("Contains information about the lens that captured the image. If the minimum f-stops are unknown, they should be encoded as 0/0."));
	add(0xc631, QObject::tr("ChromaBlurRadius"), 1, 5, QObject::tr("ChromaBlurRadius provides a hint to the DNG reader about how much chroma blur should be applied to the image. If this tag is omitted, the reader will use its default amount of chroma blurring. Normally this tag is only included for non-CFA images, since the amount of chroma blur required for mosaic images is highly dependent on the de-mosaic algorithm, in which case the DNG reader's default value is likely optimized for its particular de-mosaic algorithm."));
	add(0xc632, QObject::tr("AntiAliasStrength"), 1, 5, QObject::tr("Provides a hint to the DNG reader about how strong the camera's anti-alias filter is. A value of 0.0 means no anti-alias filter (i.e., the camera is prone to aliasing artifacts with some subjects), while a value of 1.0 means a strong anti-alias filter (i.e., the camera almost never has aliasing artifacts)."));
	add(0xc633, QObject::tr("ShadowScale"), 1, 10, QObject::tr("This tag is used by Adobe Camera Raw to control the sensitivity of its 'Shadows' slider."));
	add(0xc634, QObject::tr("DNGPrivateData"), 1, 1, QObject::tr("Provides a way for camera manufacturers to store private data in the DNG file for use by their own raw converters, and to have that data preserved by programs that edit DNG files."));
	add(0xc635, QObject::tr("MakerNoteSafety"), 1, 3, QObject::tr("MakerNoteSafety lets the DNG reader know whether the EXIF MakerNote tag is safe to preserve along with the rest of the EXIF data. File browsers and other image management software processing an image with a preserved MakerNote should be aware that any thumbnail image embedded in the MakerNote may be stale, and may not reflect the current state of the full size image."));
	add(0xc65a, QObject::tr("CalibrationIlluminant1"), 1, 3, QObject::tr("The illuminant used for the first set of color calibration tags (ColorMatrix1, CameraCalibration1, ReductionMatrix1). The legal values for this tag are the same as the legal values for the LightSource EXIF tag."));
	add(0xc65b, QObject::tr("CalibrationIlluminant2"), 1, 3, QObject::tr("The illuminant used for an optional second set of color calibration tags (ColorMatrix2, CameraCalibration2, ReductionMatrix2). The legal values for this tag are the same as the legal values for the CalibrationIlluminant1 tag; however, if both are included, neither is allowed to have a value of 0 (unknown)."));
	add(0xc65c, QObject::tr("BestQualityScale"), 1, 5, QObject::tr("For some cameras, the best possible image quality is not achieved by preserving the total pixel count during conversion. For example, Fujifilm SuperCCD images have maximum detail when their total pixel count is doubled. This tag specifies the amount by which the values of the DefaultScale tag need to be multiplied to achieve the best quality image size."));
	add(0xc65d, QObject::tr("RawDataUniqueID"), 1, 1, QObject::tr("This tag contains a 16-byte unique identifier for the raw image data in the DNG file. DNG readers can use this tag to recognize a particular raw image, even if the file's name or the metadata contained in the file has been changed. If a DNG writer creates such an identifier, it should do so using an algorithm that will ensure that it is very unlikely two different images will end up having the same identifier."));
	add(0xc68b, QObject::tr("OriginalRawFileName"), 1, 1, QObject::tr("If the DNG file was converted from a non-DNG raw file, then this tag contains the file name of that original raw file."));
	add(0xc68c, QObject::tr("OriginalRawFileData"), 1, 7, QObject::tr("If the DNG file was converted from a non-DNG raw file, then this tag contains the compressed contents of that original raw file. The contents of this tag always use the big-endian byte order. The tag contains a sequence of data blocks. Future versions of the DNG specification may define additional data blocks, so DNG readers should ignore extra bytes when parsing this tag. DNG readers should also detect the case where data blocks are missing from the end of the sequence, and should assume a default value for all the missing blocks. There are no padding or alignment bytes between data blocks."));
	add(0xc68d, QObject::tr("ActiveArea"), 1, 3, QObject::tr("This rectangle defines the active (non-masked) pixels of the sensor. The order of the rectangle coordinates is: top, left, bottom, right."));
	add(0xc68e, QObject::tr("MaskedAreas"), 1, 3, QObject::tr("This tag contains a list of non-overlapping rectangle coordinates of fully masked pixels, which can be optionally used by DNG readers to measure the black encoding level. The order of each rectangle's coordinates is: top, left, bottom, right. If the raw image data has already had its black encoding level subtracted, then this tag should not be used, since the masked pixels are no longer useful."));
	add(0xc68f, QObject::tr("AsShotICCProfile"), 1, 7, QObject::tr("This tag contains an ICC profile that, in conjunction with the AsShotPreProfileMatrix tag, provides the camera manufacturer with a way to specify a default color rendering from camera color space coordinates (linear reference values) into the ICC profile connection space. The ICC profile connection space is an output referred colorimetric space, whereas the other color calibration tags in DNG specify a conversion into a scene referred colorimetric space. This means that the rendering in this profile should include any desired tone and gamut mapping needed to convert between scene referred values and output referred values."));
	add(0xc690, QObject::tr("AsShotPreProfileMatrix"), 1, 10, QObject::tr("This tag is used in conjunction with the AsShotICCProfile tag. It specifies a matrix that should be applied to the camera color space coordinates before processing the values through the ICC profile specified in the AsShotICCProfile tag. The matrix is stored in the row scan order. If ColorPlanes is greater than three, then this matrix can (but is not required to) reduce the dimensionality of the color data down to three components, in which case the AsShotICCProfile should have three rather than ColorPlanes input components."));
	add(0xc691, QObject::tr("CurrentICCProfile"), 1, 7, QObject::tr("This tag is used in conjunction with the CurrentPreProfileMatrix tag. The CurrentICCProfile and CurrentPreProfileMatrix tags have the same purpose and usage as the AsShotICCProfile and AsShotPreProfileMatrix tag pair, except they are for use by raw file editors rather than camera manufacturers."));
	add(0xc692, QObject::tr("CurrentPreProfileMatrix"), 1, 10, QObject::tr("This tag is used in conjunction with the CurrentICCProfile tag. The CurrentICCProfile and CurrentPreProfileMatrix tags have the same purpose and usage as the AsShotICCProfile and AsShotPreProfileMatrix tag pair, except they are for use by raw file editors rather than camera manufacturers."));
	add(0xc6bf, QObject::tr("ColorimetricReference"), 1, 3, QObject::tr("The DNG color model documents a transform between camera colors and CIE XYZ values. This tag describes the colorimetric reference for the CIE XYZ values. 0 = The XYZ values are scene-referred. 1 = The XYZ values are output-referred, using the ICC profile perceptual dynamic range. This tag allows output-referred data to be stored in DNG files and still processed correctly by DNG readers."));
	add(0xc6f3, QObject::tr("CameraCalibrationSignature"), 1, 1, QObject::tr("A UTF-8 encoded string associated with the CameraCalibration1 and CameraCalibration2 tags. The CameraCalibration1 and CameraCalibration2 tags should only be used in the DNG color transform if the string stored in the CameraCalibrationSignature tag exactly matches the string stored in the ProfileCalibrationSignature tag for the selected camera profile."));
	add(0xc6f4, QObject::tr("ProfileCalibrationSignature"), 1, 1, QObject::tr("A UTF-8 encoded string associated with the camera profile tags. The CameraCalibration1 and CameraCalibration2 tags should only be used in the DNG color transfer if the string stored in the CameraCalibrationSignature tag exactly matches the string stored in the ProfileCalibrationSignature tag for the selected camera profile."));
	add(0xc6f6, QObject::tr("AsShotProfileName"), 1, 1, QObject::tr("A UTF-8 encoded string containing the name of the \"as shot\" camera profile, if any."));
	add(0xc6f7, QObject::tr("NoiseReductionApplied"), 1, 5, QObject::tr("This tag indicates how much noise reduction has been applied to the raw data on a scale of 0.0 to 1.0. A 0.0 value indicates that no noise reduction has been applied. A 1.0 value indicates that the \"ideal\" amount of noise reduction has been applied, i.e. that the DNG reader should not apply additional noise reduction by default. A value of 0/0 indicates that this parameter is unknown."));
	add(0xc6f8, QObject::tr("ProfileName"), 1, 1, QObject::tr("A UTF-8 encoded string containing the name of the camera profile. This tag is optional if there is only a single camera profile stored in the file but is required for all camera profiles if there is more than one camera profile stored in the file."));
	add(0xc6f9, QObject::tr("ProfileHueSatMapDims"), 1, 4, QObject::tr("This tag specifies the number of input samples in each dimension of the hue/saturation/value mapping tables. The data for these tables are stored in ProfileHueSatMapData1 and ProfileHueSatMapData2 tags. The most common case has ValueDivisions equal to 1, so only hue and saturation are used as inputs to the mapping table."));
	add(0xc6fa, QObject::tr("ProfileHueSatMapData1"), 1, 11, QObject::tr("This tag contains the data for the first hue/saturation/value mapping table. Each entry of the table contains three 32-bit IEEE floating-point values. The first entry is hue shift in degrees; the second entry is saturation scale factor; and the third entry is a value scale factor. The table entries are stored in the tag in nested loop order, with the value divisions in the outer loop, the hue divisions in the middle loop, and the saturation divisions in the inner loop. All zero input saturation entries are required to have a value scale factor of 1.0."));
	add(0xc6fb, QObject::tr("ProfileHueSatMapData2"), 1, 11, QObject::tr("This tag contains the data for the second hue/saturation/value mapping table. Each entry of the table contains three 32-bit IEEE floating-point values. The first entry is hue shift in degrees; the second entry is a saturation scale factor; and the third entry is a value scale factor. The table entries are stored in the tag in nested loop order, with the value divisions in the outer loop, the hue divisions in the middle loop, and the saturation divisions in the inner loop. All zero input saturation entries are required to have a value scale factor of 1.0."));
	add(0xc6fc, QObject::tr("ProfileToneCurve"), 1, 11, QObject::tr("This tag contains a default tone curve that can be applied while processing the image as a starting point for user adjustments. The curve is specified as a list of 32-bit IEEE floating-point value pairs in linear gamma. Each sample has an input value in the range of 0.0 to 1.0, and an output value in the range of 0.0 to 1.0. The first sample is required to be (0.0, 0.0), and the last sample is required to be (1.0, 1.0). Interpolated the curve using a cubic spline."));
	add(0xc6fd, QObject::tr("ProfileEmbedPolicy"), 1, 4, QObject::tr("This tag contains information about the usage rules for the associated camera profile."));
	add(0xc6fe, QObject::tr("ProfileCopyright"), 1, 1, QObject::tr("A UTF-8 encoded string containing the copyright information for the camera profile. This string always should be preserved along with the other camera profile tags."));
	add(0xc714, QObject::tr("ForwardMatrix1"), 1, 10, QObject::tr("This tag defines a matrix that maps white balanced camera colors to XYZ D50 colors."));
	add(0xc715, QObject::tr("ForwardMatrix2"), 1, 10, QObject::tr("This tag defines a matrix that maps white balanced camera colors to XYZ D50 colors."));
	add(0xc716, QObject::tr("PreviewApplicationName"), 1, 1, QObject::tr("A UTF-8 encoded string containing the name of the application that created the preview stored in the IFD."));
	add(0xc717, QObject::tr("PreviewApplicationVersion"), 1, 1, QObject::tr("A UTF-8 encoded string containing the version number of the application that created the preview stored in the IFD."));
	add(0xc718, QObject::tr("PreviewSettingsName"), 1, 1, QObject::tr("A UTF-8 encoded string containing the name of the conversion settings (for example, snapshot name) used for the preview stored in the IFD."));
	add(0xc719, QObject::tr("PreviewSettingsDigest"), 1, 1, QObject::tr("A unique ID of the conversion settings (for example, MD5 digest) used to render the preview stored in the IFD."));
	add(0xc71a, QObject::tr("PreviewColorSpace"), 1, 4, QObject::tr("This tag specifies the color space in which the rendered preview in this IFD is stored. The default value for this tag is sRGB for color previews and Gray Gamma 2.2 for monochrome previews."));
	add(0xc71b, QObject::tr("PreviewDateTime"), 1, 2, QObject::tr("This tag is an ASCII string containing the name of the date/time at which the preview stored in the IFD was rendered. The date/time is encoded using ISO 8601 format."));
	add(0xc71c, QObject::tr("RawImageDigest"), 1, 7, QObject::tr("This tag is an MD5 digest of the raw image data. All pixels in the image are processed in row-scan order. Each pixel is zero padded to 16 or 32 bits deep (16-bit for data less than or equal to 16 bits deep, 32-bit otherwise). The data for each pixel is processed in little-endian byte order."));
	add(0xc71d, QObject::tr("OriginalRawFileDigest"), 1, 7, QObject::tr("This tag is an MD5 digest of the data stored in the OriginalRawFileData tag."));
	add(0xc71e, QObject::tr("SubTileBlockSize"), 1, 4, QObject::tr("Normally, the pixels within a tile are stored in simple row-scan order. This tag specifies that the pixels within a tile should be grouped first into rectangular blocks of the specified size. These blocks are stored in row-scan order. Within each block, the pixels are stored in row-scan order. The use of a non-default value for this tag requires setting the DNGBackwardVersion tag to at least 1.2.0.0."));
	add(0xc71f, QObject::tr("RowInterleaveFactor"), 1, 4, QObject::tr("This tag specifies that rows of the image are stored in interleaved order. The value of the tag specifies the number of interleaved fields. The use of a non-default value for this tag requires setting the DNGBackwardVersion tag to at least 1.2.0.0."));
	add(0xc725, QObject::tr("ProfileLookTableDims"), 1, 4, QObject::tr("This tag specifies the number of input samples in each dimension of a default \"look\" table. The data for this table is stored in the ProfileLookTableData tag."));
	add(0xc726, QObject::tr("ProfileLookTableData"), 1, 11, QObject::tr("This tag contains a default \"look\" table that can be applied while processing the image as a starting point for user adjustment. This table uses the same format as the tables stored in the ProfileHueSatMapData1 and ProfileHueSatMapData2 tags, and is applied in the same color space. However, it should be applied later in the processing pipe, after any exposure compensation and/or fill light stages, but before any tone curve stage. Each entry of the table contains three 32-bit IEEE floating-point values. The first entry is hue shift in degrees, the second entry is a saturation scale factor, and the third entry is a value scale factor. The table entries are stored in the tag in nested loop order, with the value divisions in the outer loop, the hue divisions in the middle loop, and the saturation divisions in the inner loop. All zero input saturation entries are required to have a value scale factor of 1.0."));
	add(0xc740, QObject::tr("OpcodeList1"), 1, 7, QObject::tr("Specifies the list of opcodes that should be applied to the raw image, as read directly from the file."));
	add(0xc741, QObject::tr("OpcodeList2"), 1, 7, QObject::tr("Specifies the list of opcodes that should be applied to the raw image, just after it has been mapped to linear reference values."));
	add(0xc74e, QObject::tr("OpcodeList3"), 1, 7, QObject::tr("Specifies the list of opcodes that should be applied to the raw image, just after it has been demosaiced."));
	add(0xc761, QObject::tr("NoiseProfile"), 1, 11, QObject::tr("NoiseProfile describes the amount of noise in a raw image. Specifically, this tag models the amount of signal-dependent photon (shot) noise and signal-independent sensor readout noise, two common sources of noise in raw images. The model assumes that the noise is white and spatially independent, ignoring fixed pattern effects and other sources of noise (e.g., pixel response non-uniformity, spatially-dependent thermal effects, etc.)."));
	add(0xc763, QObject::tr("TimeCodes"), 1, 1, QObject::tr("The optional TimeCodes tag shall contain an ordered array of time codes. All time codes shall be 8 bytes long and in binary format. The tag may contain from 1 to 10 time codes. When the tag contains more than one time code, the first one shall be the default time code. This specification does not prescribe how to use multiple time codes. Each time code shall be as defined for the 8-byte time code structure in SMPTE 331M-2004, Section 8.3. See also SMPTE 12-1-2008 and SMPTE 309-1999."));
	add(0xc764, QObject::tr("FrameRate"), 1, 10, QObject::tr("The optional FrameRate tag shall specify the video frame rate in number of image frames per second, expressed as a signed rational number. The numerator shall be non-negative and the denominator shall be positive. This field value is identical to the sample rate field in SMPTE 377-1-2009."));
	add(0xc772, QObject::tr("TStop"), 1, 10, QObject::tr("The optional TStop tag shall specify the T-stop of the actual lens, expressed as an unsigned rational number. T-stop is also known as T-number or the photometric aperture of the lens. (F-number is the geometric aperture of the lens.) When the exact value is known, the T-stop shall be specified using a single number. Alternately, two numbers shall be used to indicate a T-stop range, in which case the first number shall be the minimum T-stop and the second number shall be the maximum T-stop."));
	add(0xc789, QObject::tr("ReelName"), 1, 2, QObject::tr("The optional ReelName tag shall specify a name for a sequence of images, where each image in the sequence has a unique image identifier (including but not limited to file name, frame number, date time, time code)."));
	add(0xc7a1, QObject::tr("CameraLabel"), 1, 2, QObject::tr("The optional CameraLabel tag shall specify a text label for how the camera is used or assigned in this clip. This tag is similar to CameraLabel in XMP."));
	add(0x829a, QObject::tr("ExposureTime"), 5, 5, QObject::tr("Exposure time, given in seconds (sec)."));
	add(0x829d, QObject::tr("FNumber"), 5, 5, QObject::tr("The F number."));
	add(0x8822, QObject::tr("ExposureProgram"), 5, 3, QObject::tr("The class of the program used by the camera to set exposure when the picture is taken."));
	add(0x8824, QObject::tr("SpectralSensitivity"), 5, 2, QObject::tr("Indicates the spectral sensitivity of each channel of the camera used. The tag value is an ASCII string compatible with the standard developed by the ASTM Technical Committee."));
	add(0x8827, QObject::tr("ISOSpeedRatings"), 5, 3, QObject::tr("Indicates the ISO Speed and ISO Latitude of the camera or input device as specified in ISO 12232."));
	add(0x8828, QObject::tr("OECF"), 5, 7, QObject::tr("Indicates the Opto-Electoric Conversion Function (OECF) specified in ISO 14524. <OECF> is the relationship between the camera optical input and the image values."));
	add(0x8830, QObject::tr("SensitivityType"), 5, 3, QObject::tr("The SensitivityType tag indicates which one of the parameters of ISO12232 is the PhotographicSensitivity tag. Although it is an optional tag, it should be recorded when a PhotographicSensitivity tag is recorded. Value = 4, 5, 6, or 7 may be used in case that the values of plural parameters are the same."));
	add(0x8831, QObject::tr("StandardOutputSensitivity"), 5, 4, QObject::tr("This tag indicates the standard output sensitivity value of a camera or input device defined in ISO 12232. When recording this tag, the PhotographicSensitivity and SensitivityType tags shall also be recorded."));
	add(0x8832, QObject::tr("RecommendedExposureIndex"), 5, 4, QObject::tr("This tag indicates the recommended exposure index value of a camera or input device defined in ISO 12232. When recording this tag, the PhotographicSensitivity and SensitivityType tags shall also be recorded."));
	add(0x8833, QObject::tr("ISOSpeed"), 5, 4, QObject::tr("This tag indicates the ISO speed value of a camera or input device that is defined in ISO 12232. When recording this tag, the PhotographicSensitivity and SensitivityType tags shall also be recorded."));
	add(0x8834, QObject::tr("ISOSpeedLatitudeyyy"), 5, 4, QObject::tr("This tag indicates the ISO speed latitude yyy value of a camera or input device that is defined in ISO 12232. However, this tag shall not be recorded without ISOSpeed and ISOSpeedLatitudezzz."));
	add(0x8835, QObject::tr("ISOSpeedLatitudezzz"), 5, 4, QObject::tr("This tag indicates the ISO speed latitude zzz value of a camera or input device that is defined in ISO 12232. However, this tag shall not be recorded without ISOSpeed and ISOSpeedLatitudeyyy."));
	add(0x9000, QObject::tr("ExifVersion"), 5, 7, QObject::tr("The version of this standard supported. Nonexistence of this field is taken to mean nonconformance to the standard."));
	add(0x9003, QObject::tr("DateTimeOriginal"), 5, 2, QObject::tr("The date and time when the original image data was generated. For a digital still camera the date and time the picture was taken are recorded."));
	add(0x9004, QObject::tr("DateTimeDigitized"), 5, 2, QObject::tr("The date and time when the image was stored as digital data."));
	add(0x9101, QObject::tr("ComponentsConfiguration"), 5, 7, QObject::tr("Information specific to compressed data. The channels of each component are arranged in order from the 1st component to the 4th. For uncompressed data the data arrangement is given in the <PhotometricInterpretation> tag. However, since <PhotometricInterpretation> can only express the order of Y, Cb and Cr, this tag is provided for cases when compressed data uses components other than Y, Cb, and Cr and to enable support of other sequences."));
	add(0x9102, QObject::tr("CompressedBitsPerPixel"), 5, 5, QObject::tr("Information specific to compressed data. The compression mode used for a compressed image is indicated in unit bits per pixel."));
	add(0x9201, QObject::tr("ShutterSpeedValue"), 5, 10, QObject::tr("Shutter speed. The unit is the APEX (Additive System of Photographic Exposure) setting."));
	add(0x9202, QObject::tr("ApertureValue"), 5, 5, QObject::tr("The lens aperture. The unit is the APEX value."));
	add(0x9203, QObject::tr("BrightnessValue"), 5, 10, QObject::tr("The value of brightness. The unit is the APEX value. Ordinarily it is given in the range of -99.99 to 99.99."));
	add(0x9204, QObject::tr("ExposureBiasValue"), 5, 10, QObject::tr("The exposure bias. The units is the APEX value. Ordinarily it is given in the range of -99.99 to 99.99."));
	add(0x9205, QObject::tr("MaxApertureValue"), 5, 5, QObject::tr("The smallest F number of the lens. The unit is the APEX value. Ordinarily it is given in the range of 00.00 to 99.99, but it is not limited to this range."));
	add(0x9206, QObject::tr("SubjectDistance"), 5, 5, QObject::tr("The distance to the subject, given in meters."));
	add(0x9207, QObject::tr("MeteringMode"), 5, 3, QObject::tr("The metering mode."));
	add(0x9208, QObject::tr("LightSource"), 5, 3, QObject::tr("The kind of light source."));
	add(0x9209, QObject::tr("Flash"), 5, 3, QObject::tr("This tag is recorded when an image is taken using a strobe light (flash)."));
	add(0x920a, QObject::tr("FocalLength"), 5, 5, QObject::tr("The actual focal length of the lens, in mm. Conversion is not made to the focal length of a 35 mm film camera."));
	add(0x9214, QObject::tr("SubjectArea"), 5, 3, QObject::tr("This tag indicates the location and area of the main subject in the overall scene."));
	add(0x927c, QObject::tr("MakerNote"), 5, 7, QObject::tr("A tag for manufacturers of Exif writers to record any desired information. The contents are up to the manufacturer."));
	add(0x9286, QObject::tr("UserComment"), 5, 7, QObject::tr("A tag for Exif users to write keywords or comments on the image besides those in <ImageDescription>, and without the character code limitations of the <ImageDescription> tag."));
	add(0x9290, QObject::tr("SubSecTime"), 5, 2, QObject::tr("A tag used to record fractions of seconds for the <DateTime> tag."));
	add(0x9291, QObject::tr("SubSecTimeOriginal"), 5, 2, QObject::tr("A tag used to record fractions of seconds for the <DateTimeOriginal> tag."));
	add(0x9292, QObject::tr("SubSecTimeDigitized"), 5, 2, QObject::tr("A tag used to record fractions of seconds for the <DateTimeDigitized> tag."));
	add(0xa000, QObject::tr("FlashpixVersion"), 5, 7, QObject::tr("The FlashPix format version supported by a FPXR file."));
	add(0xa001, QObject::tr("ColorSpace"), 5, 3, QObject::tr("The color space information tag is always recorded as the color space specifier. Normally sRGB is used to define the color space based on the PC monitor conditions and environment. If a color space other than sRGB is used, Uncalibrated is set. Image data recorded as Uncalibrated can be treated as sRGB when it is converted to FlashPix."));
	add(0xa002, QObject::tr("PixelXDimension"), 5, 4, QObject::tr("Information specific to compressed data. When a compressed file is recorded, the valid width of the meaningful image must be recorded in this tag, whether or not there is padding data or a restart marker. This tag should not exist in an uncompressed file."));
	add(0xa003, QObject::tr("PixelYDimension"), 5, 4, QObject::tr("Information specific to compressed data. When a compressed file is recorded, the valid height of the meaningful image must be recorded in this tag, whether or not there is padding data or a restart marker. This tag should not exist in an uncompressed file. Since data padding is unnecessary in the vertical direction, the number of lines recorded in this valid image height tag will in fact be the same as that recorded in the SOF."));
	add(0xa004, QObject::tr("RelatedSoundFile"), 5, 2, QObject::tr("This tag is used to record the name of an audio file related to the image data. The only relational information recorded here is the Exif audio file name and extension (an ASCII string consisting of 8 characters + '.' + 3 characters). The path is not recorded."));
	add(0xa005, QObject::tr("InteroperabilityTag"), 5, 4, QObject::tr("Interoperability IFD is composed of tags which stores the information to ensure the Interoperability and pointed by the following tag located in Exif IFD. The Interoperability structure of Interoperability IFD is the same as TIFF defined IFD structure but does not contain the image data characteristically compared with normal TIFF IFD."));
	add(0xa20b, QObject::tr("FlashEnergy"), 5, 5, QObject::tr("Indicates the strobe energy at the time the image is captured, as measured in Beam Candle Power Seconds (BCPS)."));
	add(0xa20c, QObject::tr("SpatialFrequencyResponse"), 5, 7, QObject::tr("This tag records the camera or input device spatial frequency table and SFR values in the direction of image width, image height, and diagonal direction, as specified in ISO 12233."));
	add(0xa20e, QObject::tr("FocalPlaneXResolution"), 5, 5, QObject::tr("Indicates the number of pixels in the image width (X) direction per <FocalPlaneResolutionUnit> on the camera focal plane."));
	add(0xa20f, QObject::tr("FocalPlaneYResolution"), 5, 5, QObject::tr("Indicates the number of pixels in the image height (V) direction per <FocalPlaneResolutionUnit> on the camera focal plane."));
	add(0xa210, QObject::tr("FocalPlaneResolutionUnit"), 5, 3, QObject::tr("Indicates the unit for measuring <FocalPlaneXResolution> and <FocalPlaneYResolution>. This value is the same as the <ResolutionUnit>."));
	add(0xa214, QObject::tr("SubjectLocation"), 5, 3, QObject::tr("Indicates the location of the main subject in the scene. The value of this tag represents the pixel at the center of the main subject relative to the left edge, prior to rotation processing as per the <Rotation> tag. The first value indicates the X column number and second indicates the Y row number."));
	add(0xa215, QObject::tr("ExposureIndex"), 5, 5, QObject::tr("Indicates the exposure index selected on the camera or input device at the time the image is captured."));
	add(0xa217, QObject::tr("SensingMethod"), 5, 3, QObject::tr("Indicates the image sensor type on the camera or input device."));
	add(0xa300, QObject::tr("FileSource"), 5, 7, QObject::tr("Indicates the image source. If a DSC recorded the image, this tag value of this tag always be set to 3, indicating that the image was recorded on a DSC."));
	add(0xa301, QObject::tr("SceneType"), 5, 7, QObject::tr("Indicates the type of scene. If a DSC recorded the image, this tag value must always be set to 1, indicating that the image was directly photographed."));
	add(0xa302, QObject::tr("CFAPattern"), 5, 7, QObject::tr("Indicates the color filter array (CFA) geometric pattern of the image sensor when a one-chip color area sensor is used. It does not apply to all sensing methods."));
	add(0xa401, QObject::tr("CustomRendered"), 5, 3, QObject::tr("This tag indicates the use of special processing on image data, such as rendering geared to output. When special processing is performed, the reader is expected to disable or minimize any further processing."));
	add(0xa402, QObject::tr("ExposureMode"), 5, 3, QObject::tr("This tag indicates the exposure mode set when the image was shot. In auto-bracketing mode, the camera shoots a series of frames of the same scene at different exposure settings."));
	add(0xa403, QObject::tr("WhiteBalance"), 5, 3, QObject::tr("This tag indicates the white balance mode set when the image was shot."));
	add(0xa404, QObject::tr("DigitalZoomRatio"), 5, 5, QObject::tr("This tag indicates the digital zoom ratio when the image was shot. If the numerator of the recorded value is 0, this indicates that digital zoom was not used."));
	add(0xa405, QObject::tr("FocalLengthIn35mmFilm"), 5, 3, QObject::tr("This tag indicates the equivalent focal length assuming a 35mm film camera, in mm. A value of 0 means the focal length is unknown. Note that this tag differs from the <FocalLength> tag."));
	add(0xa406, QObject::tr("SceneCaptureType"), 5, 3, QObject::tr("This tag indicates the type of scene that was shot. It can also be used to record the mode in which the image was shot. Note that this differs from the <SceneType> tag."));
	add(0xa407, QObject::tr("GainControl"), 5, 3, QObject::tr("This tag indicates the degree of overall image gain adjustment."));
	add(0xa408, QObject::tr("Contrast"), 5, 3, QObject::tr("This tag indicates the direction of contrast processing applied by the camera when the image was shot."));
	add(0xa409, QObject::tr("Saturation"), 5, 3, QObject::tr("This tag indicates the direction of saturation processing applied by the camera when the image was shot."));
	add(0xa40a, QObject::tr("Sharpness"), 5, 3, QObject::tr("This tag indicates the direction of sharpness processing applied by the camera when the image was shot."));
	add(0xa40b, QObject::tr("DeviceSettingDescription"), 5, 7, QObject::tr("This tag indicates information on the picture-taking conditions of a particular camera model. The tag is used only to indicate the picture-taking conditions in the reader."));
	add(0xa40c, QObject::tr("SubjectDistanceRange"), 5, 3, QObject::tr("This tag indicates the distance to the subject."));
	add(0xa420, QObject::tr("ImageUniqueID"), 5, 2, QObject::tr("This tag indicates an identifier assigned uniquely to each image. It is recorded as an ASCII string equivalent to hexadecimal notation and 128-bit fixed length."));
	add(0xa430, QObject::tr("CameraOwnerName"), 5, 2, QObject::tr("This tag records the owner of a camera used in photography as an ASCII string."));
	add(0xa431, QObject::tr("BodySerialNumber"), 5, 2, QObject::tr("This tag records the serial number of the body of the camera that was used in photography as an ASCII string."));
	add(0xa432, QObject::tr("LensSpecification"), 5, 5, QObject::tr("This tag notes minimum focal length, maximum focal length, minimum F number in the minimum focal length, and minimum F number in the maximum focal length, which are specification information for the lens that was used in photography. When the minimum F number is unknown, the notation is 0/0"));
	add(0xa433, QObject::tr("LensMake"), 5, 2, QObject::tr("This tag records the lens manufactor as an ASCII string."));
	add(0xa434, QObject::tr("LensModel"), 5, 2, QObject::tr("This tag records the lens's model name and model number as an ASCII string."));
	add(0xa435, QObject::tr("LensSerialNumber"), 5, 2, QObject::tr("This tag records the serial number of the interchangeable lens that was used in photography as an ASCII string."));
	add(0x0001, QObject::tr("InteroperabilityIndex"), 7, 2, QObject::tr("Indicates the identification of the Interoperability rule. Use \"R98\" for stating ExifR98 Rules. Four bytes used including the termination code (NULL). see the separate volume of Recommended Exif Interoperability Rules (ExifR98) for other tags used for ExifR98."));
	add(0x0002, QObject::tr("InteroperabilityVersion"), 7, 7, QObject::tr("Interoperability version"));
	add(0x1000, QObject::tr("RelatedImageFileFormat"), 7, 2, QObject::tr("File format of image file"));
	add(0x1001, QObject::tr("RelatedImageWidth"), 7, 4, QObject::tr("Image width"));
	add(0x1002, QObject::tr("RelatedImageLength"), 7, 4, QObject::tr("Image height"));
	add(0x0000, QObject::tr("GPSVersionID"), 6, 1, QObject::tr("Indicates the version of <GPSInfoIFD>. The version is given as 2.0.0.0. This tag is mandatory when <GPSInfo> tag is present. (Note: The <GPSVersionID> tag is given in bytes, unlike the <ExifVersion> tag. When the version is 2.0.0.0, the tag value is 02000000.H)."));
	add(0x0001, QObject::tr("GPSLatitudeRef"), 6, 2, QObject::tr("Indicates whether the latitude is north or south latitude. The ASCII value 'N' indicates north latitude, and 'S' is south latitude."));
	add(0x0002, QObject::tr("GPSLatitude"), 6, 5, QObject::tr("Indicates the latitude. The latitude is expressed as three RATIONAL values giving the degrees, minutes, and seconds, respectively. When degrees, minutes and seconds are expressed, the format is dd/1,mm/1,ss/1. When degrees and minutes are used and, for example, fractions of minutes are given up to two decimal places, the format is dd/1,mmmm/100,0/1."));
	add(0x0003, QObject::tr("GPSLongitudeRef"), 6, 2, QObject::tr("Indicates whether the longitude is east or west longitude. ASCII 'E' indicates east longitude, and 'W' is west longitude."));
	add(0x0004, QObject::tr("GPSLongitude"), 6, 5, QObject::tr("Indicates the longitude. The longitude is expressed as three RATIONAL values giving the degrees, minutes, and seconds, respectively. When degrees, minutes and seconds are expressed, the format is ddd/1,mm/1,ss/1. When degrees and minutes are used and, for example, fractions of minutes are given up to two decimal places, the format is ddd/1,mmmm/100,0/1."));
	add(0x0005, QObject::tr("GPSAltitudeRef"), 6, 1, QObject::tr("Indicates the altitude used as the reference altitude. If the reference is sea level and the altitude is above sea level, 0 is given. If the altitude is below sea level, a value of 1 is given and the altitude is indicated as an absolute value in the GSPAltitude tag. The reference unit is meters. Note that this tag is BYTE type, unlike other reference tags."));
	add(0x0006, QObject::tr("GPSAltitude"), 6, 5, QObject::tr("Indicates the altitude based on the reference in GPSAltitudeRef. Altitude is expressed as one RATIONAL value. The reference unit is meters."));
	add(0x0007, QObject::tr("GPSTimeStamp"), 6, 5, QObject::tr("Indicates the time as UTC (Coordinated Universal Time). <TimeStamp> is expressed as three RATIONAL values giving the hour, minute, and second (atomic clock)."));
	add(0x0008, QObject::tr("GPSSatellites"), 6, 2, QObject::tr("Indicates the GPS satellites used for measurements. This tag can be used to describe the number of satellites, their ID number, angle of elevation, azimuth, SNR and other information in ASCII notation. The format is not specified. If the GPS receiver is incapable of taking measurements, value of the tag is set to NULL."));
	add(0x0009, QObject::tr("GPSStatus"), 6, 2, QObject::tr("Indicates the status of the GPS receiver when the image is recorded. \"A\" means measurement is in progress, and \"V\" means the measurement is Interoperability."));
	add(0x000a, QObject::tr("GPSMeasureMode"), 6, 2, QObject::tr("Indicates the GPS measurement mode. \"2\" means two-dimensional measurement and \"3\" means three-dimensional measurement is in progress."));
	add(0x000b, QObject::tr("GPSDOP"), 6, 5, QObject::tr("Indicates the GPS DOP (data degree of precision). An HDOP value is written during two-dimensional measurement, and PDOP during three-dimensional measurement."));
	add(0x000c, QObject::tr("GPSSpeedRef"), 6, 2, QObject::tr("Indicates the unit used to express the GPS receiver speed of movement. \"K\" \"M\" and \"N\" represents kilometers per hour, miles per hour, and knots."));
	add(0x000d, QObject::tr("GPSSpeed"), 6, 5, QObject::tr("Indicates the speed of GPS receiver movement."));
	add(0x000e, QObject::tr("GPSTrackRef"), 6, 2, QObject::tr("Indicates the reference for giving the direction of GPS receiver movement. \"T\" denotes true direction and \"M\" is magnetic direction."));
	add(0x000f, QObject::tr("GPSTrack"), 6, 5, QObject::tr("Indicates the direction of GPS receiver movement. The range of values is from 0.00 to 359.99."));
	add(0x0010, QObject::tr("GPSImgDirectionRef"), 6, 2, QObject::tr("Indicates the reference for giving the direction of the image when it is captured. \"T\" denotes true direction and \"M\" is magnetic direction."));
	add(0x0011, QObject::tr("GPSImgDirection"), 6, 5, QObject::tr("Indicates the direction of the image when it was captured. The range of values is from 0.00 to 359.99."));
	add(0x0012, QObject::tr("GPSMapDatum"), 6, 2, QObject::tr("Indicates the geodetic survey data used by the GPS receiver. If the survey data is restricted to Japan, the value of this tag is \"TOKYO\" or \"WGS-84\"."));
	add(0x0013, QObject::tr("GPSDestLatitudeRef"), 6, 2, QObject::tr("Indicates whether the latitude of the destination point is north or south latitude. The ASCII value \"N\" indicates north latitude, and \"S\" is south latitude."));
	add(0x0014, QObject::tr("GPSDestLatitude"), 6, 5, QObject::tr("Indicates the latitude of the destination point. The latitude is expressed as three RATIONAL values giving the degrees, minutes, and seconds, respectively. If latitude is expressed as degrees, minutes and seconds, a typical format would be dd/1,mm/1,ss/1. When degrees and minutes are used and, for example, fractions of minutes are given up to two decimal places, the format would be dd/1,mmmm/100,0/1."));
	add(0x0015, QObject::tr("GPSDestLongitudeRef"), 6, 2, QObject::tr("Indicates whether the longitude of the destination point is east or west longitude. ASCII \"E\" indicates east longitude, and \"W\" is west longitude."));
	add(0x0016, QObject::tr("GPSDestLongitude"), 6, 5, QObject::tr("Indicates the longitude of the destination point. The longitude is expressed as three RATIONAL values giving the degrees, minutes, and seconds, respectively. If longitude is expressed as degrees, minutes and seconds, a typical format would be ddd/1,mm/1,ss/1. When degrees and minutes are used and, for example, fractions of minutes are given up to two decimal places, the format would be ddd/1,mmmm/100,0/1."));
	add(0x0017, QObject::tr("GPSDestBearingRef"), 6, 2, QObject::tr("Indicates the reference used for giving the bearing to the destination point. \"T\" denotes true direction and \"M\" is magnetic direction."));
	add(0x0018, QObject::tr("GPSDestBearing"), 6, 5, QObject::tr("Indicates the bearing to the destination point. The range of values is from 0.00 to 359.99."));
	add(0x0019, QObject::tr("GPSDestDistanceRef"), 6, 2, QObject::tr("Indicates the unit used to express the distance to the destination point. \"K\"), \"M\" and \"N\" represent kilometers, miles and knots."));
	add(0x001a, QObject::tr("GPSDestDistance"), 6, 5, QObject::tr("Indicates the distance to the destination point."));
	add(0x001b, QObject::tr("GPSProcessingMethod"), 6, 7, QObject::tr("A character string recording the name of the method used for location finding. The first byte indicates the character code used, and this is followed by the name of the method."));
	add(0x001c, QObject::tr("GPSAreaInformation"), 6, 7, QObject::tr("A character string recording the name of the GPS area. The first byte indicates the character code used, and this is followed by the name of the GPS area."));
	add(0x001d, QObject::tr("GPSDateStamp"), 6, 2, QObject::tr("A character string recording date and time information relative to UTC (Coordinated Universal Time). The format is \"YYYY:MM:DD.\"."));
	add(0x001e, QObject::tr("GPSDifferential"), 6, 3, QObject::tr("Indicates whether differential correction is applied to the GPS receiver."));
}

cEXIFTag* cEXIFTagList::add(const qint32& iTAGID, const QString& szTAGName, const qint32& iIFDID, const qint32& iTypeID, const QString& szDescription)
{
	cEXIFTag*	lpNew	= find(iTAGID, iIFDID);

	if(lpNew)
		return(nullptr);

	lpNew	= new cEXIFTag(iTAGID, szTAGName, iIFDID, iTypeID, szDescription);
	append(lpNew);
	return(lpNew);
}

cEXIFTag* cEXIFTagList::find(const qint32& iTAGID, const qint32& iIFDID)
{
	for(int x = 0;x < count();x++)
	{
		cEXIFTag*	lpTag	= at(x);

		if(lpTag->m_iTAGID == iTAGID && lpTag->m_iIFDID == iIFDID)
			return(lpTag);
	}
	return(nullptr);
}

cEXIFTag* cEXIFValue::exifTag()
{
	return(m_lpEXIFTag);
}

cEXIFValue::cEXIFValue(cEXIFTag* lpEXIFTag) :
	m_lpEXIFTag(lpEXIFTag)
{
}

QVariant cEXIFValue::convertValue(const QString& szValue, qint32 iTypeId)
{
	QVariant	variant;

	switch(iTypeId)
	{
	case 1:	//byte
	case 3: //short
	case 4: //long
	case 8: //signed short
		variant = QVariant::fromValue(szValue.toLong());
		break;
	case 5: //rational
	case 10: //signed rational
	case 11: //float
		if(szValue.contains("/"))
		{
			QStringList	tmp	= szValue.split("/");
			variant = QVariant::fromValue(tmp[0].toDouble()/tmp[1].toDouble());
		}
		else
			variant	= QVariant::fromValue(szValue.toDouble());
		break;
	default:
		variant	= szValue;
	}
	return(variant);
}

void cEXIFValue::setValue(const QString& szValue, qint32 iTypeId, qint32 iCount)
{
	if(iCount == 1 || iTypeId == 2 || iTypeId == 7)
	{
		m_valueList.append(convertValue(szValue, iTypeId));
		return;
	}

	QStringList	valueList	= szValue.split(" ");

	if(valueList.count() != iCount)
		return;

	for(int x = 0;x < iCount;x++)
		m_valueList.append(convertValue(valueList[x], iTypeId));
}

QVariant cEXIFValue::value()
{
	if(m_valueList.count())
		return(m_valueList[0]);

	return(QVariant());
}

QList<QVariant> cEXIFValue::valueList()
{
	return(m_valueList);
}

cEXIFValueList::cEXIFValueList()
{
}

cEXIFValue* cEXIFValueList::add(cEXIFTag* lpEXIFTag)
{
	cEXIFValue*	lpNew	= find(lpEXIFTag);

	if(lpNew)
		return(nullptr);

	lpNew	= new cEXIFValue(lpEXIFTag);

	append(lpNew);
	return(lpNew);
}

cEXIFValue* cEXIFValueList::find(cEXIFTag* lpEXIFTag)
{
	for(int x = 0;x < count();x++)
	{
		cEXIFValue*	lpValue	= at(x);
		if(lpValue->exifTag() == lpEXIFTag)
			return(lpValue);
	}
	return(nullptr);
}
