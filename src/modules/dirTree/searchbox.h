#ifndef SEARCHBOX_H
#define SEARCHBOX_H

#include <QLineEdit>
#include <QAction>

class SearchBox : public QLineEdit
{
    Q_OBJECT
public:
    explicit SearchBox(QWidget *parent = nullptr);

    QString currentFilter() const { return m_currentFilter; }

signals:
    void searchTriggered(const QString &filter, const QString &text);

public slots:
    void changeTextFromMenu(QAction *action);
    void onSearch();

private:
    QString m_currentFilter;
};

#endif // SEARCHBOX_H
