
entice_bin_src = [
  'entice_config.c',
  'entice_key.c',
  'entice_main.c',
  'entice_theme.c',
  'entice_win.c'
]

# Windows resource file

## todo


entice_bin = executable('entice', entice_bin_src,
  c_args              : [ entice_cargs ],
  dependencies        : [ entice_deps ],
  include_directories : config_dir,
  install             : true
)
