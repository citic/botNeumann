#ifndef UNIT_H
#define UNIT_H

#include <QObject>
#include <QHash>
#include <QList>
#include <QPair>
#include <QStringList>

class QXmlStreamReader;

class Unit : public QObject
{
	Q_OBJECT
	Q_DISABLE_COPY(Unit)

  public:
	/// Data type for descriptions in several languages
	typedef QHash<QString, QString> Descriptions;
	/// Test cases are pairs of input/output strings
	typedef QList< QPair<QString, QString> > TestCases;

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
	Descriptions descriptions;
	/// Partial solution or initial code provided in botnu xml file
	QString initialCode;
	/// A solution to the problem asked
	QStringList solutions;
	/// Code to generate test cases
	QString generator;
	/// Test cases provided in .botnu file, they are pairs of input/ouput text
	TestCases testCases;

  public:
	/// Constructor
	explicit Unit(QObject *parent = 0);
	/// Destructor
	virtual ~Unit();
	/// Loads the unit from the given .botnu xml filename
	/// @return true on success, false otherwise
	bool load(const QString& filename);
	/// Name of the unit, for example "sum_xy". It is not required to be unique in the world
	inline const QString& getName() const { return name; }
	/// Unique identifier of this unit on the world
	inline const QString& getId() const { return id; }
	/// Version of this unit. It is not botnu xml notation version
	inline const QString& getVersion() const { return version; }
	/// RAM memory limit to run
	inline const QString& getRam() const { return ram; }
	/// Maximum number of seconds to consider an solution valid
	inline int getTimeout() const { return timeout; }
	/// Task description in one or several languages
	inline const Descriptions& getDescriptions() const { return descriptions; }
	/// Partial solution or initial code provided in botnu xml file
	inline const QString& getInitialCode() const { return initialCode; }
	/// A solution to the problem asked
	inline const QStringList& getSolutions() const { return solutions; }
	/// Code to generate test cases
	inline const QString& getGenerator() const { return generator; }
	/// Test cases provided in .botnu file, they are pairs of input/ouput text
	inline const TestCases& getTestCases() const { return testCases; }
	/// For debugging purposes
	void print();

  protected:
	/// Load the document element (root node) and direct descendants
	bool loadDocument(QXmlStreamReader& xmlReader);
	/// Load a test case pair of input/ouput data
	bool loadTestCase(QXmlStreamReader& xmlReader);
};

#endif // UNIT_H
