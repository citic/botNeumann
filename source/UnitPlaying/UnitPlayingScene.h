#ifndef UNITPLAYINGSCENE_H
#define UNITPLAYINGSCENE_H

#include "GameScene.h"
#include "Unit.h"

class CodeEditorDockWidget;
class DataSegment;
class HeapSegment;
class CpuCores;

class UnitPlayingScene : public GameScene
{
	Q_OBJECT
	Q_DISABLE_COPY(UnitPlayingScene)

  protected:
	/// One of the following strings: "training", "challenges", "collaboration"
	QString context;
	/// The number of the level and unit separated by a dash, e.g: "2-14"
	QString levelUnit;
	/// he full resource path of the .botnu unit with details of the unit
	QString filename;
	/// Loads the unit from the .botnu xml file
	Unit unit;
	/// The code editor, allows player to input program code in editing time. At runtime it
	/// represents the text segment (aka code segment)
	CodeEditorDockWidget* codeSegment;
	/// Segment for storing variables that persist after a function is call. These variables are
	/// allocated dynamically with new operator or malloc() function family. It is represented in
	/// botNeumann by shelves at the top of the robot's office
	HeapSegment* heapSegment;
	/// Manages the execution threads (robots) working in CPU cores (workstations)
	/// The paperwork generated by each robot is its stack segment
		/// Segment for storing function calls (or function instances)
		/// Each function call has its own space, called stack frame. Stack frames are represented
	CpuCores* cpuCores;
	/// Segment to store global and static variables, available for all execution threads along the
	/// entire process execution. For simplicity it comprises two data segments: the initialized
	/// data segment, and the uninitialized data segment (BSS, Block Started by Symbol). They are
	/// merged because space in the screen is too limited and both segements are almost identical.
	/// In botNeumann, this segment is the unique initialized with zeros
	DataSegment* dataSegment;

  public:
	/// Constructor
	/// @param context One of the following strings: "training", "challenges", "collaboration"
	/// @param levelUnit The number of the level and unit separated by a dash, e.g: "2-14"
	/// @param filename The full resource path of the .botnu unit with details of the unit
	explicit UnitPlayingScene(const QString& context, const QString& levelUnit, const QString& filename, Stage* stage, QGraphicsItem* parent = nullptr);
	/// Destructor
	virtual ~UnitPlayingScene();
	/// Called when a transition starts, indicating this scene is leaving the stage
	virtual void startLeavingStage() override;
	/// Called when a transition has finished, indicating this scene entered into the stage
	virtual void finishedEnteringStage() override;

  signals:
	/// Emitted when a game mode button is pressed
	void showUnitSelectionScene(const QString& context, bool forward);

  protected slots:
	/// Called when user press the Information button
	virtual void infoButtonPressed() override;
	/// Called when the Back button is pressed
	virtual void backButtonPressed() override;

  protected:
	/// Shows the Code Editor, which switches to Code Segment in run time
	void createCodeSegment();
	/// Creates the object in charge of managing the heap segment
	void createHeapSegment();
	/// Creates the object in charge of managing cpu cores, threads, and stack segments
	void createCpuCores();
	/// Creates the object in charge of managing the two Data Segments
	void createDataSegment();
};

#endif // UNITPLAYINGSCENE_H
