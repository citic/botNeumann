#ifndef UTIL_H
#define UTIL_H

#include <QColor>
#include <QList>

class QTextStream;

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
	/// Find the first character of the first file where the second file differs, and returns the
	/// index of that character. If both files are identical, -1 is returned
	/// If an error happens, it returns another negative number
	static long findFirstDiff(const QString& filepath1, const QString& filepath2, bool ignoreWhitespace, bool caseSensitive);
	/// Get the next character of the given @a input
	/// @param eatWhitespace Send true if the current character is whitespace. If @a ignoreWhitespace
	/// is also true, the function will ignore the next whitespace under the input's cursor
	/// @param position If the address of a long variable is given, it will be increased by the
	/// number of read (or skipped if @a ignoreWhitespace is true) characters from input
	static QChar readNextChar(QTextStream& input, bool ignoreWhitespace, bool caseSensitive, bool eatWhitespace, long* position = nullptr);
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
