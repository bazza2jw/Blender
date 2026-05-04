# Blender 5.x — Access an object's Action and inspect/export f-curves & keyframes
# Paste into Blender's Text Editor and Run Script (requires bpy).

import bpy
import json

def get_object(name: str | None = None) -> bpy.types.Object:
    """Return object by name or the active object if name is None."""
    if name:
        obj = bpy.data.objects.get(name)
        if not obj:
            raise ValueError(f"No object named '{name}'")
        return obj
    obj = bpy.context.active_object
    if not obj:
        raise RuntimeError("No active object in context and no name provided.")
    return obj

def get_active_action(obj: bpy.types.Object) -> bpy.types.Action | None:
    """Return the action directly assigned to the object's animation_data (if any)."""
    ad = obj.animation_data
    return ad.action if ad else None

def get_actions_from_nla(obj: bpy.types.Object) -> list:
    """Return a list of Actions referenced by NLA strips on the object (may be empty)."""
    actions = []
    ad = obj.animation_data
    if not ad:
        return actions
    for track in ad.nla_tracks:
        for strip in track.strips:
            if strip.action and strip.action not in actions:
                actions.append(strip.action)
    return actions

def summarize_action(action: bpy.types.Action) -> dict:
    """Return a summary dict of the action (fcurve count, frame range, fcurve paths)."""
    if action is None:
        return {}
    summary = {
        "name": action.name,
        "frame_start": action.frame_range[0] if action.frame_range else None,
        "frame_end": action.frame_range[1] if action.frame_range else None,
        "fcurves": []
    }
    for fc in action.fcurves:
        fc_summary = {
            "data_path": fc.data_path,
            "array_index": fc.array_index,
            "group": getattr(fc.group, "name", None),
            "keyframe_count": len(fc.keyframe_points),
        }
        summary["fcurves"].append(fc_summary)
    return summary

def action_to_dict(action: bpy.types.Action) -> dict:
    """Convert an Action to a JSON-serializable dict with keyframes for each fcurve."""
    if action is None:
        return {}
    out = {
        "name": action.name,
        "frame_start": action.frame_range[0] if action.frame_range else None,
        "frame_end": action.frame_range[1] if action.frame_range else None,
        "fcurves": []
    }
    for fc in action.fcurves:
        fc_entry = {
            "data_path": fc.data_path,
            "array_index": fc.array_index,
            "group": getattr(fc.group, "name", None),
            "keyframes": []
        }
        for kp in fc.keyframe_points:
            # kp.co = Vector((frame, value))
            fc_entry["keyframes"].append({"frame": float(kp.co.x), "value": float(kp.co.y), "interp": kp.interpolation})
        out["fcurves"].append(fc_entry)
    return out

def evaluate_fcurve_at_frame(fcurve: bpy.types.FCurve, frame: float) -> float:
    """Evaluate an FCurve at a given frame (interpolated)."""
    return float(fcurve.evaluate(frame))

def evaluate_action_for_object(obj: bpy.types.Object, action: bpy.types.Action, frame: int) -> dict:
    """
    Temporarily assign action to object (if not already) for evaluation,
    set scene frame, update depsgraph, and read evaluated transforms if object transform f-curves are present.
    NOTE: This assigns action to object's animation_data.action (temporarily).
    """
    scene = bpy.context.scene
    ad = obj.animation_data
    created_ad = False
    if not ad:
        obj.animation_data_create()
        ad = obj.animation_data
        created_ad = True

    prev_action = ad.action
    ad.action = action

    try:
        scene.frame_set(frame)
        deps = bpy.context.evaluated_depsgraph_get()
        obj_eval = obj.evaluated_get(deps)
        result = {
            "frame": frame,
            "matrix_world": tuple(obj_eval.matrix_world),
            "location": tuple(obj_eval.matrix_world.to_translation()),
            "rotation_euler": tuple(obj_eval.matrix_world.to_euler()),
            "scale": tuple(obj_eval.matrix_world.to_scale()),
        }
    finally:
        # restore previous action (clean up)
        ad.action = prev_action
        if created_ad and not ad.action and not ad.drivers and not ad.nla_tracks:
            # remove animation_data if it was created and unused
            obj.animation_data_clear()

    return result

def print_action_info(action: bpy.types.Action):
    if not action:
        print("No action.")
        return
    print(f"Action: {action.name}")
    print(f"  Frame range: {action.frame_range}")
    print(f"  F-curves: {len(action.fcurves)}")
    for fc in action.fcurves:
        print(f"    - {fc.data_path}[{fc.array_index}] ({len(fc.keyframe_points)} keyframes)  group={getattr(fc.group,'name',None)}")

# ----------------------
# Example usage
# ----------------------
if __name__ == "__main__":
    # Use active object or pass a name: get_object("Armature")
    obj = get_object(None)
    print(f"Inspecting object: {obj.name} (type={obj.type})")

    action = get_active_action(obj)
    if action:
        print("Active action (directly assigned):")
        print_action_info(action)
    else:
        print("No direct action assigned to object.")

    nla_actions = get_actions_from_nla(obj)
    if nla_actions:
        print("Actions found in NLA strips:")
        for a in nla_actions:
            print_action_info(a)

    # If there is at least one action, export the first one to JSON-string and evaluate at frame 1
    chosen_action = action or (nla_actions[0] if nla_actions else None)
    if chosen_action:
        exported = action_to_dict(chosen_action)
        print("\nExported action summary (JSON):")
        print(json.dumps(exported, indent=2)[:4000])  # truncate print to prevent enormous console spam

        eval_info = evaluate_action_for_object(obj, chosen_action, frame=1)
        print("\nEvaluated object at frame 1 (with action applied):")
        print(f"  location: {eval_info['location']}")
        print(f"  rotation_euler: {eval_info['rotation_euler']}")
        print(f"  scale: {eval_info['scale']}")
    else:
        print("No action available to export/evaluate.")