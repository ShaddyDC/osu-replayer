#include "notification_manager.h"
#include <imgui.h>
#include <utility>

void Notification_manager::add_notification(std::string title, std::string notification)
{
    stored_title = std::move(title);
    stored_notification = std::move(notification);
    ImGui::OpenPopup(id);
}

void Notification_manager::draw()
{
    if(ImGui::BeginPopup("Notification")) {
        ImGui::Text("%s", stored_notification.c_str());
        if(ImGui::Button("Ok")) stored_notification.clear();
        ImGui::End();
    }
    id = ImGui::GetID("Notification");
}
