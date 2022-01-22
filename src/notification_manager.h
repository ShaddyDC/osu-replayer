#ifndef SLIDER_DRAW_NOTIFICATION_MANAGER_H
#define SLIDER_DRAW_NOTIFICATION_MANAGER_H

#include <string>

class Notification_manager {
public:
    void add_notification(std::string title, std::string notification);
    void draw();

private:
    static inline std::string stored_title, stored_notification;
    static inline unsigned int id;
};


#endif//SLIDER_DRAW_NOTIFICATION_MANAGER_H
