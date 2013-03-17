import os

prefix = ARGUMENTS.get ('prefix', "/usr/local")
usr_local_headers = os.path.join (prefix, "include")
usr_local_lib = os.path.join (prefix, "lib")

target_usr_local_headers = os.path.join (usr_local_headers, "wepa")

debug = ARGUMENTS.get ('debug', 0)

env = Environment ()

current_directory = Dir ('.').abspath;

# CPPPATH will be relative to src/<target>
source_headers = os.path.join (current_directory, "include")
env.Append (CPPPATH = [source_headers, usr_local_headers])
env.Append (LIBS = [''])

if int(debug):
   env.Append (CCFLAGS = '-g -O0 -D_DEBUG')
   env.Append (LIBPATH = os.path.join (current_directory, "debug"))
   env.Append (VARIANT = 'debug')
else:
   env.Append (CCFLAGS = '-O3')
   env.Append (VARIANT = 'release')

libraries = [];

source = os.path.join (current_directory, "source")
sources = Glob(source + '/*')
for source in sources:
   ss = str (source)
   ss += '/SConstruct'
   compile_library = SConscript (ss, exports='env')
   libraries.extend (compile_library)

env.Default (libraries)

#
# Run 'scons test' to compile unit-tests
#
unit_tests = []
run_tests = []

test = os.path.join (current_directory, "test")
tests = Glob(test + '/*')
for test in tests:
   ss = str (test)
   ss += '/SConstruct'
   test_unit_program = SConscript (ss, exports='env')
   unit_tests.extend (test_unit_program)
   print test_unit_program [0]
   test_unit = Builder (action = '%s > $TARGET' % test_unit_program [0])
   env ['BUILDERS']['RunTestUnit'] = test_unit
   test_unit_result = env.RunTestUnit ('%s.output' % test_unit_program [0], 'SConstruct')
   run_tests.extend (test_unit_result)
   Depends (test_unit_result, test_unit_program)

env.Alias ('test', run_tests)

