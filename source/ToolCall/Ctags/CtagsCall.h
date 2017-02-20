#ifndef CTAGSCALL_H
#define CTAGSCALL_H

#include "ToolCall.h"

#include <QProcess>
#include <QStringList>

/// A symbol is a piece of source code that has a name and a position
struct Symbol
{
  public:
	/// The type of this symbol. Currently only global variables and function definitions are
	/// extracted from source files
	enum Type { unknown, globalVariable, functionDefinition };

  public:
	/// Name of the symbol: an identifier. E.g: "~AbstractArgument"
	QString name;
	/// Filename where the symbol is found. E.g: "alli.cpp"
	QString filename;
	/// The line number where the symbol is found. E.g: 44
	qint64 line = -1;
	/// The type of symbol: global variable or function definition
	Type type = unknown;
	/// The full declaration of the symbol. For variables include their data type and init value.
	/// For function definitions, it include return type, parameters and the body for inline
	/// functions.
	QString declaration;
	/// Some details to help editors
	QString details;

  public:
	/// Loads this symbol from the given line of ctags file
	bool loadFrom(const QString& line);
	/// Creates a Symbol from the given line of the bn_tags.txt file. On success a pointer to a
	/// Symbol object allocated in dynamic memory is returned. nullptr is returned if the line is
	/// not valid
	static Symbol* createFrom(const QString& line);
	/// Matches the given Ctags text against the types in our enumeration
	static Type mapTextToType(const QString& text);
};

/** Calls ctags command line tool to extract symbols (e.g: global variables and function
	definitions). These symbols are used during the visualization in order to watch variable-changes
	and function calls.
*/
class CtagsCall : public ToolCall
{
	Q_OBJECT
	Q_DISABLE_COPY(CtagsCall)

  protected:
	/// The arguments to call the process
	QStringList arguments;
	/// The full path of the tags file: "/path/to/player/solution/bn_tags.txt"
	QString ctagsFilepath;
	/// The list of all symbols extracted from source files
	QList<Symbol*> allSymbols;
	/// The subset of @a allSymbols that are global variables found in source files
	QList<Symbol*> globalVariables;
	/// The subset of @a allSymbols that are function definitions
	QList<Symbol*> functionDefinitions;

  public:
	/// Constructor
	explicit CtagsCall(QObject* parent = nullptr);
	/// Destructor
	~CtagsCall();
	/// Start the symbol extraction process. This method does some preparation and calls the
	/// ctags command. When ctags finishes, it analyzes the output file and extracts the symbols.
	/// When the entire process is done, i.e: the symbols have been extracted, the
	/// @a symbolsExtracted() signal is emitted. If an error happens in the process, the
	/// @a extractionFailed is emitted.
	/// @param sourceFiles The list of files that will be analyzed to extract symbols from
	/// @param outputDirectory The fullpath to the directory where the output file will be generated
	/// @return true if the extraction process started, false otherwise
	bool extractSymbols(const QFileInfoList& arguments, const QString& outputDirectory);
	/// Get access to the all symbols extracted from the bn_tags.txt file
	inline const QList<Symbol*>& getAllSymbols() const { return allSymbols; }
	/// Get access to the all symbols extracted from the bn_tags.txt file
	inline const QList<Symbol*>& getGlobalVariables() const { return globalVariables; }
	/// Get access to the all symbols extracted from the bn_tags.txt file
	inline const QList<Symbol*>& getFunctionDefinitions() const { return functionDefinitions; }

  signals:
	/// Emitted when the entire symbol extraction process is finished. The symbols can be consulted
	/// with @a getGlobalVariables() and @a getFunctionDefinitions() methods
	void extractionFinished();
	/// Emitted in case the symbol extraction process fails
	void extractionFailed(QProcess::ProcessError error);

  protected slots:
	/// Called when the ctags process has finished generating the bn_tags.txt file
	bool processFinished();
	/// Called when the ctags call failed, for example, the command is not installed
	void processFailed();

  protected:
	/// Builds a symbol from the current line, and appends it to the respective lists
	/// The method ignores empty lines or comment lines
	/// @return true on success, false otherwise
	bool extractSymbolFrom(const QString& line);
};

#endif // CTAGSCALL_H
