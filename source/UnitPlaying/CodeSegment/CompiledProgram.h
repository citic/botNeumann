#ifndef COMPILEDPROGRAM_H
#define COMPILEDPROGRAM_H

#include <QObject>
#include <QFileInfoList>

class Compiler;
struct ProgramText;

/// Converts source code text in memory or files, compile it, and manages the executable
class CompiledProgram : public QObject
{
	Q_OBJECT
	Q_DISABLE_COPY(CompiledProgram)

  protected:
	/// The source code to be compiled, if coming from an Unit
	const ProgramText* programText = nullptr;
	/// The source code stored in primary memory
//	QString sourceCode;
	/// The list of source files generated on secondary memory. These files may exist or may be
	/// generated from a @a programText or @a sourceCode
	QFileInfoList sourceFiles;
	/// The folder where executable and object code will be generated
	QString destinationDirectory;
	/// The generated executable (usually on player-solution directory)
	QString executablePath;
	/// If true, debugging information will be included in object file
	bool optimizedForDebug = false;
	/// Used to compile the surce code
	Compiler* compiler =  nullptr;

  public:
	/// Constructor
	explicit CompiledProgram(QObject* parent = nullptr);
	/// Program text getter
	inline const ProgramText* getProgramText() const { return programText; }
	/// Executable path getter
	inline const QString& getExecutablePath() const { return executablePath; }
	/// Make the compiler to generate debug information in object files and executable
	inline void optimizeForDebug(bool debug = true) { this->optimizedForDebug = debug; }
	/// Compiler getter
	inline Compiler* getCompiler() const { return compiler; }
	/// Get error count only after a build process is finished, otherwise crash
	int getErrorCount() const;
	/// Compiles code loaded from an Unit. This method just start the compilation and linking
	/// process. The @a compilationFinished signal is emitted when the compilation process finishes
	/// Errors may be consulted using @a getCompilationErrors() method
	/// @return true on success, false otherwise
	bool build(const ProgramText* programText, const QString& destinationDirectory);
	/// Compiles the given source code files to the desired executable file
	bool build(const QFileInfoList& sourceFiles, const QString& executablePath);
	/// Reports errors on log
	/// @return false always
	bool logErrors() const;

  signals:
	/// Emitted when the compilation and linking process finishes
	void buildFinished();

  protected:
	/// Creates a new Compiler object for a new compilation process, and deletes a previous
	/// compiler object if an older compilation process was done
	bool createCompiler();
};

#endif // COMPILEDPROGRAM_H
