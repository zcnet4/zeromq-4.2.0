{
  'target_defaults': {
    'conditions': [
      # Linux shared libraries should always be built -fPIC.
      #
      # TODO(ajwong): For internal pepper plugins, which are statically linked
      # into chrome, do we want to build w/o -fPIC?  If so, how can we express
      # that in the build system?
      #['os_posix == 1 and OS != "mac" and OS != "android"', {
      #  'cflags': ['-fPIC', '-fvisibility=default'],

        # This is needed to make the Linux shlib build happy. Without this,
        # -fvisibility=hidden gets stripped by the exclusion in common.gypi
        # that is triggered when a shared library build is specified.
      #  'cflags/': [['include', '^-fvisibility=default$']],
	  #	'scons_variable_settings': {'SHLIBPREFIX':'',},
      #}],
      ['OS=="win"', {
        'configurations': {
          'Debug': {
            'library_dirs': [
              '../3rd/zeromq-3.2.2/lib/win32/vs2013/md/Debug',
              '../3rd/glog/lib/win32/vs2013/x86/md/Debug',
              #'../3rd/gperftools-2.5/lib/win32/vs2013/mt/Debug',
            ],
          },
          'Release': {
            'library_dirs': [
              '../3rd/zeromq-3.2.2/lib/win32/vs2013/md/Release',
              '../3rd/glog/lib/win32/vs2013/x86/md/Release',
              #'../3rd/gperftools-2.5/lib/win32/vs2013/mt/Release',
            ],
          },
        },
        'libraries': [
          '-lAdvapi32',
          '-lWs2_32', 
          '-lzeromq_static',
          '-llibglog_static',
          #'-llibtcmalloc_minimal-debug',
        ],
        'defines': [
          'ZMQ_STATIC_LIBRARY',
          'GLOG_NO_ABBREVIATED_SEVERITIES',
          'GOOGLE_GLOG_DLL_DECL=',
        ],
        'include_dirs': [
          '../3rd/glog/src/windows',
        ],
			
        }, { # OS != "win",
          'defines': [
            
          ],
          'include_dirs': [
            '../3rd/glog/src',
            '../3rd/gperftools-2.5/src',
          ],
		  'libraries': [
			'-lglog',
			'-lzmq',
      '-ltcmalloc_and_profiler',
		  ],
		  'library_dirs': [
              '<(DEPTH)/3rd/glog/lib/linux',
			  '<(DEPTH)/3rd/zeromq-3.2.2/lib/linux',
			  '<(DEPTH)/skynet/3rd/jemalloc/lib',
        '../3rd/gperftools-2.5/.libs',
           ],
           'ldflags': [
            '-Wl,-rpath=3rd/gperftools-2.5/.libs',
          ],
        },
      ]
    ],
	'include_dirs': [
    '../3rd/zeromq-3.2.2/include',
		'../3rd/rapidjson-0.11/include',
    #'../3rd/gperftools-2.5/src/',
    ],
	'defines': [
		#'DEFINE_ZC_VALUE=ZC',
		#'NOMINMAX',
	],
  },
  'includes': [
	  #'../build/win_precompile.gypi',
  ],
  'targets': [
    {
      'target_name': 'gateway',
      'type': 'executable',
      'dependencies': [
	      '../yx/yx.gyp:yx',
        '../skynet/3rd/3rd.gyp:pbc',
        'gateway-forward',
        'gateway-zmq',
      ],
      'defines': [
        'YX_YW'
      ],
      'include_dirs': [
		    '.',
      ],
      'sources': [
        'src/gateway_config.cpp',
        'src/gateway_config.h',
        'src/gateway_constants.cpp',
        'src/gateway_constants.h',
        'src/runner.cpp',
        'src/runner.h',
        "src/forward_server.cpp",
        "src/forward_server.h",
        'src/zmq_server.cpp',
        'src/zmq_server.h',
        "src/agent.cpp",
        "src/agent.h",
        "src/agent_manager.cpp",
        "src/agent_manager.h",
        'src/backend_processor.cpp',
        'src/backend_processor.h',
        'src/frontend_processor.cpp',
        'src/frontend_processor.h',
        'src/main_processor.cpp',
        'src/main_processor.h',
        'src/gateway_util.cpp',
        'src/gateway_util.h',
        'src/op_defs.h',
        'src/netproto.h',
        'src/main.cpp',
        'src/gateway.cpp',
        'src/gateway.h',
        "src/yw_server.cpp",
        "src/yw_server.h",
        "src/skynet_server.cpp",
        "src/skynet_server.h",
        "src/info_trace.cpp",
        "src/info_trace.h",
	  ],
	  'conditions': [
        
      ],
    },
    {
      'target_name': 'gateway-forward',
      'type': 'executable',
      'dependencies': [
	      '../yx/yx.gyp:yx',
        '../skynet/3rd/3rd.gyp:pbc',
      ],
      'defines': [
        'YX_GF'
      ],
      'include_dirs': [
		    '.',
      ],
      'sources': [
        'src/gateway_config.cpp',
        'src/gateway_config.h',
        'src/gateway_constants.cpp',
        'src/gateway_constants.h',
        'src/runner.cpp',
        'src/runner.h',
        "src/forward_server.cpp",
        "src/forward_server.h",
        'src/zmq_server.cpp',
        'src/zmq_server.h',
        "src/agent.cpp",
        "src/agent.h",
        "src/agent_manager.cpp",
        "src/agent_manager.h",
        'src/backend_processor.cpp',
        'src/backend_processor.h',
        'src/frontend_processor.cpp',
        'src/frontend_processor.h',
        'src/main_processor.cpp',
        'src/main_processor.h',
        'src/gateway_util.cpp',
        'src/gateway_util.h',
        'src/op_defs.h',
        'src/netproto.h',
        'src/main.cpp',
        'src/gateway.cpp',
        'src/gateway.h',
        "src/yw_server.cpp",
        "src/yw_server.h",
        "src/skynet_server.cpp",
        "src/skynet_server.h",
        "src/info_trace.cpp",
        "src/info_trace.h",
	  ],
	  'conditions': [
        
      ],
    }, 
    {
      'target_name': 'gateway-zmq',
      'type': 'executable',
      'dependencies': [
	      '../yx/yx.gyp:yx',
        '../skynet/3rd/3rd.gyp:pbc',
      ],
      'defines': [
        'YX_GF'
      ],
      'include_dirs': [
		    '.',
      ],
      'sources': [
        'zmq/main.cpp'
	  ],
	  'conditions': [
        
      ],
    },
  ],
}
