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
};

#endif // UTIL_H
