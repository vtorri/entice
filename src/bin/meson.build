entice_bin_src = [
  'entice_config.c',
  'entice_controls.c',
  'entice_image.c',
  'entice_key.c',
  'entice_main.c',
  'entice_settings.c',
  'entice_theme.c',
  'entice_win.c'
]

if sys_windows == true
  rcconf = configuration_data()
  rcconf.set('SOURCEDIR', meson.source_root())
  rcconf.set('VERSION', meson.project_version())
  rcconf.set('VMAJ', v_maj)
  rcconf.set('VMIN', v_min)
  rcconf.set('VMIC', v_mic)
  entice_rc = configure_file(
    input : 'entice.rc.in',
    output : 'entice.rc',
    configuration : rcconf
  )
  entice_rc_res = windows.compile_resources(entice_rc)
  entice_bin_src += [ entice_rc_res ]
endif

entice_bin = executable('entice', entice_bin_src,
  c_args              : [ entice_cargs ],
  dependencies        : [ entice_deps ],
  include_directories : config_dir,
  install             : true
)
