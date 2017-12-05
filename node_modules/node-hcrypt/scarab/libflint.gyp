{
	"variables": {
		"target_arch%": "x64",
		"library": "static_library"
	},
	"target_defaults": {
		"include_dirs": [
			"flint",
			"config/<(OS)/<(target_arch)"
		]
	},
	"targets": [
		{
			"target_name": "flint",
			"product_prefix": "lib",
			"type": "<(library)",
			"sources": [
				"<!@(find flint -name \\*.c -not \\( -path \"flint/doc/*\" -or -path \"flint/*/profile/*\" -or -path \"flint/*/test/*\" \\) -print)"
			]
		}
	]
}
