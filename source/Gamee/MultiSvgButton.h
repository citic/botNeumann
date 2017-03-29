#ifndef MULTISVGBUTTON_H
#define MULTISVGBUTTON_H

#include "LinearLayout.h"

#include <QStringList>

class LabelButton;
class Actor;
class QGraphicsItem;

/**
	A MultiSvgButton can contain several svg images as background, instead of just one. Each
	single image can have its own proportion

	@todo Currently this class does not support clicking events
*/
class MultiSvgButton : public LinearLayout
{
	Q_DISABLE_COPY(MultiSvgButton)

  protected:
	/// The background can be made of one or several graphics
	QList<Actor*> background;
	/// The text of the button
	LabelButton* label = nullptr;

  public:
	/// Creates a SvgButton that has several images as background
	explicit MultiSvgButton(const QStringList& prefixedSvgElementIds, const QList<qreal>& proportions, QGraphicsItem* parentItem, const QString& text, qreal zValue);
	/// Destructor
	virtual ~MultiSvgButton();
	/// Set label font
	void setFont(const QFont& font);
	/// Set foreground color
	void setBrush(const QBrush& brush);
	/// Skew the svg button
	void setShear(qreal shearX, qreal shearY);

  public slots:
	/// When disabled, the button becomes semi-transparent
	/// @remarks This method is not virtual at QGraphicsItem
	void setEnabled(bool enabled) /*override*/;

  protected:
	/// Builds the label in top of the button
	void buildLabel(const QString& text, QGraphicsItem* parentItem = nullptr, qreal zValue = 0.0);
};

#endif // MULTISVGBUTTON_H
