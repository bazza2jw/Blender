"""
transfer_mocap_action_to_bone.py

Blender 5 script to transfer animation from a Motion Capture landmark Empty
to an Armature pose bone.

How it works
- Adds a temporary COPY_TRANSFORMS constraint to the target pose bone pointing
  at the Empty.
- Bakes the pose (visual keying) over the frame range to create fcurves on the
  armature's action, so the bone receives the Empty's motion.
- Optionally removes the temporary constraint after baking.

Usage (interactive / quick):
1. Select the Armature and the Empty (both selected).
2. Make the Armature the active object and enter Pose Mode.
3. Make the target bone the active pose bone (click the bone).
4. Run this script.

Usage (scripted):
- Set the variables at the top (EMPTY_NAME, ARMATURE_NAME, BONE_NAME) and run.

Notes:
- The script chooses the frame range from the Empty's action if present,
  otherwise falls back to the scene's start/end frames.
- Baking uses visual_keying=True so what you see in the viewport gets baked.
- The script sets the COPY_TRANSFORMS constraint to WORLD->WORLD which works
  for most cases where the Empty is in world space and you want the bone to
  follow that transform. If your setup uses different spaces, adjust
  target_space/owner_space accordingly.
"""

import bpy
from mathutils import Matrix

# ---------- User-configurable (optional) ----------
# If you want to set objects explicitly, set these names (or leave None to use selection/context)
EMPTY_NAME = None       # e.g. "mocap_landmark"
ARMATURE_NAME = None    # e.g. "Armature"
BONE_NAME = None        # e.g. "spine_01"
# Whether to remove the temporary constraint after baking
REMOVE_CONSTRAINT_AFTER_BAKE = True
# Whether to switch to Pose Mode for baking (script will try to switch if needed)
ENSURE_POSE_MODE = True
# -------------------------------------------------

def find_objects():
    empty_obj = None
    arm_obj = None
    pose_bone = None

    if EMPTY_NAME:
        empty_obj = bpy.data.objects.get(EMPTY_NAME)
        if empty_obj is None:
            raise RuntimeError(f"Empty named '{EMPTY_NAME}' not found.")
    if ARMATURE_NAME:
        arm_obj = bpy.data.objects.get(ARMATURE_NAME)
        if arm_obj is None:
            raise RuntimeError(f"Armature named '{ARMATURE_NAME}' not found.")
        if arm_obj.type != 'ARMATURE':
            raise RuntimeError(f"Object '{ARMATURE_NAME}' is not an Armature.")

    # If selection or context should be used
    if not empty_obj or not arm_obj:
        # Find one empty and one armature from selection
        sel = [o for o in bpy.context.selected_objects]
        for o in sel:
            if not empty_obj and o.type == 'EMPTY':
                empty_obj = o
            elif not arm_obj and o.type == 'ARMATURE':
                arm_obj = o

    # Fallback: if active object is empty or armature
    active = bpy.context.view_layer.objects.active
    if active:
        if not empty_obj and active.type == 'EMPTY':
            empty_obj = active
        if not arm_obj and active.type == 'ARMATURE':
            arm_obj = active

    # Bone selection / BONE_NAME or active pose bone
    if arm_obj:
        # If user specified bone name, use that
        if BONE_NAME:
            pose_bone = arm_obj.pose.bones.get(BONE_NAME)
            if pose_bone is None:
                raise RuntimeError(f"Bone '{BONE_NAME}' not found in armature '{arm_obj.name}'.")
        else:
            # Try to get active pose bone in context
            # context.active_pose_bone might be available if armature is active in pose mode
            active_pose_bone = getattr(bpy.context, "active_pose_bone", None)
            if active_pose_bone and active_pose_bone.id_data == arm_obj:
                pose_bone = active_pose_bone
            else:
                # Try typical access via arm_obj.pose.bones and look for selected bone
                selected = [pb for pb in arm_obj.pose.bones if pb.bone.select]
                if selected:
                    pose_bone = selected[0]
    if not empty_obj:
        raise RuntimeError("Could not find an Empty (motion landmark). Select it or set EMPTY_NAME.")
    if not arm_obj:
        raise RuntimeError("Could not find an Armature. Select it or set ARMATURE_NAME.")
    if not pose_bone:
        raise RuntimeError("Could not find a target pose bone. Select a bone in Pose Mode or set BONE_NAME.")

    return empty_obj, arm_obj, pose_bone

def get_frame_range_from_empty(empty_obj):
    """Return (start, end) frame range to bake from empty's action or scene."""
    ad = empty_obj.animation_data
    if ad and ad.action:
        ar = ad.action.frame_range
        start, end = int(ar[0]), int(ar[1])
        return start, end
    # fallback to NLA tracks (if empty uses NLA)
    if ad and ad.nla_tracks:
        min_frame = None
        max_frame = None
        for t in ad.nla_tracks:
            for s in t.strips:
                if min_frame is None or s.frame_start < min_frame:
                    min_frame = int(s.frame_start)
                if max_frame is None or s.frame_end > max_frame:
                    max_frame = int(s.frame_end)
        if min_frame is not None:
            return min_frame, max_frame
    # fallback to scene range
    scene = bpy.context.scene
    return int(scene.frame_start), int(scene.frame_end)

def ensure_pose_mode(arm_obj):
    if ENSURE_POSE_MODE:
        # Make armature active and switch to pose mode
        bpy.context.view_layer.objects.active = arm_obj
        if bpy.context.mode != 'POSE':
            bpy.ops.object.mode_set(mode='POSE')

def bake_empty_to_bone(empty_obj, arm_obj, pose_bone, frame_start, frame_end):
    scene = bpy.context.scene

    # Ensure animation_data exists on armature
    if arm_obj.animation_data is None:
        arm_obj.animation_data_create()

    # Create temporary constraint on the pose bone
    constraint = pose_bone.constraints.new('COPY_TRANSFORMS')
    constraint.name = "TMP_COPY_TRANSFORMS_mocap_transfer"
    constraint.target = empty_obj
    # Use world spaces so the Empty's world transform is copied to the bone
    constraint.target_space = 'WORLD'
    constraint.owner_space = 'WORLD'

    # Make armature active
    bpy.context.view_layer.objects.active = arm_obj

    # Deselect all then select armature to ensure proper operator context
    for o in bpy.context.selected_objects:
        o.select_set(False)
    arm_obj.select_set(True)
    ensure_pose_mode(arm_obj)

    # Optionally set the scene frame to start before bake
    scene.frame_set(frame_start)

    # Bake the pose for the given frame range.
    # We bake the pose (bpy.ops.nla.bake) with visual_keying
