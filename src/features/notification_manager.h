#ifndef SLIDER_DRAW_NOTIFICATION_MANAGER_H
#define SLIDER_DRAW_NOTIFICATION_MANAGER_H

#include "util/component.h"
#include <string>
#include <vector>

class Notification_manager : public Component {
    struct Notification {
        std::string title, content;
        bool shown = false;
    };

public:
    void add_notification(std::string title, std::string notification);
    void draw() override;

private:
    std::vector<Notification> notifications;
};


#endif//SLIDER_DRAW_NOTIFICATION_MANAGER_H
