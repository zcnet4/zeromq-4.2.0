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
		  # Ŀ�깤����
		  'target_name': 'rapidxml',
		  'type': 'static_library',
		  # Ŀ�깤����������
		  'dependencies': [
			#'../../base/base.gyp:base'
		  ],
		  #Ŀ�깤��Ԥ����궨��
		  'defines': [
			#'DEFINE_ZC_VALUE=ZC',
			#'YGHOST_IMPLEMENTATION',
		  ],
		  #Ŀ�깤��C++ includeĿ¼
		  'include_dirs': [
		  ],
		  #Ŀ�깤��Դ����·��
		  'sources': [
			#  ����
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
