#ifndef LAYOUTHANDLER_H
#define LAYOUTHANDLER_H

#include <QObject>
#include <QLayout>

class LayoutHandler : public QObject
{
    Q_OBJECT
public:
    explicit LayoutHandler(QObject *parent = nullptr);

    static void createLayout(QLayout *layout, QString function);

signals:

};

#endif // LAYOUTHANDLER_H
