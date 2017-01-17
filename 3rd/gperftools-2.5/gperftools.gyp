{
  'targets': [
    {
      'target_name': 'gperftools',
      'type': 'none',
	  'conditions': [
        ['OS=="win"', {
		
        }, { # OS != "win",
		  'actions': [
		  {
            'action_name': 'build gperftools',
			 'inputs': [],
             'outputs': [''],
            'action': ['./build.sh'],
		    'message': 'Generating manpage'
          },
         ],
        }
		]
      ],
	},
  ],
}
