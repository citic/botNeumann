#ifndef UNITPLAYINGSCENE_H
#define UNITPLAYINGSCENE_H

#include "GameScene.h"
#include "Unit.h"
#include "VisualizationContext.h"

class CodeSegment;
class CompiledProgram;
class CpuCores;
class DataSegment;
class GdbItemTree;
class GuiBreakpoint;
class HeapSegment;
class MessagesArea;
class PlayerSolution;
class TestCaseManager;
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
	/// Manages the list of source files that compound the player's solution to an unit
	PlayerSolution* playerSolution = nullptr;
	/// The state of the unit playing scene, e.g: editing, building, animating, paused
	UnitPlayingState state = UnitPlayingState::editing;
	/// Manages the list of test cases to know if player solution actually solves the Unit
	TestCaseManager* testCaseManager = nullptr;
	/// The code editor, allows player to input program code in editing time. At runtime it
	/// represents the text segment (aka code segment)
	CodeSegment* codeSegment = nullptr;
	/// Segment for storing variables that persist after a function is call. These variables are
	/// allocated dynamically with new operator or malloc() function family. It is represented in
	/// botNeumann by shelves at the top of the robot's office
	HeapSegment* heapSegment = nullptr;
	/// Manages the execution threads (robots) working in CPU cores (workstations)
	/// The paperwork generated by each robot is its stack segment
	CpuCores* cpuCores = nullptr;
	/// Segment to store global and static variables, available for all execution threads along the
	/// entire process execution. For simplicity it comprises two data segments: the initialized
	/// data segment, and the uninitialized data segment (BSS, Block Started by Symbol). They are
	/// merged because space in the screen is too limited and both segements are almost identical.
	/// In botNeumann, this segment is the unique initialized with zeros
	DataSegment* dataSegment = nullptr;
	/// The dock widget to place all the information and messages
	MessagesArea* messagesArea = nullptr;
	/// Controller in charge of visualizing the executable
	Visualizator* visualizator = nullptr;

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
	/// Return the state of the unit playing scene
	inline UnitPlayingState getState() const { return state; }
	/// Return true if visualization is stopped and no animations must be done at all
	inline bool isStopped() const { return state == UnitPlayingState::editing; }
	/// Return true if visualization is animating
	inline bool isAnimating() const { return state == UnitPlayingState::animating || state == UnitPlayingState::starting || state == UnitPlayingState::paused; }
	/// Return the filename of the .botnu Unit file
	inline const QString& getUnitFilename() const { return filename; }
	/// Give access to the code segment object
	inline CodeSegment* getCodeSegment() const { return codeSegment; }
	/// Give access to the heap segment object
	inline HeapSegment* getHeapSegment() const { return heapSegment; }
	/// Give access to the CPU cores object
	inline CpuCores* getCpuCores() const { return cpuCores; }
	/// Give access to the data segment object
	inline DataSegment* getDataSegment() const { return dataSegment; }
	/// Give access to the messages area object
	inline MessagesArea* getMessagesArea() const { return messagesArea; }
	/// Give access to the test case manager
	inline TestCaseManager* getTestCaseManager() const { return testCaseManager; }
	/// Get access to the breakpoints list set by the user
	QList<GuiBreakpoint*> retrieveBreakpoints();
	/// Change the state of the visualization and emit the @a stateChanged signal
	void changeState(UnitPlayingState newState);
	/// On each step of the visualization, the cursors of the standard input/output/error files
	/// are requested to GDB. This signal is called when gdb replies to each of these consults.
	/// @param tree The response yield by GDB containing the cursor of the file
	/// @param context Use this enumeration to determine the type of file: input/output/error
	/// @param maxDuration Required to wait all animations finished before stepping forward
	void updateStandardInputOutput(const GdbItemTree& tree, VisualizationContext context, int& maxDuration);

  signals:
	/// Emitted when a game mode button is pressed
	void showUnitSelectionScene(const QString& context, bool forward);
	/// Emitted when the state of the scene changes
	void stateChanged(UnitPlayingState currentState);

  protected slots:
	/// Called when user press the Information button
	virtual void infoButtonPressed() override;
	/// Called when the Back button is pressed
	virtual void backButtonPressed() override;
	/// Called when user press the code editor toggle button
	virtual void codeSegmentTogglePressed() override;
	/// Called when user presses the Run/Pause button
	void userRunOrPaused();
	/// Called when the compilation and linking process has finished
	/// If there were no errorors, the visualization starts
	void playerSolutionBuilt(CompiledProgram* compiledProgram);
	/// Called when the stop button is pressed in order to stop the visualization
	/// @param removeTestCases Send true if user stopped the visualization, false if only wanted
	/// to change the active test case
	void userStopped(bool removeTestCases = true);
	/// Called to show the unit selection scene, some milliseconds later when the dock widgets
	/// (code segment and messages area) have been hidden using the docksHidingTimer
	void callUnitSelectionScene();
	/// Called when user changed the test case being visualized. Current visualization must be
	/// stopped, and a new visualization must be started
	void activeTestCaseChanged(int newTestCaseNumber);
	/// Called when player solution's output for all test cases has been generated in order to
	/// load output in standard output buffer
	void loadStandardOutput();

  protected:
	/// Starts the process of build the player solution code, test cases and symbol extraction
	/// @return true if the process started with no errors, false otherwise
	bool buildAll();
	/// Distribute the screen space between each segment according to the number of rows they
	/// require. The result is the proportion each segment requires
	void addVerticalSegments();
	/// Shows the Code Editor, which switches to Code Segment in run time
	void createCodeSegment();
	/// Create a dock widget that show information about the unit, messages from compilers...
	void createMessagesArea();
	/// Start a new visualization with the given test case number
	/// @param testCaseNumber Send 0 when called for first time. Send a positive number when
	/// user changes the active test case being visualized
	void startVisualization(int testCaseNumber = 0);
};

#endif // UNITPLAYINGSCENE_H
