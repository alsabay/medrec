{
	"variables": {
		"target_arch%": "x64",
		"library": "static_library"
	},
	"target_defaults": {
		"include_dirs": [
			".",
			"include",
			"config/<(OS)/<(target_arch)"
		],
		"libraries": [
			"-L./Default/obj.target/libflint.a",
			"-lgmp",
			"-lmpfr"
		]
	},
	"targets": [
		{
			"target_name": "scarab",
			"product_prefix": "lib",
			"type": "<(library)",
			"sources": [
				"src/scarab.c",
				"src/types.c",
				"src/util.c"
			],
			"direct_dependent_settings": {
				"include_dirs": [
					".",
					"include",
					"config/<(OS)/<(target_arch)"
				]
			},
			"dependencies": [
				"libflint.gyp:flint"
			]
		},
		{
			"target_name": "test",
			"type": "executable",
			"sources": [
				"src/test.c"
			],
			"dependencies": [
				"scarab"
			]
		}
	]
}
