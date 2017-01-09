#ifndef VISUALIZATIONSPEED_H
#define VISUALIZATIONSPEED_H

#include <QObject>

/// The maximum value that user can set in the speed control slider
const int maxSpeedFactor = 200; // %

/** Singleton in charge of controlling the speed of animation **/
class VisualizationSpeed : public QObject
{
	Q_OBJECT
	Q_DISABLE_COPY(VisualizationSpeed)

  private:
	/// Current speed factor measured as a value between 0.0 and 2.0 where:
	/// * 0.0 = fastest
	/// * 1.0 = normal
	/// * 2.0 = 2x slower than normal
	double speedFactor = 1.0;

  public:
	/// Get access to the unique instance of this object
	static VisualizationSpeed& getInstance();
	/// Apply the user speed factor to the given duration
	/// @param The normal duration in milliseconds that an animation would take in normal speed
	inline int adjust(int normalDuration) const { return normalDuration * speedFactor; }

  signals:
	/// Emitted when the visualization speed has been changed by user
	void speedChanged(double factor);

  public slots:
	/// Call this method after user has changed the speed of visualization
	/// @param value An integer from 0% to 200% defined by user interface
	void updateSpeed(int userValue);

  private:
	/// Private constructor
	explicit VisualizationSpeed(QObject *parent = nullptr);
};

#endif // VISUALIZATIONSPEED_H
