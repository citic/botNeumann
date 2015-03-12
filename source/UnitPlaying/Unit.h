#ifndef UNIT_H
#define UNIT_H

#include <QObject>
#include <QHash>
#include <QList>
#include <QPair>
#include <QStringList>

class QXmlStreamReader;

/// The size in bytes of the largest data type. It must not be sliced in visualization
/// It is represented by 'a' in the math model. The default is a double value (8 bytes)
static const int largestDataTypeSize = sizeof(double); // bytes

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
	/// The version of the botnu XML notation used. It may be used by botNeumann to know how to
	/// read the file. If omitted, the last version supported by botNeumann instance be assumed.
	QString botnuVersion;
	/// Specifies the width of the datapaths of the archicture: 32-bits or 64-bits. It affects the
	/// size of integers and memory addresses (pointers). Default is 32-bits.
	int architecture;
	/// Number of CPU or cores available to resolve this unit. It affects the number of workstations
	/// available in the stage set. Robots represent execution threads. A program can have less or
	/// more threads than CPU cores, but it is not wise. Default value is 1 cpu
	int cpuCores;
	/// RAM memory limit to run. It's a size in bytes
	size_t ramSize;
	/// True if this unit/exercise requires heap-segment (dynamic memory), therefore, player is
	/// supposed to use new/delete operators or malloc/free functions. By default is false, to
	/// avoid showing the heap segment to trainees
	bool heapSegment;
	/// Minimun number of execution threads (represented as robots) that students must implement in
	/// order to pass the level (fix the damaged unit). Default is 1.
	int minThreads;
	/// Maximum number of execution threads that students must launch in order to solve the unit.
	/// Value "0" means infinity number of threads. Default value is 0.
	int maxThreads;
	/// Maximum number of seconds to consider an solution valid
	/// a value <= 0 stands for infinite
	int timeout;
	/// In order to evaluate if sudent's solution is valid, its output is compared against the
	/// output generated by the solution program or the expected output of test case. If this
	/// attribute has the value true, the output will be compared ignoring whitespace differences.
	/// If this attribute is false and there are whitespace differences between the student's output
	/// and the expected output, the student's solution will be considered as erroneous. If a value
	/// different than "no" is provided in botnu file, true will be assumed.
	bool ignoreWhitespace;
	/// Task description in one or several languages
	Descriptions descriptions;
	/// Partial solution or initial code provided in botnu xml file
	QStringList initialCodes;
	/// A solution to the problem asked
	QStringList solutions;
	/// Code to generate test cases
	QStringList generators;
	/// Test cases provided in .botnu file, they are pairs of input/ouput text
	TestCases testCases;
	/// Number of columns in the memory distribution model (k). It is calculated after loading the
	/// unit from the botnu xml file
	size_t columns;
	/// Visible rows of the stack segment (sv)
	size_t stackSegmentVisibleRows;

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
	/// Version of the botnu xml notation used in .botnu file
	inline const QString& getBotnuVersion() const { return botnuVersion; }
	/// Get the number of bits of the simulated architecture (32 or 64)
	inline int getArchitecture() const { return architecture; }
	/// Returns true if the architecture of the given number of bits are supported by botNeumann
	static bool isArchitectureSupported(int bits);
	/// RAM memory limit to run
	inline int getRamSize() const { return ramSize; }
	/// True if this unit/exercise requires heap-segment (dynamic memory)
	inline bool requiresHeapSegment() const { return heapSegment; }
	/// Number of CPU or cores available to resolve this unit.
	inline int getCpuCores() const { return cpuCores; }
	/// Mininum number of execution threads (robots) required to resolve this unit.
	inline int getMinThreads() const { return minThreads; }
	/// Maximum number of seconds to consider an solution valid
	inline int getTimeout() const { return timeout; }
	/// True if player solution output must be compared ignoring whitespace against the answer
	inline bool shouldIgnoreWhitespace() const { return ignoreWhitespace; }

  public:
	/// Task description in one or several languages
	inline const Descriptions& getDescriptions() const { return descriptions; }
	/// Get the description for the given language, it it does not exist, returns the default
	const QString getDescription(const QString& language) const;
	/// Partial solution or initial code provided in botnu xml file
	inline const QStringList& getInitialCodes() const { return initialCodes; }
	/// Gets a random selected initial code
	QString getARandomInitialCode() const;
	/// A solution to the problem asked
	inline const QStringList& getSolutions() const { return solutions; }
	/// Gets a random selected solution
	QString getARandomSolution() const;
	/// Code to generate test cases
	inline const QStringList& getGenerators() const { return generators; }
	/// Gets a random selected generator
	QString getARandomGenerator() const;
	/// Test cases provided in .botnu file, they are pairs of input/ouput text
	inline const TestCases& getTestCases() const { return testCases; }
	/// For debugging purposes
	void print();

  public: // Memory distribution model
	/// Rv: bytes
	inline size_t getVisibleRam() const { return 7 * ramSize / 15; }
	/// Ri: bytes
	inline size_t getInvisibleRam() const { return 8 * ramSize / 15; }
	/// a: bytes
	inline size_t getLargestDataType() const { return largestDataTypeSize; }
	/// k: columns
	inline size_t getColums() const { return columns; }
	/// kc: colums/core
	inline size_t getColumsPerCore() const { return columns / cpuCores; }
	/// r: rows
	inline size_t getRows() const { return 15 * stackSegmentVisibleRows / 2; }
	/// rv: rows
	inline size_t getVisibleRows() const { return 7 * stackSegmentVisibleRows / 2; }
	/// ri: rows
	inline size_t getInvisibleRows() const { return 4 * stackSegmentVisibleRows; }
	/// Converts complete rows to bytes
	inline size_t rowsToBytes(size_t rows) const { return rows * columns * largestDataTypeSize; }

	/// h
	inline size_t getHeapSegmentRows() const { return 2 * stackSegmentVisibleRows; }
	/// hb
	inline size_t getHeapSegmentStartRow() const { return 0; }
	/// H
	inline size_t getHeapSegmentSize() const { return rowsToBytes(getHeapSegmentRows()); }
	/// Hb
	inline size_t getHeapSegmentStartByte() const { return 0; }

	/// s: rows
	inline size_t getStackSegmentEntireRows() const { return getStackSegmentVisibleRows() + getStackSegmentInvisibleRows(); }
	/// S: bytes
	inline size_t getStackSegmentEntireSize() const { return rowsToBytes(getStackSegmentEntireRows()); }
	/// Sb: byte
	inline size_t getStackSegmentStartByte() const { return getHeapSegmentSize(); }
	/// sv: rows
	inline size_t getStackSegmentVisibleRows() const { return stackSegmentVisibleRows; }
	/// Sv: bytes
	inline size_t getStackSegmentVisibleSize() const { return rowsToBytes(getStackSegmentVisibleRows()); }
	/// svb: byte
	inline size_t getStackSegmentVisibleStartRow() const { return getHeapSegmentRows(); }
	/// si: rows
	inline size_t getStackSegmentInvisibleRows() const { return 4 * stackSegmentVisibleRows; }
	/// Si: bytes
	inline size_t getStackSegmentInvisibleSize() const { return rowsToBytes(getStackSegmentInvisibleRows()); }

	/// Sc: bytes
	inline size_t getCoreEntireSize() const { return getStackSegmentEntireSize() / cpuCores; }
	/// Scb: byte
	inline size_t getCoreStartByte(int core) const { return getStackSegmentStartByte() + core * getCoreEntireSize(); }
	/// Svc: bytes
	inline size_t getCoreVisibleSize() const { return getStackSegmentVisibleSize() / cpuCores; }
	/// Svcb: byte
	inline size_t getCoreVisibleStartByte(int core) const { return getCoreStartByte(core); }
	/// Sic: bytes
	inline size_t getCoreInvisibleSize() const { return getStackSegmentInvisibleSize() / cpuCores; }
	/// Sicb: byte
	inline size_t getCoreInvisibleStartByte(int core) const { return getCoreStartByte(core) + getCoreVisibleSize(); }

	/// d: rows. Data segment has sv/2 rows, but if sv is 1, data segment should have a least 1 row
	inline size_t getDataSegmentRows() const { return stackSegmentVisibleRows % 2 ? (stackSegmentVisibleRows + 1) / 2 : stackSegmentVisibleRows / 2; }
	/// db: row
	inline size_t getDataSegmentStartRow() const { return getStackSegmentVisibleStartRow() + getStackSegmentVisibleRows(); }
	/// D: bytes
	inline size_t getDataSegmentSize() const { return rowsToBytes(getDataSegmentRows()); }
	/// Db: byte
	inline size_t getDataSegmentStartByte() const { return getStackSegmentStartByte() + getStackSegmentEntireSize(); }

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
	/// Distributes the memory among the memory segments
	void distributeMemory();
};

#endif // UNIT_H
