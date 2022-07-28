#include "notification_manager.h"
#include <imgui.h>
#include <utility>

void Notification_manager::add_notification(std::string title, std::string notification)
{
    notifications.push_back(Notification{std::move(title), std::move(notification)});
}

void Notification_manager::draw()
{
    for(auto it = notifications.begin(); it != notifications.end();) {
        auto& notification = *it;
        if(ImGui::BeginPopup(notification.title.c_str())) {
            ImGui::Text("%s", notification.content.c_str());
            if(ImGui::Button("Ok")) {// shown is already set, and we don't want to handle duplicates
                ImGui::CloseCurrentPopup();
            }
            ImGui::End();
        }
        auto shown = notification.shown;
        if(!notification.shown) {
            ImGui::OpenPopup(notification.title.c_str());
            notification.shown = true;
        }
        if(!shown || ImGui::IsPopupOpen(notification.title.c_str())) ++it;
        else
            notifications.erase(it);
    }
}
