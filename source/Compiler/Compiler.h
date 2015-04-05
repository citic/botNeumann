#ifndef COMPILER_H
#define COMPILER_H

#include <QObject>
#include <QStringList>

struct ClangData;
class Diagnostic;

/**
	@brief Represents the LLVM compiler. This object receives source files, compiles them and
	generates their intermediate representation (LLVM IR). Also retrieves diagnostics (warnings
	and errors). The compilation is made in a separated thread
 */
class Compiler : public QObject
{
	Q_OBJECT

  private:
	/// Stores the internal structures used by LLVM and Clang to avoid making
	/// this header dependent of the CLang headers
	ClangData* clangData;
	/// List of warnings and errors generated when compiling a piece of code
	QList<Diagnostic*> diagnostics;

  public:
	/// Constructor
	explicit Compiler(QObject *parent = nullptr);
	/// Destructor
	~Compiler();
	/// Get access to the list of diagnostics (warnings and errors)
	inline const QList<Diagnostic*>& getDiagnostics() const { return diagnostics; }

  signals:
	/// Emitted when a compilation process has finished
	void compilationFinished();

  public slots:
	/// Compiles the given source file. It starts a separate thread. When it finishes, the
	/// @a compilationFinished() signal is emitted. The result and products of the compilation
	/// (diagnostics, intermediate representation) can be get using accesor methods
	void compile(const QString& filename);
	/// Compiles the given set of source files
	/// @see compile(const QString&)
	void compile(const QStringList& filenames);
	/// Erases all information about a compiling. This method is automatically called by compile()
	void clear();
};

#endif // COMPILER_H
