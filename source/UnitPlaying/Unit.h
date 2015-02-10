#ifndef UNIT_H
#define UNIT_H

#include <QObject>
#include <QHash>
#include <QList>
#include <QPair>

class QXmlStreamReader;

class Unit : public QObject
{
	Q_OBJECT
	Q_DISABLE_COPY(Unit)

  protected:
	/// Name of the unit, for example "sum_xy". It is not required to be unique in the world
	QString name;
	/// Unique identifier of this unit on the world
	QString id;
	/// Version of this unit. It is not botnu xml notation version
	QString version;
	/// RAM memory limit to run
	QString ram; // must be changed to integer
	/// Maximum number of seconds to consider an solution valid
	int timeout;
	/// Task description in one or several languages
	QHash<QString, QString> descriptions;
	/// Partial solution or initial code provided in botnu xml file
	QString initialCode;
	/// A solution to the problem asked
	QString solution;
	/// Code to generate test cases
	QString generator;
	/// Test cases provided in .botnu file, they are pairs of input/ouput text
	QList< QPair<QString, QString> > testCases;

  public:
	/// Constructor
	explicit Unit(QObject *parent = 0);
	/// Destructor
	virtual ~Unit();
	/// Loads the unit from the given .botnu xml filename
	/// @return true on success, false otherwise
	bool load(const QString& filename);
	void print();

  protected:
	/// Load the document element (root node) and direct descendants
	bool loadDocument(QXmlStreamReader& xmlReader);
	/// Load a test case pair of input/ouput data
	bool loadTestCase(QXmlStreamReader& xmlReader);
};

#endif // UNIT_H
