import os
import shutil
import sys
import re

from distutils.dir_util import mkpath

from shutil import make_archive
from shutil import rmtree
from shutil import copyfile


def copy_files(src, dest):
    """Copy files in a path ignoring directories"""

    src_files = os.listdir(src)
    for file_name in src_files:
        full_file_name = os.path.join(src, file_name)
        if os.path.isfile(full_file_name):
            shutil.copy(full_file_name, dest)


arduino_base_dir = 'LibNbiot/'
arduino_src_dir = os.path.join(arduino_base_dir, "src/")
modems_dir = '../../libNbiotCore/src/modems'

if os.path.exists(arduino_src_dir):
    rmtree(arduino_src_dir)

mkpath(arduino_src_dir)

# List available firmwares and ask for desired firmware

supported_modems = [o for o in os.listdir(modems_dir) if os.path.isdir(os.path.join(modems_dir, o))]

sys.stdout.write('\n--- Available firmwares:\n')
for n, m in enumerate(supported_modems, 1):
    sys.stdout.write('--- {:2}: {:20}\n'.format(n, m))
sys.stdout.write('\n')
while True:
    idx = int(raw_input('--- Enter firmware index: '))
    try:
        if not 0 < idx <= len(supported_modems):
            sys.stderr.write('--- Invalid index!\n\n')
            continue
    except ValueError:
        pass
    else:
        firmware = supported_modems[idx - 1]
    sys.stdout.write("\n")
    break

# copy files

sys.stdout.write("--- Copying files...")

copy_files('../../libNbiot/include/', arduino_src_dir)
copy_files('../../libNbiot/src/', arduino_src_dir)

copy_files('../../libNbiotCore/include/', arduino_src_dir)
copy_files('../../libNbiotCore/src/', arduino_src_dir)

copy_files('../../libNbiotCore/src/modems', arduino_src_dir)
copy_files(os.path.join('../../libNbiotCore/src/modems', firmware), arduino_src_dir)

copy_files('../../external/src/FP/', arduino_src_dir)
copy_files('../../external/src/List/', arduino_src_dir)
copy_files('../../external/src/StateMachine/', arduino_src_dir)
copy_files('../../external/src/paho.mqtt-sn.embedded-c/MQTTSNClient', arduino_src_dir)
copy_files('../../external/src/paho.mqtt-sn.embedded-c/MQTTSNPacket', arduino_src_dir)

copyfile('README.md', 'LibNbiot/README.md')

sys.stdout.write("finished!\n")

#changing library property file

sys.stdout.write("--- Renaming Library...")

with open(arduino_base_dir+"library.properties") as file:
	filedata=file.read()

filedata = re.sub("name=.+","name=LibNbiot"+firmware,filedata)
 
with open(arduino_base_dir+"library.properties", "w") as file:
	file.write(filedata)
 
sys.stdout.write("finished!\n")

sys.stdout.write("--- Creating archive LibNbiot"+firmware+".zip...")

make_archive('LibNbiot'+firmware, 'zip', './LibNbiot')

sys.stdout.write("finished!\n")

print ('--- Successfully created Arduino Library LibNbiot'+firmware+'.zip for firmware {}!'.format(firmware))
