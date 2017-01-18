# yx.gypi
{
  'targets': [
	{
      'target_name': 'yx',
      'type': 'shared_library',
      'dependencies': [
		    'libuv/uv.gyp:libuv',
        '../3rd/gperftools-2.5/gperftools.gyp:gperftools',
			  #'skynet/3rd/jemalloc/jemalloc.gyp:jemalloc',
      ],
      'direct_dependent_settings': {
        'include_dirs': [
          '..',
        ],
      },
      'export_dependent_settings': [
        #'libuv/uv.gyp:libuv',
      ],
      'defines': [
		  'YX_IMPLEMENTATION',
		  'PACKET_SIZE_CONTAINS_PACKET_SIZE',
      ],
      'include_dirs': [
        "..",
      ],
      'sources': [
        "yx_export.h",
        "yx_stl_util.h",
        "yx_constants_macros.h",
        "yx_util.cpp",
        "yx_util.h",
        "yx_time.cpp",
        "yx_time.h",
        "yx_timer.cpp",
        "yx_timer.h",
        "yx_xml.cpp",
        "yx_xml.h",
        "yx_constants.cpp",
        "yx_constants.h",
        "yx_queue.h",
        "allocator.h",
        "allocator.cpp",
        "allocator_tracker.h",
        "allocator_tracker.cpp",
        "glogger_async.cpp",
        "glogger_async.h",
		    "id_allocator.cpp",
        "id_allocator.h",
        "atomic.h",
        "spinlock.h",
        "spinlock.cpp",
        "rwlock.h",
        "crypt.cpp",
        "crypt.h",
        "loop.cpp",
        "loop.h",
        "loop_impl.cpp",
        "loop_impl.h",
        'loop_runner.cpp',
        'loop_runner.h',
        "task_runner.cpp",
        "task_runner.h",
        "task_runner_impl.cpp",
        "task_runner_impl.h",
        "recv_cache.cpp",
        "recv_cache.h",
        "memory_pool.cpp",
        "memory_pool.h",
        "ini_reader.cpp",
        "ini_reader.h", 
        "socket_manager.cpp",
        "socket_manager.h",
        "tcp.cpp",
        "tcp.h",
        "tcp_delegate.h",
        "udp.cpp",
        "udp.h",
        "_sockaddr.cpp",
        "_sockaddr.h",
        'processor.cpp',
        'processor.h',
        #"ringbuffer.cpp",
        #"ringbuffer.h",
        "packet.cpp",
        "packet.h",
        "packet_queue.cpp",
        "packet_queue.h",
        "packet_view.cpp",
        "packet_view.h",
        "string_util.cpp",
        "string_util.h",
        "kcp/ikcp.cpp",
        "kcp/ikcp.h",
        "fnv-5.0.3/hash_32a.c",
        "fnv-5.0.3/hash_64a.c",
        "fnv-5.0.3/fnv.h",
	  ],
	  'libraries': [ 
				 
	  ],
	  'conditions': [
      [ 'OS=="win"', {
        'defines': [
          '_CRT_SECURE_NO_WARNINGS',
          'WIN32_LEAN_AND_MEAN',
          'GLOG_NO_ABBREVIATED_SEVERITIES',
          'GOOGLE_GLOG_DLL_DECL=',
        ],
		    'include_dirs': [
			    '../3rd/glog/src/windows',
          '../3rd/gperftools-2.5/src/windows',
        ],
        'configurations': {
          'Debug': {
            'library_dirs': [
              '../3rd/glog/lib/win32/vs2013/x86/md/Debug',
              '../3rd/gperftools-2.5/lib/win32/vs2013/x86/md/Debug',
            ],
          },
          'Release': {
            'library_dirs': [
              '../3rd/glog/lib/win32/vs2013/x86/md/Release',
              '../3rd/gperftools-2.5/lib/win32/vs2013/x86/md/Release',
            ],
          },
        },
        'libraries': [
          '-llibglog_static',
          '-llibtcmalloc_minimal',
        ],
        'sources': [
          'atomic_win.cpp',
        ],
      }, { # POSIX
          'defines': [ '_GNU_SOURCE' ],
          'sources': [
            
          ],
          'include_dirs': [
            '../3rd/glog/src',
            '../3rd/gperftools-2.5/src',
          ],
		      'libraries': [
            '-lglog',
            #'-ltcmalloc_and_profiler',
            '-ltcmalloc_and_profiler',
            #libjemalloc_pic.a静态库
            #'-ljemalloc_pic',
		      ],
		      'library_dirs': [
            '../3rd/glog/lib/linux',
            '../3rd/gperftools-2.5/.libs',
            #'../../skynet/3rd/jemalloc/lib',
          ],
          'ldflags': [
            '-Wl,-rpath=../3rd/gperftools-2.5/.libs',
          ],		  
        }
    ],
        [ 'OS in "mac dragonflybsd freebsd linux netbsd openbsd".split()', {
          'link_settings': {
            'libraries': [ '-lutil' ],
          },
        }],
        [ 'OS=="solaris"', { # make test-fs.c compile, needs _POSIX_C_SOURCE
          'defines': [
            '__EXTENSIONS__',
            '_XOPEN_SOURCE=500',
          ],
        }],
        [ 'OS=="aix"', {     # make test-fs.c compile, needs _POSIX_C_SOURCE
          'defines': [
            '_ALL_SOURCE',
            '_XOPEN_SOURCE=500',
          ],
        }],
      ],
      'msvs-settings': {
        'VCLinkerTool': {
          'SubSystem': 1, # /subsystem:console
        },
      },
    },
  ],
}
