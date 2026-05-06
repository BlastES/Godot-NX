# Godot-NX
Port of the Godot Engine 4.x to Switch platform with libnx
## Releases
Once the port for the 4.1_Switch branch is done, a binary editor and export template will be released
## Compiling
You need a proper setup of [devkitpro pacman](https://devkitpro.org/wiki/devkitPro_pacman) with the following packages installed: `switch-dev` `switch-portlibs` <!--Source: vrince's port dockerfile (https://github.com/vrince/godot/blob/4.x-switch-port/platform/switch/devkitpro.Dockerfile) --><br/>

Then you should compile the editor with `scons`. For more information, please consult the [Documentation](https://docs.godotengine.org/en/stable/engine_details/development/compiling/index.html)<br/>

To compile the export templates run `scons platform=switch target=template_(release or debug)` and then run the `switch_export_template.sh` script (you should modify it to put the export template in the corresponding folder or if you want to use the debug export template instead of the release one)
## Documentation
- Libnx: [Official](https://switchbrew.github.io/libnx/index.html), [Deepwiki](deepwiki.com/switchbrew/libnx/1-libnx-overview) &#8592; great for understanding stuff that isn't really explained in the official documentation
- Homebrew Development: [Homebrew Guide](https://egetrickrolled.github.io/Homebrew-Guide-site-b/homebrew_dev/app_dev.html), [switchbrew/switch-examples](https://github.com/switchbrew/switch-examples)
## Credits
- devkitPro/switchbrew for libnx
- vrince for the original port
- Stary2001/Hombrodot team for the 3.x ports which vrince's work and this are based on
