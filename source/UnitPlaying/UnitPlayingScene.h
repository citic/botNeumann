#ifndef UNITPLAYINGSCENE_H
#define UNITPLAYINGSCENE_H

#include "GameScene.h"
#include "Unit.h"

class CodeSegment;
class Compiler;
class CpuCores;
class DataSegment;
class HeapSegment;
class MessagesArea;
class Visualizator;

/**
	@brief Scene where players try to solve the unit's problem

	The UnitPlayingScene displays the unit description, the code segment with the initial unit's
	proposed code (or existing player's solution, if any), and an empty visualization area.
	Players read the problem statement. Then they write some code to solve the problem. When they
	press the Run button, their solution is compiled, linked and executed in debugging mode.

	The debugging process is made visually instead of providing only text information to the player.
	The visualization is an extended metaphor of robots (execution theads) working in their
	workstations (stack segments). Sometimes they require more work area (heap segment).
	All robots follow the program (code segment) line by line. The hyphotesis is: this
	visualization helps students to get more understanding how the computer works compared against
	traditional debuggers and other abstract visualizations.
 */
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
	CodeSegment* codeSegment;
	/// Segment for storing variables that persist after a function is call. These variables are
	/// allocated dynamically with new operator or malloc() function family. It is represented in
	/// botNeumann by shelves at the top of the robot's office
	HeapSegment* heapSegment;
	/// Manages the execution threads (robots) working in CPU cores (workstations)
	/// The paperwork generated by each robot is its stack segment
	CpuCores* cpuCores;
	/// Segment to store global and static variables, available for all execution threads along the
	/// entire process execution. For simplicity it comprises two data segments: the initialized
	/// data segment, and the uninitialized data segment (BSS, Block Started by Symbol). They are
	/// merged because space in the screen is too limited and both segements are almost identical.
	/// In botNeumann, this segment is the unique initialized with zeros
	DataSegment* dataSegment;
	/// The dock widget to place all the information and messages
	MessagesArea* messagesArea;
	/// Controller in charge of visualizing the executable
	Visualizator* visualizator;

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
	/// Give access to the messages area object
	inline MessagesArea* getMessagesArea() const { return messagesArea; }

  public:
	/// Get access to the breakpoints list set by the user
	const QSet<int>& getBreakpoints() const;

  signals:
	/// Emitted when a game mode button is pressed
	void showUnitSelectionScene(const QString& context, bool forward);

  protected slots:
	/// Called when user press the Information button
	virtual void infoButtonPressed() override;
	/// Called when the Back button is pressed
	virtual void backButtonPressed() override;
	/// Called when user press the code editor toggle button
	virtual void codeSegmentTogglePressed() override;
	/// Called when the compilation and linking process has finished
	/// If there were no errorors, the visualization starts
	void buildFinished(Compiler* compiler);

  protected:
	/// Distribute the screen space between each segment according to the number of rows they
	/// require. The result is the proportion each segment requires
	void distributeScreenSpace(double& heapSegmentProportion, double& cpuCoresProportion, double& dataSegmentProportion);
	/// Creates the object in charge of managing the heap segment
	void createHeapSegment(double heapSegmentProportion);
	/// Creates the object in charge of managing cpu cores, threads, and stack segments
	void createCpuCores(double cpuCoresProportion);
	/// Creates the object in charge of managing the two Data Segments
	void createDataSegment(double dataSegmentProportion);
	/// Shows the Code Editor, which switches to Code Segment in run time
	void createCodeSegment();
	/// Create a dock widget that show information about the unit, messages from compilers...
	void createMessagesArea();
};

#endif // UNITPLAYINGSCENE_H
