#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include <QTextEdit>

class QTimer;
class SyntaxHighlighter;
class Unit;

/**
	Edits one source file at time
*/
class CodeEditor : public QTextEdit
{
	Q_OBJECT

  protected:
	/// Store formatting rules for C++
	SyntaxHighlighter* highlighter;
	/// Current unit being played
	Unit* unit;
	/// The full path to the source file being edited
	QString filepath;
	/// Fires a short time after the last made change in the document, in order to do some tasks
	/// such as auto save() and auto compile()
	QTimer* autoSaveTimer;

  public:
	/// Constructor
	explicit CodeEditor(QWidget* parent = nullptr);
	/// Destructor
	~CodeEditor();
	/// Restores the last code made by player for the given unit, or the default unit's code if
	/// player nas not played this unit
	/// @param filepath Full path to the active source file to be edited by the player. In order
	/// to solve the current unit's problem, the player may generate several files. Just one file
	/// can be edited at time. The fullpath of the active source file is given in this parameter.
	/// The file may exist or not. If the file exists, it will be loaded. Otherwise an initial
	/// code provided by the unit will be loaded and this filename will be stored internally. When
	/// player changes the code, the autosave function will active and it will use this filename
	/// to store the changes
	/// @return true on success, false otherwise. ToDo: decide how to report errors?
	bool loadFile(Unit* unit, const QString& filepath);
	/// Loads a file given for its full path
	bool loadFile(const QString& filepath);
	/// Called when the player stopped playing the current unit, clear the code editor for being
	/// reused again for a new unit
	void reset();

  public slots:
	/// Saves if there are changes to the @a filepath document in secondary memory
	/// @return true on success or when there is not need to save, false on error
	bool saveChanges();

  protected:
	/// Load an initial code provided by the unit. This method is called when the player has never
	/// played the current unit before
	bool loadUnitInitialCode();
	/// Load the file contents of the file which full path is stored in @a filepath member data
	bool loadFileContents();

  protected slots:
	/// Called each time the document is changed
	void documentChanged();
	/// Saves always the content of editor to the @a filepath document in secondary memory
	/// @return true on success, false on error
	bool save();
};

#endif // CODEEDITOR_H
