{
  'variables': {
    'target_arch%': 'ia32',          # set v8's target architecture
    'host_arch%': 'ia32',            # set v8's host architecture
    'msvs_multi_core_compile': '0',  # we do enable multicore compiles, but not using the V8 way
	'component%': 'static_library',
	# Set to 1 compile with -fPIC cflag on linux. This is a must for shared
    # libraries on linux x86-64 and arm, plus ASLR.
    'linux_fpic%': 1,
	'c99%' : 0,
  },

  'target_defaults': {
    'default_configuration': 'Debug',
    'msvs_configuration_attributes': {
		'OutputDirectory': '<(DEPTH)\\build\\$(ConfigurationName)',
        'IntermediateDirectory': '$(OutDir)\\obj\\$(ProjectName)',
        #'CharacterSet': '1',
    },
    'configurations': {
      'Debug': {
        'defines': [ 'DEBUG', '_DEBUG' ],
        'cflags': [ '-g', '-O0', '-fwrapv' ],
        'msvs_settings': {
          'VCCLCompilerTool': {
            'RuntimeLibrary': 3, # DLL debug
            'Optimization': 0, # /Od, no optimization
            'MinimalRebuild': 'false',
            'OmitFramePointers': 'false',
            'BasicRuntimeChecks': 3, # /RTC1
          },
          'VCLinkerTool': {
            'LinkIncremental': 2, # enable incremental linking
          },
        },
        'xcode_settings': {
          'GCC_OPTIMIZATION_LEVEL': '0',
          'OTHER_CFLAGS': [ '-Wno-strict-aliasing' ],
        },
        'conditions': [
          ['OS == "android"', {
            'cflags': [ '-fPIE' ],
            'ldflags': [ '-fPIE', '-pie' ]
          }]
        ]
      },
      'Release': {
        'defines': [ 'NDEBUG' ],
        'cflags': [
          '-O3',
          '-fstrict-aliasing',
          '-fomit-frame-pointer',
          '-fdata-sections',
          '-ffunction-sections',
        ],
        'msvs_settings': {
          'VCCLCompilerTool': {
            'RuntimeLibrary': 2, # debug release
            'Optimization': 3, # /Ox, full optimization
            'FavorSizeOrSpeed': 1, # /Ot, favour speed over size
            'InlineFunctionExpansion': 2, # /Ob2, inline anything eligible
            'WholeProgramOptimization': 'true', # /GL, whole program optimization, needed for LTCG
            'OmitFramePointers': 'true',
            'EnableFunctionLevelLinking': 'true',
            'EnableIntrinsicFunctions': 'true',
          },
          'VCLibrarianTool': {
            'AdditionalOptions': [
              '/LTCG', # link time code generation
            ],
          },
          'VCLinkerTool': {
            'LinkTimeCodeGeneration': 1, # link-time code generation
            'OptimizeReferences': 2, # /OPT:REF
            'EnableCOMDATFolding': 2, # /OPT:ICF
            'LinkIncremental': 1, # disable incremental linking
          },
        },
      }
    },
    'msvs_settings': {
      'VCCLCompilerTool': {
        'StringPooling': 'true', # pool string literals
        'DebugInformationFormat': 3, # Generate a PDB
        'WarningLevel': 3,
        'BufferSecurityCheck': 'true',
        'ExceptionHandling': 1, # /EHsc
        'SuppressStartupBanner': 'true',
        'WarnAsError': 'false',
        'AdditionalOptions': [
           '/MP', # compile across multiple CPUs
         ],
      },
      'VCLibrarianTool': {
      },
      'VCLinkerTool': {
        'GenerateDebugInformation': 'true',
        'RandomizedBaseAddress': 2, # enable ASLR
        'DataExecutionPrevention': 2, # enable DEP
        'AllowIsolation': 'true',
        'SuppressStartupBanner': 'true',
        'target_conditions': [
          ['_type=="executable"', {
            'SubSystem': 1, # console executable
          }],
        ],
      },
    },
    'conditions': [
      ['OS == "win"', {
        'msvs_cygwin_shell': 0, # prevent actions from trying to use cygwin
        'defines': [
          'WIN32',
          # we don't really want VC++ warning us about
          # how dangerous C functions are...
          '_CRT_SECURE_NO_DEPRECATE',
          # ... or that C implementations shouldn't use
          # POSIX names
          '_CRT_NONSTDC_NO_DEPRECATE',
        ],
        'target_conditions': [
          ['target_arch=="x64"', {
            'msvs_configuration_platform': 'x64'
          }]
        ]
      }],
	  [ 'OS=="win" and component=="shared_library"', {
            'msvs_disabled_warnings': [
              4251,  # class 'std::xx' needs to have dll-interface.
            ],
      }],
      ['OS in "freebsd dragonflybsd linux openbsd solaris android"', {
        'cflags': [ 
			'-Wall', 
			'-g',
      '-gdwarf-2',
			# Don't export any symbols (for example, to plugins we dlopen()).
			# Note: this is *required* to make some plugins work.
			'-fvisibility=hidden',
		],
        'cflags_cc': [ 
			'-std=c++11',
			# Make inline functions have hidden visiblity by default.
			# Surprisingly, not covered by -fvisibility=hidden.
			'-fvisibility-inlines-hidden',
		],
        'target_conditions': [
          ['_type=="static_library"', {
            'standalone_static_library': 1, # disable thin archive which needs binutils >= 2.19
          }],
        ],
        'conditions': [
          [ 'host_arch != target_arch and target_arch=="ia32"', {
            'cflags': [ '-m32' ],
            'ldflags': [ '-m32' ],
          }],
          [ 'target_arch=="x32"', {
            'cflags': [ '-mx32' ],
            'ldflags': [ '-mx32' ],
          }],
          [ 'OS=="linux"', {
			'conditions': [
				['c99==1', { 'cflags_cc': [ '-std=gnu99' ],}, { 'cflags': [ '-ansi' ],}]
			],
          }],
          [ 'OS=="solaris"', {
            'cflags': [ '-pthreads' ],
            'ldflags': [ '-pthreads' ],
			'cflags!': ['-fvisibility=hidden'],
			'cflags_cc!': ['-fvisibility-inlines-hidden'],
          }],
          [ 'OS not in "solaris android"', {
            'cflags': [ '-pthread' ],
            'ldflags': [ '-pthread' ],
          }],
		  ['linux_fpic==1', {
            'cflags': ['-fPIC',],
            'ldflags': ['-fPIC',],
          }],
        ],
      }],
      ['OS=="mac"', {
        'xcode_settings': {
          'ALWAYS_SEARCH_USER_PATHS': 'NO',
          'GCC_CW_ASM_SYNTAX': 'NO',                # No -fasm-blocks
          'GCC_DYNAMIC_NO_PIC': 'NO',               # No -mdynamic-no-pic
                                                    # (Equivalent to -fPIC)
          'GCC_ENABLE_CPP_EXCEPTIONS': 'NO',        # -fno-exceptions
          'GCC_ENABLE_CPP_RTTI': 'NO',              # -fno-rtti
          'GCC_ENABLE_PASCAL_STRINGS': 'NO',        # No -mpascal-strings
		  'GCC_SYMBOLS_PRIVATE_EXTERN': 'YES',      # -fvisibility=hidden
          'GCC_THREADSAFE_STATICS': 'NO',           # -fno-threadsafe-statics
          'PREBINDING': 'NO',                       # No -Wl,-prebind
          'USE_HEADERMAP': 'NO',
          'OTHER_CFLAGS': [
            '-fstrict-aliasing',
          ],
          'WARNING_CFLAGS': [
            '-Wall',
            '-Wendif-labels',
            '-W',
            '-Wno-unused-parameter',
          ],
        },
        'conditions': [
          ['target_arch=="ia32"', {
            'xcode_settings': {'ARCHS': ['i386']},
          }],
          ['target_arch=="x64"', {
            'xcode_settings': {'ARCHS': ['x86_64']},
          }],
        ],
        'target_conditions': [
          ['_type!="static_library"', {
            'xcode_settings': {'OTHER_LDFLAGS': ['-Wl,-search_paths_first']},
          }],
        ],
      }],
     ['OS=="solaris"', {
       'cflags': [ '-fno-omit-frame-pointer' ],
       # pull in V8's postmortem metadata
       'ldflags': [ '-Wl,-z,allextract' ]
     }],
     ['component=="shared_library"', {
        #'defines': ['COMPONENT_BUILD'],
      }],
    ],
    'target_conditions': [
          ['_type=="shared_library" or _type=="loadable_module"',{
            'defines': ['COMPONENT_BUILD'],
            'cflags': [ '-shared' ],
          }],
        ],
  },
}
