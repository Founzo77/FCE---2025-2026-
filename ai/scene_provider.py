import json

def get_scene_description():
    scene = {
        "camera": { "position": [0, 1.7, -4] },
        "instances": [
            { "id": 1, "mesh": "crate", "position": [0,0,0] },
            { "id": 2, "mesh": "statue", "position": [3,0,2] }
        ],
        "available_meshes": ["crate","barrel","sphere","pillar"]
    }
    return json.dumps(scene, indent=2)
