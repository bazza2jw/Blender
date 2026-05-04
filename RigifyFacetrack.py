import bpy
import json
import pyperclip
#
# Viseme to rigify face rig action generator
#
boneList = [
    "chin",
    "nose.002",
    "nose.R.001",
    "nose.L.001",
    "cheek.B.R.001",
    "cheek.B.L.001",

    "lip.T.R.001",
    "lip.T.L.001",
    "lip.B.R.001",
    "lip.B.L.001",
    
    "lip.B",
    "lip.T",
    
    "lip_end.R.001",
    "lip_end.L.001",
    
    "lid.T.R.002",
    "lid.T.L.002",
    "lid.B.R.002",
    "lid.B.L.002",
    "brow.T.R.002",
    "brow.T.L.002",
    "brow.T.R.003",
    "brow.T.L.003"
]

# set the face bones to base position
def zeroViseme(rig):
    for i in boneList:
        rig.pose.bones[i].location[0] = 0
        rig.pose.bones[i].location[1] = 0
        rig.pose.bones[i].location[2] = 0
        
#
# key a frame
def keyframeVisime(rig,f):
    try:
        for i in boneList:
            rig.pose.bones[i].keyframe_insert(data_path='location')
    except Exception as error:
        print("keyframeVisime Exception:",error)

  
def loadRigifyLoad(context,rig,sX, sY, json_data):
    context.view_layer.objects.active = rig
    bpy.ops.object.mode_set(mode='POSE')
    
    data = json.loads(json_data)
    # number of records
    count = data["Count"]
    # array of data records
    rec = data["Data"][0] 
    # each record is a dictonary of z and x values
    for j in boneList:
        # pair of values
        rig.pose.bones[j].location[1] = rec[j][1] * sY
        rig.pose.bones[j].location[0] = rec[j][0] * sX
        
    for j in boneList:    
        rig.pose.bones[j].keyframe_insert(data_path='location')

  
class RigifyLoadLoadOperator(bpy.types.Operator):
    
    bl_idname = 'cc.control_load'
    bl_label = 'Animation Load'
    def execute(self, context):
        rigName = context.scene.armature
        rig = bpy.data.objects[rigName] 
        sX = context.scene.scaleX
        sY = context.scene.scaleY
        jsonExp = pyperclip.paste()
        print(jsonExp)
        loadRigifyLoad(context,rig,sX, sY,jsonExp)
        # load the RigifyLoad file
        return {'FINISHED'}

   
    
    
# Get the Armature    
class RigifyLoadLoadPanel(bpy.types.Panel):
    bl_label = 'Animation Load'
    bl_idname = 'cc.RigifyLoadload'
    bl_space_type = 'VIEW_3D'
    bl_region_type = 'UI'
    
    def draw(self, context):
        layout = self.layout
        row = layout.row()
        row.prop_search(context.scene, "armature", context.scene,"objects", text="Rig")
        row = layout.row()
        row.prop(context.scene,"scaleX")
        row = layout.row()
        row.prop(context.scene,"scaleY")
        row = layout.row()
        row.operator('cc.control_load', text='Paste & Set') 
 
 
 
def add_properties():
    bpy.types.Scene.armature =  bpy.props.StringProperty(
        name="armature",
        description="Rig to add face animation to",
        default="Armature"
    )

  #Horizontal
    bpy.types.Scene.scaleX =  bpy.props.FloatProperty(
        name="scaleX",
        description="Action Scaling X",
        default=0.01
    )
# Vertical    
    bpy.types.Scene.scaleY =  bpy.props.FloatProperty(
        name="scaleY",
        description="Action Scaling Y",
        default=0.01
    )
    
   
 
 
def remove_properties():
    del bpy.types.Scene.armature
    del bpy.types.Scene.scaleX
    del bpy.types.Scene.scaleY
    del bpy.types.Scene.jsonText

 
 
CLASSES = [
    RigifyLoadLoadOperator,
    RigifyLoadLoadPanel,
]

def register():
    for klass in CLASSES:
        bpy.utils.register_class(klass)
    add_properties()

def unregister():
    for klass in CLASSES:
        bpy.utils.unregister_class(klass)
    remove_properties()
        

if __name__ == '__main__':
    register()   
