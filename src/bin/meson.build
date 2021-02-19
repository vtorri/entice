entice_bin_src = [
  'entice_config.c',
  'entice_controls.c',
  'entice_image.c',
  'entice_key.c',
  'entice_main.c',
  'entice_theme.c',
  'entice_win.c'
]

entice_bin = executable('entice', entice_bin_src,
  c_args              : [ entice_cargs ],
  dependencies        : [ entice_deps ],
  include_directories : config_dir,
  install             : true
)
