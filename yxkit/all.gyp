{
  'target_defaults': {
    'conditions': [],
	'include_dirs': [],
	'defines': [
		#'DEFINE_ZC_VALUE=ZC',
		#'NOMINMAX',
	],
  },
  'includes': [
	#'fw/yx.gypi',
  ],
  'targets': [
    {
      'target_name': 'ALL_BUILD',
      'type': 'none',
      'dependencies': [
	    '../skynet/skynet.gyp:skynet_bin',
      '../gateway/gateway.gyp:gateway',
      ],
    },
  ],
  
}
