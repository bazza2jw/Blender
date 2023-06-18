import sys
import bpy
import serial


bl_info = {
    # required
    'name': 'Pan Tilt Mapper Control',
    'blender': (3, 0, 0),
    'category': 'Utility',
    # optional
    'version': (1, 0, 0),
    'author': 'B J Hill',
    'description': 'Control Camera Pan/Tilt using virtual scene camera',
}
#
# Use a GCODE controller to operate a Pan Tilt unit.
# this maps the setting of the virtual scene camera to the real camera
# The blender virtual scene can be combined with the real (green screen) scene in OBS Studio
#
# X axis is the tilt and Z axis is the pan (will use the Y stepper)
# The limits for the camera movement are, practically +/- 45 degrees
# The cabling is the main constraint
# The end stops are at (approx) X -45 degress and Z - 45 degrees
# Calibrate the 
# create the client socket

# input buffer
buff = ""
serialPort = None
_last_X = -1000
_last_Z = -1000
# periodic function
def handle_data():
    global _last_X, _last_Z,serialPort
    interval = 0.1
    data = None
    camera = bpy.context.scene.cameratrack
    rot = bpy.data.objects[camera].matrix_world.to_euler()
    next_X = rot[0] * 57.296 - 90
    next_Z = rot[2] * 57.296
    
    try:
        if serialPort is not None:
            if next_X != _last_X:
                b = "\nT%d\n" % (next_X)
                print(b)
                serialPort.write(b.encode())
                _last_X = next_X
            if next_Z != _last_Z:
                b = "\nP%d\n" % (next_Z)
                print(b)
                serialPort.write(b.encode())    
                _last_Z = next_Z
    except Exception as e:
        print("Exception",e)
        buff = ""
        pass
    # update every 100 ms
    return 1
        


class CameraControlConnectOperator(bpy.types.Operator):
    
    bl_idname = 'cc.control_connect'
    bl_label = 'Control Connect'
    def execute(self, context):
        # connect to the controller server
        # close if open
        
        try:
             print(context.scene.serialPort)
             global serialPort 
             serialPort = serial.Serial(context.scene.serialPort,9600)
             if serialPort is None:
                 print("Failed to connect");
             else:
                 print("Connected")     
        except Exception as e:
            print(e)
            print("Exception Failed to connect")    
            pass
            #
            
        return {'FINISHED'}


#
# Control a camera (or any other object) over a network
# Use this for virtual sets - initially the camera is assumed to be on a tripod
# only angles (Z and X axis)  will work and displacements may be unreliable 
# Real camera should match the scene camera
#
class CameraControlPanel(bpy.types.Panel):
    bl_label = 'Camera Control'
    bl_idname = 'cc.cameracontrol'
    bl_space_type = 'VIEW_3D'
    bl_region_type = 'UI'
    
    ipaddress: bpy.props.StringProperty(
        name="serialPort",
        description="Serial Port",
        default="/dev/ttyUSB0",
    )
    def draw(self, context):
        layout = self.layout
        row = layout.row()
        row.prop_search(context.scene, "cameratrack", context.scene,"objects", text="Camera")
        row = layout.row()
        row.prop(context.scene,"serialPort")
        row = layout.row()
        row.operator('cc.control_connect', text='Connect')
        
def add_properties():
    bpy.types.Scene.cameratrack =  bpy.props.StringProperty(
        name="Camera",
        description="Camera To Control",
        default="Camera",
    )
    bpy.types.Scene.serialPort =  bpy.props.StringProperty(
        name="SerialPort",
        description="Serial Port For Pan Tilt Access",
        default="/dev/ttyUSB0",
    )
        

def remove_properties():
    del bpy.types.Scene.cameratrack
    del bpy.types.Scene.serialPort




CLASSES = [
    CameraControlConnectOperator,
    CameraControlPanel,
]

def register():
    for klass in CLASSES:
        bpy.utils.register_class(klass)
    add_properties()

def unregister():
    for klass in CLASSES:
        bpy.utils.unregister_class(klass)
    remove_properties()
        
bpy.app.timers.register(handle_data)

if __name__ == '__main__':
    register()
