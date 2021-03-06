import os
import ycm_core

def DirectoryOfThisScript():
  return os.path.dirname( os.path.abspath( __file__ ) )


def GetDatabase():
  compilation_database_folder = os.path.join(DirectoryOfThisScript(), 'build')
  if os.path.exists(compilation_database_folder):
    return ycm_core.CompilationDatabase(compilation_database_folder)
  database = None


def IsHeaderFile(filename):
    extension = os.path.splitext(filename)[1]
    return extension == '.h'

def GetCompilationInfoForFile( filename ):
  database = GetDatabase()
  if not database:
    return None
  # The compilation_commands.json file generated by CMake does not have entries
  # for header files. So we do our best by asking the db for flags for a
  # corresponding source file, if any. If one exists, the flags for that file
  # should be good enough.
  if IsHeaderFile(filename):
    basename = os.path.splitext(filename)[0]
    replacement_file = basename + '.c'
    if os.path.exists( replacement_file ):
      compilation_info = database.GetCompilationInfoForFile(replacement_file)
      if compilation_info.compiler_flags_:
        return compilation_info
    return None
  return database.GetCompilationInfoForFile( filename )


def MakeRelativePathsInFlagsAbsolute( flags, working_directory ):
  if not working_directory:
    return list( flags )
  new_flags = []
  make_next_absolute = False
  path_flags = [ '-isystem', '-I', '-iquote', '--sysroot=' ]
  for flag in flags:
    new_flag = flag

    if make_next_absolute:
      make_next_absolute = False
      if not flag.startswith( '/' ):
        new_flag = os.path.join( working_directory, flag )

    for path_flag in path_flags:
      if flag == path_flag:
        make_next_absolute = True
        break

      if flag.startswith( path_flag ):
        path = flag[ len( path_flag ): ]
        new_flag = path_flag + os.path.join( working_directory, path )
        break

    if new_flag:
      new_flags.append( new_flag )
  return new_flags


def FlagsForFile( filename, **kwargs ):
  compilation_info = GetCompilationInfoForFile(filename)
  if not compilation_info:
    return None

  final_flags = MakeRelativePathsInFlagsAbsolute(
    compilation_info.compiler_flags_,
    compilation_info.compiler_working_dir_
  )

  return {
    'flags': final_flags,
    'do_cache': True
  }
