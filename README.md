# Blender
A collection of files for use with Blender

Comments in the code files should say what they are useful for and how to use.

faceRig.py - import MoHo / Rubarb lipsync to an NLA track that can then be processed
generateMatte.py - auto rotoscope
CameraControlGCODE.py - maps a real camera on a GCODE controlled pan-tilt to the scene camera
CameraControl.py  & panTiltControl.ino - maps a real camera on a servo controlled pan-tilt to a scene camera via an Arduino 
RigifyFKIKSwitch.py - adds a panel to quickly switch between IK and FK, Also buttons for pose and object mode for easy switch, especially when using a tablet.
RigifyViseme.py - A script to load Moho visemes to an action for Makehuman 2 Rigify rig.
HGRigifyViseme.py - A script to load Moho visemes to an action for Human Generator Rigify rig.
cameraChange.py - A script to render on camera change (name, location, rotation, lens).Use this to control render layers when there are multiple cameras.

Workflows - a set of single task ComfyUI workflows

resequenceAndLinkFilenames - this creates a set of symbolic links to a directory of files. The symbolic links are a continuous sequence. Use this to create a set of filenames for piping into ComfyUI without loseing the orginal sequence. For example a character rendered to a view layer that is then piped through ComfyUI for lipsync.

resequenceFilenames - this renames the given files into a continous numbered sequence.

