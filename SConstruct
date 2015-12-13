import os, string, sys
import SCons
import SCons.Errors

#
# set configuration options
#
opts = Options('xreal.conf')
opts.Add(EnumOption('arch', 'Choose architecture to build for', 'linux-i386', allowed_values=('linux-i386', 'linux-x86_64', 'netbsd-i386', 'win32-mingw')))
opts.Add(EnumOption('warnings', 'Choose warnings level', '1', allowed_values=('0', '1', '2')))
opts.Add(EnumOption('debug', 'Set to >= 1 to build for debug', '0', allowed_values=('0', '1', '2', '3')))
opts.Add(EnumOption('optimize', 'Set to >= 1 to build with general optimizations', '2', allowed_values=('0', '1', '2', '3', '4', '5', '6')))
opts.Add(EnumOption('simd', 'Choose special CPU register optimizations', 'none', allowed_values=('none', 'sse', '3dnow')))
#opts.Add(EnumOption('cpu', 'Set to 1 to build with special CPU register optimizations', 'i386', allowed_values=('i386', 'athlon-xp', 'core2duo')))
opts.Add(BoolOption('smp', 'Set to 1 to compile engine with symetric multiprocessor support', 0))
#opts.Add(BoolOption('purevm', 'Set to 1 to compile engine with scrict checking for vm/*.qvm modules in paks', 0))
opts.Add(BoolOption('mapping', 'Set to 1 to compile GtkRadiant, XMap and BSPC mapping tools', 0))
opts.Add(BoolOption('xmass', 'Set to 1 to compile XMass', 0))
#opts.Add(BoolOption('vectorize', 'Set to 1 to compile engine with auto-vectorization support', 0))
opts.Add(BoolOption('xppm', 'Set to 1 to compile game with alternative XreaL-PPM support', 0))
opts.Add(BoolOption('curl', 'Set to 1 to compile engine with http-download redirection support', 1))
#opts.Add(BoolOption('openal', 'Set to 1 to compile engine with OpenAL support', 0))
opts.Add(BoolOption('dedicated', 'Set to 1 to only compile the dedicated server', 0))

#
# initialize compiler environment base
#
env = Environment(ENV = {'PATH' : os.environ['PATH']}, options = opts)
#env = Environment(ENV = {'PATH' : os.environ['PATH']}, options = opts, tools = ['mingw'])

Help(opts.GenerateHelpText(env))

#
# set common compiler flags
#
print 'compiling for architecture ', env['arch']

#if env['arch'] == 'win32-mingw':
#	env.Tool('mingw')
#elif env['arch'] == 'win32-xmingw':
#	env.Tool('xmingw', ['SCons/Tools'])


env.Append(CCFLAGS = '-pipe -fsigned-char')

if env['warnings'] == '1':
	env.Append(CCFLAGS = '-Wall')
elif env['warnings'] == '2':
	env.Append(CCFLAGS = '-Wall -Werror')

if env['debug'] != '0':
	env.Append(CCFLAGS = '-ggdb${debug} -D_DEBUG -DDEBUG')
else:
	env.Append(CCFLAGS = '-DNDEBUG')

if env['optimize'] != '0':
	env.Append(CCFLAGS = '-O${optimize} -ffast-math -fno-strict-aliasing -funroll-loops')

#if env['cpu'] == 'athlon-xp':
#	env.Append(CCFLAGS = '-march=athlon-xp') # -msse -mfpmath=sse')
#elif env['cpu'] == 'core2duo':
#	env.Append(CCFLAGS = '-march=prescott')

#if env['arch'] == 'linux-i386' and env['vectorize'] == 1:
#	env.Append(CCFLAGS = '-ftree-vectorize -ftree-vectorizer-verbose=1')

if env['simd'] == 'sse':
	env.Append(CCFLAGS = '-DSIMD_SSE -msse')
elif env['simd'] == '3dnow':
	env.Append(CCFLAGS = '-DSIMD_3DNOW')

conf = Configure(env)
env = conf.Finish()

#
# save options
#
opts.Save('xreal.conf', env)

#
# compile targets
#
Export('env')
if env['dedicated'] == 1:
	SConscript('SConscript_xreal-server', build_dir='build/xreal-server', duplicate=0)
	SConscript('SConscript_base_game', build_dir='build/base/game', duplicate=0)
else:
	SConscript('SConscript_xreal', build_dir='build/xreal', duplicate=0)
	SConscript('SConscript_base_cgame', build_dir='build/base/cgame', duplicate=0)
	SConscript('SConscript_base_game', build_dir='build/base/game', duplicate=0)
	SConscript('SConscript_base_ui', build_dir='build/base/ui', duplicate=0)

if env['mapping'] == 1:
	SConscript('SConscript_gtkradiant', build_dir='build/gtkradiant', duplicate=0)
	SConscript('SConscript_xmap', build_dir='build/xmap', duplicate=0)
	SConscript('SConscript_xmap2', build_dir='build/xmap2', duplicate=0)

if env['xmass'] == 1:
	SConscript('SConscript_xmass', build_dir='build/xmass', duplicate=0)
