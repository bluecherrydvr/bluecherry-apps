#ifndef EVENTVIEWWINDOW_H
#define EVENTVIEWWINDOW_H

#include <QWidget>

struct EventData;
class QSplitter;
class QLabel;
class QListView;
class QTextEdit;
class QComboBox;
class QPushButton;
class EventTagsView;
class EventCommentsWidget;
class EventVideoPlayer;
class ExpandingTextEdit;

class EventViewWindow : public QWidget
{
    Q_OBJECT

public:
    explicit EventViewWindow(QWidget *parent = 0);

    void setEvent(EventData *event);

protected:
    virtual void closeEvent(QCloseEvent *event);

private slots:
    void commentInputChanged();
    void postComment();

private:
    EventData *m_event;

    QSplitter *m_splitter;
    QLabel *m_infoLabel;
    EventTagsView *m_tagsView;
    QComboBox *m_tagsInput;
    EventCommentsWidget *m_commentsArea;
    ExpandingTextEdit *m_commentInput;
    QPushButton *m_commentBtn;

    EventVideoPlayer *m_videoPlayer;

    QWidget *createInfoArea();
    QWidget *createPlaybackArea();
};

#endif // EVENTVIEWWINDOW_H
