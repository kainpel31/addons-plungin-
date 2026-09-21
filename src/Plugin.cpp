#include "PCH.h"

namespace
{
	// 4-character unique id for this plugin's co-save record ("MMOH").
	constexpr std::uint32_t kSerializationUniqueID = 0x4D4D4F48;

	void InitializeLog()
	{
		auto path = logger::log_directory();
		if (!path) {
			stl::report_and_fail("Failed to find the standard logging directory."sv);
		}

		*path /= std::format("{}.log", Version::LOG_NAME);

		auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true);

		auto log = std::make_shared<spdlog::logger>("global log"s, std::move(sink));
		log->set_level(spdlog::level::info);
		log->flush_on(spdlog::level::info);

		spdlog::set_default_logger(std::move(log));
		spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] %v"s);

		logger::info("{} {} ({})", Version::DISPLAY_NAME, Version::VERSION, Version::AUTHOR);
	}

	void LogRuntime()
	{
		// Runtime detection has to be right for one DLL to serve SE, AE 1.6.x/1.7.x and VR:
		// REL::Module classifies the running executable itself, and CommonLibSSE-NG resolves
		// per-runtime struct layouts on top of that. Logging it makes a wrong classification
		// visible immediately instead of showing up as bad offsets later.
		const auto runtime = REL::Module::IsVR() ? "VR"sv : (REL::Module::IsAE() ? "AE"sv : "SE"sv);
		logger::info("runtime: {} {} | address library: yes", runtime, REL::Module::get().version().string());
	}

	void OnSKSEMessage(SKSE::MessagingInterface::Message* a_message)
	{
		switch (a_message->type) {
		case SKSE::MessagingInterface::kDataLoaded:
			// M1 resolves the STB Hotkey System API and starts the binding registry here.
			// Everything the hotbar shows comes from that snapshot, so it has to exist before
			// the first frame is drawn.
			break;

		case SKSE::MessagingInterface::kPostLoadGame:
		case SKSE::MessagingInterface::kNewGame:
			// M5 loads the slot placements from the co-save here. This runs after
			// kDataLoaded, and before the player sees anything.
			break;

		default:
			break;
		}
	}
}

// The plugin declaration (name, version, author, address-library independence) is
// generated into __MMOHotbarPlugin.cpp by add_commonlibsse_plugin(), which also emits
// SKSEPlugin_Query. Only the load entry point is ours to write.
SKSE_PLUGIN_LOAD(const SKSE::LoadInterface* a_skse)
{
	InitializeLog();
	SKSE::Init(a_skse);
	LogRuntime();

	const auto serialization = SKSE::GetSerializationInterface();
	if (!serialization) {
		logger::error("no serialization interface; slot placements cannot be saved");
	} else {
		// Callbacks are added in M5; the unique id is claimed now so the record cannot clash
		// with another plugin's.
		serialization->SetUniqueID(kSerializationUniqueID);
	}

	const auto messaging = SKSE::GetMessagingInterface();
	if (!messaging || !messaging->RegisterListener("SKSE", OnSKSEMessage)) {
		logger::error("failed to register the SKSE messaging listener");
		return false;
	}

	logger::info("loaded");
	return true;
}
