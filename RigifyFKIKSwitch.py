import bpy

class RigifyIKFKSwitchPanel(bpy.types.Panel):
    bl_label = "Rigify IK FK Panel"
    bl_idname = "OBJECT_PT_IKFK_Switch"
    bl_space_type = "VIEW_3D"
    bl_region_type = "TOOLS"


    def draw(self, context):
        layout = self.layout
        row = layout.row()
        row.operator('cc.switchonfk', text = "FK")
        row = layout.row()
        row.operator('cc.switchonik', text = "IK")
        row = layout.row()
        row.operator('cc.switchtoobject', text = "Object")
        row = layout.row()
        row.operator('cc.switchtopose', text = "Pose")


def main(context):
    bpy.ops.object.mode_set(mode='POSE')
    bpy.ops.pose.select_all(action='SELECT')

class SwitchToObjectMode(bpy.types.Operator):
    bl_idname = "cc.switchtoobject"
    bl_label = "Switch To Object"
    
    def execute(self, context):
        bpy.ops.object.mode_set(mode='OBJECT')
        return {'FINISHED'}

class SwitchToPoseMode(bpy.types.Operator):
    bl_idname = "cc.switchtopose"
    bl_label = "Switch To Pose"
    
    def execute(self, context):
        bpy.ops.object.mode_set(mode='POSE')
        return {'FINISHED'}

        

class SwitchOnFK(bpy.types.Operator):
    bl_idname = "cc.switchonfk"
    bl_label = "Switch All FK"

    def execute(self, context):
        if bpy.context.active_object.type == 'ARMATURE':
            bpy.context.active_object.pose.use_auto_ik = 1
            bpy.context.active_object.pose.bones["thigh_parent.R"]["FK_limb_follow"] = 1
            bpy.context.active_object.pose.bones["thigh_parent.L"]["FK_limb_follow"] = 1
            bpy.context.active_object.pose.bones["upper_arm_parent.L"]["FK_limb_follow"] = 1
            bpy.context.active_object.pose.bones["upper_arm_parent.R"]["FK_limb_follow"] = 1
            bpy.context.active_object.pose.bones["upper_arm_parent.L"]["IK_Stretch"]=0
            bpy.context.active_object.pose.bones["upper_arm_parent.L"]["IK_FK"] = 1
            bpy.context.active_object.pose.bones["upper_arm_parent.R"]["IK_Stretch"]=0
            bpy.context.active_object.pose.bones["upper_arm_parent.R"]["IK_FK"] = 1
            bpy.context.active_object.pose.bones["thigh_parent.L"]["IK_FK"] = 1
            bpy.context.active_object.pose.bones["thigh_parent.L"]["IK_Stretch"] = 0
            bpy.context.active_object.pose.bones["thigh_parent.R"]["IK_FK"] = 1
            bpy.context.active_object.pose.bones["thigh_parent.R"]["IK_Stretch"] = 0
            bpy.ops.pose.select_all(action='SELECT')

        return {'FINISHED'}

class SwitchOnIK(bpy.types.Operator):
    bl_idname = "cc.switchonik"
    bl_label = "Switch All IK"


    def execute(self, context):
        if bpy.context.active_object.type == 'ARMATURE':
            bpy.context.active_object.pose.bones["thigh_parent.R"]["FK.Limb_follow"] = 0
            bpy.context.active_object.pose.bones["thigh_parent.L"]["FK.Limb_follow"] = 0
            bpy.context.active_object.pose.bones["upper_arm_parent.L"]["FK.Limb_follow"] = 0
            bpy.context.active_object.pose.bones["upper_arm_parent.R"]["FK.Limb_follow"] = 0
            bpy.context.active_object.pose.bones["upper_arm_parent.L"]["IK_Stretch"]=1
            bpy.context.active_object.pose.bones["upper_arm_parent.L"]["IK_FK"] = 0
            bpy.context.active_object.pose.bones["upper_arm_parent.R"]["IK_Stretch"]=1
            bpy.context.active_object.pose.bones["upper_arm_parent.R"]["IK_FK"] = 0
            bpy.context.active_object.pose.bones["thigh_parent.L"]["IK_FK"] = 0
            bpy.context.active_object.pose.bones["thigh_parent.L"]["IK_Stretch"]=1
            bpy.context.active_object.pose.bones["thigh_parent.R"]["IK_FK"] = 0
            bpy.context.active_object.pose.bones["thigh_parent.R"]["IK_Stretch"] = 1
            bpy.ops.pose.select_all(action='SELECT')
        return {'FINISHED'}



CLASSES = [SwitchToPoseMode,SwitchToObjectMode,SwitchOnFK, SwitchOnIK , RigifyIKFKSwitchPanel]


def register():
    
    for klass in CLASSES:
        bpy.utils.register_class(klass)

def unregister():

    for klass in CLASSES:
        bpy.utils.unregister_class(klass)
        

if __name__ == '__main__':
    register()
 