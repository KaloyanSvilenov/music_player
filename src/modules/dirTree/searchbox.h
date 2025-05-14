#ifndef SEARCHBOX_H
#define SEARCHBOX_H

#include <QLineEdit>
#include <player/hoverbutton.h>

class SearchBox : public QLineEdit
{
public:
    SearchBox(QWidget *lineEdit);
    void changeTextFromMenu(QAction *action);
private:
    QString menuText;
};

#endif // SEARCHBOX_H
