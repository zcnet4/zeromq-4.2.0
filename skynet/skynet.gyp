{
  'variables': {
    'c99':1,
  },
  'target_defaults': {
    'conditions': [
     ['OS in "freebsd dragonflybsd linux openbsd solaris android"', {
        'cflags!': ['-fvisibility=hidden'],
		'cflags_cc!': ['-fvisibility-inlines-hidden'],
	  #	'scons_variable_settings': {'SHLIBPREFIX':'',},
      }],
    ],
	'include_dirs': [
		# ����ȫ�ֿ���Ŀ¼��
		#'..',
		#'../common',
    ],
	#Ŀ�깤��Ԥ����궨��
	'defines': [
		#'DEFINE_ZC_VALUE=ZC',
		#'NOMINMAX',
	],
	#ȥ��libǰ׺��
	'product_prefix': '',
  },
  'includes': [
	'skynet-src/skynet.gypi',
  ],
  'targets': [
    {
	  # Ŀ�깤����
      'target_name': 'skynet_bin',
      'type': 'executable',
	  # Ŀ�깤����������
      'dependencies': [
	    '3rd/3rd.gyp:lua',
	    '3rd/3rd.gyp:md5',
		'3rd/3rd.gyp:cjson',
		#'3rd/3rd.gyp:tar',
		'3rd/3rd.gyp:protobuf',
		'skynet',
		'snlua',
		'logger',
		'harbor',
		'harbor2',
		'socketdriver',
      ],
	  #Ŀ�깤��Ԥ����궨��
      'defines': [
      ],
	  #Ŀ�깤��C++ includeĿ¼
      'include_dirs': [
		'.'
      ],
	  #Ŀ�깤��Դ����·��
      'sources': [
		"skynet-src/skynet_main.c",		
	  ],
	  'conditions': [
        ['OS=="win"', {
		  'product_name':'skynet2',
          'defines': [

          ],
		  'msvs_disabled_warnings': [
			'4204','4013','4996','4152','4047','4024','4133'
		  ],
		  'msvs_settings': {
              'VCLinkerTool': {
				'AdditionalLibraryDirectories':[
					'3rd',
				],
              },
            },
			'dependencies': [	
				'3rd/3rd.gyp:posix_win',
			],
        }, { # OS != "win",
		  'product_name':'skynet',
          'defines': [
            
          ],
        }]
      ],
    },
  ],
  
}
