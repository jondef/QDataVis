//
// Created by John on 12/06/2019.
//

#ifndef GUI_APP_NODE_H
#define GUI_APP_NODE_H

#include <QtCore>
#include <QtMath>


class Node {
public:
	Node *pParent = nullptr;
	Node *pLeftChild = nullptr;
	Node *pRightChild = nullptr;

	QString strValue;
	double doubleValue;
	QString mathOperation = nullptr;

	enum RefreshPriority {
		rpImmediateRefresh ///< Replots immediately and repaints the widget immediately by calling QWidget::repaint() after the replot
		,
		rpQueuedRefresh   ///< Replots immediately, but queues the widget repaint, by calling QWidget::update() after the replot. This way multiple redundant widget repaints can be avoided.
		,
		rpRefreshHint     ///< Whether to use immediate or queued refresh depends on whether the plotting hint \ref QCP::phImmediateRefresh is set, see \ref setPlottingHints.
		,
		rpQueuedReplot    ///< Queues the entire replot for the next event loop iteration. This way multiple redundant replots can be avoided. The actual replot is then done with \ref rpRefreshHint priority.
	};

public:
	explicit Node(QString &input, Node *aParent = nullptr);

	bool createChildren(QString string);

private:
	static QList<int> getParenthesesArray(const QString &string);

	static QList<int> findAllOccurences(QString string, const QString &ofWhat);

	bool needsChildren();
};


#endif //GUI_APP_NODE_H