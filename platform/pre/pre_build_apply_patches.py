from os import getcwd
from os.path import join, isfile, isdir, normpath
Import("env")

LIBDEP_DIR_KEY = 'PROJECT_LIBDEPS_DIR'
CONFIG_FILE = 'lv_conf.h'

DIR = getcwd()
OUTPUT_DIR = env[LIBDEP_DIR_KEY]

patches = [
    {
        'patch': join(DIR, 'platform', 'patches', '1-Adafruit_RA8875-enable-highspeed-spi.patch'),
        'outfolder': join(OUTPUT_DIR, 'due', 'Adafruit RA8875'),
        'outfile': 'Adafruit_RA8875.cpp',
    }
]


def dumpEnvData():
    print(env.Dump())


def applyPatches():
    if (OUTPUT_DIR is None) or (not isdir(OUTPUT_DIR)):
        print('#### ERROR ####')
        print("Library folder doesn't exist, patch files")
        print('###############')
        return

    for patch in patches:
        patch_file = patch['patch']
        output_folder = patch['outfolder']
        outfile_name = patch['outfile']
        patchcomplete_file = outfile_name + '.patchcomplete'
        outfile = join(output_folder, outfile_name)

        if not isfile(patch_file):
            print('#### ERROR ####')
            print('Could not find specified patch file')
            print('###############')
            continue

        if not isdir(output_folder):
            print('#### ERROR ####')
            print(f"Patch output folder does not exist: {output_folder}")
            print('###############')
            continue

        if not isfile(outfile):
            print('#### ERROR ####')
            print('File to patch does not exist')
            print('###############')
            continue

        # If we've already patched, skip
        if isfile(join(output_folder, patchcomplete_file)):
            print(f"File {outfile_name} is already patched, skipping")
            continue

        def _touch(path):
            with open(path, "w") as fp:
                fp.write("")

        try:

            outfile_escaped = normpath(outfile.replace(' ', '\\ '))
            patchfile_escaped = normpath(patch_file.replace(' ', '\\ '))

            # Patch the file
            env.Execute(f"patch {normpath(outfile_escaped)} -i {normpath(patchfile_escaped)}")

            # Write the file for patch complete
            env.Execute(lambda *args, **kwargs: _touch(join(output_folder, patchcomplete_file)))
        except Error:
            print("Either failed to patch file {}, or failed to create .patchcomplete file",
                  outfile.replace(' ', '\\ '))


applyPatches()
