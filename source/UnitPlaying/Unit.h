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
	/// Unique identifier of this unit on the world, for example "sum_xy"
	QString id;
	/// Version of this unit. It is not botnu xml notation version
	QString version;
	/// RAM memory limit to run. It's a size in bytes
	int ramSize;
	/// True if this unit/exercise requires heap-segment (dynamic memory), therefore, player is
	/// supposed to use new/delete operators or malloc/free functions. By default is false, to
	/// avoid showing the heap segment to trainees
	bool heapSegment;
	/// Number of CPU or cores required to resolve this unit. It affects the number of workstations
	/// available in the stage set. Robots represent execution threads. A program can have less or
	/// more threads than CPU cores, but it is not wise. Default value is 1 cpu
	int cpuCores;
	/// Maximum number of seconds to consider an solution valid
	/// a value <= 0 stands for infinite
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
	/// Unique identifier of this unit on the world
	inline const QString& getId() const { return id; }
	/// Version of this unit. It is not botnu xml notation version
	inline const QString& getVersion() const { return version; }
	/// RAM memory limit to run
	inline int getRamSize() const { return ramSize; }
	/// True if this unit/exercise requires heap-segment (dynamic memory)
	inline bool requiresHeapSegment() const { return heapSegment; }
	/// Number of CPU or cores required to resolve this unit.
	inline int getCpuCores() const { return cpuCores; }
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
	/// The document element has lots of attributes, and a dedicated method to load them
	bool loadDocumentAttributes(QXmlStreamReader& xmlReader);
	/// All botnu elements are direct children of the document root element, this method loads
	/// the next child. Returns true on success, false if the child is unknown
	bool loadDocumentChild(QXmlStreamReader& xmlReader);
	/// Load a test case pair of input/ouput data
	bool loadTestCase(QXmlStreamReader& xmlReader);
};

#endif // UNIT_H
