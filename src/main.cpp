#include "SKSE/SKSE.h"
#include "RE/Skyrim.h"
#include "SKSEMenuFramework.hpp" // Header dari SKSE Menu Framework
#include "STB_HotkeySystemAPI.h" // Header API resmi STB-Hotkey-System

// Variabel konfigurasi yang dapat diatur via SKSE Menu Framework
struct HotbarConfig {
    int visibleSlots = 12; // Default maksimal 12 slot aktif
};

HotbarConfig g_config;

// Fungsi untuk mengeksekusi slot hotbar melalui API STB
void TriggerHotkeySlot(int slotIndex) 
{
    if (STB_HotkeySystemAPI::GetApi()) 
    {
        // Indeks slot disesuaikan dengan sistem STB (1 sampai 12)
        STB_HotkeySystemAPI::GetApi()->ExecuteSlot(slotIndex);
    }
}

// Render UI Pengaturan menggunakan SKSE Menu Framework (ImGui)
void RenderMMOHotbarMenu() 
{
    ImGui::Text("MMO Hotbar Settings (STB Addon)");
    ImGui::Separator();
    
    // Slider untuk mengatur jumlah slot hotbar yang aktif/tampil di layar (1 - 12)
    ImGui::SliderInt("Visible Hotbar Slots", &g_config.visibleSlots, 1, 12);
    
    if (ImGui::Button("Reset to Default")) {
        g_config.visibleSlots = 12;
    }
}

// Listener Input Keyboard untuk mendeteksi tombol hotbar
class HotbarInputHandler : public RE::BSTEventSink<RE::InputEvent*> 
{
public:
    static HotbarInputHandler* GetSingleton() {
        static HotbarInputHandler singleton;
        return &singleton;
    }

    RE::BSEventNotifyControl ProcessEvent(RE::InputEvent* const* a_event, RE::BSTEventSource<RE::InputEvent*>* a_eventSource) override 
    {
        if (!a_event || !*a_event) {
            return RE::BSEventNotifyControl::kContinue;
        }

        for (auto event = *a_event; event; event = event->next) {
            if (event->GetEventType() != RE::INPUT_EVENT_TYPE::kButton) {
                continue;
            }

            auto button = event->AsButtonEvent();
            if (!button || !button->IsPressed()) {
                continue;
            }

            uint32_t keyCode = button->GetIDCode();

            // Deteksi tombol angka '1' sampai '9' (DXScanCode 0x02 - 0x0A)
            if (keyCode >= 0x02 && keyCode <= 0x0A) {
                int slot = keyCode - 0x01;
                if (slot <= g_config.visibleSlots) {
                    TriggerHotkeySlot(slot);
                }
            }
            // Tombol tambahan F1, F2, F3 untuk slot 10, 11, 12
            else if (keyCode == 0x3B && g_config.visibleSlots >= 10) { TriggerHotkeySlot(10); } // F1
            else if (keyCode == 0x3C && g_config.visibleSlots >= 11) { TriggerHotkeySlot(11); } // F2
            else if (keyCode == 0x3D && g_config.visibleSlots >= 12) { TriggerHotkeySlot(12); } // F3
        }

        return RE::BSEventNotifyControl::kContinue;
    }
};

// Inisialisasi utama saat plugin dimuat oleh SKSE
SKSE_PLUGIN_LOAD(const SKSE::LoadInterface *skse) {
    SKSE::Init(skse);

    // Registrasi menu ke SKSE Menu Framework
    if (SKSEMenuFramework::IsInstalled()) {
        SKSEMenuFramework::AddCategory("MMO Hotbar", RenderMMOHotbarMenu);
    }

    // Daftarkan event listener untuk input keyboard
    RE::BSInputDeviceManager::GetSingleton()->AddEventSink(HotbarInputHandler::GetSingleton());

    SKSE::log::info("MMO Hotbar Addon berhasil dimuat dengan dukungan SKSE Menu Framework & STB API!");
    return true;
}
