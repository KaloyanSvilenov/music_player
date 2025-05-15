// searchbox.cpp
#include "searchbox.h"
#include <QMenu>
#include <QAction>

SearchBox::SearchBox(QWidget *parent)
    : QLineEdit(parent), m_currentFilter("File Names")
{
    setPlaceholderText("Search...");
    addAction(QIcon(":/icons/search.png"), QLineEdit::LeadingPosition);

    connect(this, &QLineEdit::returnPressed, this, &SearchBox::onSearch);
}

void SearchBox::changeTextFromMenu(QAction *action)
{
    m_currentFilter = action->text();
    setPlaceholderText("Search by " + m_currentFilter + "...");
    setFocus();
}

void SearchBox::onSearch()
{
    if (!text().isEmpty()) {
        emit searchTriggered(m_currentFilter, text());
    }
}
