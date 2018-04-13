from distutils.dir_util import copy_tree
from shutil import make_archive
from shutil import rmtree
from shutil import copyfile

try:
    rmtree('LibNbiot/src/')
except:
    pass

copy_tree('../../libNbiot/include/','LibNbiot/src/.')
copy_tree('../../libNbiot/src/','LibNbiot/src/.')
copy_tree('../../libNbiotCore/include/','LibNbiot/src/.')
copy_tree('../../libNbiotCore/src/','LibNbiot/src/.')
copy_tree('../../external/src/FP/','LibNbiot/src/.')
copy_tree('../../external/src/List/','LibNbiot/src/.')
copy_tree('../../external/src/StateMachine/','LibNbiot/src/.')
copy_tree('../../external/src/paho.mqtt-sn.embedded-c/MQTTSNClient','LibNbiot/src/.')
copy_tree('../../external/src/paho.mqtt-sn.embedded-c/MQTTSNPacket','LibNbiot/src/.')
copyfile('README.md','LibNbiot/README.md')

make_archive('LibNbiot', 'zip', './LibNbiot')
