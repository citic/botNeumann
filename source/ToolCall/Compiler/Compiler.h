#ifndef COMPILER_H
#define COMPILER_H

#include <QObject>
#include <QFileInfoList>
#include <QStringList>

class CompilerCall;
class Diagnostic;
class LinkerCall;

/**
	@brief Frontent to the command line compiler

	This object is in charge of compiling the solution generated by the player, that is,
	the source code files, and generate the executable. This object collects diagnostics (warnings
	and errors). The compilation is made by a system call in desktop operating systems. In
	mobile devices, the compilation is sent to a TCP server.

	All files are always compiled with all warnings enabled (-Wall), C++11 enabled (-std=c++11),
	debug information enabled (-g) and no optimizations (-O0). Each source file is compiled by
	separate, generating a .o file. If there is an updated .o, the file is ommited in order to make
	the process faster.
 */
class Compiler : public QObject
{
	Q_OBJECT
	Q_DISABLE_COPY(Compiler)

  protected:
	/// For each .c/.cpp source file a compiler call is scheduled in this list
	QList<CompilerCall*> compilerCalls;
	/// True if the executable is scheduled to be compiled
	bool shouldLinkExecutable;
	/// Indicates the number of error collected during the compilation process. If value is 0
	/// the solution can be run, even it would have warnings. This value is valid only
	/// after a compilation process has finished.
	int errorCount;
	/// The number of warnings collected during the compilation process. -1 if no compilation
	/// has been done This value is valid only after a compilation process has finished.
	int warningCount;
	/// A concatenation of all generated diagnostics in all compiler calls
	QList<Diagnostic*> allDiagnostics;
	/// Path of the target executable file. It will be linked only if it is out-dated
	QFileInfo executablePath;
	/// The list of objectfiles to link
	QStringList objectFiles;
	/// Index of the current compiler call being executed
	int currentCompilerCall;
	/// Pointer to the linker call, if it is being called
	LinkerCall* linkerCall;

  public:
	/// Constructor
	explicit Compiler(QObject *parent = nullptr);
	/// Destructor
	~Compiler();
	/// Indicates the number of error collected during the compilation process. If value is 0
	/// the solution can be run, even it would have warnings. This value is valid only
	/// after a compilation process has finished. If called before, it will return -1
	inline int getErrorCount() const { return errorCount; }
	/// The number of warnings collected during the compilation process. -1 if no compilation
	/// has been done This value is valid only after a compilation process has finished.
	inline int getWarningCount() const { return warningCount; }
	/// Get access to all the diagnostics generated during the compiling and linking process of
	/// all the sources files
	inline const QList<Diagnostic*>& getAllDiagnostics() const { return allDiagnostics; }
	/// Get access to the executable full path that should be generated after compiler the player
	/// solution
	inline const QFileInfo& getExecutablePath() const { return executablePath; }

  signals:
	/// Emitted when a compilation process has finished
	void finished();

  public slots:
	/// Compiles the given source files. It starts a separate thread. When it finishes, the
	/// @a finished() signal is emitted. The result and products of the compilation
	/// (diagnostics, intermediate representation) can be get using accesor methods
	void compile(const QFileInfoList& filepaths, const QFileInfo& executablePath);
	/// Erases all information about a compiling. This method is automatically called by compile()
	void clear();

  protected slots:
	/// Called when the current compilation unit has finished its compiling process
	void compilerCallFinished();
	/// Called when the linker call has finished
	void linkerCallFinished();

  protected:
	/// Compiles the next scheduled .cpp file and generates its respective .o in the same folder
	void compileNextSourceFile();
	/// Call the compilers to generate the executable or update it
	/// @param The complete list of object files that will compound the executable
	void linkExecutable();
	/// Fills the compilationUnits with the list of source files that must be compiled
	void scheduleCompilerCalls(const QFileInfoList& filepaths);
};

#endif // COMPILER_H
