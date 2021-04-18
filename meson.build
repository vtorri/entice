project('entice', 'c',
  version         : '0.0.1',
  license         : 'BSD 2 clause',
  default_options : [ 'buildtype=plain' ],
  meson_version   : '>= 0.53',
)

v_array = meson.project_version().split('.')
v_maj = v_array[0]
v_min = v_array[1]
v_mic = v_array[2]

# install paths
dir_prefix = get_option('prefix')
dir_bin = join_paths(dir_prefix, get_option('bindir'))
dir_lib = join_paths(dir_prefix, get_option('libdir'))
dir_data = join_paths(dir_prefix, get_option('datadir'))
dir_pkgdata = join_paths(dir_data, meson.project_name())
dir_locale = join_paths(dir_prefix, get_option('localedir'))

# host

windows = import('windows')

host_os = host_machine.system()
win32 = ['windows']
sys_windows = win32.contains(host_os)

# binaries
cc = meson.get_compiler('c')
efl_req = '>= 1.25'
entice_deps = [
               dependency('elementary', version : efl_req),
               dependency('libexif'),
              ]
edje = dependency('edje', version: efl_req)

# edje_cc tool - allow ti as an option for cross-compile
edje_cmd = get_option('edje-cc')
if edje_cmd == ''
  edje_cmd = join_paths(edje.get_pkgconfig_variable('prefix'),
                        'bin', 'edje_cc')
endif

# translations
if get_option('nls') == true
  entice_cargs = [ '-DGETTEXT_PACKAGE="entice"' ]
  entice_deps += cc.find_library('intl', required: false)
endif

# configuration
config_dir = [include_directories('.')]
config_h = configuration_data()
config_h.set_quoted('PACKAGE_NAME', meson.project_name())
config_h.set_quoted('PACKAGE_VERSION', meson.project_version())
config_h.set_quoted('PACKAGE_BIN_DIR', dir_bin)
config_h.set_quoted('PACKAGE_LIB_DIR', dir_lib)
config_h.set_quoted('PACKAGE_DATA_DIR', dir_data)
config_h.set_quoted('LOCALE_DIR', dir_locale)

subdir('src/bin')
subdir('data/themes')
subdir('data/desktop')
subdir('data/icons')

# use config_h after all subdirs have set values
configure_file(output : 'config.h', configuration : config_h)

# translations - not used yet
# if get_option('nls') == true
#  subdir('po')
# endif
