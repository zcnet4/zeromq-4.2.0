# Copyright (c) 2012 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

# This is the "public" ppapi.gyp file, which must have dependencies on the
# redistributable portions of PPAPI only. This prevents circular dependencies
# in the .gyp files (since ppapi_internal depends on parts of Chrome).

{
  'variables': {
    'chromium_code': 1,  # Use higher warning level.
  },
	 'targets': [
		{
		  # 目标工程名
		  'target_name': 'rapidxml',
		  'type': 'static_library',
		  # 目标工程依赖工程
		  'dependencies': [
			#'../../base/base.gyp:base'
		  ],
		  #目标工程预处理宏定义
		  'defines': [
			#'DEFINE_ZC_VALUE=ZC',
			#'YGHOST_IMPLEMENTATION',
		  ],
		  #目标工程C++ include目录
		  'include_dirs': [
		  ],
		  #目标工程源代码路径
		  'sources': [
			#  宿主
			'rapidxml.hpp',
			'rapidxml_print.hpp',
			'yygame_xml.h',
			'yygame_xml.cc',
		  ],
		  'direct_dependent_settings': {
			'include_dirs': [
			  '..',
			],
		  },
		},
	  ],
  
}
