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

emptyList = [
    "E.chin",
    "E.nose.002",
    "E.nose.R.001",
    "E.nose.L.001",
    "E.cheek.B.R.001",
    "E.cheek.B.L.001",
    "E.lip.T.R.001",
    "E.lip.T.L.001",
    "E.lip.B.R.001",
    "E.lip.B.L.001",
    "E.lip.B",
    "E.lip.T",
    "E.lip_end.R.001",
    "E.lip_end.L.001",
    "E.lid.T.R.002",
    "E.lid.T.L.002",
    "E.lid.B.R.002",
    "E.lid.B.L.002",
    "E.brow.T.R.002",
    "E.brow.T.L.002",
    "E.brow.T.R.003",
    "E.brow.T.L.003"
]



# face rest pose default for constraints
DefaultEmpty = {
    "E.chin":[0.030303,-1.34737],
    "E.nose.002":[0,0],
    "E.nose.R.001":[-0.272727,-0.115789],
    "E.nose.L.001":[0.310606,-0.115789],
    "E.cheek.B.R.001":[-0.575758,-0.0526316],
    "E.cheek.B.L.001":[0.636364,-0.0421053],
    "E.lip.B":[0.0227273,-0.936842],
    "E.lip.T":[0.0151515,-0.684211],
    "E.lip.T.R.001":[-0.189394,-0.652632],
    "E.lip.T.L.001":[0.227273,-0.684211],
    "E.lip.B.R.001":[-0.174242,-0.810526],
    "E.lip.B.L.001":[0.227273,-0.8],
    "E.lip_end.R.001":[-0.356061,-0.726316],
    "E.lip_end.L.001":[0.393939,-0.715789],
    "E.lid.T.R.002":[-0.492424,1],
    "E.lid.T.L.002":[0.537879,1],
    "E.lid.B.R.002":[-0.462121,0.810526],
    "E.lid.B.L.002":[0.515152,0.810526],
    "E.brow.T.R.002":[-0.515152,1.27368],
    "E.brow.T.L.002":[0.545455,1.28421],
    "E.brow.T.R.003":[-0.69697,1.16842],
    "E.brow.T.L.003":[0.742424,1.18947],
    "E.nose":[0.0151515,1]
}
# Add constraints
def addConstraints(rig):
    for i in boneList:  
        s = "E." + i
        pb = rig.pose.bones[i]
        constraint = pb.constraints.new('TRANSFORM')
        constraint.name = "TRANSFORMS_mocap_transfer"
        constraint.target = bpy.data.objects.get(s)
        l = DefaultEmpty[s]
    # Use world spaces so the Empty's world transform is copied to the bone
        constraint.target_space = 'LOCAL'
        constraint.owner_space = 'LOCAL'
        constraint.from_min_x = l[0] - 0.2
        constraint.from_max_x = l[0] + 0.2
        constraint.to_min_x = -0.01
        constraint.to_max_x =  0.01
        constraint.from_min_y = l[1] - 0.2
        constraint.from_max_y = l[1] + 0.2
        constraint.to_min_y = - 0.01
        constraint.to_max_y =  0.01
        constraint.from_min_z = 0.0
        constraint.from_max_z = 0.0
        constraint.to_min_z = 0
        constraint.to_max_z = 0
        constraint.mix_mode = 'REPLACE'


    
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


def createEmpties():
    emptyRoot = bpy.data.objects.get("EmptyRoot")
    if not emptyRoot:
        bpy.ops.object.empty_add(type='SPHERE', location=(0, 0, 0))
        emptyRoot = bpy.context.active_object
        emptyRoot.name = "EmptyRoot"
        emptyRoot.scale = (0.1, 0.1, 0.1)
        emptyRoot.rotation_euler = (0, 0, 0)

        
    # each record is a dictonary of z and x values
    for part in emptyList:
    # for the empty name
        empty = bpy.data.objects.get(part)
        if not empty:
            bpy.ops.object.empty_add(type='SPHERE', location=(0, 0, 0))
            empty = bpy.context.active_object
            empty.name = part
            empty.scale = (0.05, 0.05, 0.05)
            empty.rotation_euler = (0, 0, 0)
            l = DefaultEmpty[part]
            empty.location[1] = l[1]
            empty.location[0] = l[0]
            empty.location[2] = 0
            empty.parent = emptyRoot
            empty.show_name = True

  
def loadRigifyLoad(context,rig,sX, sY, json_data):
    context.view_layer.objects.active = rig
    bpy.ops.object.mode_set(mode='OBJECT')
    
    data = json.loads(json_data)
    # number of records
    count = data["Count"]
    # array of data records
    rec = data["Data"][0] 
                
    # set the location in X/Z plane
    for j in boneList:
        s = "E." + j 
        empty = bpy.data.objects.get(s)       
        empty.location[1] = rec[j][1] * sY
        empty.location[0] = rec[j][0] * sX
        empty.keyframe_insert(data_path='location')
    
 

class RigifySetConstraintsOperator(bpy.types.Operator):
    
    bl_idname = 'cc.control_constraints'
    bl_label = 'Set Constraints'
    def execute(self, context):
        rigName = context.scene.armature
        rig = bpy.data.objects[rigName] 
        addConstraints(rig)
        return {'FINISHED'}

class RigifyCreateEmptiesOperator(bpy.types.Operator):
    
    bl_idname = 'cc.control_empty'
    bl_label = 'Create Empties'
    def execute(self, context):
        createEmpties()
        return {'FINISHED'}
 
  
class RigifyLoadLoadOperator(bpy.types.Operator):
    
    bl_idname = 'cc.control_load'
    bl_label = 'Animation Load'
    def execute(self, context):
        rigName = context.scene.armature
        rig = bpy.data.objects[rigName] 
        sX = context.scene.scaleX
        sY = context.scene.scaleY
        jsonExp = pyperclip.paste()
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
        row.operator('cc.control_load', text='Paste') 
        row = layout.row()
        row.operator('cc.control_constraints', text='Set Constraints') 
        row = layout.row()
        row.operator('cc.control_empty', text='Create Empties') 
 
 
 
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
    RigifySetConstraintsOperator,
    RigifyCreateEmptiesOperator,
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
