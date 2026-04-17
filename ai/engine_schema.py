ENGINE_TOOLS = [
    {
        "name": "teleport",
        "description": "Teleport the camera to an absolute world position.",
        "parameters": {
            "type": "object",
            "properties": {
                "x": {"type": "number"},
                "y": {"type": "number"},
                "z": {"type": "number"}
            },
            "required": ["x", "y", "z"]
        }
    },
    {
        "name": "Teleportation_To_Instance",
        "description": "Teleport the camera to an existing instance.",
        "parameters": {
            "type": "object",
            "properties": {
                "instance_id": {"type": "integer"}
            },
            "required": ["instance_id"]
        }
    },
    {
        "name": "Instanciate_Object",
        "description": "Spawn an object from an existing mesh.",
        "parameters": {
            "type": "object",
            "properties": {
                "mesh_id": {"type": "string"}
            },
            "required": ["mesh_id"]
        }
    },
    {
        "name": "Generate_Mesh",
        "description": "Generate a brand new mesh.",
        "parameters": {
            "type": "object",
            "properties": {
                "description": {"type": "string"}
            },
            "required": ["description"]
        }
    }
]
