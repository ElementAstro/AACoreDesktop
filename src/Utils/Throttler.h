// Throttler.h
#ifndef THROTTLER_H
#define THROTTLER_H

#include <QObject>
#include <QTimer>
#include <functional>

class Throttler : public QObject {
    Q_OBJECT

public:
    explicit Throttler(int intervalMs, QObject *parent = nullptr);

    void trigger(const std::function<void()> &func);
    void setInterval(int intervalMs);
    bool isThrottling() const;
    void stop();

private:
    int interval;
    QTimer *timer;
    std::function<void()> queuedFunc;
};

#endif  // THROTTLER_H
