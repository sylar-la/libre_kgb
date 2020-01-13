#ifndef MAINVIEWMODEL_H
#define MAINVIEWMODEL_H

#include <QObject>

class MainViewModel : public QObject
{
    Q_OBJECT
public:
    MainViewModel();

    Q_INVOKABLE
    void        initialize();

    Q_INVOKABLE
    void        shutdown();
};

extern MainViewModel* sMainViewModel;

#endif
