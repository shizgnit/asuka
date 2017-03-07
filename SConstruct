import os
import re
import shutil

#
# Source paths
#
root = os.path.join('src', 'asuka')

def configure(defines, target):
  config_in = file(str(target + '.in'), "r")
  config_out = file(str(target), "w")
  config_out.write(config_in.read() % defines)
  config_in.close()
  config_out.close()

#
# Get user supplied values
#
opts = Options('config.py')

opts.Add('BUILD_MAJOR', 'Major', 0)
opts.Add('BUILD_MINOR', 'Minor', 0)
opts.Add('BUILD_REV', 'Revision', 0)

platform = ARGUMENTS.get('OS', Platform())
debug = ARGUMENTS.get('DEBUG', 0)

env = Environment(options = opts)

#
# Setup OS specific configurations
#
build = 'unknown'

if(str(platform) == 'win32'):
  build = 'windows'
  env['PLATFORM'] = '__windows__';
  env['CPPPATH'] = [os.path.join('#win32', 'dependencies', 'include'), '#src']
  env['LIBPATH'] = [os.path.join('#win32', 'dependencies', 'lib')]

if(str(platform) == 'posix'):
  build = re.sub('\s+', '_', os.popen('uname -sr').read().rstrip())
  env['PLATFORM'] = '__' + str(platform) + '__'
  env['CPPPATH'] = ['#src']

#
# Install paths
#
lib_install = os.path.join(os.getcwd(), str(build), 'lib')
header_install = os.path.join(os.getcwd(), str(build), 'include')
module_install = os.path.join(os.getcwd(), str(build), 'lib', 'asuka')

#
# Default configurations
#
env['DEBUG'] = debug
env['ZLIB'] = '__no_zlib__'
env['PTHREAD'] = '__no_pthread__'
env['MYSQL'] = '__no_mysql__'
env['FREETYPE2'] = '__no_freetype__'
env['LUA'] = '__no_lua__'

print "platform: ", env['PLATFORM']

conf = Configure(env)

#
# Check dependencies
#
libraries = []

if conf.CheckLibWithHeader('z', 'zlib.h', 'cpp'):
  env['ZLIB'] = '__zlib__'
  libraries.append('z')

if conf.CheckLibWithHeader('pthread', 'pthread.h', 'cpp'):
  env['PTHREAD'] = '__pthread__'
  libraries.append('pthread')

if conf.CheckLibWithHeader('mysqlclient_r', 'mysql/mysql.h', 'cpp'):
  env['MYSQL'] = '__mysql__'
  libraries.append('mysqlclient_r')

if conf.CheckLibWithHeader('freetype', 'ft2build.h', 'c'):
  env['FREETYPE2'] = '__freetype2__'
  libraries.append('freetype')

if conf.CheckLibWithHeader('lua5.1', 'lua5.1/lua.h', 'c'):
  env['LUA'] = '__lua__'
  libraries.append('lua5.1')
  libraries.append('lua5.1-filesystem')

env = conf.Finish()

configure(env, os.path.join('src', 'asuka.hpp'))

#
# Setup some compiler specific settings
#
if(env['PLATFORM'] == '__windows__'):
  env.Append(CCFLAGS='/Od /D__exports__ /D__module_exports__ /EHsc /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "_WINDLL" /D "_MBCS" /Gm /EHsc /RTC1 /MTd /W3 /nologo /c /Wp64 /ZI')
  env.Append(LINKFLAGS='/INCREMENTAL /NOLOGO /SUBSYSTEM:WINDOWS /DLL /DEBUG')
  libraries.append('wsock32')
else:
  if(debug):
    env.Append(CCFLAGS='-g')
  else:
    env.Append(CCFLAGS='-O3')
    #env.Append(CCFLAGS='-Wno-deprecated')

#
# Provide help based on the environment
#
Help(opts.GenerateHelpText(env))

Export('env')

objects = []

base = os.path.join(str(build), root)
script = os.path.join(base, 'SConscript')

BuildDir(base, root, duplicate=0)
objects.append(SConscript(script, build_path=str(build), duplicate=0))

asuka_shared = env.SharedLibrary(os.path.join(lib_install,'asuka'), objects, LIBS=libraries, LIBPATH=os.path.join('dependencies', 'lib'))
asuka_static = env.StaticLibrary(os.path.join(lib_install,'asuka'), objects, LIBS=libraries, LIBPATH=os.path.join('dependencies', 'lib'))

for category in os.listdir(root):
  current = os.path.join(root, category)
  if os.path.isdir(current):
    if re.search('\.svn$', current):
      continue
    for mod in os.listdir(current):
      if re.search('\.svn$', mod):
        continue
      source = os.path.join(current, mod)
      if os.path.isdir(source) and source != '.svn':
        module_objects = []
        base = os.path.join(str(build), source)
        script = os.path.join(base, 'SConscript')
        BuildDir(base, source, duplicate=0)
        module_objects.append(SConscript(script, build_path=str(build), duplicate=0))
        module_shared = env.SharedLibrary(os.path.join(module_install,category,mod), module_objects, LIBS=libraries)

#
# Install instructions
#
root = os.path.join(os.getcwd(), 'src')

def callback( headers, dirname, fnames ):
  target = re.sub(root, header_install, dirname)
  if not os.path.isdir(target):
    os.makedirs(target)
  for filename in fnames:
    header = os.path.join(dirname, filename)
    if re.search('(\.h|\.hpp)$', filename, re.IGNORECASE) and not os.path.isdir(header):
      new = os.path.join(target, filename)
      if not os.path.isfile(new) or not os.stat(new).st_mtime == os.stat(header).st_mtime:
        print 'Install file:', new
        shutil.copy2(header, new)
  if 'CVS' in fnames:
      fnames.remove('CVS');
  if '.svn' in fnames:
      fnames.remove('.svn');

os.path.walk(root, callback, [])
