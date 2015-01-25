#ifndef DIRECTOR_H
#define DIRECTOR_H

#include <QObject>

class Stage;

class Director : public QObject
{
	Q_OBJECT

  protected:
	/// The current stage where this director is running the game
	Stage* stage;

  public:
	/// Constructor
	explicit Director(Stage* stage = nullptr, QObject* parent = nullptr);
	/// Destructor
	virtual ~Director();
	/// Get access to the stage where this director is working
	inline Stage* getStage() const { return stage; }
	/// Change the stage where this director is working
	inline void setStage(Stage* stage) { this->stage = stage; }
	/// Begin the game. Run the first scene
	virtual void begin() = 0;
};

#endif // DIRECTOR_H
