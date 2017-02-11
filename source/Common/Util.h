#ifndef UTIL_H
#define UTIL_H

#include <QColor>
#include <QList>

class Util
{
	Q_DISABLE_COPY(Util)

  public:
	/// This is a static-pure class
	Util() = delete;
	/// Merge colors using the ratio first/second
	static QColor mixColors(const QColor& first, const QColor& second, qreal ratio = 0.5);
	/// Calculate the color that is the average of the given colors in the list
	/// Returns black if the list is empty
	static QColor averageColors(const QList<QColor>& colors);

  public:
	/// Creates the directory with the given path. If there are inexistent directories in the
	/// middle of the path, they will be created as well.
	/// @return true on success, false on error
	static bool createDirectory(const QString& dirPath);
	/// Read all lines from the give filename
	/// @return A list of the read lines, empty list on error
	static QStringList readAllLines(const QString& filepath);
};

/// Copies Qt resources or Strings to text files
class ResourceToFileDumper
{
  public:
	/// When dumping a text resource, some search-and-replace actions can be done
	typedef QList< QPair<QString, QString> > SearchAndReplaceList;

  protected:
	/// A list of search-and-replace actions that should be done when dumping text resources
	SearchAndReplaceList searchAndReplaceRules;

  public:
	/// Creates or overwites the outputFilename, and writes data as the file contents
	/// @return true on success, false otherwise
	bool dumpString(const QString& data, const QString& outputFilename);
	/// Creates or overwites the outputFilename, and writes the string list one string per line
	/// @return true on success, false otherwise
	bool dumpStringList(const QStringList& data, const QString& outputFilename);
	/// Adds a search-and-replace rule to be done while dumping a text resource
	void addSearchAndReplace(const QString& search, const QString& replace);
	/// Creates a text file with the contents of the given resource
	/// @param resource Path to the resource, e.g: ":/path/to/resource.ext"
	/// @param outputFilename Path to the desired file that will content the resource
	/// @return true on success, false otherwise
	bool dumpTextResource(const QString& resource, const QString& outputFilename);
};

#endif // UTIL_H
