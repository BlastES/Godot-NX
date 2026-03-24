#ifndef SWITCH_EXPORT_PLUGIN_H
#define SWITCH_EXPORT_PLUGIN_H

#include "core/io/file_access.h"
#include "editor/editor_settings.h"
#include "editor/export/editor_export.h"
#include "editor/export/editor_export_plugin.h"
#include "editor/export/editor_export_platform.h"
#include "scene/resources/texture.h"

#include "platform/switch/export/logo_svg.gen.h"

#include "thirdparty/libnx/nro.h"
#include "thirdparty/libnx/nacp.h"

#define TEMPLATE_RELEASE "switch_release.nro"
#define TEMPLATE_APPLET_SPLASH "switch_applet_splash.rgba.gz"

class ExportPluginSwitch : public EditorExportPlugin {
    public:
        Vector<uint8_t> editor_id_vec;

    protected:
        virtual void _export_begin(const HashSet<String> &p_features, bool p_debug, const String &p_path, int p_flags);

};

class EditorExportPlatformSwitch : public EditorExportPlatform {
    GDCLASS(EditorExportPlatformSwitch, EditorExportPlatform);

    Ref<ImageTexture> logo;
    Vector<String> devices;
    volatile bool devices_changed;
    Mutex device_lock;
    Thread device_thread;
    volatile bool quit_request;
    ExportPluginSwitch *export_plugin;

    static void _device_poll_thread(void *ud);

    public:
        virtual void get_preset_features(const Ref<EditorExportPreset> &p_preset, List<String> *r_features) const override;
        virtual void get_platform_features(List<String> *r_features) const override;
        virtual void get_export_options(List<ExportOption> *r_options) const override;

        virtual String get_name() const override;
        virtual String get_os_name() const override;
        virtual Ref<Texture2D> get_logo() const override;
        virtual Ref<Texture2D> get_run_icon() const override;
        virtual String get_template_file_name(const String &p_target, const String &p_arch) const;


        virtual bool poll_export() override;
        virtual int get_options_count() const override;
        virtual String get_options_tooltip() const override;
        virtual String get_option_label(int p_index) const override;
        virtual String get_option_tooltip(int p_index) const override;

        virtual Error run(const Ref<EditorExportPreset> &p_preset, int p_device, int p_debug_flags) override;

        virtual bool can_export(const Ref<EditorExportPreset> &p_preset, String &r_error, bool &r_missing_templates, bool p_debug = false) const;
        virtual List<String> get_binary_extensions(const Ref<EditorExportPreset> &p_preset) const override;
        virtual void resolve_platform_feature_priorities(const Ref<EditorExportPreset> &p_preset, HashSet<String> &p_features) override;
        
        virtual bool get_export_option_visibility(const EditorExportPreset *p_preset, const String &p_option) const override;
        virtual String get_export_option_warning(const EditorExportPreset *p_preset, const StringName &p_name) const override;

        virtual bool has_valid_export_configuration(const Ref<EditorExportPreset> &p_preset, String &r_error, bool &r_missing_templates, bool p_debug = false) const override;
        virtual bool has_valid_project_configuration(const Ref<EditorExportPreset> &p_preset, String &r_error) const override; 

        virtual Error export_pack(const Ref<EditorExportPreset> &p_preset, bool p_debug, const String &p_path, int p_flags = 0) override;
        virtual Error export_project(const Ref<EditorExportPreset> &p_preset, bool p_debug, const String &p_path, int p_flags = 0) override;

        void copy_chunked(Ref<FileAccess> src, Ref<FileAccess> dst, size_t len, size_t chunk_size = 0x100000);
        
        Error create_nro(const String &template_path, const String &output_path, NacpStruct *nacp, String &icon_path, String &romfs_path);
        void create_nacp(NacpStruct *nacp, String &title, String &author, String &version);

        EditorExportPlatformSwitch();
};

#endif //SWITCH_EXPORT_PLUGGIN_H