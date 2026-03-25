#include "export_plugin.h"

#include "core/io/packet_peer_udp.h"
#include "core/io/ip_address.h"
#include "core/config/project_settings.h"
#include "editor/editor_node.h"
#include "editor/editor_paths.h"

void ExportPluginSwitch::_export_begin(const HashSet<String> &p_features, bool p_debug, const String &p_path, int p_flags){
    if (editor_id_vec.size() != 0){
        add_file("custom_editor_id", editor_id_vec, false);
    }
}

void EditorExportPlatformSwitch::_device_poll_thread(void *ud){
    EditorExportPlatformSwitch *ea = (EditorExportPlatformSwitch *)ud;

    PacketPeerUDP peer;

		peer.set_broadcast_enabled(true);
		peer.set_dest_address(IPAddress("255.255.255.255"), 28280);

		const uint8_t *packet_buff;
		int packet_len;

		while (!ea->quit_request) {
			bool different = false;

			peer.put_packet((unsigned char *)"nxboot", strlen("nxboot"));
			OS::get_singleton()->delay_usec(500000); // delay 500ms to allow for actual replies

			Vector<String> ndevices;

			while (peer.get_packet(&packet_buff, packet_len) != ERR_UNAVAILABLE) {
				IPAddress a = peer.get_packet_address();
				ndevices.push_back(String(a));
			}

			ndevices.sort();

			ea->device_lock.lock();

			if (ndevices.size() != ea->devices.size()) {
				different = true;
			} else {
				for (int i = 0; i < ea->devices.size(); i++) {
					if (ea->devices[i] != ndevices[i]) {
						different = true;
						break;
					}
				}
			}

			if (different) {
				ea->devices = ndevices;
				ea->devices_changed = true;
			}

			ea->device_lock.unlock();
		}
}

void EditorExportPlatformSwitch::get_preset_features(const Ref<EditorExportPreset> &p_preset, List<String> *r_features){
    String driver = ProjectSettings::get_singleton()->get("rendering/quality/driver/driver_name");
    if (driver == "GLES2"){
        r_features->push_back("etc");
    } else if (driver == "GLES3") {
        r_features->push_back("etc2");
        if (ProjectSettings::get_singleton()->get("rendering/quality/driver/fallback_to_gles2")) {
            r_features->push_back("etc");
        }
    }
}

void EditorExportPlatformSwitch::get_platform_features(List<String> *r_features) {
    r_features->push_back("mobile");
}

void EditorExportPlatformSwitch::get_export_options(List<ExportOption> *r_options){
    String title = ProjectSettings::get_singleton()->get("application/config/name");
		r_options->push_back(ExportOption(PropertyInfo(Variant::BOOL, "application/fused_build"), false));
		r_options->push_back(ExportOption(PropertyInfo(Variant::STRING, "application/custom_editor_id"), ""));
		r_options->push_back(ExportOption(PropertyInfo(Variant::STRING, "application/title", PROPERTY_HINT_PLACEHOLDER_TEXT, title), title));
		r_options->push_back(ExportOption(PropertyInfo(Variant::STRING, "application/author", PROPERTY_HINT_PLACEHOLDER_TEXT, "Game Author"), "Stary & Cpasjuste"));
		r_options->push_back(ExportOption(PropertyInfo(Variant::STRING, "application/version", PROPERTY_HINT_PLACEHOLDER_TEXT, "Game Version"), "1.0"));
		r_options->push_back(ExportOption(PropertyInfo(Variant::STRING, "application/icon_256x256", PROPERTY_HINT_GLOBAL_FILE, "*.jpg"), ""));
}

String EditorExportPlatformSwitch::get_name() const{
    return "Switch";
}

String EditorExportPlatformSwitch::get_os_name() const{
    return "Switch";
}

Ref<Texture2D> EditorExportPlatformSwitch::get_logo() const{
    return logo;
}

Ref<Texture2D> EditorExportPlatformSwitch::get_run_icon() const{
    return logo;
}

bool EditorExportPlatformSwitch::poll_export(){
    bool dc = devices_changed;
    if (dc) {
        // don't clear unless we're reporting true, to avoid race
        devices_changed = false;
    }
    return dc;
}

int EditorExportPlatformSwitch::get_options_count() const{
    device_lock.lock();
    int dc = devices.size();
	device_lock.unlock();
	return dc;
}

String EditorExportPlatformSwitch::get_options_tooltip() const{
    return TTRC("Select device from the list");
}

String EditorExportPlatformSwitch::get_option_label(int p_index) const{
    ERR_FAIL_INDEX_V(p_index, devices.size(), "");
    device_lock.lock();
    String s = devices[p_index];
    device_lock.unlock();
    return s;
}

String EditorExportPlatformSwitch::get_option_tooltip(int p_index) const{
    ERR_FAIL_INDEX_V(p_index, devices.size(), "");
	device_lock.lock();
	String s = devices[p_index];
	device_lock.unlock();
	return s;
}

Error EditorExportPlatformSwitch::run(const Ref<EditorExportPreset> &p_preset, int p_device, int p_debug_flags){
    String can_export_error;
    bool can_export_missing_templates;
    if (!can_export(p_preset, can_export_error, can_export_missing_templates)) {
        EditorNode::add_io_error(can_export_error);
        return ERR_UNCONFIGURED;
    }

    EditorProgress ep("run", "Running", 2);

    print_line("Exporting...");
    if (ep.step("Exporting...", 0)) {
        return ERR_SKIP;
    }

    String tmp_pack_path = EditorPaths::get_singleton()->get_cache_dir() + "tmpexport.pck";

    Error err = save_pack(p_preset, false, tmp_pack_path);

    if (err != OK) {
        DirAccess::remove_file_or_error(tmp_pack_path);
        return err;
    }

    print_line("Sending...");
    if (ep.step("Sending...", 1)) {
        DirAccess::remove_file_or_error(tmp_pack_path);
        return err;
    }

    String nxlink = EditorSettings::get_singleton()->get("export/switch/nxlink");
    // If we can't find it, look for a bundled copy.
    if (nxlink == "") {
        String exe_ext;
        if (OS::get_singleton()->get_name() == "Windows") {
            exe_ext = ".exe";
        }
        nxlink = OS::get_singleton()->get_executable_path().get_base_dir() + "/nxlink" + exe_ext;
    }

    if (FileAccess::exists(nxlink)) {
        List<String> args;
        int ec = 0;
        String output;

        args.push_back(tmp_pack_path);
        args.push_back("-a");
        args.push_back(devices[p_device]);
        args.push_back("-p");
        args.push_back("TempExport.pck");
        args.push_back("--args");

        Vector<String> inner_args;
        // todo: editor arg
        inner_args.push_back("-v");

        gen_export_flags(inner_args, p_debug_flags);
        args.push_back(String(" ").join(inner_args));

        OS::get_singleton()->execute(nxlink, args, &output, &ec, true);
    } else {
        EditorNode::get_singleton()->show_warning(TTRC("nxlink binary not found! Set its path in Editor Settings."));
    }

    DirAccess::remove_file_or_error(tmp_pack_path);
    return OK;

}

bool EditorExportPlatformSwitch::can_export(const Ref<EditorExportPreset> &p_preset, String &r_error, bool &r_missing_templates, bool p_debug = false) const{
    String err;
    r_missing_templates =
            find_export_template(TEMPLATE_RELEASE) == String() ||
            find_export_template(TEMPLATE_APPLET_SPLASH) == String();

    bool valid = !r_missing_templates;
    String etc_error = test_etc2();
    if (etc_error != String()) {
        err += etc_error;
        valid = false;
    }

    if (!err.is_empty()) {
        r_error = err;
    }

    return valid;
}

List<String> EditorExportPlatformSwitch::get_binary_extensions(const Ref<EditorExportPreset> &p_preset) const{
    List<String> list;
    list.push_back("nro");
    return list;
}

void EditorExportPlatformSwitch::resolve_platform_feature_priorities(const Ref<EditorExportPreset> &p_preset, HashSet<String> &p_features){
}

Error EditorExportPlatformSwitch::export_pack(const Ref<EditorExportPreset> &p_preset, bool p_debug, const String &p_path, int p_flags = 0){
    // XXX i hate this - we have to do this _before_ the export notifier
    String custom_editor_id = p_preset->get("application/custom_editor_id");
    export_plugin->editor_id_vec.clear();

    if (custom_editor_id != "") {
        // XXX what the hell is this
        const char *chars = custom_editor_id.utf8().ptr();
        for (size_t i = 0; i < strlen(chars); i++) {
            export_plugin->editor_id_vec.push_back(chars[i]);
        }
    }

    ExportNotifier notifier(*this, p_preset, p_debug, p_path, p_flags);

    return save_pack(p_preset, false, p_path);
}

Error EditorExportPlatformSwitch::export_project(const Ref<EditorExportPreset> &p_preset, bool p_debug, const String &p_path, int p_flags = 0){
    // XXX i hate this - we have to do this _before_ the export notifier
    String custom_editor_id = p_preset->get("application/custom_editor_id");
    export_plugin->editor_id_vec.clear();

    if (custom_editor_id != "") {
        // XXX what the hell is this
        const char *chars = custom_editor_id.utf8().ptr();
        for (size_t i = 0; i < strlen(chars); i++) {
            export_plugin->editor_id_vec.push_back(chars[i]);
        }
    }

    ExportNotifier notifier(*this, p_preset, p_debug, p_path, p_flags);

    if (!DirAccess::exists(p_path.get_base_dir())) {
        return ERR_FILE_BAD_PATH;
    }

    String nro_path = find_export_template(TEMPLATE_RELEASE);
    if (nro_path != String() && !FileAccess::exists(nro_path)) {
        EditorNode::get_singleton()->show_warning(TTRC("Template file not found:\n" + nro_path));
        return ERR_FILE_NOT_FOUND;
    }

    Ref<DirAccess> da = DirAccess::create(DirAccess::ACCESS_FILESYSTEM);

    Error err;
    // update nro icon/title/author/version
    String title = p_preset->get("application/title");
    String author = p_preset->get("application/author");
    String version = p_preset->get("application/version");
    String icon = p_preset->get("application/icon_256x256");

    NacpStruct *nacp = memnew(NacpStruct);
    memset(nacp, 0, sizeof(NacpStruct));
    create_nacp(nacp, title, author, version);

    if (p_preset->get("application/fused_build")) {
        String build_romfs = EditorSettings::get_singleton()->get("export/switch/build_romfs");

        // If we can't find it, look for a bundled copy.
        if (build_romfs == "") {
            String exe_ext;
            if (OS::get_singleton()->get_name() == "Windows") {
                exe_ext = ".exe";
            }
            build_romfs = OS::get_singleton()->get_executable_path().get_base_dir() + "/build_romfs" + exe_ext;
        }

        if (build_romfs != String() && FileAccess::exists(build_romfs)) {
            String cache = EditorPaths::get_singleton()->get_cache_dir();
            String romfs_dir = cache + "romfs";
            String romfs_bin_path = cache + "romfs.bin";

            da->make_dir(romfs_dir);
            err = save_pack(p_preset, false, romfs_dir + "game.pck");
            if (err == OK) {
                String applet_splash = find_export_template(TEMPLATE_APPLET_SPLASH);
                if (FileAccess::exists(applet_splash)) {
                    da->copy(applet_splash, romfs_dir + "applet_splash.rgba.gz");

                    List<String> args;
                    int ec;
                    String output;
                    args.push_back(romfs_dir);
                    args.push_back(romfs_bin_path);
                    OS::get_singleton()->execute(build_romfs, args, &output, &ec, true);
                    if (ec == 0) {
                        err = create_nro(nro_path, p_path, nacp, icon, romfs_bin_path);
                        DirAccess::remove_file_or_error(romfs_bin_path);
                    } else {
                        EditorNode::get_singleton()->show_warning(TTRC("build_romfs failed!"));
                        err = ERR_BUG;
                    }
                } else {
                    EditorNode::get_singleton()->show_warning(TTRC("Template file not found:\n" + applet_splash));
                    err = ERR_FILE_NOT_FOUND;
                }
            } else {
                EditorNode::get_singleton()->show_warning(TTRC("Pack export failed!"));
            }

            DirAccess::remove_file_or_error(romfs_dir);
        } else {
            EditorNode::get_singleton()->show_warning(TTRC("build_romfs binary not found! Set its path in Editor Settings."));
            err = ERR_FILE_NOT_FOUND;
        }
    } else {
        String empty_string = "";

        err = save_pack(p_preset, false, p_path.get_basename() + ".pck");
        if (err == OK) {
            err = create_nro(nro_path, p_path, nacp, icon, empty_string);
        } else {
            EditorNode::get_singleton()->show_warning(TTRC("Pack export failed!"));
        }
    }

    memdelete(nacp);
    return err;
}

void EditorExportPlatformSwitch::copy_chunked(Ref<FileAccess> src, Ref<FileAccess> dst, size_t len, size_t chunk_size = 0x100000){
    uint8_t *buffer = (uint8_t *)malloc(chunk_size);
    while (len > chunk_size) {
        size_t amt = src->get_buffer(buffer, chunk_size);
        dst->store_buffer(buffer, amt);
        len -= amt;
    }

    src->get_buffer(buffer, len);
    dst->store_buffer(buffer, len);

    free(buffer);
}

Error EditorExportPlatformSwitch::create_nro(const String &template_path, const String &output_path, NacpStruct *nacp, String &icon_path, String &romfs_path){
    NroHeader nro_header;
    NroAssetHeader asset_header;

    Ref<FileAccess> template_f = FileAccess::open(template_path, FileAccess::READ);
    Ref<FileAccess> nro = FileAccess::open(output_path, FileAccess::WRITE);

    if (!template_f.is_valid() || !nro.is_valid())
        return ERR_FILE_NOT_FOUND;

    template_f->seek(sizeof(NroStart));
    template_f->get_buffer((uint8_t *)&nro_header, sizeof(NroHeader));

    if (memcmp(&nro_header.magic, "NRO0", 4) != 0) {
        template_f->close();
        nro->close();
        return ERR_INVALID_DATA;
    }

    // copy nro body to output
    template_f->seek(0);
    copy_chunked(template_f, nro, nro_header.size);

    template_f->seek(nro_header.size);
    template_f->get_buffer((uint8_t *)&asset_header, sizeof(NroAssetHeader));
    if (memcmp(&asset_header.magic, "ASET", 4) != 0) {
        template_f->close();
        nro->close();
        return ERR_INVALID_DATA;
    }

    NroAssetHeader new_asset_header;
    memset(&new_asset_header, 0, sizeof(NroAssetHeader));

    // write dummy asset header here
    nro->store_buffer((uint8_t *)&new_asset_header, sizeof(NroAssetHeader));

    memcpy(&new_asset_header.magic, "ASET", 4);
    new_asset_header.version = 0;
    new_asset_header.icon.offset = nro->get_position() - nro_header.size;

    if (icon_path != String() && FileAccess::exists(icon_path)) {
        // replace icon
        Ref<FileAccess> icon = FileAccess::open(icon_path, FileAccess::READ);
        size_t icon_len = icon->get_length();
        copy_chunked(icon, nro, icon_len);

        new_asset_header.icon.size = icon_len;
        icon->close();
    } else {
        // copy icon
        template_f->seek(nro_header.size + asset_header.icon.offset);
        copy_chunked(template_f, nro, asset_header.icon.size);
        new_asset_header.icon.size = asset_header.icon.size;
    }

    // write new nacp
    new_asset_header.nacp.offset = nro->get_position() - nro_header.size;
    new_asset_header.nacp.size = sizeof(NacpStruct);
    nro->store_buffer((uint8_t *)nacp, sizeof(NacpStruct));

    new_asset_header.romfs.offset = nro->get_position() - nro_header.size;

    if (romfs_path != String() && FileAccess::exists(romfs_path)) {
        Ref<FileAccess> romfs = FileAccess::open(romfs_path, FileAccess::READ);
        size_t romfs_len = romfs->get_length();
        copy_chunked(romfs, nro, romfs_len);

        new_asset_header.romfs.size = romfs_len;
        romfs->close();
    } else {
        template_f->seek(nro_header.size + asset_header.romfs.offset);
        copy_chunked(template_f, nro, asset_header.romfs.size);

        new_asset_header.romfs.size = asset_header.romfs.size;
    }

    // Go back and actually write the asset header
    nro->seek(nro_header.size);
    nro->store_buffer((uint8_t *)&new_asset_header, sizeof(NroAssetHeader));

    template_f->close();
    nro->close();

    return OK;
}

void EditorExportPlatformSwitch::create_nacp(NacpStruct *nacp, String &title, String &author, String &version){
   CharString title_utf8 = title.utf8();
    CharString author_utf8 = author.utf8();
    CharString version_utf8 = version.utf8();

    const char *title_cstr = title_utf8.ptr();
    const char *author_cstr = author_utf8.ptr();
    const char *version_cstr = version_utf8.ptr();

    for (int i = 0; i < 12; i++) {
        if (title.length() != 0) {
            strncpy(nacp->lang[i].name, title_cstr, sizeof(nacp->lang[i].name) - 1);
        }
        if (author.length() != 0) {
            strncpy(nacp->lang[i].author, author_cstr, sizeof(nacp->lang[i].author) - 1);
        }
    }

    if (version.length() != 0) {
        strncpy(nacp->display_version, version_cstr, sizeof(nacp->display_version) - 1);
    } 
}

EditorExportPlatformSwitch::EditorExportPlatformSwitch(){
    Ref<ImageTexture> img(_switch_logo_svg);
    logo->create_from_image(img);

    devices_changed = true;
    quit_request = false;
    device_thread.start(_device_poll_thread, this);

    export_plugin = memnew(ExportPluginSwitch);
    EditorExport::get_singleton()->add_export_plugin(export_plugin);
}
