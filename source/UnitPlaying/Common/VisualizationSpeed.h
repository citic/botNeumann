#ifndef VISUALIZATIONSPEED_H
#define VISUALIZATIONSPEED_H

#include <QObject>

/// The maximum value that user can set in the speed control slider
const int maxDelayFactor = 200; // %

/** Singleton in charge of controlling the speed of animation **/
class VisualizationSpeed : public QObject
{
	Q_OBJECT
	Q_DISABLE_COPY(VisualizationSpeed)

  private:
	/// Current delay factor measured as a value between 0.0 and 2.0 where:
	/// * 0.0 = fastest
	/// * 1.0 = normal
	/// * 2.0 = 2x slower than normal
	double delayFactor = 1.0;
	/// True if visualization must run as fast as possible until reach the first user-defined
	/// breakpoint. If user defines at least a breakpoint, the viualization must "seek". Otherwise
	/// it will run at the speed defined by user using the speed slider.
	bool seeking = false;

  public:
	/// Get access to the unique instance of this object
	static VisualizationSpeed& getInstance();
	/// Get the current delay factor, where 0.0 is fast-as-possible, 1.0 is normal and a number
	/// greater than 1.0 is slow
	inline double getDelayFactor() const { return seeking ? 0.0 : delayFactor; }
	/// Apply the user speed factor to the given duration
	/// @param The normal duration in milliseconds that an animation would take in normal speed
	inline int adjust(int normalDuration) const { return normalDuration * getDelayFactor(); }

  signals:
	/// Emitted when the visualization speed has been changed by user
	void speedChanged(double factor);

  public slots:
	/// Call this method after user has changed the speed of visualization
	/// @param value An integer from 0% to 200% defined by user interface
	void updateSpeed(int userValue);
	/// @see seeking
	/// If seeking is changed, it will emit a @a speedChanged() signal
	void setSeeking(bool seeking);

  private:
	/// Private constructor
	explicit VisualizationSpeed(QObject *parent = nullptr);
};

#endif // VISUALIZATIONSPEED_H
