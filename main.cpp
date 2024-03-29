#include <QCoreApplication>

#include "cpicture.h"

#include <QCommandLineParser>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QMimeDatabase>

#include <QDebug>

#include <iostream>
#include <cstdlib>

#define SEPARATOR	"\t"


void readDirectory(const QString& szPath, QTextStream& out)
{
	QTextStream textOut(stdout);

	textOut << "*** DIRECTORY ***: " << szPath << "\n";

	QMimeDatabase	mimeDB;
	QDir			dir(szPath);
	QStringList		szDirs	= dir.entryList(QDir::Dirs);
	QFileInfoList	szFiles	= dir.entryInfoList(QDir::Files);
	QDateTime		oldDateTime;

	szDirs.removeAll(".");
	szDirs.removeAll("..");

	for(int x = 0;x < szDirs.count();x++)
		readDirectory(szPath + QDir::separator() + szDirs[x], out);

	for(int x = 0;x < szFiles.count();x++)
	{
		QFileInfo	fileInfo	= szFiles[x];
		QMimeType	mimeType	= mimeDB.mimeTypeForFile(szFiles[x]);

		if(mimeType.name().startsWith("image"))
		{
			textOut << "--- File: " << fileInfo.fileName() << "\n";

			cPicture		picture;
			if(picture.fromFile(fileInfo.filePath()))
				out << fileInfo.absolutePath() << SEPARATOR << fileInfo.fileName() << SEPARATOR << fileInfo.size() << SEPARATOR << picture.dateTime().toString("yyyy-MM-dd hh:mm:ss") << SEPARATOR << picture.imageWidth() << SEPARATOR << picture.imageHeight() << SEPARATOR << picture.cameraModel() << "\n";
		}
	}
}

int main(int argc, char *argv[])
{
	QCoreApplication	a(argc, argv);
	QCommandLineParser	parser;

	parser.setApplicationDescription("qtEXIF2File help");
	parser.addHelpOption();
	parser.addVersionOption();
	parser.addPositionalArgument("source", QCoreApplication::translate("main", "directory to parse"));
	parser.addPositionalArgument("destination", QCoreApplication::translate("main", "output file name"));

	parser.process(a);

	const QStringList	args	= parser.positionalArguments();
	QFile				file(args[1]);
	QDir				dir(args[0]);

	if(dir.exists())
	{
		if(file.open(QFile::WriteOnly | QFile::Truncate))
		{
			QTextStream	out(&file);

			out << "directory" << SEPARATOR << "name" << SEPARATOR << "size" << SEPARATOR << "date" << SEPARATOR << "width" << SEPARATOR << "height" << SEPARATOR << "camera\n";

			readDirectory(args[0], out);
		}
		file.close();
	}
	return(0);
//	return a.exec();
}
