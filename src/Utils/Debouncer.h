// Debouncer.h
#ifndef DEBOUNCER_H
#define DEBOUNCER_H

#include <QObject>
#include <QTimer>
#include <functional>

class Debouncer : public QObject {
    Q_OBJECT

public:
    explicit Debouncer(int delayMs, QObject *parent = nullptr);

    void trigger(const std::function<void()> &func);
    void setDelay(int delayMs);
    bool isDebouncing() const;
    void stop();
    void clearCallback();

private:
    int delay;
    QTimer *timer;
    std::function<void()> callback;
};

#endif  // DEBOUNCER_H
