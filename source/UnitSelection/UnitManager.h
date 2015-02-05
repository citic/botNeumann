#ifndef UnitManager_h
#define UnitManager_h

#include <QList>
#include <QStringList>

/** A level has a name and a list of units. Units are grouped in levels
Levels and their units are numbered automatically, for example:

@code
intro:        <-- level 1
sum_xy        <-- unit 1-1
abs_diff      <-- unit 1-2
average_n     <-- unit 1-3

dyn_mem:      <-- level 2
median        <-- unit 2-1
@endcode
*/
struct UnitLevel
{
	QString name;
	QStringList units;

	explicit UnitLevel(const QString& name = "") : name(name) { }
};

/** Loads the list of units for a given context */
class UnitManager
{
	Q_DISABLE_COPY(UnitManager)

  protected:
	/// The context for loading units: e.g: "Training", "Missions"
	QString context;
	/// Units are grouped in levels. Each level has exercises of the same concept
	QList<UnitLevel> levels;

  public:
	/// Constructor
	UnitManager();
	/// Loads units for the given context
	/// @return true on success, false on fail
	bool load(const QString& context);
	/// Get access to the levels, be sure to call load() before this method
	inline const QList<UnitLevel>& getLevels() const { return levels; }
	/// Returns the count of units of the level with more units, -1 if levels have not been loaded
	int calculateMaxUnitsPerLevel() const;

  protected:
	/// Loads the given line from the list file
	bool loadLine(const QString& line);
};

#endif // UnitManager_h
