#include "searchbox.h"

SearchBox::SearchBox(QWidget *lineEdit)
{
    this->menuText = "";
}

void SearchBox::changeTextFromMenu(QAction *action){
    this->setText(action->text());
    this->menuText = action->text();
}
