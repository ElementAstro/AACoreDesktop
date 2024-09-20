#include "Throttler.h"

Throttler::Throttler(int intervalMs, QObject *parent)
    : QObject(parent), interval(intervalMs), timer(new QTimer(this)) {
    timer->setInterval(intervalMs);
    timer->setSingleShot(true);
    connect(timer, &QTimer::timeout, this, [this]() {
        if (queuedFunc) {
            queuedFunc();
            queuedFunc = nullptr;
        }
    });
}

void Throttler::trigger(const std::function<void()> &func) {
    if (!timer->isActive()) {
        func();
        timer->start();
    } else {
        queuedFunc = func;
    }
}

void Throttler::setInterval(int intervalMs) {
    interval = intervalMs;
    timer->setInterval(interval);
}

bool Throttler::isThrottling() const { return timer->isActive(); }

void Throttler::stop() {
    timer->stop();
    queuedFunc = nullptr;
}
