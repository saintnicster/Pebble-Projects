Requires the Pebble SDK from here -> https://account.getpebble.com/sdk_releases

Drop projects folder into pebble-sdk-release-001.

Run the following command from inside the projects/ folder
	../tools/create_pebble_project.py --symlink-only ../sdk/ <watchface_folder>

	cd <watchface_folder>/

	./waf configure

	./waf build