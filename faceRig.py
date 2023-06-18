
import bpy
# Creates a NLA strip of actions - intended for rapid lipsyncing using the PoseLib 3+ and Papagyo
# 1) Define the face poses for each phoneme
# 2) Load this script and run it. A panel will be added to the NLA 
# 3) In the NLA editor add a strip to the armature and set the field accordingly
# 4) Load the MOho file. The format is <fame> <Action Name>. This is the output from Papagyo, but any any list of actions could be supported if the pose has been defined.
# 5) The NLA strip will now be the list of assumed face actions
# 6) Bake the track to an action and edit as required
# 

# your rig ... or use bpy.context.active_object if you prefer
def loadMoho(context,rig,track, filepath):
    visTrack = rig.animation_data.nla_tracks[track]
    context.view_layer.objects.active = rig
    bpy.ops.object.mode_set(mode='POSE')
    with open(filepath, "r", encoding='utf-8') as fp:
        for line in fp:
            try:
                words= line.split()
                if len(words) < 2:
                    pass
                else:
                    frame = int(words[0]) 
                    if frame > 0:
                        vis = words[1]
                        bpy.context.scene.frame_current = frame
                        action = bpy.data.actions[vis]
                        if action is None:
                            continue
                        cycleStrip = visTrack.strips.new("Cycle",start=frame,action=action)
            except:
                print("Record Error:",line)
                pass        
  


class MohoLoadOperator(bpy.types.Operator):
    
    bl_idname = 'cc.control_load'
    bl_label = 'Moho Load'
    filter_glob: bpy.props.StringProperty(default = '*.dat',options = {'HIDDEN'})
    filepath: bpy.props.StringProperty(subtype="FILE_PATH")
    def execute(self, context):
        print("filepath=", self.filepath)
        rigName = context.scene.armature
        rig = bpy.data.objects[rigName] 
        track = context.scene.trackName
        loadMoho(context,rig,track,self.filepath)
        # load the moho file
        return {'FINISHED'}

    def invoke(self, context, event): # See comments at end  [1]        
        context.window_manager.fileselect_add(self)  
        #Open browser, take reference to 'self' read the path to selected 
        #file, put path in predetermined data structure self.filepath
        return {'RUNNING_MODAL'}  
        # Tells Blender to hang on for the slow user input

class MohoLoadPanel(bpy.types.Panel):
    bl_label = 'Moho Load'
    bl_idname = 'cc.mohoload'
    bl_space_type = 'NLA_EDITOR'
    bl_region_type = 'UI'
    
    def draw(self, context):
        layout = self.layout
        row = layout.row()
        row.prop_search(context.scene, "armature", context.scene,"objects", text="Rig")
        row = layout.row()
        row.prop(context.scene,"viseme")
        row = layout.row()
        row.prop(context.scene,"trackName")
        row = layout.row()
        row.operator('cc.control_load', text='Load')



def add_properties():
    bpy.types.Scene.armature =  bpy.props.StringProperty(
        name="armature",
        description="Rig to add face animation to",
        default="Armature",
    )
    bpy.types.Scene.viseme =  bpy.props.StringProperty(
        name="viseme",
        description="Face animation file",
        default="viseme.dat",
    )
    bpy.types.Scene.trackName =  bpy.props.StringProperty(
        name="trackName",
        description="Track Name",
        default="NlaTrack",
    )
    
        

def remove_properties():
    del bpy.types.Scene.armature
    del bpy.types.Scene.viseme
    del bpy.types.Scene.trackName



CLASSES = [
    MohoLoadOperator,
    MohoLoadPanel,
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