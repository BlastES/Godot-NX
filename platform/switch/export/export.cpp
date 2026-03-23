#include "export.h"
#include "export_plugin.h"

void register_switch_exporter(){
    String exe_ext;
    if (OS::get_singleton()->get_name() == "Windows"){
        exe_ext = "*.exe";
    }
    EDITOR_DEF("export/switch/nxlink", "");
    EditorSettings::get_singleton()->add_property_hint(PropertyInfo(Variant::STRING, "export/switch/nxlink", PROPERTY_HINT_GLOBAL_FILE, exe_ext));
    
    EDITOR_DEF("export/switch/build_romfs", "");
	EditorSettings::get_singleton()->add_property_hint(PropertyInfo(Variant::STRING, "export/switch/build_romfs", PROPERTY_HINT_GLOBAL_FILE, exe_ext));

	Ref<EditorExportPlatformSwitch> exporter = Ref<EditorExportPlatformSwitch>();
	EditorExport::get_singleton()->add_export_platform(exporter);
}